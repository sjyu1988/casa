#
# This file was generated using xslt from its XML file
#
# Copyright 2014, Associated Universities Inc., Washington DC
#
import sys
import os
#from casac import *
import casac
import string
import time
import inspect
import gc
import numpy
from casa_stack_manip import stack_frame_find
from odict import odict
from types import *
from task_hsd_applycal import hsd_applycal
class hsd_applycal_cli_:
    __name__ = "hsd_applycal"
    rkey = None
    i_am_a_casapy_task = None
    # The existence of the i_am_a_casapy_task attribute allows help()
    # (and other) to treat casapy tasks as a special case.

    def __init__(self) :
       self.__bases__ = (hsd_applycal_cli_,)
       self.__doc__ = self.__call__.__doc__

       self.parameters={'vis':None, 'field':None, 'intent':None, 'spw':None, 'antenna':None, 'applymode':None, 'calwt':None, 'flagbackup':None, 'pipelinemode':None, 'dryrun':None, 'acceptresults':None, }


    def result(self, key=None):
	    #### and add any that have completed...
	    return None


    def __call__(self, vis=None, field=None, intent=None, spw=None, antenna=None, applymode=None, calwt=None, flagbackup=None, pipelinemode=None, dryrun=None, acceptresults=None, ):

        """Apply the calibration(s) to the data

	Detailed Description:

Apply the calibration to the data.

	Arguments :
		vis:	List of input MeasurementSets
		   Default Value: 

		field:	Set of data selection field names or ids
		   Default Value: 

		intent:	Set of data selection observing intents
		   Default Value: 

		spw:	Set of data selection spectral window/channels
		   Default Value: 

		antenna:	Set of data selection antenna ids
		   Default Value: 

		applymode:	Calibration mode: ""="calflagstrict","calflag","calflagstrict","trial","flagonly","flagonlystrict", or "calonly"
		   Default Value: 
		   Allowed Values:
				
				calflag
				calflagstrict
				trial
				flagonly
				flagonlystrict
				calonly

		calwt:	Calibrate the weights as well as the data
		   Default Value: True

		flagbackup:	Backup the flags before the apply
		   Default Value: True

		pipelinemode:	The pipeline operating mode
		   Default Value: automatic
		   Allowed Values:
				automatic
				interactive
				getinputs

		dryrun:	Run task (False) or display the command(True)
		   Default Value: False

		acceptresults:	Automatically accept results into the context
		   Default Value: True

	Returns: void

	Example :


Apply precomputed calibrations to the data.

---- pipeline parameter arguments which can be set in any pipeline mode

applymode -- Calibration apply mode 
    ''='calflagstrict': calibrate data and apply flags from solutions using
        the strict flagging convention
    'trial': report on flags from solutions, dataset entirely unchanged
    'flagonly': apply flags from solutions only, data not calibrated
    'calonly': calibrate data only, flags from solutions NOT applied
    'calflagstrict':
    'flagonlystrict':same as above except flag spws for which calibration is
        unavailable in one or more tables (instead of allowing them to pass
        uncalibrated and unflagged)
   default: ''


pipelinemode -- The pipeline operating mode. In 'automatic' mode the pipeline
   determines the values of all context defined pipeline inputs automatically.
   In interactive mode the user can set the pipeline context defined parameters
   manually.  In 'getinputs' mode the user can check the settings of all
   pipeline parameters without running the task.
   default: 'automatic'.


---- pipeline context defined parameter arguments which can be set only in
'interactive mode'

vis -- The list of input MeasurementSets. Defaults to the list of MeasurementSets
    in the pipeline context.
    default: []
    example: ['X227.ms'] 

field -- A string containing the list of field names or field ids to which
    the calibration will be applied. Defaults to all fields in the pipeline
    context.
    default: '' 
    example: '3C279', '3C279, M82'

intent -- A string containing a the list of intents against which the 
    selected fields will be matched. Defaults to all supported intents
    in the pipeline context.
    default: ''
    example: '*TARGET*'

spw -- The list of spectral windows and channels to which the calibration
    will be applied. Defaults to all science windows in the pipeline
    context.
    default: '' 
    example: '17', '11, 15'

antenna -- The list of antennas to which the calibration will be applied.
    Defaults to all antennas. Not currently supported.


--- pipeline task execution modes
dryrun -- Run the commands (True) or generate the commands to be run but
   do not execute (False).
   default: False

acceptresults -- Add the results of the task to the pipeline context (True) or
   reject them (False).
   default: True

Output:

results -- If pipeline mode is 'getinputs' then None is returned. Otherwise
    the results object for the pipeline task is returned

Description

hif_applycal applies the precomputed calibration tables stored in the pipeline
context to the set of visibility files using predetermined field and
spectral window maps and default values for the interpolation schemes.

Users can interact with the pipeline calibration state using the tasks
hif_export_calstate and hif_import_calstate.

Issues

There is some discussion about the appropriate values of calwt. Given 
properly scaled data, the correct value should be the CASA default of True.
However at the current time ALMA is suggesting that calwt be set to True for
applying observatory calibrations, e.g. antenna postions, WVR, and system
temperature corrections, and to False for applying instrument calibrations,
e.g. bandpass, gain, and flux.


Examples

1. Apply the calibration to the target data

hsd_applycal (intent='TARGET')


        """
	if not hasattr(self, "__globals__") or self.__globals__ == None :
           self.__globals__=stack_frame_find( )
	#casac = self.__globals__['casac']
	casalog = self.__globals__['casalog']
	casa = self.__globals__['casa']
	#casalog = casac.casac.logsink()
        self.__globals__['__last_task'] = 'hsd_applycal'
        self.__globals__['taskname'] = 'hsd_applycal'
        ###
        self.__globals__['update_params'](func=self.__globals__['taskname'],printtext=False,ipython_globals=self.__globals__)
        ###
        ###
        #Handle globals or user over-ride of arguments
        #
        if type(self.__call__.func_defaults) is NoneType:
            function_signature_defaults={}
	else:
	    function_signature_defaults=dict(zip(self.__call__.func_code.co_varnames[1:],self.__call__.func_defaults))
	useLocalDefaults = False

        for item in function_signature_defaults.iteritems():
                key,val = item
                keyVal = eval(key)
                if (keyVal == None):
                        #user hasn't set it - use global/default
                        pass
                else:
                        #user has set it - use over-ride
			if (key != 'self') :
			   useLocalDefaults = True

	myparams = {}
	if useLocalDefaults :
	   for item in function_signature_defaults.iteritems():
	       key,val = item
	       keyVal = eval(key)
	       exec('myparams[key] = keyVal')
	       self.parameters[key] = keyVal
	       if (keyVal == None):
	           exec('myparams[key] = '+ key + ' = self.itsdefault(key)')
		   keyVal = eval(key)
		   if(type(keyVal) == dict) :
                      if len(keyVal) > 0 :
		         exec('myparams[key] = ' + key + ' = keyVal[len(keyVal)-1][\'value\']')
		      else :
		         exec('myparams[key] = ' + key + ' = {}')

        else :
            print ''

            myparams['vis'] = vis = self.parameters['vis']
            myparams['field'] = field = self.parameters['field']
            myparams['intent'] = intent = self.parameters['intent']
            myparams['spw'] = spw = self.parameters['spw']
            myparams['antenna'] = antenna = self.parameters['antenna']
            myparams['applymode'] = applymode = self.parameters['applymode']
            myparams['calwt'] = calwt = self.parameters['calwt']
            myparams['flagbackup'] = flagbackup = self.parameters['flagbackup']
            myparams['pipelinemode'] = pipelinemode = self.parameters['pipelinemode']
            myparams['dryrun'] = dryrun = self.parameters['dryrun']
            myparams['acceptresults'] = acceptresults = self.parameters['acceptresults']

        if type(vis)==str: vis=[vis]
        if type(calwt)==bool: calwt=[calwt]

	result = None

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['vis'] = vis
        mytmp['field'] = field
        mytmp['intent'] = intent
        mytmp['spw'] = spw
        mytmp['antenna'] = antenna
        mytmp['applymode'] = applymode
        mytmp['calwt'] = calwt
        mytmp['flagbackup'] = flagbackup
        mytmp['pipelinemode'] = pipelinemode
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hsd/cli/"
	trec = casac.casac.utils().torecord(pathname+'hsd_applycal.xml')

        casalog.origin('hsd_applycal')
	try :
          #if not trec.has_key('hsd_applycal') or not casac.casac.utils().verify(mytmp, trec['hsd_applycal']) :
	    #return False

          casac.casac.utils().verify(mytmp, trec['hsd_applycal'], True)
          scriptstr=['']
          saveinputs = self.__globals__['saveinputs']
          if type(self.__call__.func_defaults) is NoneType:
              saveinputs=''
          else:
              saveinputs('hsd_applycal', 'hsd_applycal.last', myparams, self.__globals__,scriptstr=scriptstr)
          tname = 'hsd_applycal'
          spaces = ' '*(18-len(tname))
          casalog.post('\n##########################################'+
                       '\n##### Begin Task: ' + tname + spaces + ' #####')
          if (casa['state']['telemetry-enabled']):
              casalog.poststat('Begin Task: ' + tname)
          if type(self.__call__.func_defaults) is NoneType:
              casalog.post(scriptstr[0]+'\n', 'INFO')
          else :
              casalog.post(scriptstr[1][1:]+'\n', 'INFO')
          result = hsd_applycal(vis, field, intent, spw, antenna, applymode, calwt, flagbackup, pipelinemode, dryrun, acceptresults)
          if (casa['state']['telemetry-enabled']):
              casalog.poststat('End Task: ' + tname)
          casalog.post('##### End Task: ' + tname + '  ' + spaces + ' #####'+
                       '\n##########################################')

	except Exception, instance:
          if(self.__globals__.has_key('__rethrow_casa_exceptions') and self.__globals__['__rethrow_casa_exceptions']) :
             raise
          else :
             #print '**** Error **** ',instance
	     tname = 'hsd_applycal'
             casalog.post('An error occurred running task '+tname+'.', 'ERROR')
             pass
	casalog.origin('')

        gc.collect()
        return result
#
#
#
#    def paramgui(self, useGlobals=True, ipython_globals=None):
#        """
#        Opens a parameter GUI for this task.  If useGlobals is true, then any relevant global parameter settings are used.
#        """
#        import paramgui
#	if not hasattr(self, "__globals__") or self.__globals__ == None :
#           self.__globals__=stack_frame_find( )
#
#        if useGlobals:
#	    if ipython_globals == None:
#                myf=self.__globals__
#            else:
#                myf=ipython_globals
#
#            paramgui.setGlobals(myf)
#        else:
#            paramgui.setGlobals({})
#
#        paramgui.runTask('hsd_applycal', myf['_ip'])
#        paramgui.setGlobals({})
#
#
#
#
    def defaults(self, param=None, ipython_globals=None, paramvalue=None, subparam=None):
	if not hasattr(self, "__globals__") or self.__globals__ == None :
           self.__globals__=stack_frame_find( )
        if ipython_globals == None:
            myf=self.__globals__
        else:
            myf=ipython_globals

        a = odict()
        a['applymode']  = ''
        a['pipelinemode']  = 'automatic'

        a['pipelinemode'] = {
                    0:{'value':'automatic'}, 
                    1:odict([{'value':'interactive'}, {'vis':[]}, {'field':''}, {'intent':''}, {'spw':''}, {'antenna':''}, {'calwt':[True]}, {'flagbackup':True}, {'dryrun':False}, {'acceptresults':True}]), 
                    2:odict([{'value':'getinputs'}, {'vis':[]}, {'field':''}, {'intent':''}, {'spw':''}, {'antenna':''}, {'calwt':[True]}, {'flagbackup':True}])}

### This function sets the default values but also will return the list of
### parameters or the default value of a given parameter
        if(param == None):
                myf['__set_default_parameters'](a)
        elif(param == 'paramkeys'):
                return a.keys()
        else:
            if(paramvalue==None and subparam==None):
               if(a.has_key(param)):
                  return a[param]
               else:
                  return self.itsdefault(param)
            else:
               retval=a[param]
               if(type(a[param])==dict):
                  for k in range(len(a[param])):
                     valornotval='value'
                     if(a[param][k].has_key('notvalue')):
                        valornotval='notvalue'
                     if((a[param][k][valornotval])==paramvalue):
                        retval=a[param][k].copy()
                        retval.pop(valornotval)
                        if(subparam != None):
                           if(retval.has_key(subparam)):
                              retval=retval[subparam]
                           else:
                              retval=self.itsdefault(subparam)
		     else:
                        retval=self.itsdefault(subparam)
               return retval


#
#
    def check_params(self, param=None, value=None, ipython_globals=None):
      if ipython_globals == None:
          myf=self.__globals__
      else:
          myf=ipython_globals
#      print 'param:', param, 'value:', value
      try :
         if str(type(value)) != "<type 'instance'>" :
            value0 = value
            value = myf['cu'].expandparam(param, value)
            matchtype = False
            if(type(value) == numpy.ndarray):
               if(type(value) == type(value0)):
                  myf[param] = value.tolist()
               else:
                  #print 'value:', value, 'value0:', value0
                  #print 'type(value):', type(value), 'type(value0):', type(value0)
                  myf[param] = value0
                  if type(value0) != list :
                     matchtype = True
            else :
               myf[param] = value
            value = myf['cu'].verifyparam({param:value})
            if matchtype:
               value = False
      except Exception, instance:
         #ignore the exception and just return it unchecked
         myf[param] = value
      return value
#
#
    def description(self, key='hsd_applycal', subkey=None):
        desc={'hsd_applycal': 'Apply the calibration(s) to the data',
               'vis': 'List of input MeasurementSets',
               'field': 'Set of data selection field names or ids',
               'intent': 'Set of data selection observing intents',
               'spw': 'Set of data selection spectral window/channels',
               'antenna': 'Set of data selection antenna ids',
               'applymode': 'Calibration mode: ""="calflagstrict","calflag","calflagstrict","trial","flagonly","flagonlystrict", or "calonly"',
               'calwt': 'Calibrate the weights as well as the data',
               'flagbackup': 'Backup the flags before the apply',
               'pipelinemode': 'The pipeline operating mode',
               'dryrun': 'Run task (False) or display the command(True)',
               'acceptresults': 'Automatically accept results into the context',

              }

#
# Set subfields defaults if needed
#

        if(desc.has_key(key)) :
           return desc[key]

    def itsdefault(self, paramname) :
        a = {}
        a['vis']  = ['']
        a['field']  = ''
        a['intent']  = ''
        a['spw']  = ''
        a['antenna']  = ''
        a['applymode']  = ''
        a['calwt']  = [True]
        a['flagbackup']  = True
        a['pipelinemode']  = 'automatic'
        a['dryrun']  = False
        a['acceptresults']  = True

        #a = sys._getframe(len(inspect.stack())-1).f_globals

        if self.parameters['pipelinemode']  == 'interactive':
            a['vis'] = []
            a['field'] = ''
            a['intent'] = ''
            a['spw'] = ''
            a['antenna'] = ''
            a['calwt'] = [True]
            a['flagbackup'] = True
            a['dryrun'] = False
            a['acceptresults'] = True

        if self.parameters['pipelinemode']  == 'getinputs':
            a['vis'] = []
            a['field'] = ''
            a['intent'] = ''
            a['spw'] = ''
            a['antenna'] = ''
            a['calwt'] = [True]
            a['flagbackup'] = True

        if a.has_key(paramname) :
	      return a[paramname]
hsd_applycal_cli = hsd_applycal_cli_()
