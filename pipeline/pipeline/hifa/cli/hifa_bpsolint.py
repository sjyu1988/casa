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
import task_hifa_bpsolint
def hifa_bpsolint(vis=[''], field='', intent='BANDPASS', spw='', phaseupsnr=20.0, minphaseupints=2, evenbpints=False, bpsnr=50.0, minbpnchan=8, hm_nantennas='all', maxfracflagged=0.90, pipelinemode='automatic', dryrun=False, acceptresults=True):

        """Compute optimal bandpass calibration solution intervals

Compute the bandpass phaseup time solution interval and bandpass frequency solution interval.

---- pipeline parameter arguments which can be set in any pipeline mode

pipelinemode -- The pipeline operating mode. In 'automatic' mode the pipeline
       determines the values of all context defined pipeline inputs
       automatically.  In interactive mode the user can set the pipeline
       context defined parameters manually.  In 'getinputs' mode the user
       can check the settings of all pipeline parameters without running
       the task.
       default: 'automatic'.

phaseupnsr -- The required phaseup gain time interval solution signal to noise
    default: 20.0
    example: phaseupsnr = 10.0

minphaseupints -- The minimum number of time intervals in the phaseup gain
    solution.
    default: 2
    example: minphaseupints=4

bpnsr -- The required bandpass frequency interval solution signal to noise
    default: 50.0
    example: phaseupsnr = 20.0

minbpnchan -- The minimum number of frequency intervals in the bandpass
    solution.
    default: 8
    example: minbpnchans=16

hm_nantennas -- The heuristics for determines the number of antennas to use
    in the signal to noise estimate. The options are 'all' and 'unflagged'.
    The 'unflagged' options is not currently supported.
    default: 'all'
    example: hm_nantennas='unflagged'

maxfracflagged -- The maximum fraction of an antenna that can be flagged
    before its is excluded from the signal to noise estimate.
    default: 0.90
    example: maxfracflagged=0.80

---- pipeline context defined parameter arguments which can be set only in
'interactive mode'

vis -- The list of input MeasurementSets. Defaults to the list of MeasurementSets
    specified in the pipeline context
    default: ''
    example: ['M82A.ms', 'M82B.ms'] 

field -- The list of field names of sources to be used for signal to noise
    estimation. Defaults to all fields with the standard intent.
    default: '' 
    example: '3C279'

intent -- A string the intent against which the the selected fields are matched.
    Defaults to 'BANDPASS'.
    default: 'BANDPASS' 
    example: intent='PHASE'

spw -- The list of spectral windows and channels for which gain solutions are
    computed. Defaults to all the science spectral windows for which there are
    both 'intent' and TARGET intents.
    default: '' 
    example: '13,15'

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

The phaseup gain time and bandpass frequency intervals are determined as
follows.

o For each data set the list of source(s) to use for bandpass solution signal
  to noise estimation is compiled based on the values of the field, intent,
  and spw parameters. 

o Source fluxes are determined for each spw and source combination.
    o Fluxes in Jy are derived from the pipeline context.
    o Pipeline context fluxes are derived from the online flux calibrator catalog,
      the ASDM, or the user via the flux.csv file.
    o If no fluxes are available the task terminates.

o Atmospheric calibration and observations scans are determined for each spw
  and source combination. 
    o If intent is set to 'PHASE' are there are no atmospheric scans
      associated with the 'PHASE' calibrator, 'TARGET' atmospheric scans
      will be used in stead.
    o If atmospheric scans cannot be associated with any of the spw and
      source combinations the task terminates.

o Science spws are mapped to atmospheric spws for each science spw and
  source combinations.
    o If mappings cannot be determined for any of the spws the task
      terminates

o The median Tsys value for each atmospheric spw and source combination is
  determined from the SYSCAL table. Medians are computed first by channel,
  then by antenna, in order to reduce sensitivity to deviant values.

o The science spw parameters, exposure time(s), and integration time(s) are
  determined.

o The phase up time interval, in time units and number of integrations required
  to meet the phaseupsnr are computed, along with the phaseup sensitivity in mJy
  and the signal to noise per integration.  Nominal Tsys and sensitivity values
  per receiver band provide by the ALMA project are used for this estimate.

o Warnings are issued if estimated phaseup gain time solution would contain fewer
  than minphasupints  solutions

o The frequency interval, in MHz and number of channels required to meet the
  bpsnr are computed, along with the per channel sensitivity in mJy and the
  per channel signal to noise.  Nominal Tsys and sensitivity values per receiver
  band provide by the ALMA project are used for this estimate.

o Warnings are issued if estimated bandpass solution would contain fewer than
  minbpnchan solutions



Examples

1. Estimate the phaseup gain time interval and the bandpass frequency interval
   required to match the desired signal to noise for bandpass solutions.

hifa_bpsolint()


        """
        if type(vis)==str: vis=[vis]

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['vis'] = vis
        mytmp['field'] = field
        mytmp['intent'] = intent
        mytmp['spw'] = spw
        mytmp['phaseupsnr'] = phaseupsnr
        mytmp['minphaseupints'] = minphaseupints
        mytmp['evenbpints'] = evenbpints
        mytmp['bpsnr'] = bpsnr
        mytmp['minbpnchan'] = minbpnchan
        mytmp['hm_nantennas'] = hm_nantennas
        mytmp['maxfracflagged'] = maxfracflagged
        mytmp['pipelinemode'] = pipelinemode
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hifa/cli/"
	trec = casac.utils().torecord(pathname+'hifa_bpsolint.xml')

        casalog.origin('hifa_bpsolint')
        if trec.has_key('hifa_bpsolint') and casac.utils().verify(mytmp, trec['hifa_bpsolint']) :
	    result = task_hifa_bpsolint.hifa_bpsolint(vis, field, intent, spw, phaseupsnr, minphaseupints, evenbpints, bpsnr, minbpnchan, hm_nantennas, maxfracflagged, pipelinemode, dryrun, acceptresults)

	else :
	  result = False
        return result
