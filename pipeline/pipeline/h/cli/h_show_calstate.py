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
import task_h_show_calstate
def h_show_calstate():

        """Show the current pipeline calibration state
Keyword arguments:
None

Description
h_show_calstate displays the current on-the-fly calibration state of the 
pipeline as a set of equivalent applycal calls.

        """

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/h/cli/"
	trec = casac.utils().torecord(pathname+'h_show_calstate.xml')

        casalog.origin('h_show_calstate')
        if trec.has_key('h_show_calstate') and casac.utils().verify(mytmp, trec['h_show_calstate']) :
	    result = task_h_show_calstate.h_show_calstate()

	else :
	  result = False
        return result
