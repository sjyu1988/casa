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
import task_hifv_circfeedpolcal
def hifv_circfeedpolcal(vis=[''], Dterm_solint='2MHz', refantignore='', leakage_poltype='', mbdkcross=True, clipminmax=[0.0,0.25], pipelinemode='automatic', dryrun=False, acceptresults=True):

        """Base circfeedpolcal task
The hifv_circfeedpolcal task

Keyword arguments:

---- pipeline parameter arguments which can be set in any pipeline mode

vis -- List of visibility data files. These may be ASDMs, tar files of ASDMs,
   MSs, or tar files of MSs, If ASDM files are specified, they will be
   converted  to MS format.
   default: []
   example: vis=['X227.ms', 'asdms.tar.gz']

Dterms_solint -- D-terms spectral averaging
    default: 2MHz

refantignore -- string list to be ignored as reference antennas.
    default: ''
    Example:  refantignore='ea02,ea03'

leakage_poltype -- string of poltype to use for override in the first polcal execution
    default: ''   (blank means the task heuristics will decide what poltype to use)

mbdkcross -- run gaincal KCROSS grouped by basenad
    default: False

clipminmax --  Range to use for clipping
    default: [0.0,0.25]

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

Output:

results -- If pipeline mode is 'getinputs' then None is returned. Otherwise
   the results object for the pipeline task is returned.


Examples

1. Basic circfeedpolcal task

   hifv_circfeedpolcal()



        """
        if type(vis)==str: vis=[vis]
        if type(clipminmax)==float: clipminmax=[clipminmax]

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['vis'] = vis
        mytmp['Dterm_solint'] = Dterm_solint
        mytmp['refantignore'] = refantignore
        mytmp['leakage_poltype'] = leakage_poltype
        mytmp['mbdkcross'] = mbdkcross
        mytmp['clipminmax'] = clipminmax
        mytmp['pipelinemode'] = pipelinemode
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hifv/cli/"
	trec = casac.utils().torecord(pathname+'hifv_circfeedpolcal.xml')

        casalog.origin('hifv_circfeedpolcal')
        if trec.has_key('hifv_circfeedpolcal') and casac.utils().verify(mytmp, trec['hifv_circfeedpolcal']) :
	    result = task_hifv_circfeedpolcal.hifv_circfeedpolcal(vis, Dterm_solint, refantignore, leakage_poltype, mbdkcross, clipminmax, pipelinemode, dryrun, acceptresults)

	else :
	  result = False
        return result
