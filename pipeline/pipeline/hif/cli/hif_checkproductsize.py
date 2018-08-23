#
# This file was generated using xslt from its XML file
#
# Copyright 2009, Associated Universities Inc., Washington DC
#
import sys
import os
from  casac import *
import string
from taskinit import casalog
from taskinit import xmlpath
#from taskmanager import tm
import task_hif_checkproductsize
def hif_checkproductsize(vis=[''], maxcubesize=-1.0, maxcubelimit=-1.0, maxproductsize=-1.0, calcsb=False, parallel='automatic', pipelinemode='automatic', dryrun=False, acceptresults=True):

        """Check imaging product size

Check interferometry imaging product size and try to mitigate to maximum
allowed values. The task implements a mitigation cascade computing the largest
cube size and trying to reduce it below a given limit by adjusting the nbins,
hm_imsize and hm_cell parameters. If this step succeeds, it also checks the
overall imaging product size and if necessary reduces the number of fields to
be imaged.

Keyword arguments:

--- pipeline parameter arguments which can be set in any pipeline mode

pipelinemode -- The pipeline operating mode. In 'automatic' mode the pipeline
       determines the values of all context defined pipeline inputs
       automatically.  In interactive mode the user can set the pipeline
       context defined parameters manually.  In 'getinputs' mode the user
       can check the settings of all pipeline parameters without running
       the task.
       default: 'automatic'.


---- pipeline context defined parameter arguments which can be set only in
'interactive mode'

vis -- The list of input MeasurementSets. Defaults to the list of MeasurementSets
    specified in the h_init or hif_importdata sets.
    example: vis='ngc5921.ms'
             vis=['ngc5921a.ms', ngc5921b.ms', 'ngc5921c.ms']
    default: use all MeasurementSets in the context 

maxcubesize -- Maximum allowed cube size mitigation goal in GB.
    default: -1 (automatic from performance parameters)
    example: 30.0

maxcubelimit -- Maximum allowed cube size mitigation failure limit in GB.
    default: -1 (automatic from performance parameters)
    example: 30.0

maxproductsize -- Maximum allowed product size mitigation goal
                  and failure limit in GB.
    default: -1 (automatic from performance parameters)
    example: 200.0

calcsb -- Force (re-)calculation of sensitivities and beams
            default=False
            Options: False, True

parallel -- use multiple CPU nodes to compute dirty images
    default: \'automatic\'

--- pipeline task execution modes
dryrun -- Run the commands (True) or generate the commands to be run but
   do not execute (False).
   default: False

acceptresults -- Add the results of the task to the pipeline context (True) or
   reject them (False).
   default: True

Output:

results -- If pipeline mode is 'getinputs' then None is returned. Otherwise
    the results object for the pipeline task is returned.


Examples:


        """
        if type(vis)==str: vis=[vis]

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['vis'] = vis
        mytmp['maxcubesize'] = maxcubesize
        mytmp['maxcubelimit'] = maxcubelimit
        mytmp['maxproductsize'] = maxproductsize
        mytmp['calcsb'] = calcsb
        mytmp['parallel'] = parallel
        mytmp['pipelinemode'] = pipelinemode
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hif/cli/"
	trec = casac.utils().torecord(pathname+'hif_checkproductsize.xml')

        casalog.origin('hif_checkproductsize')
        if trec.has_key('hif_checkproductsize') and casac.utils().verify(mytmp, trec['hif_checkproductsize']) :
	    result = task_hif_checkproductsize.hif_checkproductsize(vis, maxcubesize, maxcubelimit, maxproductsize, calcsb, parallel, pipelinemode, dryrun, acceptresults)

	else :
	  result = False
        return result
