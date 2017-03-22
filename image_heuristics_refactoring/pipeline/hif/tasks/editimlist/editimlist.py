from __future__ import absolute_import
import os

import pipeline.infrastructure as infrastructure
import pipeline.infrastructure.basetask as basetask
from .resultobjects import EditimlistResult
from pipeline.hif.tasks.makeimlist.cleantarget import CleanTarget

LOG = infrastructure.get_logger(__name__)

class EditimlistInputs(basetask.StandardInputs):
    @basetask.log_equivalent_CASA_call
    def __init__(self, context, output_dir=None, vis=None,
                 cell=None,
                 cyclefactor=None,
                 cycleniter=None,
                 deconvolver=None,
                 editmode=None,
                 field=None,
                 imagename=None,
                 imsize=None,
                 intent=None,
                 gridder=None,
                 mask=None,
                 nbin=None,
                 nchan=None,
                 niter=None,
                 nterms=None,
                 parameter_file=None,
                 phasecenter=None,
                 robust=None,
                 scales=None,
                 specmode=None,
                 spw=None,
                 start=None,
                 stokes=None,
                 uvtaper=None,
                 uvrange=None,
                 width=None,
                 ):

        self._init_properties(vars())

        keys_to_consider = ('field', 'intent', 'spw', 'cell', 'deconvolver', 'imsize',
                            'phasecenter', 'specmode', 'gridder', 'imagename', 'scales',
                            'start', 'width', 'nbin', 'nchan', 'uvrange', 'stokes', 'nterms',
                            'robust', 'uvtaper', 'niter', 'cyclefactor', 'cycleniter', 'mask')
        self.keys_to_change = []
        for key in keys_to_consider:
            # print key, eval(key)
            if self.__dict__[key] is not None:
                self.keys_to_change.append(key)

# tell the infrastructure to give us mstransformed data when possible by
# registering our preference for imaging measurement sets
basetask.ImagingMeasurementSetsPreferred.register(EditimlistInputs)


class Editimlist(basetask.StandardTaskTemplate):
    Inputs = EditimlistInputs

    def is_multi_vis_task(self):
        return True

    def prepare(self):

        # this python class will produce a list of images to be calculated.
        inputs = self.inputs

        # if a file is given, read whatever parameters are defined in the file
        if inputs.parameter_file:
            if os.access(inputs.parameter_file, os.R_OK):
                with open(inputs.parameter_file) as parfile:
                    for line in parfile:
                        if line.startswith('#') or '=' not in line:
                            continue
                        parameter, value = line.partition('=')[::2]
                        parameter = parameter.strip()
                        value = value.strip()
                        exec ('inputs.' + parameter + '=' + value)
                        inputs.keys_to_change.append(parameter)
            else:
                LOG.error('Input parameter file is not readable: {fname}'.format(fname=inputs.parameter_file))

        # now construct the list of imaging command parameter lists that must
        # be run to obtain the required images
        result = EditimlistResult()

        target = dict()
        inputs.editmode = 'add' if not inputs.editmode else inputs.editmode

        if not isinstance(inputs.field, list):
            LOG.error('field type for hif_editimlist is not a list.')

        ms = self.inputs.context.observing_run.get_ms(inputs.vis[0])
        fieldnames = []
        for fid in inputs.field:
            if isinstance(fid, int):
                fieldobj = ms.get_fields(field_id=int(fid))
                fieldnames.append(fieldobj[0].name)
            else:
                fieldnames.append(fid)

        for targetname in fieldnames:
            if inputs.editmode == 'add':
                target = CleanTarget()
                target['deconvolver'] = '' if not inputs.deconvolver else None
                target['scales'] = [0] if not inputs.scales else None
                target['robust'] = 1.0 if not inputs.robust else None
                target['uvtaper'] = [] if not inputs.uvtaper else None
                target['niter'] = 20000 if not inputs.niter else None
                target['cycleniter'] = -1 if not inputs.cycleniter else None
                target['cyclefactor'] = 3.0 if not inputs.cyclefactor else None
                target['mask'] = '' if not inputs.mask else None
                target['specmode'] = 'cont' if not inputs.specmode else None
                target['field'] = targetname
                target['imagename'] = targetname
                inputsdict = inputs.__dict__
                for parameter in inputsdict.keys():
                    if inputsdict[parameter] and parameter not in ('field', 'imagename') and \
                            not parameter.startswith('_') and (parameter in target):
                        inputspar_value = eval('inputs.' + parameter)
                        # print(parameter + '=' + str(inputspar_value))
                        cmd = "target['" + parameter + "'] = inputs." + parameter
                        # print(cmd)
                        exec cmd
            elif inputs.editmode == 'edit':
                for parameter in inputs.keys_to_change:
                    if parameter is not None and parameter not in ('editmode', 'field', 'imagename'):
                        # inputspar_value = eval('inputs.' + parameter)
                        # print(parameter + '=' + str(inputspar_value))
                        cmd = 'target["' + parameter + '"] = inputs.' + parameter
                        # print(cmd)
                        exec cmd

            result.add_target(target)

        return result

    def analyse(self, result):
        return result

