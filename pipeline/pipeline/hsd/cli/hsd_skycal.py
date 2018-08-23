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
import task_hsd_skycal
def hsd_skycal(calmode='auto', fraction='10%', noff=-1, width=0.5, elongated=False, pipelinemode='automatic', infiles=[''], field='', spw='', scan='', dryrun=False, acceptresults=True):

        """Calibrate data
The hsd_calsky task generates a caltable for sky calibration that  
stores reference spectra, which is to be subtracted from on-source 
spectra to filter out non-source contribution.

Keyword arguments:

---- pipeline parameter arguments which can be set in any pipeline mode

calmode -- Calibration mode. Available options are 'auto' (default), 
   'ps', 'otf', and 'otfraster'. When 'auto' is set, the task will 
   use preset calibration mode that is determined by inspecting data.
   'ps' mode is simple position switching using explicit reference 
   scans. Other two modes, 'otf' and 'otfraster', will generate 
   reference data from scans at the edge of the map. Those modes 
   are intended for OTF observation and the former is defined for 
   generic scanning pattern such as Lissajous, while the later is 
   specific use for raster scan.
   default: 'auto'
   options: 'auto', 'ps', 'otf', 'otfraster'

fraction -- Subparameter for calmode. Edge marking parameter for
   'otf' and 'otfraster' mode. It specifies a number of OFF scans
   as a fraction of total number of data points.
   default: '10%'
   options: String style like '20%', or float value less than 1.0.
            For 'otfraster' mode, you can also specify 'auto'.
            
noff -- Subparameter for calmode. Edge marking parameter for 'otfraster'
   mode. It is used to specify a number of OFF scans near edge directly
   instead to specify it by fractional number by 'fraction'. If it is
   set, the value will come before setting by 'fraction'.
   default: -1 (use setting by 'fraction')
   options: any positive integer value

width -- Subparameter for calmode. Edge marking parameter for 'otf'
   mode. It specifies pixel width with respect to a median spatial
   separation between neighboring two data in time. Default will
   be fine in most cases.
   default: 0.5
   options: any float value

elongated -- Subparameter for calmode. Edge marking parameter for
   'otf' mode. Please set True only if observed area is elongated
   in one direction.
   default: False

pipelinemode -- The pipeline operating mode. In 'automatic' mode the 
   pipeline determines the values of all context defined pipeline inputs
   automatically.  In 'interactive' mode the user can set the pipeline
   context defined parameters manually.  In 'getinputs' mode the user
   can check the settings of all pipeline parameters without running
   the task.
   default: 'automatic'.

---- pipeline context defined parameter argument which can be set only in
'interactive mode'

infiles -- List of data files. These must be a name of Scantables that 
   are registered to context via hsd_importdata task.
   default: []
   example: vis=['X227.ms', 'X228.ms']

field -- Data selection by field name.
   default: '' (all fields)

spw -- Data selection by spw.
   default: '' (all spws)
   example: '3,4' (generate caltable for spw 3 and 4)
            ['0','2'] (spw 0 for first data, 2 for second)

scan -- Data selection by scan number.
   default: '' (all scans)
   example: '22,23' (use scan 22 and 23 for calibration)
            ['22','24'] (scan 22 for first data, 24 for second)

pol -- Data selection by pol.
   default: '' (all polarizations)
   example: '0' (generate caltable for pol 0)
            ['0~1','0'] (pol 0 and 1 for first data, only 0 for second)

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

1. Generate caltables for all data managed by context.
   default(hsd_skycal)
   hsd_skycal()



        """
        if type(infiles)==str: infiles=[infiles]

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['calmode'] = calmode
        mytmp['fraction'] = fraction
        mytmp['noff'] = noff
        mytmp['width'] = width
        mytmp['elongated'] = elongated
        mytmp['pipelinemode'] = pipelinemode
        mytmp['infiles'] = infiles
        mytmp['field'] = field
        mytmp['spw'] = spw
        mytmp['scan'] = scan
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hsd/cli/"
	trec = casac.utils().torecord(pathname+'hsd_skycal.xml')

        casalog.origin('hsd_skycal')
        if trec.has_key('hsd_skycal') and casac.utils().verify(mytmp, trec['hsd_skycal']) :
	    result = task_hsd_skycal.hsd_skycal(calmode, fraction, noff, width, elongated, pipelinemode, infiles, field, spw, scan, dryrun, acceptresults)

	else :
	  result = False
        return result
