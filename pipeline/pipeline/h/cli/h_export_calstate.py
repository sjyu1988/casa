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
import task_h_export_calstate
def h_export_calstate(filename='', state='active'):

        """Save the pipeline calibration state to disk

h_export_calstate saves the current pipeline calibration state to disk
in the form of a set of equivalent applycal calls.

Keyword arguments:

filename -- Name for the saved calibration state.
state -- calibration state to export

Description

h_export_calstate saves the current pipeline calibration state to disk
in the form of a set of equivalent applycal calls.

If filename is not given, h_export_calstate saves the calibration state to
disk with a filename based on the pipeline context creation time, using the
extension '.calstate'

One of two calibration states can be exported: either the active calibration
state (those calibrations currently applied on-the-fly but scheduled for
permanent application to the MeasurementSet in a subsequent hif_applycal
call) or the applied calibration state (calibrations that were previously
applied to the MeasurementSet using hif_applycal). The default is to export
the active calibration state.

Issues

If run several times in one pipeline session does the automatic export
file naming scheme, overwrite previous versions?

Example

1. Save the calibration state.

   h_export_calstate()

2. Save the active calibration state with a custom filename

   h_export_calstate(filename='afterbandpass.calstate')

3. Save the applied calibration state with a custom filename

   h_export_calstate(filename='applied.calstate', state='applied')


        """

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['filename'] = filename
        mytmp['state'] = state
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/h/cli/"
	trec = casac.utils().torecord(pathname+'h_export_calstate.xml')

        casalog.origin('h_export_calstate')
        if trec.has_key('h_export_calstate') and casac.utils().verify(mytmp, trec['h_export_calstate']) :
	    result = task_h_export_calstate.h_export_calstate(filename, state)

	else :
	  result = False
        return result
