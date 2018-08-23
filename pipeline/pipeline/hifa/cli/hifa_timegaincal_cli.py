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
from task_hifa_timegaincal import hifa_timegaincal
class hifa_timegaincal_cli_:
    __name__ = "hifa_timegaincal"
    rkey = None
    i_am_a_casapy_task = None
    # The existence of the i_am_a_casapy_task attribute allows help()
    # (and other) to treat casapy tasks as a special case.

    def __init__(self) :
       self.__bases__ = (hifa_timegaincal_cli_,)
       self.__doc__ = self.__call__.__doc__

       self.parameters={'vis':None, 'calamptable':None, 'offsetstable':None, 'calphasetable':None, 'targetphasetable':None, 'amptable':None, 'field':None, 'intent':None, 'spw':None, 'antenna':None, 'calsolint':None, 'targetsolint':None, 'combine':None, 'refant':None, 'solnorm':None, 'minblperant':None, 'calminsnr':None, 'targetminsnr':None, 'smodel':None, 'pipelinemode':None, 'dryrun':None, 'acceptresults':None, }


    def result(self, key=None):
	    #### and add any that have completed...
	    return None


    def __call__(self, vis=None, calamptable=None, offsetstable=None, calphasetable=None, targetphasetable=None, amptable=None, field=None, intent=None, spw=None, antenna=None, calsolint=None, targetsolint=None, combine=None, refant=None, solnorm=None, minblperant=None, calminsnr=None, targetminsnr=None, smodel=None, pipelinemode=None, dryrun=None, acceptresults=None, ):

        """Determine temporal gains from calibrator observations

	Detailed Description:

The time dependent complex gains for each antenna/spwid are determined
from the raw data (DATA column) divided by the model (MODEL column), for the
specified fields.  The gains are computed independently for each specified
spectral window. One gain solution is computed for the calibrator source
targets and one for the science targets.

Previous calibrations are applied on the fly.


	Arguments :
		vis:	List of input MeasurementSets
		   Default Value: 

		calamptable:	List of diagnostic output amplitude caltables for calibrator targets
		   Default Value: 

		offsetstable:	List of diagnostic output phase offset caltables for calibrator targets
		   Default Value: 

		calphasetable:	List of output phase caltables for calibrator targets
		   Default Value: 

		targetphasetable:	List of output phase caltables for science targets
		   Default Value: 

		amptable:	List of output amp caltables for science targets
		   Default Value: 

		field:	Set of data selection field names or ids
		   Default Value: 

		intent:	Set of data selection observing intents
		   Default Value: 

		spw:	Set of data selection spectral window/channels
		   Default Value: 

		antenna:	Set of data selection antenna ids
		   Default Value: 

		calsolint:	Phase solution interval for calibrator sources
		   Default Value: int

		targetsolint:	Phase solution interval for science target sources
		   Default Value: inf

		combine:	Data axes which to combine for solve (scan, spw, and/or field)
		   Default Value: 

		refant:	Reference antenna names
		   Default Value: 

		solnorm:	Normalize average solution amplitudes to 1.0
		   Default Value: False

		minblperant:	Minimum baselines per antenna required for solve
		   Default Value: 4

		calminsnr:	Reject solutions below this SNR for calibrator solutions
		   Default Value: 2.0

		targetminsnr:	Reject solutions below this SNR for science solutions
		   Default Value: 3.0

		smodel:	Point source Stokes parameters for source model
		   Default Value: 

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


Compute the gain solutions.

---- pipeline parameter arguments which can be set in any pipeline mode

pipelinemode -- The pipeline operating mode. In 'automatic' mode the pipeline
       determines the values of all context defined pipeline inputs
       automatically.  In interactive mode the user can set the pipeline
       context defined parameters manually.  In 'getinputs' mode the user
       can check the settings of all pipeline parameters without running
       the task.
       default: 'automatic'.

calsolint --  Time solution interval in CASA syntax for calibrator source solutions. 
    default: 'int'
    example: 'inf', 'int', '100sec'

targetsolint --  Time solution interval in CASA syntax for target source solutions. 
    default: 'inf'
    example: 'inf', 'int', '100sec'

combine -- Data axes to combine for solving. Options are  '','scan','spw',field'
    or any comma-separated combination.
    default; ''
    example: combine=''

minblperant -- Minimum number of baselines required per antenna for each solve
    Antennas with fewer baselines are excluded from solutions.
    default: 4
    example: minblperant=2

calminsnr -- Solutions below this SNR are rejected for calibrator solutions. 
    default: 2.0

targetminsnr -- Solutions below this SNR are rejected for science target solutions. 
    default: 3.0

---- pipeline context defined parameter arguments which can be set only in
'interactive mode'

vis -- The list of input MeasurementSets. Defaults to the list of MeasurementSets
    specified in the pipeline context
    default: ''
    example: ['M82A.ms', 'M82B.ms'] 

calamptable -- The list of output diagnostic calibration amplitude tables for the
    calibration targets.
    Defaults to the standard pipeline naming convention.
    default: ''
    example: ['M82.gacal', 'M82B.gacal']

offsetstable -- The list of output diagnostic phase offset tables for the
    calibration targets.
    Defaults to the standard pipeline naming convention.
    default: ''
    example: ['M82.offsets.gacal', 'M82B.offsets.gacal']

calphasetable -- The list of output calibration phase tables for the
    calibration targets.
    Defaults to the standard pipeline naming convention.
    default: ''
    example: ['M82.gcal', 'M82B.gcal']

amptable -- The list of output calibration amplitude tables for the
    calibration and science targets.
    Defaults to the standard pipeline naming convention.
    default: ''
    example: ['M82.gcal', 'M82B.gcal']

targetphasetable -- The list of output phase calibration tables for the science targets.
    Defaults to the standard pipeline naming convention.
    default: ''
    example: ['M82.gcal', 'M82B.gcal']

field -- The list of field names or field ids for which gain solutions are
    to be computed. Defaults to all fields with the standard intent.
    default: '' 
    example: '3C279', '3C279, M82'

intent -- A string containing a comma delimited list of intents against
    which the the selected fields are matched. Defaults to the
    equivalent of 'AMPLITUDE,PHASE,BANDPASS'.
    default: '' 
    example: '', 'PHASE'

spw -- The list of spectral windows and channels for which gain solutions are
    computed. Defaults to all science spectral windows.
    default: '' 
    example: '3C279', '3C279, M82'

smodel -- Point source Stokes parameters for source model (experimental)
    Defaults to using standard MODEL_DATA column data.
    default: [] 
    example: [1,0,0,0] (I=1, unpolarized)

refant -- Reference antenna name(s) in priority order. Defaults to most recent
    values set in the pipeline context.  If no reference antenna is defined in
    the pipeline context use the CASA defaults.
    default: '' 
    example: refant='DV01', refant='DV05,DV07'

solnorm -- Normalise the gain solutions
    default: False

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

The complex gains  are derived from the data column (raw data) divided
by the model column (usually set with hif_setjy). The gains are obtained for 
the specified solution intervals, spw combination and field combination.
One gain solution is computed for the science targets and one for
the calibrator targets.

Good candidate reference antennas can be determined using the hif_refant
task.

Previous calibrations that have been stored in the pipeline context are
applied on the fly. Users can interact with these calibrations via the
hif_export_calstate and hif_import_calstate tasks.

Issues


Examples

1. Compute standard per scan gain solutions that will be used to calibrate
the target.

    hifa_timegaincal()



        """
	if not hasattr(self, "__globals__") or self.__globals__ == None :
           self.__globals__=stack_frame_find( )
	#casac = self.__globals__['casac']
	casalog = self.__globals__['casalog']
	casa = self.__globals__['casa']
	#casalog = casac.casac.logsink()
        self.__globals__['__last_task'] = 'hifa_timegaincal'
        self.__globals__['taskname'] = 'hifa_timegaincal'
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
            myparams['calamptable'] = calamptable = self.parameters['calamptable']
            myparams['offsetstable'] = offsetstable = self.parameters['offsetstable']
            myparams['calphasetable'] = calphasetable = self.parameters['calphasetable']
            myparams['targetphasetable'] = targetphasetable = self.parameters['targetphasetable']
            myparams['amptable'] = amptable = self.parameters['amptable']
            myparams['field'] = field = self.parameters['field']
            myparams['intent'] = intent = self.parameters['intent']
            myparams['spw'] = spw = self.parameters['spw']
            myparams['antenna'] = antenna = self.parameters['antenna']
            myparams['calsolint'] = calsolint = self.parameters['calsolint']
            myparams['targetsolint'] = targetsolint = self.parameters['targetsolint']
            myparams['combine'] = combine = self.parameters['combine']
            myparams['refant'] = refant = self.parameters['refant']
            myparams['solnorm'] = solnorm = self.parameters['solnorm']
            myparams['minblperant'] = minblperant = self.parameters['minblperant']
            myparams['calminsnr'] = calminsnr = self.parameters['calminsnr']
            myparams['targetminsnr'] = targetminsnr = self.parameters['targetminsnr']
            myparams['smodel'] = smodel = self.parameters['smodel']
            myparams['pipelinemode'] = pipelinemode = self.parameters['pipelinemode']
            myparams['dryrun'] = dryrun = self.parameters['dryrun']
            myparams['acceptresults'] = acceptresults = self.parameters['acceptresults']

        if type(vis)==str: vis=[vis]
        if type(calamptable)==str: calamptable=[calamptable]
        if type(offsetstable)==str: offsetstable=[offsetstable]
        if type(calphasetable)==str: calphasetable=[calphasetable]
        if type(targetphasetable)==str: targetphasetable=[targetphasetable]
        if type(amptable)==str: amptable=[amptable]
        if type(smodel)==float: smodel=[smodel]

	result = None

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['vis'] = vis
        mytmp['calamptable'] = calamptable
        mytmp['offsetstable'] = offsetstable
        mytmp['calphasetable'] = calphasetable
        mytmp['targetphasetable'] = targetphasetable
        mytmp['amptable'] = amptable
        mytmp['field'] = field
        mytmp['intent'] = intent
        mytmp['spw'] = spw
        mytmp['antenna'] = antenna
        mytmp['calsolint'] = calsolint
        mytmp['targetsolint'] = targetsolint
        mytmp['combine'] = combine
        mytmp['refant'] = refant
        mytmp['solnorm'] = solnorm
        mytmp['minblperant'] = minblperant
        mytmp['calminsnr'] = calminsnr
        mytmp['targetminsnr'] = targetminsnr
        mytmp['smodel'] = smodel
        mytmp['pipelinemode'] = pipelinemode
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hifa/cli/"
	trec = casac.casac.utils().torecord(pathname+'hifa_timegaincal.xml')

        casalog.origin('hifa_timegaincal')
	try :
          #if not trec.has_key('hifa_timegaincal') or not casac.casac.utils().verify(mytmp, trec['hifa_timegaincal']) :
	    #return False

          casac.casac.utils().verify(mytmp, trec['hifa_timegaincal'], True)
          scriptstr=['']
          saveinputs = self.__globals__['saveinputs']
          if type(self.__call__.func_defaults) is NoneType:
              saveinputs=''
          else:
              saveinputs('hifa_timegaincal', 'hifa_timegaincal.last', myparams, self.__globals__,scriptstr=scriptstr)
          tname = 'hifa_timegaincal'
          spaces = ' '*(18-len(tname))
          casalog.post('\n##########################################'+
                       '\n##### Begin Task: ' + tname + spaces + ' #####')
          if (casa['state']['telemetry-enabled']):
              casalog.poststat('Begin Task: ' + tname)
          if type(self.__call__.func_defaults) is NoneType:
              casalog.post(scriptstr[0]+'\n', 'INFO')
          else :
              casalog.post(scriptstr[1][1:]+'\n', 'INFO')
          result = hifa_timegaincal(vis, calamptable, offsetstable, calphasetable, targetphasetable, amptable, field, intent, spw, antenna, calsolint, targetsolint, combine, refant, solnorm, minblperant, calminsnr, targetminsnr, smodel, pipelinemode, dryrun, acceptresults)
          if (casa['state']['telemetry-enabled']):
              casalog.poststat('End Task: ' + tname)
          casalog.post('##### End Task: ' + tname + '  ' + spaces + ' #####'+
                       '\n##########################################')

	except Exception, instance:
          if(self.__globals__.has_key('__rethrow_casa_exceptions') and self.__globals__['__rethrow_casa_exceptions']) :
             raise
          else :
             #print '**** Error **** ',instance
	     tname = 'hifa_timegaincal'
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
#        paramgui.runTask('hifa_timegaincal', myf['_ip'])
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
        a['calsolint']  = 'int'
        a['targetsolint']  = 'inf'
        a['combine']  = ''
        a['minblperant']  = 4
        a['calminsnr']  = 2.0
        a['targetminsnr']  = 3.0
        a['pipelinemode']  = 'automatic'

        a['pipelinemode'] = {
                    0:{'value':'automatic'}, 
                    1:odict([{'value':'interactive'}, {'vis':[]}, {'calamptable':[]}, {'offsetstable':[]}, {'calphasetable':[]}, {'targetphasetable':[]}, {'amptable':[]}, {'field':''}, {'intent':''}, {'spw':''}, {'antenna':''}, {'smodel':[]}, {'solnorm':False}, {'refant':''}, {'dryrun':False}, {'acceptresults':True}]), 
                    2:odict([{'value':'getinputs'}, {'vis':[]}, {'calamptable':[]}, {'offsetstable':[]}, {'calphasetable':[]}, {'targetphasetable':[]}, {'amptable':[]}, {'field':''}, {'intent':''}, {'spw':''}, {'antenna':''}, {'smodel':[]}, {'solnorm':False}, {'refant':''}])}

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
    def description(self, key='hifa_timegaincal', subkey=None):
        desc={'hifa_timegaincal': 'Determine temporal gains from calibrator observations',
               'vis': 'List of input MeasurementSets',
               'calamptable': 'List of diagnostic output amplitude caltables for calibrator targets',
               'offsetstable': 'List of diagnostic output phase offset caltables for calibrator targets',
               'calphasetable': 'List of output phase caltables for calibrator targets',
               'targetphasetable': 'List of output phase caltables for science targets',
               'amptable': 'List of output amp caltables for science targets',
               'field': 'Set of data selection field names or ids',
               'intent': 'Set of data selection observing intents',
               'spw': 'Set of data selection spectral window/channels',
               'antenna': 'Set of data selection antenna ids',
               'calsolint': 'Phase solution interval for calibrator sources',
               'targetsolint': 'Phase solution interval for science target sources',
               'combine': 'Data axes which to combine for solve (scan, spw, and/or field)',
               'refant': 'Reference antenna names',
               'solnorm': 'Normalize average solution amplitudes to 1.0',
               'minblperant': 'Minimum baselines per antenna required for solve',
               'calminsnr': 'Reject solutions below this SNR for calibrator solutions',
               'targetminsnr': 'Reject solutions below this SNR for science solutions',
               'smodel': 'Point source Stokes parameters for source model',
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
        a['calamptable']  = ['']
        a['offsetstable']  = ['']
        a['calphasetable']  = ['']
        a['targetphasetable']  = ['']
        a['amptable']  = ['']
        a['field']  = ''
        a['intent']  = ''
        a['spw']  = ''
        a['antenna']  = ''
        a['calsolint']  = 'int'
        a['targetsolint']  = 'inf'
        a['combine']  = ''
        a['refant']  = ''
        a['solnorm']  = False
        a['minblperant']  = 4
        a['calminsnr']  = 2.0
        a['targetminsnr']  = 3.0
        a['smodel']  = []
        a['pipelinemode']  = 'automatic'
        a['dryrun']  = False
        a['acceptresults']  = True

        #a = sys._getframe(len(inspect.stack())-1).f_globals

        if self.parameters['pipelinemode']  == 'interactive':
            a['vis'] = []
            a['calamptable'] = []
            a['offsetstable'] = []
            a['calphasetable'] = []
            a['targetphasetable'] = []
            a['amptable'] = []
            a['field'] = ''
            a['intent'] = ''
            a['spw'] = ''
            a['antenna'] = ''
            a['smodel'] = []
            a['solnorm'] = False
            a['refant'] = ''
            a['dryrun'] = False
            a['acceptresults'] = True

        if self.parameters['pipelinemode']  == 'getinputs':
            a['vis'] = []
            a['calamptable'] = []
            a['offsetstable'] = []
            a['calphasetable'] = []
            a['targetphasetable'] = []
            a['amptable'] = []
            a['field'] = ''
            a['intent'] = ''
            a['spw'] = ''
            a['antenna'] = ''
            a['smodel'] = []
            a['solnorm'] = False
            a['refant'] = ''

        if a.has_key(paramname) :
	      return a[paramname]
hifa_timegaincal_cli = hifa_timegaincal_cli_()
