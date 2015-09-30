import os
import shutil
import numpy

import pipeline.domain.measures as measures
from pipeline.hif.heuristics import tclean
import pipeline.infrastructure as infrastructure
import pipeline.infrastructure.basetask as basetask
import pipeline.infrastructure.casatools as casatools
import pipeline.infrastructure.mpihelpers as mpihelpers
import pipeline.infrastructure.pipelineqa as pipelineqa
from pipeline.infrastructure import casa_tasks
from .basecleansequence import BaseCleanSequence
from .imagecentrethresholdsequence import ImageCentreThresholdSequence
from .iterativesequence import IterativeSequence
from .iterativesequence2 import IterativeSequence2
from . import cleanbase

LOG = infrastructure.get_logger(__name__)


class TcleanInputs(cleanbase.CleanBaseInputs):
    def __init__(self, context, output_dir=None, vis=None, imagename=None,
                 intent=None, field=None, spw=None, spwsel=None, uvrange=None, specmode=None,
                 gridder=None, deconvolver=None, outframe=None, imsize=None, cell=None,
                 phasecenter=None, nchan=None, start=None, width=None,
                 weighting=None, robust=None, noise=None, npixels=None,
                 restoringbeam=None, iter=None, mask=None, niter=None, threshold=None,
                 noiseimage=None, hm_masking=None, hm_cleaning=None, tlimit=None,
                 masklimit=None, maxncleans=None, subcontms=None, parallel=None):
        self._init_properties(vars())
        self.heuristics = tclean.TcleanHeuristics(self.context, self.vis, self.spw)

    # Add extra getters and setters here
    spwsel = basetask.property_with_default('spwsel', {})
    hm_cleaning = basetask.property_with_default('hm_cleaning', 'rms')
    hm_masking = basetask.property_with_default('hm_masking', 'centralquarter')
    masklimit = basetask.property_with_default('masklimit', 4.0)
    tlimit = basetask.property_with_default('tlimit', 2.0)
    subcontms = basetask.property_with_default('subcontms', False)

    @property
    def noiseimage(self):
        return self._noiseimage

    @noiseimage.setter
    def noiseimage(self, value):
        if value is None:
            ms = self.context.observing_run.get_ms(name=self.vis[0])
            observatory = ms.antenna_array.name
            if 'VLA' in observatory:
                value = 'V'
            else:
                value = 'Q'
        self._noiseimage = value

    @property
    def maxncleans(self):
        if self._maxncleans is None:
            return 10
        return 10
        return self._maxncleans

    @maxncleans.setter
    def maxncleans(self, value):
        self._maxncleans = value

    @property
    def deconvolver(self):
        if not self._deconvolver:
            return self.heuristics.deconvolver(self.specmode, self.spw)
        else:
            return self._deconvolver

    @deconvolver.setter
    def deconvolver(self, value):
        self._deconvolver = value

    @property
    def robust(self):
        if self._robust == -999.0:
            if (self.spw.find(',') == -1):
                return self.heuristics.robust(self.spw)
            else:
                robust = 0.0
                spws = self.spw.split(',')
                for spw in spws:
                    robust += self.heuristics.robust(spw)
                robust /= len(spws)
                return robust
        else:
            return self._robust

    @robust.setter
    def robust(self, value):
        self._robust = value


class Tclean(cleanbase.CleanBase):
    Inputs = TcleanInputs

    def is_multi_vis_task(self):
        return True

    def prepare(self):
        inputs = self.inputs

        LOG.info('\nCleaning for intent "%s", field %s, spw %s\n',
                 inputs.intent, inputs.field, inputs.spw)

        # try:
        result = None

        # delete any old files with this naming root. One of more
        # of these (don't know which) will interfere with this run.
        LOG.info('deleting %s*.iter*', inputs.imagename)
        shutil.rmtree('%s*.iter*' % inputs.imagename, ignore_errors=True)

        # Determine masking limits depending on image and cell sizes
        self.pblimit_image, self.pblimit_cleanmask = \
            inputs.heuristics.pblimits(inputs.imsize, inputs.cell)
        inputs.pblimit = self.pblimit_image

        # Get an empirical noise estimate by generating Q or V image.
        #    Assumes presence of XX and YY, or RR and LL
        #    Assumes source is unpolarized
        #    Make code more efficient (use MS XX and YY correlations) directly.
        #    Update / replace  code when sensitity function is working.
        #model_sum, cleaned_rms, non_cleaned_rms, residual_max, \
        #  residual_min, rms2d, image_max = \
        #  self._do_noise_estimate(stokes=inputs.noiseimage)
        #sensitivity = non_cleaned_rms
        #LOG.info('Noise rms estimate from %s image is %s' %
        #  (inputs.noiseimage, sensitivity))

        # Get a noise estimate from the CASA sensitivity calculator
        sensitivity = self._do_sensitivity()
        LOG.info('Sensitivity estimate from CASA %s', sensitivity)

        # Choose cleaning method.
        if inputs.hm_masking == 'centralquarter':
            if inputs.hm_cleaning == 'manual':
                threshold = inputs.threshold
            elif inputs.hm_cleaning == 'sensitivity':
                raise Exception, 'sensitivity threshold not yet implemented'
            elif inputs.hm_cleaning == 'rms':
                threshold = '%sJy' % (inputs.tlimit * sensitivity)
            sequence_manager = ImageCentreThresholdSequence(
                gridder = inputs.gridder, threshold=threshold,
                sensitivity = sensitivity, niter=inputs.niter,
                pblimit_image = self.pblimit_image,
                pblimit_cleanmask = self.pblimit_cleanmask)

        elif inputs.hm_masking == 'psfiter':
            sequence_manager = IterativeSequence(
                maxncleans=inputs.maxncleans,
                sensitivity=sensitivity)

        elif inputs.hm_masking == 'psfiter2':
            sequence_manager = IterativeSequence2(
                maxncleans=inputs.maxncleans,
                sensitivity=sensitivity)

        result = self._do_iterative_imaging(
            sequence_manager=sequence_manager, result=result)

        # except Exception, e:
        #     raise Exception, '%s/%s/SpW%s Iterative imaging error: %s' % (
        #         inputs.intent, inputs.field, inputs.spw, str(e))

        return result

    def analyse(self, result):
        # Perform QA here if this is a sub-task
        context = self.inputs.context
        pipelineqa.registry.do_qa(context, result)

        return result

    def _do_iterative_imaging(self, sequence_manager, result):

        context = self.inputs.context
        inputs = self.inputs

        # TODO: For inputs.specmode=='cont' get continuum frequency ranges from
        # dirty cubes if no lines.dat is defined
        #if (inputs.specmode == 'cont') and ("no lines.dat") ...
            # Make dirty cubes, run detection algorithm on each of them

        # Check if a matching 'cont' image exists for continuum subtraction.
        # NOTE: For Cycle 3 we use 'mfs' images due to possible
        #       inaccuracies in the nterms=2 cont images.
        cont_image_name = ''
        if (('TARGET' in inputs.intent) and (inputs.specmode == 'cube')):
            imlist = self.inputs.context.sciimlist.get_imlist()
            for iminfo in imlist[::-1]:
                if ((iminfo['sourcetype'] == 'TARGET') and \
                    (iminfo['sourcename'] == inputs.field) and \
                    (iminfo['specmode'] == 'mfs') and \
                    (inputs.spw in iminfo['spwlist'].split(','))):
                    cont_image_name = iminfo['imagename'][:iminfo['imagename'].rfind('.image')]
                    cont_image_name = cont_image_name.replace('.pbcor', '')
                    break

            if (cont_image_name != ''):
                LOG.info('Using %s for continuum subtraction.' % (os.path.basename(cont_image_name)))
            else:
                LOG.warning('Could not find any matching continuum image. Skipping continuum subtraction.')

        # Do continuum subtraction for target cubes
        # NOTE: This currently needs to be done as a separate step.
        #       In the future the subtraction will be handled
        #       on-the-fly in tclean.
        if (cont_image_name != ''):
            self._do_continuum(cont_image_name = cont_image_name, mode = 'sub')

        # Compute the dirty image
        LOG.info('Compute the dirty image')
        iter = 0
        result = self._do_clean(iter=iter, stokes='I', cleanmask='', niter=0,
                                threshold='0.0mJy',
                                sensitivity=sequence_manager.sensitivity,
                                result=None)

        # Give the result to the sequence_manager for analysis
        model_sum, cleaned_rms, non_cleaned_rms, residual_max, residual_min,\
            rms2d, image_max = sequence_manager.iteration_result(iter=0,
                    multiterm = result.multiterm, psf = result.psf, model = result.model,
                    restored = result.image, residual = result.residual,
                    flux = result.flux, cleanmask=None, threshold = None,
                    pblimit_image = self.pblimit_image,
                    pblimit_cleanmask = self.pblimit_cleanmask)

        LOG.info('Dirty image stats')
        LOG.info('    Rms %s', non_cleaned_rms)
        LOG.info('    Residual max %s', residual_max)
        LOG.info('    Residual min %s', residual_min)

        iterating = True
        iter = 1
        while iterating:
            # Create the name of the next clean mask from the root of the 
            # previous residual image.
            rootname, ext = os.path.splitext(result.residual)
            rootname, ext = os.path.splitext(rootname)
            new_cleanmask = '%s.iter%s.cleanmask' % (rootname, iter)
            try:
                shutil.rmtree(new_cleanmask)
            except OSError:
                pass

            # perform an iteration.
            seq_result = sequence_manager.iteration(new_cleanmask)

            # Check the iteration status.
            if not seq_result.iterating:
                break

            # Determine the cleaning threshold
            threshold = seq_result.threshold

            LOG.info('Iteration %s: Clean control parameters' % iter)
            LOG.info('    Mask %s', new_cleanmask)
            LOG.info('    Threshold %s', seq_result.threshold)
            LOG.info('    Niter %s', seq_result.niter)

            result = self._do_clean(iter=iter, stokes='I',
                    cleanmask=new_cleanmask, niter=seq_result.niter,
                    threshold=threshold,
                    sensitivity=sequence_manager.sensitivity, result=result)

            # Give the result to the clean 'sequencer'
            model_sum, cleaned_rms, non_cleaned_rms, residual_max, residual_min, rms2d, image_max = sequence_manager.iteration_result(
                iter=iter, multiterm=result.multiterm, psf=result.psf, model=result.model, restored=result.image, residual=result.residual,
                flux=result.flux, cleanmask=new_cleanmask, threshold=seq_result.threshold, pblimit_image = self.pblimit_image,
                pblimit_cleanmask = self.pblimit_cleanmask)

            # Keep RMS for QA
            result.set_rms(non_cleaned_rms)

            LOG.info('Clean image iter %s stats' % iter)
            LOG.info('    Clean rms %s', cleaned_rms)
            LOG.info('    Nonclean rms %s', non_cleaned_rms)
            LOG.info('    Residual max %s', residual_max)
            LOG.info('    Residual min %s', residual_min)

            # Up the iteration counter
            iter += 1

        # Re-add continuum so that the MS is unchanged afterwards.
        if (cont_image_name != ''):
            if (inputs.subcontms == False):
                self._do_continuum(cont_image_name = cont_image_name, mode = 'add')
            else:
                LOG.warn('Not re-adding continuum model. MS is modified !')

        return result

    def _do_noise_estimate (self, stokes):
        """Compute a noise estimate from the specified stokes image.
        """
        # Compute the dirty Q or V image.
        try:
            LOG.info("Compute the 'noise' image")
            result = self._do_clean (iter=0, stokes=stokes,
                                     cleanmask='', niter=0, threshold='0.0mJy', sensitivity=0.0, result=None)
            if result.empty():
                raise Exception, '%s/%s/SpW%s Error creating Stokes %s noise image' % (
                    self.inputs.intent, self.inputs.field, self.inputs.spw, stokes)
        except Exception, e:
            raise Exception, '%s/%s/SpW%s Error creating Stokes %s noise image: %s' % (
                self.inputs.intent, self.inputs.field, self.inputs.spw, stokes, str(e))

        # Create the base sequence manager and use it to get noise stats
        sequence_manager = BaseCleanSequence()
        model_sum, cleaned_rms, non_cleaned_rms, residual_max, \
        residual_min, rms2d, image_max = \
            sequence_manager.iteration_result(iter=0, multiterm=result.multiterm,
                                              psf=result.psf, model=result.model, restored=result.image,
                                              residual=result.residual, flux=result.flux, cleanmask=None)

        LOG.info('Noise image stats')
        LOG.info('    Rms %s', non_cleaned_rms)
        LOG.info('    Residual max %s', residual_max)
        LOG.info('    Residual min %s', residual_min)

        return model_sum, cleaned_rms, non_cleaned_rms, residual_max, \
               residual_min, rms2d, image_max

    def _do_sensitivity(self):
        """Compute sensitivity estimate using CASA."""

        context = self.inputs.context
        inputs = self.inputs
        field = inputs.field
        spw = inputs.spw

        # Calculate sensitivities
        sensitivities = []
        if inputs.specmode in ['mfs','cont']:
            specmode = 'mfs'
        elif inputs.specmode == 'cube':
            specmode = 'cube'
        else:
            raise Exception, 'Unknown specmode "%s"' % (inputs.specmode)
        for ms in context.observing_run.measurement_sets:
            for intSpw in [int(s) for s in spw.split(',')]:
                with casatools.ImagerReader(ms.name) as imTool:
                    try:
                        # TODO: Add scan selection
                        imTool.selectvis(spw=intSpw, field=field)
                        imTool.defineimage(mode=specmode, spw=intSpw,
                                           cellx=inputs.cell[0], celly=inputs.cell[0],
                                           nx=inputs.imsize[0], ny=inputs.imsize[1])
                        # TODO: Mosaic switch needed ?
                        imTool.weight(type=inputs.weighting, robust=inputs.robust)

                        result = imTool.apparentsens()
                        if (result[1] != 0.0):
                            sensitivities.append(result[1])
                    except Exception as e:
                        # Simply pass as this could be a case of a source not
                        # being present in the MS.
                        pass

        if (len(sensitivities) != 0):
            sensitivity = 1.0 / numpy.sqrt(numpy.sum(1.0 / numpy.array(sensitivities)**2))
        else:
            defaultSensitivity = 0.001
            LOG.warning('Exception in calculating sensitivity. Assuming %g Jy/beam.' % (defaultSensitivity))
            sensitivity = defaultSensitivity

        if inputs.specmode == 'cube':
            if inputs.nchan != -1:
                sensitivity *= numpy.sqrt(inputs.nchan)
            else:
                qaTool = casatools.quanta
                ms = context.observing_run.measurement_sets[0]
                spwDesc = ms.get_spectral_window(spw)
                if qaTool.convert(inputs.width, 'GHz')['value'] == 0.0:
                    sensitivity *= numpy.sqrt(len(spwDesc.channels))
                else:
                    min_frequency = float(spwDesc.min_frequency.to_units(measures.FrequencyUnits.GIGAHERTZ))
                    max_frequency = float(spwDesc.max_frequency.to_units(measures.FrequencyUnits.GIGAHERTZ))
                    sensitivity *= numpy.sqrt(abs((max_frequency - min_frequency) /
                                                  qaTool.convert(inputs.width, 'GHz')['value']))

        return sensitivity

    def _do_continuum(self, cont_image_name, mode):
        """
        Add/Subtract continuum model.
        """

        context = self.inputs.context
        inputs = self.inputs

        LOG.info('Predict continuum model.')

        # Predict continuum model
        job = casa_tasks.tclean(vis=inputs.vis, imagename='%s.I.cont_%s_pred' %
                (os.path.basename(inputs.imagename), mode),
                spw=inputs.spw,
                intent='*TARGET*',
                scan='', specmode='mfs', gridder=inputs.gridder,
                pblimit=self.pblimit_image, niter=0,
                threshold='0.0mJy', deconvolver=inputs.deconvolver,
                interactive=False, outframe=inputs.outframe, nchan=inputs.nchan,
                start=inputs.start, width=inputs.width, imsize=inputs.imsize,
                cell=inputs.cell, phasecenter=inputs.phasecenter,
                stokes='I',
                weighting=inputs.weighting, robust=inputs.robust,
                npixels=inputs.npixels,
                restoringbeam=inputs.restoringbeam, uvrange=inputs.uvrange,
                mask='', startmodel=cont_image_name,
                savemodel='modelcolumn',
                parallel=False)
        self._executor.execute(job)

        # Add/subtract continuum model
        if mode == 'sub':
            LOG.info('Subtract continuum model.')
        else:
            LOG.info('Add continuum model.')
        # Need to use MS tool to get the proper data selection.
        # The uvsub task does not provide this.
        cms = casatools.ms
        for vis in inputs.vis:
            ms_info = context.observing_run.get_ms(vis)

            field_ids = []
            field_infos = ms_info.get_fields()
            for i in xrange(len(field_infos)):
                if ((field_infos[i].name == inputs.field) and ('TARGET' in field_infos[i].intents)):
                    field_ids.append(str(i))
            field_ids = reduce(lambda x, y: '%s,%s' % (x, y), field_ids)

            scan_numbers = []
            for scan_info in ms_info.scans:
                if ((inputs.field in [f.name for f in scan_info.fields]) and ('TARGET' in scan_info.intents)):
                    scan_numbers.append(scan_info.id)
            scan_numbers = reduce(lambda x, y: '%s,%s' % (x, y), scan_numbers)

            if mode == 'sub':
                LOG.info('Subtracting continuum for %s.' % (os.path.basename(vis)))
            else:
                LOG.info('Adding continuum for %s.' % (os.path.basename(vis)))
            cms.open(vis, nomodify=False)
            cms.msselect({'field': field_ids, 'scan': scan_numbers, 'spw': inputs.spw})
            if mode == 'sub':
                cms.uvsub()
            else:
                cms.uvsub(reverse=True)
            cms.close()

    def _do_clean(self, iter, stokes, cleanmask, niter, threshold, sensitivity, result):
        """
        Do basic cleaning.
        """
        inputs = self.inputs

        parallel = mpihelpers.parse_mpi_input_parameter(inputs.parallel)

        clean_inputs = cleanbase.CleanBase.Inputs(inputs.context,
                                                  output_dir=inputs.output_dir,
                                                  vis=inputs.vis,
                                                  imagename=inputs.imagename,
                                                  intent=inputs.intent,
                                                  field=inputs.field,
                                                  spw=inputs.spw,
                                                  spwsel=inputs.spwsel,
                                                  uvrange=inputs.uvrange,
                                                  specmode=inputs.specmode,
                                                  gridder=inputs.gridder,
                                                  deconvolver=inputs.deconvolver,
                                                  outframe=inputs.outframe,
                                                  imsize=inputs.imsize,
                                                  cell=inputs.cell,
                                                  phasecenter=inputs.phasecenter,
                                                  nchan=inputs.nchan,
                                                  start=inputs.start,
                                                  width=inputs.width,
                                                  stokes=stokes,
                                                  weighting=inputs.weighting,
                                                  robust=inputs.robust,
                                                  noise=inputs.noise,
                                                  npixels=inputs.npixels,
                                                  restoringbeam=inputs.restoringbeam,
                                                  iter=iter,
                                                  mask=cleanmask,
                                                  niter=niter,
                                                  threshold=threshold,
                                                  sensitivity=sensitivity,
                                                  pblimit=self.pblimit_image,
                                                  result=result,
                                                  parallel=parallel)
        clean_task = cleanbase.CleanBase(clean_inputs)

        return self._executor.execute(clean_task)

    # Remove pointing table.
    def _empty_pointing_table(self):
        # Concerned that simply renaming things directly 
        # will corrupt the table cache, so do things using only the
        # table tool.
        for vis in self.inputs.vis:
            with casatools.TableReader('%s/POINTING' % vis,
                                       nomodify=False) as table:
                # make a copy of the table
                LOG.debug('Making copy of POINTING table')
                copy = table.copy('%s/POINTING_COPY' % vis, valuecopy=True)
                LOG.debug('Removing all POINTING table rows')
                table.removerows(range(table.nrows()))
                copy.done()

                # Restore pointing table
    def _restore_pointing_table(self):
        for vis in self.inputs.vis:
            # restore the copy of the POINTING table
            with casatools.TableReader('%s/POINTING_COPY' % vis,
                                       nomodify=False) as table:
                LOG.debug('Copying back into POINTING table')
                original = table.copy('%s/POINTING' % vis, valuecopy=True)
                original.done()
