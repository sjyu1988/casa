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
import task_hifa_fluxdb
def hifa_fluxdb(vis=[''], pipelinemode='automatic', dryrun=False, acceptresults=True):

        """Connect to flux calibrator database

Connect to the ALMA flux calibrator database


Keyword arguments:

pipelinemode -- The pipeline operating mode. In 'automatic' mode the pipeline
   determines the values of all context defined pipeline inputs
   automatically.  In interactive mode the user can set the pipeline
   context defined parameters manually.  In 'getinputs' mode the user
   can check the settings of all pipeline parameters without running
   the task.
   default: 'automatic'.



vis -- List of input visibility files
    default: none; example: vis='ngc5921.ms'



-- Pipeline task execution modes

dryrun -- Run the commands (True) or generate the commands to be run but
   do not execute (False).
   default: True

acceptresults -- Add the results of the task to the pipeline context (True) or
   reject them (False).
   default: True

Output:

results -- If pipeline mode is 'getinputs' then None is returned. Otherwise
    the results object for the pipeline task is returned.

Description

Connect to the ALMA flux calibrator database

Issues

Example

1. Connect to the ALMA flux calibrator database

    hifa_fluxdb()



        """
        if type(vis)==str: vis=[vis]

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['vis'] = vis
        mytmp['pipelinemode'] = pipelinemode
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hifa/cli/"
	trec = casac.utils().torecord(pathname+'hifa_fluxdb.xml')

        casalog.origin('hifa_fluxdb')
        if trec.has_key('hifa_fluxdb') and casac.utils().verify(mytmp, trec['hifa_fluxdb']) :
	    result = task_hifa_fluxdb.hifa_fluxdb(vis, pipelinemode, dryrun, acceptresults)

	else :
	  result = False
        return result
