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
import task_hifv_solint
def hifv_solint(vis=[''], pipelinemode='automatic', dryrun=False, acceptresults=True, limit_short_solint='', refantignore=''):

        """Determines different solution intervals
The hifv_solint task determines different solution intervals

Keyword arguments:

---- pipeline parameter arguments which can be set in any pipeline mode

vis -- List of visibility data files. These may be ASDMs, tar files of ASDMs,
   MSs, or tar files of MSs, If ASDM files are specified, they will be
   converted  to MS format.
   default: []
   example: vis=['X227.ms', 'asdms.tar.gz']

limit_short_solint -- keyword argument in units of seconds to limit the short solution interval.
    Can be a string or float numerica value in units of seconds of '0.45' or 0.45.
    Can be set to a string value of 'int'.
    default: ''

refantignore -- string list to be ignored as reference antennae.
    default: ''
    Example:  refantignore='ea02,ea03'


pipelinemode -- The pipeline operating mode. In 'automatic' mode the pipeline
   determines the values of all context defined pipeline inputs
   automatically.  In 'interactive' mode the user can set the pipeline
   context defined parameters manually.  In 'getinputs' mode the user
   can check the settings of all pipeline parameters without running
   the task.
   default: 'automatic'.

---- pipeline context defined parameter argument which can be set only in
'interactive mode'


--- pipeline task execution modes

dryrun -- Run the commands (True) or generate the commands to be run but
   do not execute (False).
   default: True

acceptresults -- Add the results of the task to the pipeline context (True) or
   reject them (False).
   default: True

limit_short_solint -- string keyword argument in units of seconds to limit the short solution interval.
    Can be set to a string value of 'int'.
    default: ''

refantignore -- string list to be ignored as reference antennas.
    default: ''
    Example:  refantignore='ea02,ea03'

Output:

results -- If pipeline mode is 'getinputs' then None is returned. Otherwise
   the results object for the pipeline task is returned.


Examples

1. Determines different solution intervals:

hifv_solint()



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
        mytmp['limit_short_solint'] = limit_short_solint
        mytmp['refantignore'] = refantignore
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hifv/cli/"
	trec = casac.utils().torecord(pathname+'hifv_solint.xml')

        casalog.origin('hifv_solint')
        if trec.has_key('hifv_solint') and casac.utils().verify(mytmp, trec['hifv_solint']) :
	    result = task_hifv_solint.hifv_solint(vis, pipelinemode, dryrun, acceptresults, limit_short_solint, refantignore)

	else :
	  result = False
        return result
