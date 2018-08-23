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
from task_hifa_bpsolint import hifa_bpsolint
class hifa_bpsolint_cli_:
    __name__ = "hifa_bpsolint"
    rkey = None
    i_am_a_casapy_task = None
    # The existence of the i_am_a_casapy_task attribute allows help()
    # (and other) to treat casapy tasks as a special case.

    def __init__(self) :
       self.__bases__ = (hifa_bpsolint_cli_,)
       self.__doc__ = self.__call__.__doc__

       self.parameters={'vis':None, 'field':None, 'intent':None, 'spw':None, 'phaseupsnr':None, 'minphaseupints':None, 'evenbpints':None, 'bpsnr':None, 'minbpnchan':None, 'hm_nantennas':None, 'maxfracflagged':None, 'pipelinemode':None, 'dryrun':None, 'acceptresults':None, }


    def result(self, key=None):
	    #### and add any that have completed...
	    return None


    def __call__(self, vis=None, field=None, intent=None, spw=None, phaseupsnr=None, minphaseupints=None, evenbpints=None, bpsnr=None, minbpnchan=None, hm_nantennas=None, maxfracflagged=None, pipelinemode=None, dryrun=None, acceptresults=None, ):

        """Compute optimal bandpass calibration solution intervals

	Detailed Description:

The optimal bandpass phaseup time and frequency solution intervals required to achieve
the required ignal to noise ratio is estimated based on nominal ALMA array characteristics
the meta data associated with the observation.



	Arguments :
		vis:	List of input MeasurementSets
		   Default Value: 

		field:	Set of data selection field names
		   Default Value: 

		intent:	Set of data selection observing intents
		   Default Value: BANDPASS

		spw:	Set of data selection spectral window ids
		   Default Value: 

		phaseupsnr:	The required bandpass phaseup signal to noise
		   Default Value: 20.0

		minphaseupints:	The minimum number of phaseup intervals in the time solution
		   Default Value: 2

		evenbpints:	Force the bandpass frequency solution intervals to be an even number of channels
		   Default Value: False

		bpsnr:	The required bandpass frequency solution signal to noise
		   Default Value: 50.0

		minbpnchan:	The minimum number of channels in the frequency solution
		   Default Value: 8

		hm_nantennas:	The antenna selection heuristic (unsupported) 
		   Default Value: all
		   Allowed Values:
				all
				unflagged

		maxfracflagged:	The maximum fraction of data flagged per antenna (unsupported)
		   Default Value: 0.90

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


Compute the bandpass phaseup time solution interval and bandpass frequency solution interval.

---- pipeline parameter arguments which can be set in any pipeline mode

pipelinemode -- The pipeline operating mode. In 'automatic' mode the pipeline
       determines the values of all context defined pipeline inputs
       automatically.  In interactive mode the user can set the pipeline
       context defined parameters manually.  In 'getinputs' mode the user
       can check the settings of all pipeline parameters without running
       the task.
       default: 'automatic'.

phaseupnsr -- The required phaseup gain time interval solution signal to noise
    default: 20.0
    example: phaseupsnr = 10.0

minphaseupints -- The minimum number of time intervals in the phaseup gain
    solution.
    default: 2
    example: minphaseupints=4

bpnsr -- The required bandpass frequency interval solution signal to noise
    default: 50.0
    example: phaseupsnr = 20.0

minbpnchan -- The minimum number of frequency intervals in the bandpass
    solution.
    default: 8
    example: minbpnchans=16

hm_nantennas -- The heuristics for determines the number of antennas to use
    in the signal to noise estimate. The options are 'all' and 'unflagged'.
    The 'unflagged' options is not currently supported.
    default: 'all'
    example: hm_nantennas='unflagged'

maxfracflagged -- The maximum fraction of an antenna that can be flagged
    before its is excluded from the signal to noise estimate.
    default: 0.90
    example: maxfracflagged=0.80

---- pipeline context defined parameter arguments which can be set only in
'interactive mode'

vis -- The list of input MeasurementSets. Defaults to the list of MeasurementSets
    specified in the pipeline context
    default: ''
    example: ['M82A.ms', 'M82B.ms'] 

field -- The list of field names of sources to be used for signal to noise
    estimation. Defaults to all fields with the standard intent.
    default: '' 
    example: '3C279'

intent -- A string the intent against which the the selected fields are matched.
    Defaults to 'BANDPASS'.
    default: 'BANDPASS' 
    example: intent='PHASE'

spw -- The list of spectral windows and channels for which gain solutions are
    computed. Defaults to all the science spectral windows for which there are
    both 'intent' and TARGET intents.
    default: '' 
    example: '13,15'

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

The phaseup gain time and bandpass frequency intervals are determined as
follows.

o For each data set the list of source(s) to use for bandpass solution signal
  to noise estimation is compiled based on the values of the field, intent,
  and spw parameters. 

o Source fluxes are determined for each spw and source combination.
    o Fluxes in Jy are derived from the pipeline context.
    o Pipeline context fluxes are derived from the online flux calibrator catalog,
      the ASDM, or the user via the flux.csv file.
    o If no fluxes are available the task terminates.

o Atmospheric calibration and observations scans are determined for each spw
  and source combination. 
    o If intent is set to 'PHASE' are there are no atmospheric scans
      associated with the 'PHASE' calibrator, 'TARGET' atmospheric scans
      will be used in stead.
    o If atmospheric scans cannot be associated with any of the spw and
      source combinations the task terminates.

o Science spws are mapped to atmospheric spws for each science spw and
  source combinations.
    o If mappings cannot be determined for any of the spws the task
      terminates

o The median Tsys value for each atmospheric spw and source combination is
  determined from the SYSCAL table. Medians are computed first by channel,
  then by antenna, in order to reduce sensitivity to deviant values.

o The science spw parameters, exposure time(s), and integration time(s) are
  determined.

o The phase up time interval, in time units and number of integrations required
  to meet the phaseupsnr are computed, along with the phaseup sensitivity in mJy
  and the signal to noise per integration.  Nominal Tsys and sensitivity values
  per receiver band provide by the ALMA project are used for this estimate.

o Warnings are issued if estimated phaseup gain time solution would contain fewer
  than minphasupints  solutions

o The frequency interval, in MHz and number of channels required to meet the
  bpsnr are computed, along with the per channel sensitivity in mJy and the
  per channel signal to noise.  Nominal Tsys and sensitivity values per receiver
  band provide by the ALMA project are used for this estimate.

o Warnings are issued if estimated bandpass solution would contain fewer than
  minbpnchan solutions



Examples

1. Estimate the phaseup gain time interval and the bandpass frequency interval
   required to match the desired signal to noise for bandpass solutions.

hifa_bpsolint()


        """
	if not hasattr(self, "__globals__") or self.__globals__ == None :
           self.__globals__=stack_frame_find( )
	#casac = self.__globals__['casac']
	casalog = self.__globals__['casalog']
	casa = self.__globals__['casa']
	#casalog = casac.casac.logsink()
        self.__globals__['__last_task'] = 'hifa_bpsolint'
        self.__globals__['taskname'] = 'hifa_bpsolint'
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
            myparams['phaseupsnr'] = phaseupsnr = self.parameters['phaseupsnr']
            myparams['minphaseupints'] = minphaseupints = self.parameters['minphaseupints']
            myparams['evenbpints'] = evenbpints = self.parameters['evenbpints']
            myparams['bpsnr'] = bpsnr = self.parameters['bpsnr']
            myparams['minbpnchan'] = minbpnchan = self.parameters['minbpnchan']
            myparams['hm_nantennas'] = hm_nantennas = self.parameters['hm_nantennas']
            myparams['maxfracflagged'] = maxfracflagged = self.parameters['maxfracflagged']
            myparams['pipelinemode'] = pipelinemode = self.parameters['pipelinemode']
            myparams['dryrun'] = dryrun = self.parameters['dryrun']
            myparams['acceptresults'] = acceptresults = self.parameters['acceptresults']

        if type(vis)==str: vis=[vis]

	result = None

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['vis'] = vis
        mytmp['field'] = field
        mytmp['intent'] = intent
        mytmp['spw'] = spw
        mytmp['phaseupsnr'] = phaseupsnr
        mytmp['minphaseupints'] = minphaseupints
        mytmp['evenbpints'] = evenbpints
        mytmp['bpsnr'] = bpsnr
        mytmp['minbpnchan'] = minbpnchan
        mytmp['hm_nantennas'] = hm_nantennas
        mytmp['maxfracflagged'] = maxfracflagged
        mytmp['pipelinemode'] = pipelinemode
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hifa/cli/"
	trec = casac.casac.utils().torecord(pathname+'hifa_bpsolint.xml')

        casalog.origin('hifa_bpsolint')
	try :
          #if not trec.has_key('hifa_bpsolint') or not casac.casac.utils().verify(mytmp, trec['hifa_bpsolint']) :
	    #return False

          casac.casac.utils().verify(mytmp, trec['hifa_bpsolint'], True)
          scriptstr=['']
          saveinputs = self.__globals__['saveinputs']
          if type(self.__call__.func_defaults) is NoneType:
              saveinputs=''
          else:
              saveinputs('hifa_bpsolint', 'hifa_bpsolint.last', myparams, self.__globals__,scriptstr=scriptstr)
          tname = 'hifa_bpsolint'
          spaces = ' '*(18-len(tname))
          casalog.post('\n##########################################'+
                       '\n##### Begin Task: ' + tname + spaces + ' #####')
          if (casa['state']['telemetry-enabled']):
              casalog.poststat('Begin Task: ' + tname)
          if type(self.__call__.func_defaults) is NoneType:
              casalog.post(scriptstr[0]+'\n', 'INFO')
          else :
              casalog.post(scriptstr[1][1:]+'\n', 'INFO')
          result = hifa_bpsolint(vis, field, intent, spw, phaseupsnr, minphaseupints, evenbpints, bpsnr, minbpnchan, hm_nantennas, maxfracflagged, pipelinemode, dryrun, acceptresults)
          if (casa['state']['telemetry-enabled']):
              casalog.poststat('End Task: ' + tname)
          casalog.post('##### End Task: ' + tname + '  ' + spaces + ' #####'+
                       '\n##########################################')

	except Exception, instance:
          if(self.__globals__.has_key('__rethrow_casa_exceptions') and self.__globals__['__rethrow_casa_exceptions']) :
             raise
          else :
             #print '**** Error **** ',instance
	     tname = 'hifa_bpsolint'
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
#        paramgui.runTask('hifa_bpsolint', myf['_ip'])
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
        a['phaseupsnr']  = 20.0
        a['minphaseupints']  = 2
        a['evenbpints']  = False
        a['bpsnr']  = 50.0
        a['minbpnchan']  = 8
        a['hm_nantennas']  = 'all'
        a['pipelinemode']  = 'automatic'

        a['hm_nantennas'] = {
                    0:{'value':'all'}, 
                    1:odict([{'value':'unflagged'}, {'maxfracflagged':0.90}])}
        a['pipelinemode'] = {
                    0:{'value':'automatic'}, 
                    1:odict([{'value':'interactive'}, {'vis':[]}, {'field':''}, {'intent':'BANDPASS'}, {'spw':''}, {'dryrun':False}, {'acceptresults':True}]), 
                    2:odict([{'value':'getinputs'}, {'vis':[]}, {'field':''}, {'intent':'BANDPASS'}, {'spw':''}])}

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
    def description(self, key='hifa_bpsolint', subkey=None):
        desc={'hifa_bpsolint': 'Compute optimal bandpass calibration solution intervals',
               'vis': 'List of input MeasurementSets',
               'field': 'Set of data selection field names',
               'intent': 'Set of data selection observing intents',
               'spw': 'Set of data selection spectral window ids',
               'phaseupsnr': 'The required bandpass phaseup signal to noise',
               'minphaseupints': 'The minimum number of phaseup intervals in the time solution',
               'evenbpints': 'Force the bandpass frequency solution intervals to be an even number of channels',
               'bpsnr': 'The required bandpass frequency solution signal to noise',
               'minbpnchan': 'The minimum number of channels in the frequency solution',
               'hm_nantennas': 'The antenna selection heuristic (unsupported) ',
               'maxfracflagged': 'The maximum fraction of data flagged per antenna (unsupported)',
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
        a['intent']  = 'BANDPASS'
        a['spw']  = ''
        a['phaseupsnr']  = 20.0
        a['minphaseupints']  = 2
        a['evenbpints']  = False
        a['bpsnr']  = 50.0
        a['minbpnchan']  = 8
        a['hm_nantennas']  = 'all'
        a['maxfracflagged']  = 0.90
        a['pipelinemode']  = 'automatic'
        a['dryrun']  = False
        a['acceptresults']  = True

        #a = sys._getframe(len(inspect.stack())-1).f_globals

        if self.parameters['hm_nantennas']  == 'unflagged':
            a['maxfracflagged'] = 0.90

        if self.parameters['pipelinemode']  == 'interactive':
            a['vis'] = []
            a['field'] = ''
            a['intent'] = 'BANDPASS'
            a['spw'] = ''
            a['dryrun'] = False
            a['acceptresults'] = True

        if self.parameters['pipelinemode']  == 'getinputs':
            a['vis'] = []
            a['field'] = ''
            a['intent'] = 'BANDPASS'
            a['spw'] = ''

        if a.has_key(paramname) :
	      return a[paramname]
hifa_bpsolint_cli = hifa_bpsolint_cli_()
