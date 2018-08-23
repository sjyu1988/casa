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
from task_h_mssplit import h_mssplit
class h_mssplit_cli_:
    __name__ = "h_mssplit"
    rkey = None
    i_am_a_casapy_task = None
    # The existence of the i_am_a_casapy_task attribute allows help()
    # (and other) to treat casapy tasks as a special case.

    def __init__(self) :
       self.__bases__ = (h_mssplit_cli_,)
       self.__doc__ = self.__call__.__doc__

       self.parameters={'vis':None, 'outputvis':None, 'field':None, 'intent':None, 'spw':None, 'datacolumn':None, 'chanbin':None, 'timebin':None, 'replace':None, 'pipelinemode':None, 'dryrun':None, 'acceptresults':None, }


    def result(self, key=None):
	    #### and add any that have completed...
	    return None


    def __call__(self, vis=None, outputvis=None, field=None, intent=None, spw=None, datacolumn=None, chanbin=None, timebin=None, replace=None, pipelinemode=None, dryrun=None, acceptresults=None, ):

        """Select data from calibrated MS(s) to form new MS(s) for imaging

	Detailed Description:


	Arguments :
		vis:	The list of input MeasurementSets
		   Default Value: 

		outputvis:	The list of output split MeasurementSets
		   Default Value: 

		field:	Set of data selection field names or ids, \'\' for all
		   Default Value: 

		intent:	Set of data selection intents, \'\' for all
		   Default Value: 

		spw:	Set of data selection spectral window ids \'\' for all
		   Default Value: 

		datacolumn:	The data columns to process
		   Default Value: data
		   Allowed Values:
				corrected
				data
				model
				data,model,corrected
				float_data
				lag_data
				float_data,data
				lag_data,data
				all

		chanbin:	Channel bin width for spectral averaging
		   Default Value: 1
		   Allowed Values:
				1
				2
				4
				8
				16

		timebin:	Bin width for time averaging
		   Default Value: 0s

		replace:	Remove the parent ms and replace with the split ms
		   Default Value: True

		pipelinemode:	The pipeline operating mode
		   Default Value: automatic
		   Allowed Values:
				automatic
				interactive
				getinputs

		dryrun:	Run the task (False) or display the command(True)
		   Default Value: False

		acceptresults:	Add the results to the pipeline context
		   Default Value: True

	Returns: void

	Example :


Create a list of science target MS(s) for imaging 

Keyword Arguments


pipelinemode -- The pipeline operating mode. In 'automatic' mode the pipeline
    determines the values of all context defined pipeline inputs automatically.
    In 'interactive' mode the user can set the pipeline context defined
    parameters manually.  In 'getinputs' mode the user can check the settings
    of all pipeline parameters without running the task.
    default: 'automatic'.

--- pipeline parameter arguments which can be set in any pipeline mode



---- pipeline context defined parameter arguments which can be set only in
     'interactive mode'

vis -- The list of input MeasurementSets to be transformed. Defaults to the
    list of MeasurementSets specified in the pipeline import data task.
    default '': Split all MeasurementSets in the context. 
    example: 'ngc5921.ms', ['ngc5921a.ms', ngc5921b.ms', 'ngc5921c.ms']

outputvis -- The list of output split MeasurementSets. The output list must
    be the same length as the input list and the output names must be different
    from the input names.
    default '', The output name defaults to <msrootname>_split.ms
    example: 'ngc5921.ms', ['ngc5921a.ms', ngc5921b.ms', 'ngc5921c.ms']

field -- Select fields name(s) or id(s) to split.
    default: '', All fields will be selected
    example: '3C279', '"5795"' Note the double quotes around names which can be interpreted as numbers

intent -- Select intents to split
    default: '', All data is selected.
    example: 'TARGET'

spw -- Select spectral windows to split.
    default: '', All spws are selected
    example: '9', '9,13,15'

datacolumn -- Select spectral windows to split. The standard CASA options are
    supported
    default: 'data', 
    example: 'corrected', 'model'

chanbin -- The channel binning factor. 1 for no binning, otherwise 2, 4, 8, or 16.
    supported
    default: 1, 
    example: 2, 4

timebin -- The time binning factor. '0s' for no binning
    default: '0s' 
    example: '10s' for 10 second binning

replace -- If a split was performed delete the parent MS and remove it from the context.
    default:  True
    example: False

--- pipeline task execution modes
dryrun -- Run the commands (True) or generate the commands to be run but
   do not execute (False).
   default: False

acceptresults -- Add the results of the task to the pipeline context (True) or
   reject them (False).
   default: True

Output

results -- If pipeline mode is 'getinputs' then None is returned. Otherwise
    the results object for the pipeline task is returned.

Description

Create new MeasurementSets for imaging from the corrected column of the input
MeasurementSet. By default all science target data is copied to the new ms. The new
MeasurementSet is not re-indexed to the selected data in the new ms will have the
same source, field, and spw names and ids as it does in the parent ms. 

Issues

TBD
Examples

1. Create a 4X channel smoothed output ms from the input ms 

   h_mssplit(chanbin=4)


        """
	if not hasattr(self, "__globals__") or self.__globals__ == None :
           self.__globals__=stack_frame_find( )
	#casac = self.__globals__['casac']
	casalog = self.__globals__['casalog']
	casa = self.__globals__['casa']
	#casalog = casac.casac.logsink()
        self.__globals__['__last_task'] = 'h_mssplit'
        self.__globals__['taskname'] = 'h_mssplit'
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
            myparams['outputvis'] = outputvis = self.parameters['outputvis']
            myparams['field'] = field = self.parameters['field']
            myparams['intent'] = intent = self.parameters['intent']
            myparams['spw'] = spw = self.parameters['spw']
            myparams['datacolumn'] = datacolumn = self.parameters['datacolumn']
            myparams['chanbin'] = chanbin = self.parameters['chanbin']
            myparams['timebin'] = timebin = self.parameters['timebin']
            myparams['replace'] = replace = self.parameters['replace']
            myparams['pipelinemode'] = pipelinemode = self.parameters['pipelinemode']
            myparams['dryrun'] = dryrun = self.parameters['dryrun']
            myparams['acceptresults'] = acceptresults = self.parameters['acceptresults']

        if type(vis)==str: vis=[vis]
        if type(outputvis)==str: outputvis=[outputvis]

	result = None

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['vis'] = vis
        mytmp['outputvis'] = outputvis
        mytmp['field'] = field
        mytmp['intent'] = intent
        mytmp['spw'] = spw
        mytmp['datacolumn'] = datacolumn
        mytmp['chanbin'] = chanbin
        mytmp['timebin'] = timebin
        mytmp['replace'] = replace
        mytmp['pipelinemode'] = pipelinemode
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/h/cli/"
	trec = casac.casac.utils().torecord(pathname+'h_mssplit.xml')

        casalog.origin('h_mssplit')
	try :
          #if not trec.has_key('h_mssplit') or not casac.casac.utils().verify(mytmp, trec['h_mssplit']) :
	    #return False

          casac.casac.utils().verify(mytmp, trec['h_mssplit'], True)
          scriptstr=['']
          saveinputs = self.__globals__['saveinputs']
          if type(self.__call__.func_defaults) is NoneType:
              saveinputs=''
          else:
              saveinputs('h_mssplit', 'h_mssplit.last', myparams, self.__globals__,scriptstr=scriptstr)
          tname = 'h_mssplit'
          spaces = ' '*(18-len(tname))
          casalog.post('\n##########################################'+
                       '\n##### Begin Task: ' + tname + spaces + ' #####')
          if (casa['state']['telemetry-enabled']):
              casalog.poststat('Begin Task: ' + tname)
          if type(self.__call__.func_defaults) is NoneType:
              casalog.post(scriptstr[0]+'\n', 'INFO')
          else :
              casalog.post(scriptstr[1][1:]+'\n', 'INFO')
          result = h_mssplit(vis, outputvis, field, intent, spw, datacolumn, chanbin, timebin, replace, pipelinemode, dryrun, acceptresults)
          if (casa['state']['telemetry-enabled']):
              casalog.poststat('End Task: ' + tname)
          casalog.post('##### End Task: ' + tname + '  ' + spaces + ' #####'+
                       '\n##########################################')

	except Exception, instance:
          if(self.__globals__.has_key('__rethrow_casa_exceptions') and self.__globals__['__rethrow_casa_exceptions']) :
             raise
          else :
             #print '**** Error **** ',instance
	     tname = 'h_mssplit'
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
#        paramgui.runTask('h_mssplit', myf['_ip'])
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
        a['chanbin']  = 1
        a['timebin']  = '0s'
        a['replace']  = True
        a['pipelinemode']  = 'automatic'

        a['pipelinemode'] = {
                    0:{'value':'automatic'}, 
                    1:odict([{'value':'interactive'}, {'vis':[]}, {'outputvis':[]}, {'field':''}, {'intent':''}, {'spw':''}, {'datacolumn':'data'}, {'dryrun':False}, {'acceptresults':True}]), 
                    2:odict([{'value':'getinputs'}, {'vis':[]}, {'outputvis':[]}, {'field':''}, {'intent':''}, {'spw':''}, {'datacolumn':'data'}])}

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
    def description(self, key='h_mssplit', subkey=None):
        desc={'h_mssplit': 'Select data from calibrated MS(s) to form new MS(s) for imaging',
               'vis': 'The list of input MeasurementSets',
               'outputvis': 'The list of output split MeasurementSets',
               'field': 'Set of data selection field names or ids, \'\' for all',
               'intent': 'Set of data selection intents, \'\' for all',
               'spw': 'Set of data selection spectral window ids \'\' for all',
               'datacolumn': 'The data columns to process',
               'chanbin': 'Channel bin width for spectral averaging',
               'timebin': 'Bin width for time averaging',
               'replace': 'Remove the parent ms and replace with the split ms',
               'pipelinemode': 'The pipeline operating mode',
               'dryrun': 'Run the task (False) or display the command(True)',
               'acceptresults': 'Add the results to the pipeline context',

              }

#
# Set subfields defaults if needed
#

        if(desc.has_key(key)) :
           return desc[key]

    def itsdefault(self, paramname) :
        a = {}
        a['vis']  = ['']
        a['outputvis']  = ['']
        a['field']  = ''
        a['intent']  = ''
        a['spw']  = ''
        a['datacolumn']  = 'data'
        a['chanbin']  = 1
        a['timebin']  = '0s'
        a['replace']  = True
        a['pipelinemode']  = 'automatic'
        a['dryrun']  = False
        a['acceptresults']  = True

        #a = sys._getframe(len(inspect.stack())-1).f_globals

        if self.parameters['pipelinemode']  == 'interactive':
            a['vis'] = []
            a['outputvis'] = []
            a['field'] = ''
            a['intent'] = ''
            a['spw'] = ''
            a['datacolumn'] = 'data'
            a['dryrun'] = False
            a['acceptresults'] = True

        if self.parameters['pipelinemode']  == 'getinputs':
            a['vis'] = []
            a['outputvis'] = []
            a['field'] = ''
            a['intent'] = ''
            a['spw'] = ''
            a['datacolumn'] = 'data'

        if a.has_key(paramname) :
	      return a[paramname]
h_mssplit_cli = h_mssplit_cli_()
