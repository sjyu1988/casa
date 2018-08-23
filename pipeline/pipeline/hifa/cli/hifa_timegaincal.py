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
import task_hifa_timegaincal
def hifa_timegaincal(vis=[''], calamptable=[''], offsetstable=[''], calphasetable=[''], targetphasetable=[''], amptable=[''], field='', intent='', spw='', antenna='', calsolint='int', targetsolint='inf', combine='', refant='', solnorm=False, minblperant=4, calminsnr=2.0, targetminsnr=3.0, smodel=[], pipelinemode='automatic', dryrun=False, acceptresults=True):

        """Determine temporal gains from calibrator observations

Compute the gain solutions.

---- pipeline parameter arguments which can be set in any pipeline mode

pipelinemode -- The pipeline operating mode. In 'automatic' mode the pipeline
       determines the values of all context defined pipeline inputs
       automatically.  In interactive mode the user can set the pipeline
       context defined parameters manually.  In 'getinputs' mode the user
       can check the settings of all pipeline parameters without running
       the task.
       default: 'automatic'.

calsolint --  Time solution interval in CASA syntax for calibrator source solutions. 
    default: 'int'
    example: 'inf', 'int', '100sec'

targetsolint --  Time solution interval in CASA syntax for target source solutions. 
    default: 'inf'
    example: 'inf', 'int', '100sec'

combine -- Data axes to combine for solving. Options are  '','scan','spw',field'
    or any comma-separated combination.
    default; ''
    example: combine=''

minblperant -- Minimum number of baselines required per antenna for each solve
    Antennas with fewer baselines are excluded from solutions.
    default: 4
    example: minblperant=2

calminsnr -- Solutions below this SNR are rejected for calibrator solutions. 
    default: 2.0

targetminsnr -- Solutions below this SNR are rejected for science target solutions. 
    default: 3.0

---- pipeline context defined parameter arguments which can be set only in
'interactive mode'

vis -- The list of input MeasurementSets. Defaults to the list of MeasurementSets
    specified in the pipeline context
    default: ''
    example: ['M82A.ms', 'M82B.ms'] 

calamptable -- The list of output diagnostic calibration amplitude tables for the
    calibration targets.
    Defaults to the standard pipeline naming convention.
    default: ''
    example: ['M82.gacal', 'M82B.gacal']

offsetstable -- The list of output diagnostic phase offset tables for the
    calibration targets.
    Defaults to the standard pipeline naming convention.
    default: ''
    example: ['M82.offsets.gacal', 'M82B.offsets.gacal']

calphasetable -- The list of output calibration phase tables for the
    calibration targets.
    Defaults to the standard pipeline naming convention.
    default: ''
    example: ['M82.gcal', 'M82B.gcal']

amptable -- The list of output calibration amplitude tables for the
    calibration and science targets.
    Defaults to the standard pipeline naming convention.
    default: ''
    example: ['M82.gcal', 'M82B.gcal']

targetphasetable -- The list of output phase calibration tables for the science targets.
    Defaults to the standard pipeline naming convention.
    default: ''
    example: ['M82.gcal', 'M82B.gcal']

field -- The list of field names or field ids for which gain solutions are
    to be computed. Defaults to all fields with the standard intent.
    default: '' 
    example: '3C279', '3C279, M82'

intent -- A string containing a comma delimited list of intents against
    which the the selected fields are matched. Defaults to the
    equivalent of 'AMPLITUDE,PHASE,BANDPASS'.
    default: '' 
    example: '', 'PHASE'

spw -- The list of spectral windows and channels for which gain solutions are
    computed. Defaults to all science spectral windows.
    default: '' 
    example: '3C279', '3C279, M82'

smodel -- Point source Stokes parameters for source model (experimental)
    Defaults to using standard MODEL_DATA column data.
    default: [] 
    example: [1,0,0,0] (I=1, unpolarized)

refant -- Reference antenna name(s) in priority order. Defaults to most recent
    values set in the pipeline context.  If no reference antenna is defined in
    the pipeline context use the CASA defaults.
    default: '' 
    example: refant='DV01', refant='DV05,DV07'

solnorm -- Normalise the gain solutions
    default: False

--- pipeline task execution modes
dryrun -- Run the commands (True) or generate the commands to be run but
   do not execute (False).
   default: False

acceptresults -- Add the results of the task to the pipeline context (True) or
   reject them (False).
   default: True

Output:

results -- If pipeline mode is 'getinputs' then None is returned. Otherwise
    the results object for the pipeline task is returned

Description

The complex gains  are derived from the data column (raw data) divided
by the model column (usually set with hif_setjy). The gains are obtained for 
the specified solution intervals, spw combination and field combination.
One gain solution is computed for the science targets and one for
the calibrator targets.

Good candidate reference antennas can be determined using the hif_refant
task.

Previous calibrations that have been stored in the pipeline context are
applied on the fly. Users can interact with these calibrations via the
hif_export_calstate and hif_import_calstate tasks.

Issues


Examples

1. Compute standard per scan gain solutions that will be used to calibrate
the target.

    hifa_timegaincal()



        """
        if type(vis)==str: vis=[vis]
        if type(calamptable)==str: calamptable=[calamptable]
        if type(offsetstable)==str: offsetstable=[offsetstable]
        if type(calphasetable)==str: calphasetable=[calphasetable]
        if type(targetphasetable)==str: targetphasetable=[targetphasetable]
        if type(amptable)==str: amptable=[amptable]
        if type(smodel)==float: smodel=[smodel]

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['vis'] = vis
        mytmp['calamptable'] = calamptable
        mytmp['offsetstable'] = offsetstable
        mytmp['calphasetable'] = calphasetable
        mytmp['targetphasetable'] = targetphasetable
        mytmp['amptable'] = amptable
        mytmp['field'] = field
        mytmp['intent'] = intent
        mytmp['spw'] = spw
        mytmp['antenna'] = antenna
        mytmp['calsolint'] = calsolint
        mytmp['targetsolint'] = targetsolint
        mytmp['combine'] = combine
        mytmp['refant'] = refant
        mytmp['solnorm'] = solnorm
        mytmp['minblperant'] = minblperant
        mytmp['calminsnr'] = calminsnr
        mytmp['targetminsnr'] = targetminsnr
        mytmp['smodel'] = smodel
        mytmp['pipelinemode'] = pipelinemode
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hifa/cli/"
	trec = casac.utils().torecord(pathname+'hifa_timegaincal.xml')

        casalog.origin('hifa_timegaincal')
        if trec.has_key('hifa_timegaincal') and casac.utils().verify(mytmp, trec['hifa_timegaincal']) :
	    result = task_hifa_timegaincal.hifa_timegaincal(vis, calamptable, offsetstable, calphasetable, targetphasetable, amptable, field, intent, spw, antenna, calsolint, targetsolint, combine, refant, solnorm, minblperant, calminsnr, targetminsnr, smodel, pipelinemode, dryrun, acceptresults)

	else :
	  result = False
        return result
