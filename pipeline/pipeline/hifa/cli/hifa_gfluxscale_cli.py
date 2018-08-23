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
from task_hifa_gfluxscale import hifa_gfluxscale
class hifa_gfluxscale_cli_:
    __name__ = "hifa_gfluxscale"
    rkey = None
    i_am_a_casapy_task = None
    # The existence of the i_am_a_casapy_task attribute allows help()
    # (and other) to treat casapy tasks as a special case.

    def __init__(self) :
       self.__bases__ = (hifa_gfluxscale_cli_,)
       self.__doc__ = self.__call__.__doc__

       self.parameters={'vis':None, 'reference':None, 'transfer':None, 'refintent':None, 'transintent':None, 'refspwmap':None, 'reffile':None, 'phaseupsolint':None, 'solint':None, 'minsnr':None, 'refant':None, 'hm_resolvedcals':None, 'antenna':None, 'peak_fraction':None, 'pipelinemode':None, 'dryrun':None, 'acceptresults':None, }


    def result(self, key=None):
	    #### and add any that have completed...
	    return None


    def __call__(self, vis=None, reference=None, transfer=None, refintent=None, transintent=None, refspwmap=None, reffile=None, phaseupsolint=None, solint=None, minsnr=None, refant=None, hm_resolvedcals=None, antenna=None, peak_fraction=None, pipelinemode=None, dryrun=None, acceptresults=None, ):

        """Derive flux density scales from standard calibrators

	Detailed Description:

Derive the flux density scale from standard calibrators.

	Arguments :
		vis:	List of input MeasurementSets
		   Default Value: 

		reference:	Reference calibrator field name(s)
		   Default Value: 

		transfer:	Transfer calibrator field name(s)
		   Default Value: 

		refintent:	Observing intent of reference fields
		   Default Value: 

		transintent:	Observing intent of transfer fields
		   Default Value: 

		refspwmap:	Map across spectral window boundaries
		   Default Value: 

		reffile:	Path to file with fluxes for non-solar system calibrators
		   Default Value: 

		phaseupsolint:	Phaseup correction solution interval
		   Default Value: int

		solint:	Amplitude correction solution interval
		   Default Value: inf

		minsnr:	Minimum SNR for gain solutions
		   Default Value: 2.0

		refant:	The name or ID of the reference antenna
		   Default Value: 

		hm_resolvedcals:	The resolved calibrators heuristics method
		   Default Value: automatic
		   Allowed Values:
				automatic
				manual

		antenna:	Antennas to be used in fluxscale
		   Default Value: 

		peak_fraction:	Fraction of peak visibility at uv-distance limit of antennas to be used
		   Default Value: 0.2

		pipelinemode:	The pipeline operating mode
		   Default Value: automatic
		   Allowed Values:
				automatic
				interactive
				getinputs

		dryrun:	Run the task (False) or display commands (True)
		   Default Value: False

		acceptresults:	Automatically accept results into context
		   Default Value: True

	Returns: void

	Example :


Derive flux densities for point source transfer calibrators using flux models
for reference calibrators.

---- pipeline parameter arguments which can be set in any pipeline mode

pipelinemode -- The pipeline operating mode. In 'automatic' mode the pipeline
    determines the values of all context defined pipeline inputs automatically.
    In interactive mode the user can set the pipeline context defined
    parameters manually.  In 'getinputs' mode the users can check the settings
    of all pipeline parameters without running the task.
    default: 'automatic'.

phaseupsolint --  Time solution intervals in CASA syntax for the phase solution.
    default: 'int'
    example: 'inf', 'int', '100sec'

solint --  Time solution intervals in CASA syntax for the amplitude solution.
    default: 'inf'
    example: 'inf', 'int', '100sec'

minsnr -- Minimum signal to noise ratio for gain calibration solutions.
    default: 2.0
    example: 1.5, 0.0

hm_resolvedcals - Heuristics method for handling resolved calibrators. The
    options are 'automatic' and 'manual'. In automatic mode antennas closer
    to the reference antenna than the uv distance where visibilities fall to
    'peak_fraction' of the peak are used. In manual mode the antennas specified
    in 'antenna' are used.

antenna -- A comma delimited string specifying the antenna names or ids to be
    used for the fluxscale determination. Used in hm_resolvedcals='manual' mode. 
    default: ''.
    example: 'DV16,DV07,DA12,DA08'  

peak_fraction -- The limiting UV distance from the reference antenna for antennas
    to be included in the flux calibration. Defined as the point where the 
    calibrator visibilities have fallen to 'peak_fraction' of the peak value. 


---- pipeline context defined parameter arguments which can be set only in
'interactive mode'

vis -- The list of input MeasurementSets. Defaults to the list of MeasurementSets
    specified in the pipeline context
    default: ''
    example: ['M32A.ms', 'M32B.ms']

reference -- A string containing a comma delimited list of  field names
    defining the reference calibrators. Defaults to field names with
    intent '*AMP*'.
    default: ''
    example: 'M82,3C273'

transfer -- A string containing a comma delimited list of  field names
    defining the transfer calibrators. Defaults to field names with
    intent '*PHASE*'.
    default: ''
    example: 'J1328+041,J1206+30'

refintent -- A string containing a comma delimited list of intents 
    used to select the reference calibrators. Defaults to *AMP*.
    default: ''
    example: '', '*AMP*'

refspwmap -- Vector of spectral window ids enabling scaling across
    spectral windows. Defaults to no scaling
    default: []
    example: [1,1,3,3] (4 spws, reference fields in 1 and 3, transfer
             fields in 0,1,2,3

reffile -- Path to a file containing flux densities for calibrators unknown to
    CASA. Values given in this file take precedence over the CASA-derived values
    for all calibrators except solar system calibrators. By default the path is
    set to the CSV file created by hifa_importdata, consisting of catalogue fluxes
    extracted from the ASDM and / or edited by the user.
    default: ''
    example: '', 'working/flux.csv'


transintent -- A string containing a comma delimited list of intents 
    defining the transfer calibrators. Defaults to *PHASE*.
    default: ''
    example: '', '*PHASE*'

refant -- A string specifying the reference antenna(s). By default
    this is read from the context.
    default: ''
    example: 'DV05'

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

Derive flux densities for point source transfer calibrators using flux models
for reference calibrators.

Flux values are determined by:

o computing complex gain phase only solutions for all the science spectral
  windows using the calibrator data selected by the 'reference' and
  'refintent' parameters and the 'transfer' and 'transintent' parameters,
  and the value of the 'phaseupsolint' parameter.

o computing complex amplitude only solutions for all the science spectral
  windows using calibrator data selected with 'reference' and 'refintent'
  parameters and the 'transfer' and 'transintent' parameters, the value
  of the 'solint' parameter. 

o transferring the flux scale from the reference calibrators to the transfer
  calibrators using refspwmap for windows without data in the reference
  calibrators

o extracted the computed flux values from the CASA logs and inserting
  them into the MODEL_DATA column.


Resolved calibrators are handled via antenna selection either automatically,
hm_resolvedcals='automatic' or manually, hm_resolvedcals='manual'. In
the former case antennas closer to the reference antenna than the uv
distance where visibilities fall to 'peak_fraction' of the peak are used.
In manual mode the antennas specified in 'antenna' are used.

Note that the flux corrected calibration table computed internally is
not currently used in later pipeline apply calibration steps. 


Issues

Should we add a spw window selection option here?

The code which extracts the flux scales from the logs needs to be replaced
with code which uses the values returned from the CASA fluxscale task.


Examples

1. Compute flux flux values for the phase calibrator using model data from
   the amplitude calibrator.

   hifa_gfluxscale ()


        """
	if not hasattr(self, "__globals__") or self.__globals__ == None :
           self.__globals__=stack_frame_find( )
	#casac = self.__globals__['casac']
	casalog = self.__globals__['casalog']
	casa = self.__globals__['casa']
	#casalog = casac.casac.logsink()
        self.__globals__['__last_task'] = 'hifa_gfluxscale'
        self.__globals__['taskname'] = 'hifa_gfluxscale'
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
            myparams['reference'] = reference = self.parameters['reference']
            myparams['transfer'] = transfer = self.parameters['transfer']
            myparams['refintent'] = refintent = self.parameters['refintent']
            myparams['transintent'] = transintent = self.parameters['transintent']
            myparams['refspwmap'] = refspwmap = self.parameters['refspwmap']
            myparams['reffile'] = reffile = self.parameters['reffile']
            myparams['phaseupsolint'] = phaseupsolint = self.parameters['phaseupsolint']
            myparams['solint'] = solint = self.parameters['solint']
            myparams['minsnr'] = minsnr = self.parameters['minsnr']
            myparams['refant'] = refant = self.parameters['refant']
            myparams['hm_resolvedcals'] = hm_resolvedcals = self.parameters['hm_resolvedcals']
            myparams['antenna'] = antenna = self.parameters['antenna']
            myparams['peak_fraction'] = peak_fraction = self.parameters['peak_fraction']
            myparams['pipelinemode'] = pipelinemode = self.parameters['pipelinemode']
            myparams['dryrun'] = dryrun = self.parameters['dryrun']
            myparams['acceptresults'] = acceptresults = self.parameters['acceptresults']

        if type(refspwmap)==int: refspwmap=[refspwmap]

	result = None

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['vis'] = vis
        mytmp['reference'] = reference
        mytmp['transfer'] = transfer
        mytmp['refintent'] = refintent
        mytmp['transintent'] = transintent
        mytmp['refspwmap'] = refspwmap
        mytmp['reffile'] = reffile
        mytmp['phaseupsolint'] = phaseupsolint
        mytmp['solint'] = solint
        mytmp['minsnr'] = minsnr
        mytmp['refant'] = refant
        mytmp['hm_resolvedcals'] = hm_resolvedcals
        mytmp['antenna'] = antenna
        mytmp['peak_fraction'] = peak_fraction
        mytmp['pipelinemode'] = pipelinemode
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hifa/cli/"
	trec = casac.casac.utils().torecord(pathname+'hifa_gfluxscale.xml')

        casalog.origin('hifa_gfluxscale')
	try :
          #if not trec.has_key('hifa_gfluxscale') or not casac.casac.utils().verify(mytmp, trec['hifa_gfluxscale']) :
	    #return False

          casac.casac.utils().verify(mytmp, trec['hifa_gfluxscale'], True)
          scriptstr=['']
          saveinputs = self.__globals__['saveinputs']
          if type(self.__call__.func_defaults) is NoneType:
              saveinputs=''
          else:
              saveinputs('hifa_gfluxscale', 'hifa_gfluxscale.last', myparams, self.__globals__,scriptstr=scriptstr)
          tname = 'hifa_gfluxscale'
          spaces = ' '*(18-len(tname))
          casalog.post('\n##########################################'+
                       '\n##### Begin Task: ' + tname + spaces + ' #####')
          if (casa['state']['telemetry-enabled']):
              casalog.poststat('Begin Task: ' + tname)
          if type(self.__call__.func_defaults) is NoneType:
              casalog.post(scriptstr[0]+'\n', 'INFO')
          else :
              casalog.post(scriptstr[1][1:]+'\n', 'INFO')
          result = hifa_gfluxscale(vis, reference, transfer, refintent, transintent, refspwmap, reffile, phaseupsolint, solint, minsnr, refant, hm_resolvedcals, antenna, peak_fraction, pipelinemode, dryrun, acceptresults)
          if (casa['state']['telemetry-enabled']):
              casalog.poststat('End Task: ' + tname)
          casalog.post('##### End Task: ' + tname + '  ' + spaces + ' #####'+
                       '\n##########################################')

	except Exception, instance:
          if(self.__globals__.has_key('__rethrow_casa_exceptions') and self.__globals__['__rethrow_casa_exceptions']) :
             raise
          else :
             #print '**** Error **** ',instance
	     tname = 'hifa_gfluxscale'
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
#        paramgui.runTask('hifa_gfluxscale', myf['_ip'])
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
        a['phaseupsolint']  = 'int'
        a['solint']  = 'inf'
        a['minsnr']  = 2.0
        a['hm_resolvedcals']  = 'automatic'
        a['pipelinemode']  = 'automatic'

        a['pipelinemode'] = {
                    0:{'value':'automatic'}, 
                    1:odict([{'value':'interactive'}, {'vis':[]}, {'reference':''}, {'transfer':''}, {'refintent':''}, {'transintent':''}, {'refspwmap':[]}, {'reffile':''}, {'refant':''}, {'hm_resolvedcals':'automatic'}, {'dryrun':False}, {'acceptresults':True}]), 
                    2:odict([{'value':'getinputs'}, {'vis':[]}, {'reference':''}, {'transfer':''}, {'refintent':''}, {'transintent':''}, {'refspwmap':[]}, {'reffile':''}, {'refant':''}, {'hm_resolvedcals':'automatic'}])}
        a['hm_resolvedcals'] = {
                    0:odict([{'value':'automatic'}, {'peak_fraction':0.2}]), 
                    1:odict([{'value':'manual'}, {'antenna':''}])}

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
    def description(self, key='hifa_gfluxscale', subkey=None):
        desc={'hifa_gfluxscale': 'Derive flux density scales from standard calibrators',
               'vis': 'List of input MeasurementSets',
               'reference': 'Reference calibrator field name(s)',
               'transfer': 'Transfer calibrator field name(s)',
               'refintent': 'Observing intent of reference fields',
               'transintent': 'Observing intent of transfer fields',
               'refspwmap': 'Map across spectral window boundaries',
               'reffile': 'Path to file with fluxes for non-solar system calibrators',
               'phaseupsolint': 'Phaseup correction solution interval',
               'solint': 'Amplitude correction solution interval',
               'minsnr': 'Minimum SNR for gain solutions',
               'refant': 'The name or ID of the reference antenna',
               'hm_resolvedcals': 'The resolved calibrators heuristics method',
               'antenna': 'Antennas to be used in fluxscale',
               'peak_fraction': 'Fraction of peak visibility at uv-distance limit of antennas to be used',
               'pipelinemode': 'The pipeline operating mode',
               'dryrun': 'Run the task (False) or display commands (True)',
               'acceptresults': 'Automatically accept results into context',

              }

#
# Set subfields defaults if needed
#

        if(desc.has_key(key)) :
           return desc[key]

    def itsdefault(self, paramname) :
        a = {}
        a['vis']  = ''
        a['reference']  = ''
        a['transfer']  = ''
        a['refintent']  = ''
        a['transintent']  = ''
        a['refspwmap']  = []
        a['reffile']  = ''
        a['phaseupsolint']  = 'int'
        a['solint']  = 'inf'
        a['minsnr']  = 2.0
        a['refant']  = ''
        a['hm_resolvedcals']  = 'automatic'
        a['antenna']  = ''
        a['peak_fraction']  = 0.2
        a['pipelinemode']  = 'automatic'
        a['dryrun']  = False
        a['acceptresults']  = True

        #a = sys._getframe(len(inspect.stack())-1).f_globals

        if self.parameters['pipelinemode']  == 'interactive':
            a['vis'] = []
            a['reference'] = ''
            a['transfer'] = ''
            a['refintent'] = ''
            a['transintent'] = ''
            a['refspwmap'] = []
            a['reffile'] = ''
            a['refant'] = ''
            a['hm_resolvedcals'] = 'automatic'
            a['dryrun'] = False
            a['acceptresults'] = True

        if self.parameters['pipelinemode']  == 'getinputs':
            a['vis'] = []
            a['reference'] = ''
            a['transfer'] = ''
            a['refintent'] = ''
            a['transintent'] = ''
            a['refspwmap'] = []
            a['reffile'] = ''
            a['refant'] = ''
            a['hm_resolvedcals'] = 'automatic'

        if self.parameters['hm_resolvedcals']  == 'automatic':
            a['peak_fraction'] = 0.2

        if self.parameters['hm_resolvedcals']  == 'manual':
            a['antenna'] = ''

        if a.has_key(paramname) :
	      return a[paramname]
hifa_gfluxscale_cli = hifa_gfluxscale_cli_()
