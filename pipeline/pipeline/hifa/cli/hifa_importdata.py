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
import task_hifa_importdata
def hifa_importdata(vis=[''], session=[''], pipelinemode='automatic', asis='SBSummary ExecBlock Antenna Station Receiver Source CalAtmosphere CalWVR', process_caldevice=False, overwrite=False, nocopy=False, bdfflags=True, asimaging=False, lazy=False, dbservice=True, ocorr_mode='ca', createmms='false', dryrun=False, acceptresults=True):

        """Imports data into the interferometry pipeline
The hifa_importdata task loads the specified visibility data into the pipeline
context unpacking and / or converting it as necessary.

Keyword arguments:

---- pipeline parameter arguments which can be set in any pipeline mode

vis -- List of visibility data files. These may be ASDMs, tar files of ASDMs,
   MSs, or tar files of MSs, If ASDM files are specified, they will be
   converted  to MS format.
   default: []
   example: vis=['X227.ms', 'asdms.tar.gz']

session -- List of sessions to which the visibility files belong. Defaults
   to a single session containing all the visibility files, otherwise
   a session must be assigned to each vis file.
   default: []
   example: session=['session_1', 'session_2']

pipelinemode -- The pipeline operating mode. In 'automatic' mode the pipeline
   determines the values of all context defined pipeline inputs
   automatically.  In 'interactive' mode the user can set the pipeline
   context defined parameters manually.  In 'getinputs' mode the user
   can check the settings of all pipeline parameters without running
   the task.
   default: 'automatic'.

---- pipeline context defined parameter argument which can be set only in
'interactive mode'

asis -- ASDM tables to convert as is
   default: 'SBSummary ExecBlock Antenna Station Receiver Source CalAtmosphere CalWVR'
   example: 'Receiver', ''

process_caldevice -- Ingest the ASDM caldevice table
   default: False
   example: True

overwrite -- Overwrite existing MSs on output.
   default: False

nocopy -- When importing an MS, disable copying of the MS to the working
    directory
    default: False

bdfflags -- Apply BDF flags on line
   default: True

ocorr_mode -- Read in cross- and auto-correlation data(ca), cross-
   correlation data only (co), or autocorrelation data only (ao).
   default: ca

lazy -- Use the lazy filter import
   default: False

dbservice -- Use online flux catalog on import
   default: True

ocorr_mode -- Read in cross- and auto-correlation data(ca), cross-
   correlation data only (co), or autocorrelation data only (ao).
   default: ca

createmms -- Create a multi-MeasurementSet ('true') ready for full parallel 
    processing, or a standard MeasurementSet ('false'). The default setting
    ('automatic') creates an MMS if running in a cluster environment.
    default: 'false'

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

1. Load an ASDM list in the ../rawdata subdirectory into the context:

   hifa_importdata(vis=['../rawdata/uid___A002_X30a93d_X43e',
                        '../rawdata/uid_A002_x30a93d_X44e'])

2. Load an MS in the current directory into the context:

   hifa_importdata(vis=[uid___A002_X30a93d_X43e.ms])

3. Load a tarred ASDM in ../rawdata into the context:

   hifa_importdata(vis=['../rawdata/uid___A002_X30a93d_X43e.tar.gz'])

4. Check the hif_importdata inputs, then import the data:

   myvislist = ['uid___A002_X30a93d_X43e.ms', 'uid_A002_x30a93d_X44e.ms']
   hifa_importdata(vis=myvislist, pipelinemode='getinputs')
   hifa_importdata(vis=myvislist)

5. Load an ASDM but check the results before accepting them into the context.

   results = hifa_importdata(vis=['uid___A002_X30a93d_X43e.ms'],
                             acceptresults=False)
   results.accept()

6. Run in  dryrun mode before running for real:

   results = hifa_importdata(vis=['uid___A002_X30a93d_X43e.ms'], dryrun=True)
   results = hifa_importdata(vis=['uid___A002_X30a93d_X43e.ms'])
        
        """
        if type(vis)==str: vis=[vis]
        if type(session)==str: session=[session]

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['vis'] = vis
        mytmp['session'] = session
        mytmp['pipelinemode'] = pipelinemode
        mytmp['asis'] = asis
        mytmp['process_caldevice'] = process_caldevice
        mytmp['overwrite'] = overwrite
        mytmp['nocopy'] = nocopy
        mytmp['bdfflags'] = bdfflags
        mytmp['asimaging'] = asimaging
        mytmp['lazy'] = lazy
        mytmp['dbservice'] = dbservice
        mytmp['ocorr_mode'] = ocorr_mode
        mytmp['createmms'] = createmms
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hifa/cli/"
	trec = casac.utils().torecord(pathname+'hifa_importdata.xml')

        casalog.origin('hifa_importdata')
        if trec.has_key('hifa_importdata') and casac.utils().verify(mytmp, trec['hifa_importdata']) :
	    result = task_hifa_importdata.hifa_importdata(vis, session, pipelinemode, asis, process_caldevice, overwrite, nocopy, bdfflags, asimaging, lazy, dbservice, ocorr_mode, createmms, dryrun, acceptresults)

	else :
	  result = False
        return result
