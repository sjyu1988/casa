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
import task_hsd_exportdata
def hsd_exportdata(pprfile='', targetimages=[''], products_dir='', pipelinemode='automatic', dryrun=False, acceptresults=True):

        """Prepare singledish data for export
The hsd_exportdata task exports the data defined in the pipeline context
and exports it to the data products directory, converting and or
packing it as necessary.

Keyword arguments:

---- pipeline parameter arguments which can be set in any pipeline mode

pipelinemode -- The pipeline operating mode. In 'automatic' mode the pipeline
   determines the values of all context defined pipeline inputs automatically.
   In 'interactive' mode the user can set the pipeline context defined
   parameters manually.  In 'getinputs' mode the user can check the settings
   of all pipeline parameters without running the task.
   default: 'automatic'.

---- pipeline context defined parameter argument which can be set only in
'interactive mode'

pprfile -- Name of the pipeline processing request to be exported. Defaults
   to a file matching the template 'PPR_*.xml'.
   default: []
   example: pprfile=['PPR_GRB021004.xml']

targetimages -- List of science target images to be exported. Defaults to all
   science target images recorded in the pipeline context.
   default: []
   example: targetimages=['r_aqr.CM02.spw5.line0.XXYY.sd.im', 'r_aqr.CM02.spw5.XXYY.sd.cont.im']

products_dir -- Name of the data products subdirectory. Defaults to './'
   default: ''
   example: products_dir='../products'


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

Description

The hsd_exportdata task exports the data defined in the pipeline context
and exports it to the data products directory, converting and or packing
it as necessary.

The current version of the task exports the following products

o a FITS image for each selected science target source image
o a tar file per ASDM containing the final flags version and blparam
o a tar file containing the file web log

TBD
o a file containing the line feature table(frequency,width,spatial distribution)
o a file containing the list of identified transitions from line catalogs

Examples

1. Export the pipeline results for a single sessions to the data products
directory

    !mkdir ../products
    hsd_exportdata (products_dir='../products')


        """
        if type(targetimages)==str: targetimages=[targetimages]

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['pprfile'] = pprfile
        mytmp['targetimages'] = targetimages
        mytmp['products_dir'] = products_dir
        mytmp['pipelinemode'] = pipelinemode
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hsd/cli/"
	trec = casac.utils().torecord(pathname+'hsd_exportdata.xml')

        casalog.origin('hsd_exportdata')
        if trec.has_key('hsd_exportdata') and casac.utils().verify(mytmp, trec['hsd_exportdata']) :
	    result = task_hsd_exportdata.hsd_exportdata(pprfile, targetimages, products_dir, pipelinemode, dryrun, acceptresults)

	else :
	  result = False
        return result
