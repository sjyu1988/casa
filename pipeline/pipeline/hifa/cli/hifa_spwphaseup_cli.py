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
from task_hifa_spwphaseup import hifa_spwphaseup
class hifa_spwphaseup_cli_:
    __name__ = "hifa_spwphaseup"
    rkey = None
    i_am_a_casapy_task = None
    # The existence of the i_am_a_casapy_task attribute allows help()
    # (and other) to treat casapy tasks as a special case.

    def __init__(self) :
       self.__bases__ = (hifa_spwphaseup_cli_,)
       self.__doc__ = self.__call__.__doc__

       self.parameters={'vis':None, 'caltable':None, 'field':None, 'intent':None, 'spw':None, 'hm_spwmapmode':None, 'maxnarrowbw':None, 'minfracmaxbw':None, 'samebb':None, 'phasesnr':None, 'bwedgefrac':None, 'hm_nantennas':None, 'maxfracflagged':None, 'combine':None, 'refant':None, 'minblperant':None, 'minsnr':None, 'pipelinemode':None, 'dryrun':None, 'acceptresults':None, }


    def result(self, key=None):
	    #### and add any that have completed...
	    return None


    def __call__(self, vis=None, caltable=None, field=None, intent=None, spw=None, hm_spwmapmode=None, maxnarrowbw=None, minfracmaxbw=None, samebb=None, phasesnr=None, bwedgefrac=None, hm_nantennas=None, maxfracflagged=None, combine=None, refant=None, minblperant=None, minsnr=None, pipelinemode=None, dryrun=None, acceptresults=None, ):

        """Compute phase calibration spw map and per spw phase offsets

	Detailed Description:

The spw map for phase calibration is computed. Phase offsets as a function of spectral
window are computed using high signal to noise calibration observations. 

Previous calibrations are applied on the fly.


	Arguments :
		vis:	List of input MeasurementSets
		   Default Value: 

		caltable:	List of output caltables
		   Default Value: 

		field:	Set of data selection field names or ids
		   Default Value: 

		intent:	Set of data selection observing intents
		   Default Value: 

		spw:	Set of data selection spectral window/channels
		   Default Value: 

		hm_spwmapmode:	The spw mapping mode
		   Default Value: auto
		   Allowed Values:
				auto
				combine
				simple
				default

		maxnarrowbw:	The maximum bandwidth defining narrow spectral windows
		   Default Value: 300MHz

		minfracmaxbw:	The minimum fraction of the maximum bandpass for spw matching
		   Default Value: 0.8

		samebb:	Match within the same baseband if possible ?
		   Default Value: True

		phasesnr:	The minimum snr for triggering spw combination in auto spw mapping mode
		   Default Value: 32.0

		bwedgefrac:	The fraction of the bandwidth edge that is flagged
		   Default Value: 0.03125

		hm_nantennas:	The antenna selection heuristic 
		   Default Value: all
		   Allowed Values:
				all
				unflagged

		maxfracflagged:	The maximum fraction of data flagged per antenna
		   Default Value: 0.90

		combine:	Data axes which to combine for solve (scan, spw, and/or field)
		   Default Value: 

		refant:	Reference antenna names
		   Default Value: 

		minblperant:	Minimum baselines per antenna required for solve
		   Default Value: 4

		minsnr:	Reject solutions below this SNR
		   Default Value: 3.0

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

hm_spwmapmode -- The spectral window mapping mode. The options are: 'auto',
    'combine', 'simple', and 'default'. In 'auto' mode hifa_spwphaseup estimates
    the SNR of the phase calibrator observations and uses these estimates to
    choose between 'combine' mode (low SNR) and  'default' mode (high SNR). In
    combine mode all spectral windows are combined and mapped to one spectral
    window. In 'simple' mode narrow spectral windows are mapped to wider ones 
    sing an algorithm defined by 'maxnarrowbw', 'minfracmaxbw', and 'samebb'.
    In 'default' mode the spectral window map defaults to the standard one
    to one mapping.
    default: 'auto'
    example: hm_spwmapmode='combine' 

maxnarrowbw -- The maximum bandwidth defining narrow spectral windows. Values
    must be in CASA compatible frequency units.
    default: '300MHz'
    example: maxnarrowbw=''

minfracmaxbw -- The minimum fraction of the maximum bandwidth in the set of
    spws to use for matching.
    default: 0.8
    example: minfracmaxbw=0.75

samebb -- Match within the same baseband if possible ?
    default: True
    example: samebb=False

phasesnr -- The required gaincal solution signal to noise
    default: 32.0
    example: phaseupsnr = 20.0

bwedgefrac -- The fraction of the bandwidth edges that is flagged
    default: 0.03125
    example: bwedgefrac = 0.0

hm_nantennas -- The heuristics for determines the number of antennas to use
    in the signal to noise estimate. The options are 'all' and 'unflagged'.
    The 'unflagged' options is not currently supported.
    default: 'all'
    example: hm_nantennas='unflagged'

maxfracflagged -- The maximum fraction of an antenna that can be flagged
    before its is excluded from the signal to noise estimate.
    default: 0.90
    example: maxfracflagged=0.80


combine -- Data axes to combine for solving. Options are  '','scan','spw',field'
    or any comma-separated combination.
    default: ''
    example: combine=''

minblperant -- Minimum number of baselines required per antenna for each solve
    Antennas with fewer baselines are excluded from solutions. 
    default: 4
    example: minblperant=2

minsnr -- Solutions below this SNR are rejected. 
    default: 3.0

---- pipeline context defined parameter arguments which can be set only in
'interactive mode'

vis -- The list of input MeasurementSets. Defaults to the list of MeasurementSets
    specified in the pipeline context
    default: ''
    example: ['M82A.ms', 'M82B.ms'] 

caltable -- The list of output calibration tables. Defaults to the standard
    pipeline naming convention.
    default: ''
    example: ['M82.gcal', 'M82B.gcal']

field -- The list of field names or field ids for which phase offset solutions are
    to be computed. Defaults to all fields with the default intent.
    default: '' 
    example: '3C279', '3C279, M82'

intent -- A string containing a comma delimited list of intents against
    which the the selected fields are matched. Defaults to the BANDPASS
    observations/
    default: '' 
    example:  intent='PHASE'

spw -- The list of spectral windows and channels for which gain solutions are
    computed. Defaults to all the science spectral windows.
    default: '' 
    example: '13,15'

refant -- Reference antenna name(s) in priority order. Defaults to most recent
    values set in the pipeline context.  If no reference antenna is defined in
    the pipeline context the CASA defaults are used.
    default: '' 
    example: refant='DV01', refant='DV05,DV07'

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

hif_spwphaseup performs tow functions
    o determines the spectral window mapping mode for the phase vs time
      calibrations and computes spectral window map that will be used to
      apply those calibrations
    o computes the per spectral window phase offset table that will be
      applied to the data to remove mean phase differences between
      the spectral windows

If hm_spwmapmode = 'auto' the spectral window map is computed using the
following algorithm

o estimate the per spectral window per scan signal to noise ratio of the phase
  calibrator observations
o if the signal to noise of any single phase calibration spectral window is less
  than the value of 'phasesnr' hm_spwmapmode defaults to 'combine'
o if all phase calibrator spectral windows meet the low  signal to noise criterion
  then hm_spwmapmode defaults to default'
o if the phase calibrator signal to noise values cannot be computed for any reason,
  for example there is no flux information, then hm_spwmapmode defaults to 'combine'

If hm_spwmapmode = 'combine' hifa_spwphaseup maps all the science windows to a single
science spectral window. For example if the list of science spectral windows is
[9, 11, 13, 15] then all the science spectral windows in the data will be combined and
mapped to the science window 9 in the combined phase vs time calibration table.

If hm_spwmapmode = 'simple', a mapping from narrow science to wider science
spectral windows is computed using the following algorithms:

o construct a list of the bandwidths of all the science spectral windows
o determine the maximum bandwidth in this list maxbandwidth
o for each science spectral window  with bandwidth less than maxbandwidth
    o construct a list of spectral windows with bandwidths greater than
      minfracmaxbw * maxbandwidth
    o select the spectral window in this list whose band center most closely
      matches the band center of the narrow spectral window
    o preferentially match within the same baseband if samebb is True


If hm_spwmapmode = 'default' the spw mapping is assumed to be one to one.

Phase offsets per spectral window are determined by computing a phase only gain calibration
on the selected data, normally the high signal to noise bandpass calibrator observations,
using the solution interval 'inf'.

At the end of the task the spectral window map and the phase offset calibration table
in the pipeline are stored in the  context for use by later tasks.


Examples

1. Compute the default spectral window map and the per spectral window phase offsets.

hif_spwphaseup()

2. Compute the default spectral window map and the per spectral window phase offsets
   set the spectral window mapping mode to 'simple'.

hif_spwphaseup(hm_spwmapmode='simple')


        """
	if not hasattr(self, "__globals__") or self.__globals__ == None :
           self.__globals__=stack_frame_find( )
	#casac = self.__globals__['casac']
	casalog = self.__globals__['casalog']
	casa = self.__globals__['casa']
	#casalog = casac.casac.logsink()
        self.__globals__['__last_task'] = 'hifa_spwphaseup'
        self.__globals__['taskname'] = 'hifa_spwphaseup'
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
            myparams['caltable'] = caltable = self.parameters['caltable']
            myparams['field'] = field = self.parameters['field']
            myparams['intent'] = intent = self.parameters['intent']
            myparams['spw'] = spw = self.parameters['spw']
            myparams['hm_spwmapmode'] = hm_spwmapmode = self.parameters['hm_spwmapmode']
            myparams['maxnarrowbw'] = maxnarrowbw = self.parameters['maxnarrowbw']
            myparams['minfracmaxbw'] = minfracmaxbw = self.parameters['minfracmaxbw']
            myparams['samebb'] = samebb = self.parameters['samebb']
            myparams['phasesnr'] = phasesnr = self.parameters['phasesnr']
            myparams['bwedgefrac'] = bwedgefrac = self.parameters['bwedgefrac']
            myparams['hm_nantennas'] = hm_nantennas = self.parameters['hm_nantennas']
            myparams['maxfracflagged'] = maxfracflagged = self.parameters['maxfracflagged']
            myparams['combine'] = combine = self.parameters['combine']
            myparams['refant'] = refant = self.parameters['refant']
            myparams['minblperant'] = minblperant = self.parameters['minblperant']
            myparams['minsnr'] = minsnr = self.parameters['minsnr']
            myparams['pipelinemode'] = pipelinemode = self.parameters['pipelinemode']
            myparams['dryrun'] = dryrun = self.parameters['dryrun']
            myparams['acceptresults'] = acceptresults = self.parameters['acceptresults']

        if type(vis)==str: vis=[vis]
        if type(caltable)==str: caltable=[caltable]

	result = None

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['vis'] = vis
        mytmp['caltable'] = caltable
        mytmp['field'] = field
        mytmp['intent'] = intent
        mytmp['spw'] = spw
        mytmp['hm_spwmapmode'] = hm_spwmapmode
        mytmp['maxnarrowbw'] = maxnarrowbw
        mytmp['minfracmaxbw'] = minfracmaxbw
        mytmp['samebb'] = samebb
        mytmp['phasesnr'] = phasesnr
        mytmp['bwedgefrac'] = bwedgefrac
        mytmp['hm_nantennas'] = hm_nantennas
        mytmp['maxfracflagged'] = maxfracflagged
        mytmp['combine'] = combine
        mytmp['refant'] = refant
        mytmp['minblperant'] = minblperant
        mytmp['minsnr'] = minsnr
        mytmp['pipelinemode'] = pipelinemode
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hifa/cli/"
	trec = casac.casac.utils().torecord(pathname+'hifa_spwphaseup.xml')

        casalog.origin('hifa_spwphaseup')
	try :
          #if not trec.has_key('hifa_spwphaseup') or not casac.casac.utils().verify(mytmp, trec['hifa_spwphaseup']) :
	    #return False

          casac.casac.utils().verify(mytmp, trec['hifa_spwphaseup'], True)
          scriptstr=['']
          saveinputs = self.__globals__['saveinputs']
          if type(self.__call__.func_defaults) is NoneType:
              saveinputs=''
          else:
              saveinputs('hifa_spwphaseup', 'hifa_spwphaseup.last', myparams, self.__globals__,scriptstr=scriptstr)
          tname = 'hifa_spwphaseup'
          spaces = ' '*(18-len(tname))
          casalog.post('\n##########################################'+
                       '\n##### Begin Task: ' + tname + spaces + ' #####')
          if (casa['state']['telemetry-enabled']):
              casalog.poststat('Begin Task: ' + tname)
          if type(self.__call__.func_defaults) is NoneType:
              casalog.post(scriptstr[0]+'\n', 'INFO')
          else :
              casalog.post(scriptstr[1][1:]+'\n', 'INFO')
          result = hifa_spwphaseup(vis, caltable, field, intent, spw, hm_spwmapmode, maxnarrowbw, minfracmaxbw, samebb, phasesnr, bwedgefrac, hm_nantennas, maxfracflagged, combine, refant, minblperant, minsnr, pipelinemode, dryrun, acceptresults)
          if (casa['state']['telemetry-enabled']):
              casalog.poststat('End Task: ' + tname)
          casalog.post('##### End Task: ' + tname + '  ' + spaces + ' #####'+
                       '\n##########################################')

	except Exception, instance:
          if(self.__globals__.has_key('__rethrow_casa_exceptions') and self.__globals__['__rethrow_casa_exceptions']) :
             raise
          else :
             #print '**** Error **** ',instance
	     tname = 'hifa_spwphaseup'
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
#        paramgui.runTask('hifa_spwphaseup', myf['_ip'])
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
        a['hm_spwmapmode']  = 'auto'
        a['combine']  = ''
        a['minblperant']  = 4
        a['minsnr']  = 3.0
        a['pipelinemode']  = 'automatic'

        a['hm_spwmapmode'] = {
                    0:odict([{'value':'auto'}, {'phasesnr':32.0}, {'bwedgefrac':0.03125}, {'hm_nantennas':'all'}, {'maxfracflagged':0.90}]), 
                    1:{'value':'combine'}, 
                    2:odict([{'value':'simple'}, {'maxnarrowbw':'300MHz'}, {'minfracmaxbw':0.8}, {'samebb':True}]), 
                    3:{'value':'default'}}
        a['pipelinemode'] = {
                    0:{'value':'automatic'}, 
                    1:odict([{'value':'interactive'}, {'vis':[]}, {'caltable':[]}, {'field':''}, {'intent':''}, {'spw':''}, {'refant':''}, {'dryrun':False}, {'acceptresults':True}]), 
                    2:odict([{'value':'getinputs'}, {'vis':[]}, {'caltable':[]}, {'field':''}, {'intent':''}, {'spw':''}, {'antenna':''}, {'refant':''}])}

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
    def description(self, key='hifa_spwphaseup', subkey=None):
        desc={'hifa_spwphaseup': 'Compute phase calibration spw map and per spw phase offsets',
               'vis': 'List of input MeasurementSets',
               'caltable': 'List of output caltables',
               'field': 'Set of data selection field names or ids',
               'intent': 'Set of data selection observing intents',
               'spw': 'Set of data selection spectral window/channels',
               'hm_spwmapmode': 'The spw mapping mode',
               'maxnarrowbw': 'The maximum bandwidth defining narrow spectral windows',
               'minfracmaxbw': 'The minimum fraction of the maximum bandpass for spw matching',
               'samebb': 'Match within the same baseband if possible ?',
               'phasesnr': 'The minimum snr for triggering spw combination in auto spw mapping mode',
               'bwedgefrac': 'The fraction of the bandwidth edge that is flagged',
               'hm_nantennas': 'The antenna selection heuristic ',
               'maxfracflagged': 'The maximum fraction of data flagged per antenna',
               'combine': 'Data axes which to combine for solve (scan, spw, and/or field)',
               'refant': 'Reference antenna names',
               'minblperant': 'Minimum baselines per antenna required for solve',
               'minsnr': 'Reject solutions below this SNR',
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
        a['caltable']  = ['']
        a['field']  = ''
        a['intent']  = ''
        a['spw']  = ''
        a['hm_spwmapmode']  = 'auto'
        a['maxnarrowbw']  = '300MHz'
        a['minfracmaxbw']  = 0.8
        a['samebb']  = True
        a['phasesnr']  = 32.0
        a['bwedgefrac']  = 0.03125
        a['hm_nantennas']  = 'all'
        a['maxfracflagged']  = 0.90
        a['combine']  = ''
        a['refant']  = ''
        a['minblperant']  = 4
        a['minsnr']  = 3.0
        a['pipelinemode']  = 'automatic'
        a['dryrun']  = False
        a['acceptresults']  = True

        #a = sys._getframe(len(inspect.stack())-1).f_globals

        if self.parameters['hm_spwmapmode']  == 'auto':
            a['phasesnr'] = 32.0
            a['bwedgefrac'] = 0.03125
            a['hm_nantennas'] = 'all'
            a['maxfracflagged'] = 0.90

        if self.parameters['hm_spwmapmode']  == 'simple':
            a['maxnarrowbw'] = '300MHz'
            a['minfracmaxbw'] = 0.8
            a['samebb'] = True

        if self.parameters['pipelinemode']  == 'interactive':
            a['vis'] = []
            a['caltable'] = []
            a['field'] = ''
            a['intent'] = ''
            a['spw'] = ''
            a['refant'] = ''
            a['dryrun'] = False
            a['acceptresults'] = True

        if self.parameters['pipelinemode']  == 'getinputs':
            a['vis'] = []
            a['caltable'] = []
            a['field'] = ''
            a['intent'] = ''
            a['spw'] = ''
            a['antenna'] = ''
            a['refant'] = ''

        if a.has_key(paramname) :
	      return a[paramname]
hifa_spwphaseup_cli = hifa_spwphaseup_cli_()
