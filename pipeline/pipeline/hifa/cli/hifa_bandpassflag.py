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
import task_hifa_bandpassflag
def hifa_bandpassflag(vis=[''], caltable=[''], intent='', field='', spw='', antenna='', hm_phaseup='snr', phaseupsolint='int', phaseupbw='', phaseupsnr=20.0, phaseupnsols=2, hm_bandpass='snr', solint='inf', maxchannels=240, evenbpints=True, bpsnr=50.0, bpnsols=8, combine='scan', refant='', minblperant=4, minsnr=3.0, solnorm=True, antnegsig=4.0, antpossig=4.6, tmantint=0.063, tmint=0.085, tmbl=0.175, antblnegsig=3.4, antblpossig=3.2, relaxed_factor=2.0, niter=2, pipelinemode='automatic', dryrun=False, acceptresults=True):

        """Compute bandpass calibration with flagging

Keyword arguments:

--- pipeline parameter arguments which can be set in any pipeline mode

pipelinemode -- The pipeline operating mode. In 'automatic' mode the pipeline
   determines the values of all context defined pipeline inputs automatically.
   In interactive mode the user can set the pipeline context defined parameters
   manually.  In 'getinputs' mode the user can check the settings of all
   pipeline parameters without running the task.
   default: 'automatic'.

hm_phaseup -- The pre-bandpass solution phaseup gain heuristics. The options
   are 'snr' (compute solution required to achieve the specified SNR),
   'manual' (use manual solution parameters), and '' (none).
   default: 'snr'
   example: hm_phaseup='manual'

phaseupsolint -- The phase correction solution interval in CASA syntax.
    Used when hm_phaseup='manual' or as a default if the hm_phasup='snr'
    heuristic computation fails.
    default: 'int'
    example: phaseupsolint='300s'

phaseupbw -- Bandwidth to be used for phaseup. Defaults to 500MHz.
    Used when hm_phaseup='manual'.
    default: ''
    example: '' default to entire bandpass, '500MHz' use central 500MHz

phaseupsnr -- The required SNR for the phaseup solution. Used only if
    hm_phaseup='snr'
    default: 20.0
    example: phaseupsnr=10.0

phaseupnsols -- The minimum number of phaseup gain solutions. Used only if
    hm_phaseup='snr'.
    default: 2
    example: phaseupnsols=4

hm_bandpass -- The bandpass solution heuristics. The options are 'snr'
    (compute the solution required to achieve the specified SNR),
    'smoothed' (simple smoothing heuristics), and 'fixed' (use
    the user defined parameters for all spws).

solint --  Time and channel solution intervals in CASA syntax.
    default: 'inf,7.8125MHz' for hm_bandpass='fixed'
             'inf' for hm_bandpass='snr' or 'smoothed'
    example: solint='inf,10ch', solint='inf'

maxchannels --  The bandpass solution smoothing factor in channels. The
    solution interval is bandwidth / 240. Set to 0 for no smoothing.
    Used if hm_bandpass='smoothed".
    default: 240
    example: 0

evenbpints -- Force the per spw frequency solint to be evenly divisible
    into the spw bandpass if hm_bandpass='snr'
    default: True
    example: evenbpints=False

bpsnr -- The required SNR for the bandpass solution. Used only if
    hm_bandpass='snr'
    default: 50.0
    example: bpsnr=20.0

bpnsols -- The minimum number of bandpass solutions. Used only if
    hm_bandpass='snr'.
    default: 8

combine -- Data axes to combine for solving. Axes are '', 'scan','spw','field'
    or any comma-separated combination.
    default; 'scan'
    example: combine='scan,field'

minblperant -- Minimum number of baselines required per antenna for each solve
    Antennas with fewer baselines are excluded from solutions.
    default: 4

minsnr -- Solutions below this SNR are rejected.
    default: 3.0

---- pipeline context defined parameter arguments which can be set only in
'interactive mode'

vis -- The list of input MeasurementSets. Defaults to the list of MeasurementSets
    specified in the pipeline context.
    default: ''
    example: ['M51.ms']

caltable -- The list of output calibration tables. Defaults to the standard
    pipeline naming convention.
    default: ''
    example: ['M51.bcal']

field -- The list of field names or field ids for which bandpasses are
    computed. If undefined (default), it will select all fields.
    default: ''
    example: '3C279', '3C279, M82'

intent -- A string containing a comma delimited list of intents against
    which the selected fields are matched. If undefined (default), it
    will select all data with the BANDPASS intent.
    default: ''
    example: '*PHASE*'

spw -- The list of spectral windows and channels for which bandpasses are
    computed. If undefined (default), it will select all science spectral
    windows.
    default: ''
    example: '11,13,15,17'

refant -- Reference antenna names. Defaults to the value(s) stored in the
    pipeline context. If undefined in the pipeline context defaults to
    the CASA reference antenna naming scheme.
    default: ''
    example: refant='DV01', refant='DV06,DV07'

solnorm -- Normalise the bandpass solutions
    default: False

antnegsig -- Lower sigma threshold for identifying outliers as a result of bad
    antennas within individual timestamps.
    default: 4.0

antpossig -- Upper sigma threshold for identifying outliers as a result of bad
    antennas within individual timestamps.
    default: 4.6

tmantint -- Threshold for maximum fraction of timestamps that are allowed to
    contain outliers.
    default: 0.063

tmint -- Initial threshold for maximum fraction of "outlier timestamps" over
    "total timestamps" that a baseline may be a part of.
    default: 0.085

tmbl -- Initial threshold for maximum fraction of "bad baselines" over "all
    baselines" that an antenna may be a part of.
    default: 0.175

antblnegsig -- Lower sigma threshold for identifying outliers as a result of
    "bad baselines" and/or "bad antennas" within baselines, across all
    timestamps.
    default: 3.4

antblpossig -- Upper sigma threshold for identifying outliers as a result of
    "bad baselines" and/or "bad antennas" within baselines, across all
    timestamps.
    default: 3.2

relaxed_factor -- Relaxed value to set the threshold scaling factor to under
    certain conditions (see task description).
    default: 2.0

niter -- Maximum number of times to iterate on evaluation of flagging
    heuristics. If an iteration results in no new flags, then subsequent
    iterations are skipped.
    default: 2

--- pipeline task execution modes
dryrun -- Run the commands (True) or generate the commands to be run but
   do not execute (False).
   default: False

acceptresults -- Add the results of the task to the pipeline context (True) or
   reject them (False).
   default: True


Output:

results -- If pipeline mode is 'getinputs' then None is returned. Otherwise
    the results object for the pipeline task is returned.


Examples:

1. run with recommended settings to create bandpass solution with flagging
    using recommended thresholds:

    hifa_bandpassflag()



        """
        if type(vis)==str: vis=[vis]
        if type(caltable)==str: caltable=[caltable]

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['vis'] = vis
        mytmp['caltable'] = caltable
        mytmp['intent'] = intent
        mytmp['field'] = field
        mytmp['spw'] = spw
        mytmp['antenna'] = antenna
        mytmp['hm_phaseup'] = hm_phaseup
        mytmp['phaseupsolint'] = phaseupsolint
        mytmp['phaseupbw'] = phaseupbw
        mytmp['phaseupsnr'] = phaseupsnr
        mytmp['phaseupnsols'] = phaseupnsols
        mytmp['hm_bandpass'] = hm_bandpass
        mytmp['solint'] = solint
        mytmp['maxchannels'] = maxchannels
        mytmp['evenbpints'] = evenbpints
        mytmp['bpsnr'] = bpsnr
        mytmp['bpnsols'] = bpnsols
        mytmp['combine'] = combine
        mytmp['refant'] = refant
        mytmp['minblperant'] = minblperant
        mytmp['minsnr'] = minsnr
        mytmp['solnorm'] = solnorm
        mytmp['antnegsig'] = antnegsig
        mytmp['antpossig'] = antpossig
        mytmp['tmantint'] = tmantint
        mytmp['tmint'] = tmint
        mytmp['tmbl'] = tmbl
        mytmp['antblnegsig'] = antblnegsig
        mytmp['antblpossig'] = antblpossig
        mytmp['relaxed_factor'] = relaxed_factor
        mytmp['niter'] = niter
        mytmp['pipelinemode'] = pipelinemode
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hifa/cli/"
	trec = casac.utils().torecord(pathname+'hifa_bandpassflag.xml')

        casalog.origin('hifa_bandpassflag')
        if trec.has_key('hifa_bandpassflag') and casac.utils().verify(mytmp, trec['hifa_bandpassflag']) :
	    result = task_hifa_bandpassflag.hifa_bandpassflag(vis, caltable, intent, field, spw, antenna, hm_phaseup, phaseupsolint, phaseupbw, phaseupsnr, phaseupnsols, hm_bandpass, solint, maxchannels, evenbpints, bpsnr, bpnsols, combine, refant, minblperant, minsnr, solnorm, antnegsig, antpossig, tmantint, tmint, tmbl, antblnegsig, antblpossig, relaxed_factor, niter, pipelinemode, dryrun, acceptresults)

	else :
	  result = False
        return result
