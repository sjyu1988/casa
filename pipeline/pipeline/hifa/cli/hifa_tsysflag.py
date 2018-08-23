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
import task_hifa_tsysflag
def hifa_tsysflag(vis=[''], caltable=[''], flag_nmedian=True, fnm_limit=2.0, fnm_byfield=False, flag_derivative=True, fd_max_limit=5.0, flag_edgechans=True, fe_edge_limit=3.0, flag_fieldshape=True, ff_refintent='BANDPASS', ff_max_limit=5.0, flag_birdies=True, fb_sharps_limit=0.05, flag_toomany=True, tmf1_limit=0.666, tmef1_limit=0.666, metric_order='nmedian,derivative,edgechans,fieldshape,birdies,toomany', normalize_tsys=False, filetemplate=[''], pipelinemode='automatic', dryrun=False, acceptresults=True):

        """Flag deviant system temperature measurements

Flag deviant system temperatures for ALMA interferometry measurements.

Flag all deviant system temperature measurements in the system temperature
calibration table by running a sequence of flagging tests, each designed
to look for a different type of error.

If a file with manual Tsys flags is provided with the 'filetemplate'
parameter, then these flags are applied prior to the evaluation of the
flagging heuristics listed below.

The tests are:

1. Flag Tsys spectra with high median values

2. Flag Tsys spectra with high median derivatives. This is meant to spot 
spectra that are 'ringing'.

3. Flag the edge channels of the Tsys spectra in each SpW.

4. Flag Tsys spectra whose shape is different from that associated with
the BANDPASS intent.

5. Flag 'birdies'.

6. Flag the Tsys spectra of all antennas in a timestamp and spw if 
proportion of antennas already flagged in this timestamp and spw exceeds 
a threshold, and flag Tsys spectra for all antennas and all timestamps 
in a spw, if proportion of antennas that are already entirely flagged 
in all timestamps exceeds a threshold.


Keyword arguments:

--- Pipeline parameter arguments which can be set in any pipeline mode

pipelinemode -- The pipeline operating mode. In 'automatic' mode the pipeline
    determines the values of all context defined pipeline inputs automatically.
    In interactive mode the user can set the pipeline context defined 
    parameters manually.  In 'getinputs' mode the user can check the settings 
    of all pipeline parameters without running the task.
    default: 'automatic'.

flag_nmedian -- True to flag Tsys spectra with high median value.
    default: True

fnm_limit -- Flag spectra with median value higher than fnm_limit * median 
    of this measure over all spectra.
    default: 2.0

fnm_byfield -- Evaluate the nmedian metric separately for each field.
    default: False
 
flag_derivative -- True to flag Tsys spectra with high median derivative.
    default: True

fd_max_limit -- Flag spectra with median derivative higher than 
    fd_max_limit * median of this measure over all spectra.
    default: 5.0

flag_edgechans -- True to flag edges of Tsys spectra.
    default: True

fe_edge_limit -- Flag channels whose channel to channel difference > 
    fe_edge_limit * median across spectrum.
    default: 3.0

flag_fieldshape -- True to flag Tsys spectra with a radically different 
    shape to those of the ff_refintent.
    default: True

ff_refintent -- Data intent that provides the reference shape for 
    'flag_fieldshape'.
    default: BANDPASS

ff_max_limit -- Flag Tsys spectra with 'fieldshape' metric values >
    ff_max_limit.
    default: 5.0

flag_birdies -- True to flag channels covering sharp spectral features.
    default: True

fb_sharps_limit -- Flag channels bracketing a channel to channel
    difference > fb_sharps_limit.
    default: 0.05

flag_toomany -- True to flag Tsys spectra for which a proportion of 
    antennas for given timestamp and/or proportion of antennas that are 
    entirely flagged in all timestamps exceeds their respective thresholds.
    default: True

tmf1_limit -- Flag Tsys spectra for all antennas in a timestamp and spw if 
    proportion of antennas already flagged in this timestamp and spw exceeds 
    tmf1_limit.
    default: 0.666

tmef1_limit -- Flag Tsys spectra for all antennas and all timestamps
    in a spw, if proportion of antennas that are already entirely flagged 
    in all timestamps exceeds tmef1_limit.
    default: 0.666

metric_order -- Order in which to evaluate the flagging metrics that are 
    enables. Disabled metrics are skipped.
    default: 'nmedian,derivative,edgechans,fieldshape,birdies,toomany'

normalize_tsys -- True to create a normalized Tsys table that is used to 
    evaluate the Tsys flagging metrics. All newly found flags are also applied
    to the original Tsys caltable that continues to be used for subsequent 
    calibration.
    default: False

filetemplate -- The name of a text file that contains the manual Tsys flagging
    template. If the template flags file is undefined, a name of the form
    'msname.flagtsystemplate.txt' is assumed.
    default: ''

--- Pipeline context defined parameter arguments which can be set only in
'interactive mode'

caltable -- List of input Tsys calibration tables
    default: [] - Use the table currently stored in the pipeline context. 
    example: caltable=['X132.ms.tsys.s2.tbl']

--- Pipeline task execution modes

dryrun -- Run the commands (True) or generate the commands to be run but
    do not execute (False).
    default: True

acceptresults -- Add the results of the task to the pipeline context (True) or
    reject them (False).
    default: True



Output:

results -- If pipeline mode is 'getinputs' then None is returned. Otherwise
    the results object for the pipeline task is returned.



Examples:

1. Flag Tsys measurements using currently recommended tests:

   hifa_tsysflag()

2. Flag Tsys measurements using all recommended tests apart from that
   using the 'fieldshape' metric.
 
   hifa_tsysflag(flag_fieldshape=False)

        """
        if type(vis)==str: vis=[vis]
        if type(caltable)==str: caltable=[caltable]
        if type(filetemplate)==str: filetemplate=[filetemplate]

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['vis'] = vis
        mytmp['caltable'] = caltable
        mytmp['flag_nmedian'] = flag_nmedian
        mytmp['fnm_limit'] = fnm_limit
        mytmp['fnm_byfield'] = fnm_byfield
        mytmp['flag_derivative'] = flag_derivative
        mytmp['fd_max_limit'] = fd_max_limit
        mytmp['flag_edgechans'] = flag_edgechans
        mytmp['fe_edge_limit'] = fe_edge_limit
        mytmp['flag_fieldshape'] = flag_fieldshape
        mytmp['ff_refintent'] = ff_refintent
        mytmp['ff_max_limit'] = ff_max_limit
        mytmp['flag_birdies'] = flag_birdies
        mytmp['fb_sharps_limit'] = fb_sharps_limit
        mytmp['flag_toomany'] = flag_toomany
        mytmp['tmf1_limit'] = tmf1_limit
        mytmp['tmef1_limit'] = tmef1_limit
        mytmp['metric_order'] = metric_order
        mytmp['normalize_tsys'] = normalize_tsys
        mytmp['filetemplate'] = filetemplate
        mytmp['pipelinemode'] = pipelinemode
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hifa/cli/"
	trec = casac.utils().torecord(pathname+'hifa_tsysflag.xml')

        casalog.origin('hifa_tsysflag')
        if trec.has_key('hifa_tsysflag') and casac.utils().verify(mytmp, trec['hifa_tsysflag']) :
	    result = task_hifa_tsysflag.hifa_tsysflag(vis, caltable, flag_nmedian, fnm_limit, fnm_byfield, flag_derivative, fd_max_limit, flag_edgechans, fe_edge_limit, flag_fieldshape, ff_refintent, ff_max_limit, flag_birdies, fb_sharps_limit, flag_toomany, tmf1_limit, tmef1_limit, metric_order, normalize_tsys, filetemplate, pipelinemode, dryrun, acceptresults)

	else :
	  result = False
        return result
