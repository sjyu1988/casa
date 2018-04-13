from __future__ import absolute_import

import os
import types

import pipeline.infrastructure as infrastructure
import pipeline.infrastructure.basetask as basetask
import pipeline.infrastructure.casatools as casatools
import pipeline.infrastructure.mpihelpers as mpihelpers
import pipeline.infrastructure.utils as utils
import pipeline.infrastructure.vdp as vdp
from pipeline.infrastructure import casa_tasks
from .resultobjects import TcleanResult

LOG = infrastructure.get_logger(__name__)

# The basic clean tasks classes. Clean performs a single clean run.


class CleanBaseInputs(vdp.StandardInputs):

    # simple properties ------------------------------------------------------------------------------------------------

    datacolumn = vdp.VisDependentProperty(default='')
    deconvolver = vdp.VisDependentProperty(default='')
    cyclefactor = vdp.VisDependentProperty(default=None)
    cycleniter = vdp.VisDependentProperty(default=None)
    field = vdp.VisDependentProperty(default='')
    gridder = vdp.VisDependentProperty(default='')
    imagename = vdp.VisDependentProperty(default='')
    intent = vdp.VisDependentProperty(default='')
    iter = vdp.VisDependentProperty(default=0)
    mask = vdp.VisDependentProperty(default='')
    hm_growiterations = vdp.VisDependentProperty(default=-999)
    hm_lownoisethreshold = vdp.VisDependentProperty(default=-999.0)
    hm_masking = vdp.VisDependentProperty(default='auto')
    hm_minbeamfrac = vdp.VisDependentProperty(default=-999.0)
    hm_negativethreshold = vdp.VisDependentProperty(default=-999.0)
    hm_noisethreshold = vdp.VisDependentProperty(default=-999.0)
    hm_sidelobethreshold = vdp.VisDependentProperty(default=-999.0)
    nchan = vdp.VisDependentProperty(default=-1)
    niter = vdp.VisDependentProperty(default=5000)
    noise = vdp.VisDependentProperty(default='1.0Jy')
    npixels = vdp.VisDependentProperty(default=0)
    nterms = vdp.VisDependentProperty(default=None)
    orig_specmode = vdp.VisDependentProperty(default='')
    outframe = vdp.VisDependentProperty(default='LSRK')
    parallel = vdp.VisDependentProperty(default='automatic')
    pblimit = vdp.VisDependentProperty(default=0.2)
    phasecenter = vdp.VisDependentProperty(default='')
    restoringbeam = vdp.VisDependentProperty(default='common')
    robust = vdp.VisDependentProperty(default=-999.0)
    scales = vdp.VisDependentProperty(default=None)
    sensitivity = vdp.VisDependentProperty(default=None)
    start = vdp.VisDependentProperty(default='')
    stokes = vdp.VisDependentProperty(default='I')
    threshold = vdp.VisDependentProperty(default=None)
    uvrange = vdp.VisDependentProperty(default='')
    uvtaper = vdp.VisDependentProperty(default=None)
    weighting = vdp.VisDependentProperty(default='briggs')
    width = vdp.VisDependentProperty(default='')

    # properties requiring some logic ----------------------------------------------------------------------------------

    @vdp.VisDependentProperty
    def cell(self):
        return []

    @vdp.VisDependentProperty
    def imsize(self):
        return []

    @imsize.convert
    def imsize(self, value):
        if isinstance(value, str) and value.startswith('['):
            temp = value.translate(None, '[]\'')
            temp = temp.split(',')
            return map(int, temp)
        return value

    @vdp.VisDependentProperty
    def specmode(self):
        if 'TARGET' in self.intent:
            return 'cube'
        return 'mfs'

    @vdp.VisDependentProperty
    def spw(self):
        first_ms = self.context.observing_run.measurement_sets[0]
        return ','.join([spw.id for spw in first_ms.get_spectral_windows()])

    @vdp.VisDependentProperty
    def spwsel(self):
        return []

    def __init__(self, context, output_dir=None, vis=None, imagename=None, datacolumn=None, intent=None, field=None,
                 spw=None, spwsel=None, uvrange=None, orig_specmode=None, specmode=None, gridder=None, deconvolver=None,
                 uvtaper=None, nterms=None, cycleniter=None, cyclefactor=None, scales=None, outframe=None, imsize=None,
                 cell=None, phasecenter=None, nchan=None, start=None, width=None, stokes=None, weighting=None,
                 robust=None, noise=None, npixels=None, restoringbeam=None, iter=None, mask=None, hm_masking=None,
                 hm_sidelobethreshold=None, hm_noisethreshold=None, hm_lownoisethreshold=None,
                 hm_negativethreshold=None, hm_minbeamfrac=None, hm_growiterations=None, pblimit=None, niter=None,
                 threshold=None, sensitivity=None, reffreq=None, conjbeams=None, result=None, parallel=None,
                 heuristics=None):
        self.context = context
        self.output_dir = output_dir
        self.vis = vis

        self.imagename = imagename
        self.datacolumn = datacolumn
        self.intent = intent
        self.field = field
        self.spw = spw
        self.spwsel = spwsel
        self.uvrange = uvrange
        self.orig_specmode = orig_specmode
        self.specmode = specmode
        self.gridder = gridder
        self.deconvolver = deconvolver
        self.uvtaper = uvtaper
        self.nterms = nterms
        self.cycleniter = cycleniter
        self.cyclefactor = cyclefactor
        self.scales = scales
        self.outframe = outframe
        self.imsize = imsize
        self.cell = cell
        self.phasecenter = phasecenter
        self.nchan = nchan
        self.start = start
        self.width = width
        self.stokes = stokes
        self.weighting = weighting
        self.robust = robust
        self.noise = noise
        self.npixels = npixels
        self.restoringbeam = restoringbeam
        self.iter = iter
        self.mask = mask

        self.hm_masking = hm_masking
        self.hm_sidelobethreshold = hm_sidelobethreshold
        self.hm_noisethreshold = hm_noisethreshold
        self.hm_lownoisethreshold = hm_lownoisethreshold
        self.hm_negativethreshold = hm_negativethreshold
        self.hm_minbeamfrac = hm_minbeamfrac
        self.hm_growiterations = hm_growiterations

        self.pblimit = pblimit
        self.niter = niter
        self.threshold = threshold
        self.sensitivity = sensitivity
        self.reffreq = reffreq
        self.conjbeams = conjbeams
        self.result = result
        self.parallel = parallel
        self.heuristics = heuristics


class CleanBase(basetask.StandardTaskTemplate):
    Inputs = CleanBaseInputs

    is_multi_vis_task = True

    def prepare(self):
        context = self.inputs.context
        inputs = self.inputs

        # Make sure inputs.vis is a list, even it is one that contains a
        # single measurement set
        if type(inputs.vis) is not types.ListType:
            inputs.vis = [inputs.vis]

        # Set the data column
        targetmslist = [vis for vis in inputs.vis if context.observing_run.get_ms(name=vis).is_imaging_ms]
        if not inputs.datacolumn:
            if len(targetmslist) > 0:
                if inputs.specmode == 'cube':
                    inputs.datacolumn = 'corrected'
                else:
                    inputs.datacolumn = 'data'
            else:
                inputs.datacolumn = 'corrected'

        # Remove MSs that do not contain data for the given field(s)
        scanidlist, visindexlist = inputs.heuristics.get_scanidlist(inputs.vis, inputs.field, inputs.intent)
        inputs.vis = [inputs.vis[i] for i in visindexlist]
        inputs.spwsel = [inputs.spwsel[i] for i in visindexlist]

        # Initialize imaging results structure
        if not inputs.result:
            plotdir = os.path.join(inputs.context.report_dir,
                                   'stage%s' % inputs.context.stage.split('_')[0])
            result = TcleanResult(sourcename=inputs.field,
                                  intent=inputs.intent,
                                  spw=inputs.spw,
                                  orig_specmode=inputs.orig_specmode,
                                  specmode=inputs.specmode,
                                  multiterm=inputs.nterms if inputs.deconvolver=='mtmfs' else None,
                                  plotdir=plotdir, imaging_mode=inputs.heuristics.imaging_mode)
        else:
            result = inputs.result

        try:
            result = self._do_clean_cycle(scanidlist, result, iter=inputs.iter)
        except Exception, e:
            LOG.error('%s/%s/spw%s clean error: %s' % (inputs.field, inputs.intent, inputs.spw, str(e)))
            result.error = '%s/%s/spw%s clean error: %s' % (inputs.field, inputs.intent, inputs.spw, str(e))

        return result

    def analyse(self, result):
        return result

    def _do_clean_cycle (self, scanidlist=None, result=None, iter=1):
        """
        Compute a clean image.
        """
        if scanidlist is None:
            scanidlist = []

        context = self.inputs.context
        inputs = self.inputs

        # Derive names of clean products for this iteration
        old_model_name = result.model
        model_name = '%s.%s.iter%s.model' % (inputs.imagename, inputs.stokes, iter)
        if old_model_name is not None:
            if os.path.exists(old_model_name):
                if (result.multiterm):
                    rename_image(old_name=old_model_name, new_name=model_name, extensions=['.tt%d' % (nterm) for nterm in xrange(result.multiterm)])
                else:
                    rename_image(old_name=old_model_name, new_name=model_name)

        if (inputs.niter == 0):
            image_name = ''
        else:
            image_name = '%s.%s.iter%s.image' % (
                inputs.imagename, inputs.stokes, iter)

        residual_name = '%s.%s.iter%s.residual' % (
            inputs.imagename, inputs.stokes, iter)
        psf_name = '%s.%s.iter%s.psf' % (
            inputs.imagename, inputs.stokes, iter)
        flux_name = '%s.%s.iter%s.pb' % (
            inputs.imagename, inputs.stokes, iter)
        mask_name = '%s.%s.iter%s.mask' % (
            inputs.imagename, inputs.stokes, iter)

        # Starting with CASA 4.7.79 tclean can calculate chanchunks automatically.
        chanchunks = -1

        parallel = all([mpihelpers.parse_mpi_input_parameter(inputs.parallel),
                        'TARGET' in inputs.intent])

        # Need to translate the virtual spw IDs to real ones
        real_spwsel = context.observing_run.get_real_spwsel(inputs.spwsel, inputs.vis)

        # Common tclean parameters
        tclean_job_parameters = {
            'vis':           inputs.vis,
            'imagename':     '%s.%s.iter%s' % (os.path.basename(inputs.imagename), inputs.stokes, iter),
            'datacolumn':    inputs.datacolumn,
            'field':         inputs.field,
            'spw':           real_spwsel,
            'intent':        utils.to_CASA_intent(inputs.ms[0], inputs.intent),
            'specmode':      inputs.specmode if inputs.specmode != 'cont' else 'mfs',
            'gridder':       inputs.gridder,
            'pblimit':       inputs.pblimit,
            'niter':         inputs.niter,
            'threshold':     inputs.threshold,
            'deconvolver':   inputs.deconvolver,
            'interactive':   0,
            'outframe':      inputs.outframe,
            'nchan':         inputs.nchan,
            'start':         inputs.start,
            'width':         inputs.width,
            'imsize':        inputs.imsize,
            'cell':          inputs.cell,
            'phasecenter':   inputs.phasecenter,
            'stokes':        inputs.stokes,
            'weighting':     inputs.weighting,
            'robust':        inputs.robust,
            'npixels':       inputs.npixels,
            'restoringbeam': inputs.restoringbeam,
            'uvrange':       inputs.uvrange,
            'savemodel':     'none',
            'chanchunks':    chanchunks,
            'parallel':      parallel
            }

        if scanidlist not in [[], None]:
            tclean_job_parameters['scan'] = scanidlist

        # Set up masking parameters
        if inputs.hm_masking == 'auto':
            tclean_job_parameters['usemask'] = 'auto-multithresh'

            # get heuristics parameters 
            sidelobethreshold, noisethreshold, lownoisethreshold, negativethreshold, minbeamfrac, growiterations = inputs.heuristics.get_autobox_params(inputs.intent, inputs.specmode)

            # Override individually with manual settings
            if inputs.hm_sidelobethreshold != -999.0:
                tclean_job_parameters['sidelobethreshold'] = inputs.hm_sidelobethreshold
            elif sidelobethreshold is not None:
                tclean_job_parameters['sidelobethreshold'] = sidelobethreshold

            if inputs.hm_noisethreshold != -999.0:
                tclean_job_parameters['noisethreshold'] = inputs.hm_noisethreshold
            elif noisethreshold is not None:
                tclean_job_parameters['noisethreshold'] = noisethreshold

            if inputs.hm_lownoisethreshold != -999.0:
                tclean_job_parameters['lownoisethreshold'] = inputs.hm_lownoisethreshold
            elif lownoisethreshold is not None:
                tclean_job_parameters['lownoisethreshold'] = lownoisethreshold

            if inputs.hm_negativethreshold != -999.0:
                tclean_job_parameters['negativethreshold'] = inputs.hm_negativethreshold
            elif negativethreshold is not None:
                tclean_job_parameters['negativethreshold'] = negativethreshold

            if inputs.hm_minbeamfrac != -999.0:
                tclean_job_parameters['minbeamfrac'] = inputs.hm_minbeamfrac
            elif minbeamfrac is not None:
                tclean_job_parameters['minbeamfrac'] = minbeamfrac

            if inputs.hm_growiterations != -999:
                tclean_job_parameters['growiterations'] = inputs.hm_growiterations
            elif growiterations is not None:
                tclean_job_parameters['growiterations'] = growiterations
        else:
            if (inputs.hm_masking != 'none') and (inputs.mask != ''):
                tclean_job_parameters['usemask'] = 'user'
                tclean_job_parameters['mask'] = inputs.mask

        # Show nterms parameter only if it is used.
        if (result.multiterm):
            tclean_job_parameters['nterms'] = result.multiterm

        # Select whether to restore image
        if (inputs.niter == 0):
            tclean_job_parameters['restoration'] = False
            tclean_job_parameters['pbcor'] = False
        else:
            tclean_job_parameters['restoration'] = True
            tclean_job_parameters['pbcor'] = inputs.heuristics.pb_correction()

        # Re-use products from previous iteration.
        if (iter > 0):
            tclean_job_parameters['restart'] = True
            tclean_job_parameters['calcpsf'] = False
            tclean_job_parameters['calcres'] = False

        # Additional heuristics or task parameters
        if inputs.cyclefactor:
            tclean_job_parameters['cyclefactor'] = inputs.cyclefactor
        else:
            # Call first and assign to variable to avoid calling slow methods twice
            cyclefactor = inputs.heuristics.cyclefactor()
            if cyclefactor:
                tclean_job_parameters['cyclefactor'] = cyclefactor

        if inputs.cycleniter:
            tclean_job_parameters['cycleniter'] = inputs.cycleniter
        else:
            cycleniter = inputs.heuristics.cycleniter()
            if cycleniter:
                tclean_job_parameters['cycleniter'] = cycleniter

        if inputs.scales:
            tclean_job_parameters['scales'] = inputs.scales
        else:
            scales = inputs.heuristics.scales()
            if scales:
                tclean_job_parameters['scales'] = scales

        if inputs.uvrange:
            tclean_job_parameters['uvrange'] = inputs.uvrange
        else:
            uvrange = inputs.heuristics.uvrange()
            if uvrange:
                tclean_job_parameters['uvrange'] = uvrange

        if inputs.uvtaper:
            tclean_job_parameters['uvtaper'] = inputs.uvtaper
        else:
            uvtaper = inputs.heuristics.uvtaper(None)
            if uvtaper:
                tclean_job_parameters['uvtaper'] = uvtaper

        if inputs.reffreq:
            tclean_job_parameters['reffreq'] = inputs.reffreq
        else:
            reffreq = inputs.heuristics.reffreq()
            if reffreq:
                tclean_job_parameters['reffreq'] = reffreq

        if inputs.conjbeams is not None:
            tclean_job_parameters['conjbeams'] = inputs.conjbeams
        else:
            conjbeams = inputs.heuristics.conjbeams()
            if conjbeams is not None:
                tclean_job_parameters['conjbeams'] = conjbeams

        # Up until CASA 5.2 it is necessary to run tclean calls with
        # restoringbeam == 'common' in two steps in HPC mode (CAS-10849).
        if (tclean_job_parameters['parallel'] == True) and \
           (tclean_job_parameters['specmode'] == 'cube') and \
           (tclean_job_parameters['restoration'] == True) and \
           (tclean_job_parameters['restoringbeam'] == 'common'):

            tclean_job_parameters['restoration'] = False
            tclean_job_parameters['restoringbeam'] = ''
            job = casa_tasks.tclean(**tclean_job_parameters)
            tclean_result = self._executor.execute(job)

            tclean_job_parameters['parallel'] = False
            tclean_job_parameters['niter'] = 0
            tclean_job_parameters['restoration'] = True
            tclean_job_parameters['restoringbeam'] = 'common'
            tclean_job_parameters['calcpsf'] = False
            tclean_job_parameters['calcres'] = False
            job = casa_tasks.tclean(**tclean_job_parameters)
            tclean_result2 = self._executor.execute(job)
        else:
            job = casa_tasks.tclean(**tclean_job_parameters)
            tclean_result = self._executor.execute(job)

        pbcor_image_name = '%s.%s.iter%s.image.pbcor' % (inputs.imagename, inputs.stokes, iter)

        if (inputs.niter > 0):
            if tclean_result.has_key('stopcode'):
                # Serial tclean result
                tclean_stopcode = tclean_result['stopcode']
                tclean_iterdone = tclean_result['iterdone']
                tclean_niter = tclean_result['niter']
            else:
                # Parallel tclean result structure is currently (2017-03) different
                tclean_stopcode = max([tclean_result[key][int(key.replace('node',''))]['stopcode'] for key in tclean_result.keys()])
                tclean_iterdone = sum([tclean_result[key][int(key.replace('node',''))]['iterdone'] for key in tclean_result.keys()])
                tclean_niter = max([tclean_result[key][int(key.replace('node',''))]['niter'] for key in tclean_result.keys()])

            LOG.info('tclean used %d iterations' % (tclean_iterdone))
            if ((tclean_stopcode == 1) and (tclean_iterdone >= tclean_niter)):
                result.error = CleanBaseError('tclean reached niter limit. Field: %s SPW: %s' % (inputs.field, inputs.spw), 'Reached niter limit')
                LOG.warning('tclean reached niter limit of %d for %s / spw%s !' % (tclean_niter, utils.dequote(inputs.field), inputs.spw))

            result.set_tclean_stopcode(tclean_stopcode)
            result.set_tclean_stopreason(tclean_stopcode)
            result.set_tclean_iterdone(tclean_iterdone)

            if (tclean_stopcode == 5):
                result.error = CleanBaseError('tclean stopped to prevent divergence. Field: %s SPW: %s' % (inputs.field, inputs.spw), 'tclean stopped to prevent divergence.')
                LOG.warning('tclean stopped to prevent divergence. Field: %s SPW: %s' % (inputs.field, inputs.spw))

        if (iter > 0):
            # Store the model.
            set_miscinfo(name=model_name, spw=inputs.spw, field=inputs.field,
                         type='model', iter=iter, multiterm=result.multiterm,
                         intent=inputs.intent, specmode=inputs.specmode,
                         observing_run=context.observing_run)
            result.set_model(iter=iter, image=model_name)

            # Always set info on the uncorrected image for plotting
            set_miscinfo(name=image_name, spw=inputs.spw, field=inputs.field,
                         type='image', iter=iter, multiterm=result.multiterm,
                         intent=inputs.intent, specmode=inputs.specmode,
                         observing_run=context.observing_run)

            # Store the PB corrected image.
            if os.path.exists('%s' % (pbcor_image_name.replace('.image.pbcor', '.image.tt0.pbcor' if result.multiterm else '.image.pbcor'))):
                set_miscinfo(name=pbcor_image_name, spw=inputs.spw, field=inputs.field,
                             type='pbcorimage', iter=iter, multiterm=result.multiterm,
                             intent=inputs.intent, specmode=inputs.specmode,
                             observing_run=context.observing_run)
                result.set_image(iter=iter, image=pbcor_image_name)
            else:
                result.set_image(iter=iter, image=image_name)

        # Store the residual.
        set_miscinfo(name=residual_name, spw=inputs.spw, field=inputs.field,
                     type='residual', iter=iter, multiterm=result.multiterm,
                     intent=inputs.intent, specmode=inputs.specmode,
                         observing_run=context.observing_run)
        result.set_residual(iter=iter, image=residual_name)

        # Store the PSF.
        set_miscinfo(name=psf_name, spw=inputs.spw, field=inputs.field,
                     type='psf', iter=iter, multiterm=result.multiterm,
                     intent=inputs.intent, specmode=inputs.specmode,
                     observing_run=context.observing_run)
        result.set_psf(image=psf_name)

        # Store the flux image.
        set_miscinfo(name=flux_name, spw=inputs.spw, field=inputs.field,
                     type='flux', iter=iter, multiterm=result.multiterm,
                     intent=inputs.intent, specmode=inputs.specmode,
                     observing_run=context.observing_run)
        result.set_flux(image=flux_name)

        # Make sure mask has path name
        if os.path.exists(inputs.mask):
            set_miscinfo(name=inputs.mask, spw=inputs.spw, field=inputs.field,
                         type='cleanmask', iter=iter,
                         intent=inputs.intent, specmode=inputs.specmode,
                         observing_run=context.observing_run)
            result.set_cleanmask(iter=iter, image=inputs.mask)
        elif os.path.exists(mask_name):
            # Use mask made by tclean
            set_miscinfo(name=mask_name, spw=inputs.spw, field=inputs.field,
                         type='cleanmask', iter=iter,
                         intent=inputs.intent, specmode=inputs.specmode,
                         observing_run=context.observing_run)
            result.set_cleanmask(iter=iter, image=mask_name)

        # Keep threshold and sensitivity for QA and weblog
        result.set_threshold(inputs.threshold)
        result.set_sensitivity(inputs.sensitivity)

        return result

def rename_image(old_name, new_name, extensions=['']):
    """
    Rename an image
    """
    if old_name is not None:
        for extension in extensions:
            with casatools.ImageReader('%s%s' % (old_name, extension)) as image:
                image.rename(name=new_name, overwrite=True)

def set_miscinfo(name, spw=None, field=None, type=None, iter=None, multiterm=None, intent=None, specmode=None, observing_run=None):
    """
    Define miscellaneous image information
    """
    if name != '':
        if (multiterm):
            if (name.find('.image.pbcor') != -1):
                imagename = name.replace('.image.pbcor', '.image.tt0.pbcor')
            else:
                imagename = '%s.tt0' % (name)
        else:
            imagename = name
        with casatools.ImageReader(imagename) as image:
            info = image.miscinfo()
            if imagename is not None:
                filename_components = os.path.basename(imagename).split('.')
                info['nfilnam'] = len(filename_components)
                for i in xrange(len(filename_components)):
                    info['filnam%02d' % (i+1)] = filename_components[i]
            if spw:
                if observing_run is not None:
                    spw_names = ','.join([observing_run.virtual_science_spw_ids.get(int(spw_id), 'N/A') for spw_id in spw.split(',')])
                else:
                    spw_names = 'N/A'
                info['spw'] = '%s / %s' % (spw, spw_names)
            if field:
                # TODO: Find common key calculation. Long VLASS lists cause trouble downstream.
                #       Truncated list may cause duplicates.
                #       Temporarily (?) remove any '"' characters
                tmpfield = field.split(',')[0].replace('"', '')
                info['field'] = tmpfield
            if type:
                info['type'] = type
            if iter is not None:
                info['iter'] = iter
            if intent is not None:
                info['intent'] = intent
            if specmode is not None:
                info['specmode'] = specmode
            image.setmiscinfo(info)


class CleanBaseError(object):

    '''Clean Base Error Class to transfer detailed messages for weblog
       reporting.'''

    def __init__(self, longmsg='', shortmsg=''):
        self.longmsg = longmsg
        self.shortmsg= shortmsg
