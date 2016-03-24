from __future__ import absolute_import

import pipeline.infrastructure.basetask as basetask
from pipeline.infrastructure import casa_tasks
import pipeline.infrastructure as infrastructure


LOG = infrastructure.get_logger(__name__)

# CHECKING FLAGGING OF ALL CALIBRATORS
# use rflag mode of flagdata


class TargetflagInputs(basetask.StandardInputs):
    @basetask.log_equivalent_CASA_call
    def __init__(self, context, vis=None, intents=None, spw=None):
        # set the properties to the values given as input arguments
        self._init_properties(vars())

    @property
    def intents(self):
        if type(self.vis) is types.ListType:
            return self._handle_multiple_vis('intents')

        if self._intents is not None:
            return self._intents

        # return just the unwanted intents that are present in the MS
        # intents_to_flag = set(['POINTING','FOCUS','ATMOSPHERE','SIDEBAND','UNKNOWN', 'SYSTEM_CONFIGURATION'])
        return ''

    @intents.setter
    def intents(self, value):
        self._intents = value


class TargetflagResults(basetask.Results):
    def __init__(self, jobs=[]):
        super(TargetflagResults, self).__init__()

        self.jobs=jobs
        
    def __repr__(self):
        s = 'Targetflag (rflag mode) results:\n'
        for job in self.jobs:
            s += '%s performed. Statistics to follow?' % str(job)
        return s 


class Targetflag(basetask.StandardTaskTemplate):
    Inputs = TargetflagInputs
    
    def prepare(self):
        
        # Default values
        m = self.inputs.context.observing_run.get_ms(self.inputs.vis)
        # corrstring = self.inputs.context.evla['msinfo'][m.name].corrstring
        corrstring = m.get_vla_corrstring()
        
        method_args = {'field'       : '',
                       'correlation' : 'ABS_' + corrstring,
                       'scan'        : '',
                       'intent'      : '',
                       'spw'         : ''}

        rflag_result = self._do_rflag(**method_args)
        
        return TargetflagResults([rflag_result])
    
    def analyse(self, results):
        return results
    
    def _do_rflag(self, field=None, correlation=None, scan=None, intent=None, spw=None):
        
        task_args = {'vis'          : self.inputs.vis,
                     'mode'         : 'rflag',
                     'field'        : field,
                     'correlation'  : correlation,
                     'scan'         : scan,
                     'intent'       : intent,
                     'spw'          : spw,
                     'ntime'        : 'scan',
                     'combinescans' : False,
                     'datacolumn'   : 'corrected',
                     'winsize'      : 3,
                     'timedevscale' : 4.0,
                     'freqdevscale' : 4.0,
                     'action'       : 'apply',
                     'display'      : '',
                     'extendflags'  : False,
                     'flagbackup'   : False,
                     'savepars'     : True}
                     
        job = casa_tasks.flagdata(**task_args)
            
        return self._executor.execute(job)