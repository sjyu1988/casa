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
import task_hifa_gfluxscale
def hifa_gfluxscale(vis='', reference='', transfer='', refintent='', transintent='', refspwmap=[], reffile='', phaseupsolint='int', solint='inf', minsnr=2.0, refant='', hm_resolvedcals='automatic', antenna='', peak_fraction=0.2, pipelinemode='automatic', dryrun=False, acceptresults=True):

        """Derive flux density scales from standard calibrators

Derive flux densities for point source transfer calibrators using flux models
for reference calibrators.

---- pipeline parameter arguments which can be set in any pipeline mode

pipelinemode -- The pipeline operating mode. In 'automatic' mode the pipeline
    determines the values of all context defined pipeline inputs automatically.
    In interactive mode the user can set the pipeline context defined
    parameters manually.  In 'getinputs' mode the users can check the settings
    of all pipeline parameters without running the task.
    default: 'automatic'.

phaseupsolint --  Time solution intervals in CASA syntax for the phase solution.
    default: 'int'
    example: 'inf', 'int', '100sec'

solint --  Time solution intervals in CASA syntax for the amplitude solution.
    default: 'inf'
    example: 'inf', 'int', '100sec'

minsnr -- Minimum signal to noise ratio for gain calibration solutions.
    default: 2.0
    example: 1.5, 0.0

hm_resolvedcals - Heuristics method for handling resolved calibrators. The
    options are 'automatic' and 'manual'. In automatic mode antennas closer
    to the reference antenna than the uv distance where visibilities fall to
    'peak_fraction' of the peak are used. In manual mode the antennas specified
    in 'antenna' are used.

antenna -- A comma delimited string specifying the antenna names or ids to be
    used for the fluxscale determination. Used in hm_resolvedcals='manual' mode. 
    default: ''.
    example: 'DV16,DV07,DA12,DA08'  

peak_fraction -- The limiting UV distance from the reference antenna for antennas
    to be included in the flux calibration. Defined as the point where the 
    calibrator visibilities have fallen to 'peak_fraction' of the peak value. 


---- pipeline context defined parameter arguments which can be set only in
'interactive mode'

vis -- The list of input MeasurementSets. Defaults to the list of MeasurementSets
    specified in the pipeline context
    default: ''
    example: ['M32A.ms', 'M32B.ms']

reference -- A string containing a comma delimited list of  field names
    defining the reference calibrators. Defaults to field names with
    intent '*AMP*'.
    default: ''
    example: 'M82,3C273'

transfer -- A string containing a comma delimited list of  field names
    defining the transfer calibrators. Defaults to field names with
    intent '*PHASE*'.
    default: ''
    example: 'J1328+041,J1206+30'

refintent -- A string containing a comma delimited list of intents 
    used to select the reference calibrators. Defaults to *AMP*.
    default: ''
    example: '', '*AMP*'

refspwmap -- Vector of spectral window ids enabling scaling across
    spectral windows. Defaults to no scaling
    default: []
    example: [1,1,3,3] (4 spws, reference fields in 1 and 3, transfer
             fields in 0,1,2,3

reffile -- Path to a file containing flux densities for calibrators unknown to
    CASA. Values given in this file take precedence over the CASA-derived values
    for all calibrators except solar system calibrators. By default the path is
    set to the CSV file created by hifa_importdata, consisting of catalogue fluxes
    extracted from the ASDM and / or edited by the user.
    default: ''
    example: '', 'working/flux.csv'


transintent -- A string containing a comma delimited list of intents 
    defining the transfer calibrators. Defaults to *PHASE*.
    default: ''
    example: '', '*PHASE*'

refant -- A string specifying the reference antenna(s). By default
    this is read from the context.
    default: ''
    example: 'DV05'

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

Derive flux densities for point source transfer calibrators using flux models
for reference calibrators.

Flux values are determined by:

o computing complex gain phase only solutions for all the science spectral
  windows using the calibrator data selected by the 'reference' and
  'refintent' parameters and the 'transfer' and 'transintent' parameters,
  and the value of the 'phaseupsolint' parameter.

o computing complex amplitude only solutions for all the science spectral
  windows using calibrator data selected with 'reference' and 'refintent'
  parameters and the 'transfer' and 'transintent' parameters, the value
  of the 'solint' parameter. 

o transferring the flux scale from the reference calibrators to the transfer
  calibrators using refspwmap for windows without data in the reference
  calibrators

o extracted the computed flux values from the CASA logs and inserting
  them into the MODEL_DATA column.


Resolved calibrators are handled via antenna selection either automatically,
hm_resolvedcals='automatic' or manually, hm_resolvedcals='manual'. In
the former case antennas closer to the reference antenna than the uv
distance where visibilities fall to 'peak_fraction' of the peak are used.
In manual mode the antennas specified in 'antenna' are used.

Note that the flux corrected calibration table computed internally is
not currently used in later pipeline apply calibration steps. 


Issues

Should we add a spw window selection option here?

The code which extracts the flux scales from the logs needs to be replaced
with code which uses the values returned from the CASA fluxscale task.


Examples

1. Compute flux flux values for the phase calibrator using model data from
   the amplitude calibrator.

   hifa_gfluxscale ()


        """
        if type(refspwmap)==int: refspwmap=[refspwmap]

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['vis'] = vis
        mytmp['reference'] = reference
        mytmp['transfer'] = transfer
        mytmp['refintent'] = refintent
        mytmp['transintent'] = transintent
        mytmp['refspwmap'] = refspwmap
        mytmp['reffile'] = reffile
        mytmp['phaseupsolint'] = phaseupsolint
        mytmp['solint'] = solint
        mytmp['minsnr'] = minsnr
        mytmp['refant'] = refant
        mytmp['hm_resolvedcals'] = hm_resolvedcals
        mytmp['antenna'] = antenna
        mytmp['peak_fraction'] = peak_fraction
        mytmp['pipelinemode'] = pipelinemode
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hifa/cli/"
	trec = casac.utils().torecord(pathname+'hifa_gfluxscale.xml')

        casalog.origin('hifa_gfluxscale')
        if trec.has_key('hifa_gfluxscale') and casac.utils().verify(mytmp, trec['hifa_gfluxscale']) :
	    result = task_hifa_gfluxscale.hifa_gfluxscale(vis, reference, transfer, refintent, transintent, refspwmap, reffile, phaseupsolint, solint, minsnr, refant, hm_resolvedcals, antenna, peak_fraction, pipelinemode, dryrun, acceptresults)

	else :
	  result = False
        return result
