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
import task_hsd_baseline
def hsd_baseline(fitfunc='cspline', fitorder=-1, linewindow='', linewindowmode='replace', edge=[], broadline=True, clusteringalgorithm='kmean', deviationmask=True, pipelinemode='automatic', infiles=[''], field='', antenna='', spw='', pol='', dryrun=False, acceptresults=True, parallel='automatic'):

        """Detect and validate spectral lines, subtract baseline by masking detected lines
The hsd_baseline task subtracts baseline from calibrated spectra.
By default, the task tries to find spectral line feature using
line detection and validation algorithms. Then, the task puts a
mask on detected lines and perform baseline subtraction. The user
is able to turn off automatic line masking by setting linewindow
parameter, which specifies pre-defined line window. 

Fitting order is automatically determined by default. It can be
disabled by specifying fitorder as non-negative value. In this case,
the value specified by fitorder will be used.
  
Keyword arguments:

---- pipeline parameter arguments which can be set in any pipeline mode
fitfunc -- fitting function for baseline subtraction. You can only choose
   cubic spline ('spline' or 'cspline')
   default: 'cspline'.

fitorder -- Fitting order for polynomial. For cubic spline, it is used
   to determine how much the spectrum is segmented into. Default (-1) is
   to determine the order automatically.
   default: -1 (auto determination)

linewindow -- Pre-defined line window. If this is set, specified line 
   windows are used as a line mask for baseline subtraction instead to 
   determine masks based on line detection and validation stage. Several  
   types of format are acceptable. One is channel-based window, 
   
      [min_chan, max_chan] 
      
   where min_chan and max_chan should be an integer. For multiple 
   windows, nested list is also acceptable, 
   
      [[min_chan0, max_chan0], [min_chan1, max_chan1], ...]
      
   Another way is frequency-based window, 
   
      [min_freq, max_freq]
      
   where min_freq and max_freq should be either a float or a string. 
   If float value is given, it is interpreted as a frequency in Hz. 
   String should be a quantity consisting of "value" and "unit", e.g., 
   '100GHz'. Multiple windows are also supported.
   
      [[min_freq0, max_freq0], [min_freq1, max_freq1], ...]
      
   Note that the specified frequencies are assumed to be the value in 
   LSRK frame. Note also that there is a limitation when multiple MSs are 
   processed. If native frequency frame of the data is not LSRK (e.g. TOPO), 
   frequencies need to be converted to that frame. As a result, corresponding 
   chnnnel range may vary between MSs. However, current implementation is 
   not able to handle such case. Frequencies are converted to desired frame 
   using representative MS (time, position, direction). 
      
   In the above cases, specified line windows are applied to all science 
   spws. In case when line windows vary with spw, line windows can be 
   specified by a dictionary whose key is spw id while value is line window.
   For example, the following dictionary gives different line windows 
   to spws 17 and 19. Other spws, if available, will have an empty line 
   window.
   
      {17: [[100, 200], [1200, 1400]], 19: ['112115MHz', '112116MHz']}
      
   Furthermore, linewindow accepts MS selection string. The following 
   string gives [[100,200],[1200,1400]] for spw 17 while [1000,1500] 
   for spw 21.
   
      "17:100~200;1200~1400,21:1000~1500"
      
   The string also accepts frequency with units. Note, however, that 
   frequency reference frame in this case is not fixed to LSRK. 
   Instead, the frame will be taken from the MS (typically TOPO for ALMA). 
   Thus, the following two frequency-based line windows result different 
   channel selections.
   
      {19: ['112115MHz', '112116MHz']} # frequency frame is LSRK
      "19:11215MHz~11216MHz" # frequency frame is taken from the data
                             # (TOPO for ALMA)
   
   default: [] (do line detection and validation)
   example: [100,200] (channel), [115e9, 115.1e9] (frequency in Hz)
            ['115GHz', '115.1GHz'], see above for more examples

linewindowmode -- Merge or replace given manual line window with line 
   detection/validation result. If 'replace' is given, line detection 
   and validation will not be performed. On the other hand, when 
   'merge' is specified, line detection/validation will be performed 
   and manually specified line windows are added to the result. 
   Note that this has no effect when linewindow for target spw is 
   empty. In that case, line detection/validation will be performed 
   regardless of the value of linewindowmode.
   default: 'replace'
   options: 'replace', 'merge'

edge -- number of edge channels to be dropped from baseline subtraction.
   the value must be a list with length of 2, whose values specifies
   left and right edge channels respectively.
   default: [] ([0,0])
   example: [10,10]

broadline -- Try to detect broad component of spectral line if True.
   default: True

clusteringalgorithm -- selection of the algorithm used in the clustering
   analysis to check the validity of detected line features. 'kmean'
   algorithm and hierarchical clustering algorithm 'hierarchy' are so
   far implemented.
   default: 'kmean'.
   
deviationmask -- Apply deviation mask in addition to masks determined by 
   the automatic line detection.
   default: True

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
   
field -- Data selection by field.
   default: '' (all fields)
   example: '1' (select by FIELD_ID)
            'M100*' (select by field name)

antenna -- Data selection by antenna.
   default: '' (all antennas)
   example: '1' (select by ANTENNA_ID)
            'PM03' (select by antenna name)

spw -- Data selection by spw.
   default: '' (all spws)
   example: '3,4' (generate caltable for spw 3 and 4)
            ['0','2'] (spw 0 for first data, 2 for second)

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



        """
        if type(edge)==int: edge=[edge]
        if type(infiles)==str: infiles=[infiles]

#
#    The following is work around to avoid a bug with current python translation
#
        mytmp = {}

        mytmp['fitfunc'] = fitfunc
        mytmp['fitorder'] = fitorder
        mytmp['linewindow'] = linewindow
        mytmp['linewindowmode'] = linewindowmode
        mytmp['edge'] = edge
        mytmp['broadline'] = broadline
        mytmp['clusteringalgorithm'] = clusteringalgorithm
        mytmp['deviationmask'] = deviationmask
        mytmp['pipelinemode'] = pipelinemode
        mytmp['infiles'] = infiles
        mytmp['field'] = field
        mytmp['antenna'] = antenna
        mytmp['spw'] = spw
        mytmp['pol'] = pol
        mytmp['dryrun'] = dryrun
        mytmp['acceptresults'] = acceptresults
        mytmp['parallel'] = parallel
	pathname="file:///Users/ksugimot/devel/eclipsedev/pipeline-trunk/pipeline/hsd/cli/"
	trec = casac.utils().torecord(pathname+'hsd_baseline.xml')

        casalog.origin('hsd_baseline')
        if trec.has_key('hsd_baseline') and casac.utils().verify(mytmp, trec['hsd_baseline']) :
	    result = task_hsd_baseline.hsd_baseline(fitfunc, fitorder, linewindow, linewindowmode, edge, broadline, clusteringalgorithm, deviationmask, pipelinemode, infiles, field, antenna, spw, pol, dryrun, acceptresults, parallel)

	else :
	  result = False
        return result
