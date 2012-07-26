##########################################################################
#                                                                        #
# Use Case Script for reducing NGC 2403 HI line data                     #
# This script reads four VLA archive files and stores them as a CASA     #
# measurement set, and then processes it.                                #
#                                                                        #
# Original version GvM 2007-11-30        (Beta)                          #
# Updated          GvM 2008-06-11        Beta Patch 2                    #
# Modified/merge   STM 2008-07-25        Regressable version             #
#                                                                        #
# Based on 2008 Synthesis Imaging Summer School scripts                  #
# NOTE: REGRESSION VERSION FOR CASA TESTING                              #
#                                                                        #
# Script Notes:                                                          #
#    o WARNING: The MS generated by this script is 5.2GB (from the       #
#      VLA export files that total 564MB).  This script can take         #
#      40 minutes to run depending on your machine.                      #
#                                                                        #
#    o Uses the VLA export files AS649_1 to 4, which should be in the    #
#      working directory that casapy was started in.  These VLA archive  #
#      can be found in in the data repository at data/regression/ngc2403 #
#                                                                        #
#    o This script has some interactive commands, such as with           #
#      the viewer.  If scriptmode=True, then this script                 #
#      will stop and require a carriage-return to continue at these      #
#      points.                                                           #
#                                                                        #
#    o Sometimes cut-and-paste of a series of lines from this script     #
#      into the casapy terminal will get garbled (usually a single       #
#      dropped character). In this case, try fewer lines, like groups    #
#      of 4-6.                                                           #
#                                                                        #
#    o The results are written out as a dictionary in a pickle file      #
#         out.ngc2403.tutorial.regression.<datestring>.pickle            #
#      (not auto-deleted at start of script)                             #
#                                                                        #
#      This script keeps internal regression values, but you can provide #
#      a file ngc2403_tutorial_regression.pickle from a previous run     #
#                                                                        #
#    o This script also generates a text file                            #
#         out.ngc2403.tutorial.<datestring>.log                          #
#      (not auto-deleted at start of script)                             #
#                                                                        #
##########################################################################

import time
import os
import pickle

# 
#=====================================================================
#
# This script has some interactive commands: scriptmode = True
# if you are running it and want it to stop during interactive parts.

scriptmode = False

# Enable benchmarking?
benchmarking = True

#=====================================================================
#
# Set up some useful variables

pathname=os.environ.get('CASAPATH').split()[0]

prefix='n2403.tutorial'

# Sets a shorthand for fixed input script/regression files
scriptprefix='ngc2403_tutorial_regression'

# These will be set later on, but set here also
msfile = prefix + '.ms'
splitfile = prefix + '.split.ms'
outname = prefix + '.final.clean'
momfile = outname + '.mom'

#
#=====================================================================
# Clean up old versions of files to be created in this script

os.system('rm -rf '+prefix+'.*')

if benchmarking:
    startTime=time.time()
    startProc=time.clock()

print 'Tutorial Regression Script for VLA NGC2403 HI data'
print 'Will do: import, flagging, calibration, imaging'
print ''
#=====================================================================
# Data Import
#=====================================================================
#
# Import the data from VLA archive files to MS
#
print "--importvla--"
print ""
print " Use importvla to read 4 VLA archive files and write the data"
print " into a Measurement Set (MS).  This will take a while ..."
print ""

# Set up the MS filename and save as new global variable
msfile = prefix + '.ms'

print " MS will be called "+msfile

default('importvla')

archivefiles=['AS649_1','AS649_2','AS649_3','AS649_4']
vis = msfile
importvla()

if benchmarking:
    import2time=time.time()

#=====================================================================
# List a summary of the MS
#=====================================================================
#
#
print "--listobs--"
print ""
print " Use listobs to print verbose summary to logger"
print " see the logger window for the listobs output"

# Don't default this one and make use of the previous setting of
# vis.  Remember, the variables are GLOBAL!

# You may wish to see more detailed information, in this case
# use the verbose = True option
#
verbose = True

listobs()

print ""
print "The listobs output will be displayed in your logger window and in"
print "the casapy.log file"

if benchmarking:
    list2time=time.time()

#=====================================================================
# FLAGGING
#=====================================================================
#

    
default('flagdata')

vis=msfile
spw='0:5~112'
correlation='RR'
field='0'
mode='manualflag'
timerange='03:51:07~03:52:48'
saveinputs('flagdata',prefix+'.saved.flagdata.n2403.rr.time0351')

flagdata()

print ""
print " now we clip RR above 0.4Jy"
print ""

timerange = ''
clipexpr = 'RR'
clipminmax = [-100, 0.4]
saveinputs('flagdata',prefix+'.saved.flagdata.n2403.rr.clip')

flagdata()

print ""
print " now we clip LL above 1.0Jy"
print ""

timerange = ''
correlation='LL'
clipexpr = 'LL'
clipminmax = [-100, 1.0]
saveinputs('flagdata',prefix+'.saved.flagdata.n2403.ll.clip')

flagdata()

#=====================================================================
# Save flagging done up to this point
#=====================================================================
#
print ""
print "--flagmanager--"
print ""
print " It is a good idea to save the flagging at certain times"
print " First, list the flag versions using flagmanager"
print ""

# first we list the current flagging tables

mode='list'

flagmanager()

print ""
print " then, we save the flagging we just did"

# then we save the flagging we just did

mode='save'
versionname='afterflagdata'
comment='flags after running flagdata'
merge='replace'

flagmanager()

# and now we list one more time to show the changes made

print ""
print " then, list one more time to show the changes"


mode='list'

flagmanager()

print " Done Flagging - proceed to Calibration"
print ""

if benchmarking:
    flag2time=time.time()

#=====================================================================
# CALIBRATION
# Fill the model column for flux density calibrators
#=====================================================================
#
print "--setjy--"
print ""
print " find the flux of the flux calibrators, and write it to the"
print " column labeled MODEL_DATA"

default('setjy')

vis=msfile
field='1,3,4'     # note: field 1 is the source NGC 2403, and field 2 is
                  # the phase calibrator 0841+708'
spw='0:5~112'     # use spectral window 0 (which is the only one).  In that
                  # window, use channels 5 - 112 (ignoring edge channels)

scalebychan=False

standard='Perley-Taylor 99'  # enforce the older standard

saveinputs('setjy',prefix+'.saved.setjy')

setjy()

if benchmarking:
    setjy2time=time.time()

#=====================================================================
# Determine antenna gains
#=====================================================================
#
print "--gaincal--"
print ""
print " creates user defined table containing antenna gain solutions"
print " once for each calibrator since uv ranges are different."
print " Note: append is False first, then True"
print ""

default('gaincal')
vis        = msfile
caltable   = prefix + '.gcal'
field      = '1'
spw        = '0:5~112'
selectdata = True
uvrange    = '0~40klambda'
solint     = 'inf'
combine    = ''
append     = False

print " starting field 1"
print ""

saveinputs('gaincal',prefix+'.saved.gaincal.field1')

gaincal()

field='2'
uvrange='0~20klambda'
append=True

print " starting field 2"
print ""

saveinputs('gaincal',prefix+'.saved.gaincal.field2')

gaincal()

field='3'
uvrange='0~50klambda'

print " starting field 3"
print ""

saveinputs('gaincal',prefix+'.saved.gaincal.field3')

gaincal()

field='4'
uvrange=''

print " starting field 4"
print ""

saveinputs('gaincal',prefix+'.saved.gaincal.field4')

gaincal()

if benchmarking:
    gaincal2time=time.time()

#=====================================================================
# Plot antenna gains
#=====================================================================
#
print "--plotcal--"
print ""
print " first we plot the amplitude gains for antennas 9 - 12"

default('plotcal')
caltable= prefix + '.gcal'
xaxis='time'
yaxis='amp'
field='2'
antenna='9~12'

print ""
if scriptmode:
    showgui=T
    figfile=''
    saveinputs('plotcal',prefix+'.saved.plotcal.gaincal.ant9to12amp')

    plotcal()

    user_check=raw_input('Return to continue script\n')
else:
    showgui=F
    figfile=prefix+'.plotcal.gaincal.amp.png'
    saveinputs('plotcal',prefix+'.saved.plotcal.gaincal.ant9to12amp')

    plotcal()


print " then, we plot R and L just for antenna 9 in separate plots on"
print " the same page.  Note use of the subplot parameter"

yaxis='phase'
antenna='9'
poln='R'
subplot=211
saveinputs('plotcal',prefix+'.saved.plotcal.gaincal.ant9phase.panel1')

if scriptmode:
    showgui=T
    plotcal()
else:
    showgui=F
    figfile=''
    plotcal()

poln='L'
subplot=212

print ""
# Pause script if you are running in scriptmode
if scriptmode:
    showgui=T
    figfile=''
    saveinputs('plotcal',prefix+'.saved.plotcal.gaincal.ant9phase.panel2')

    plotcal()

    print ""
    print " Next, we will determine the flux of 0841+708"
    user_check=raw_input('Return to continue script\n')
else:
    showgui=F
    figfile=prefix+'.plotcal.gaincal.phase.png'
    saveinputs('plotcal',prefix+'.saved.plotcal.gaincal.ant9phase.panel2')

    plotcal()

if benchmarking:
    plotgcal2time=time.time()

#=====================================================================
# Bootstrap flux of 0841+708
#=====================================================================
#
print "--fluxscale--"
print ""
print " determines flux based on gains and flux calibrator fluxes"
print " see Log window for flux value found"
default('fluxscale')
vis=msfile
caltable= prefix + '.gcal'
fluxtable=prefix + '.fcal'
reference='1,3~4'
transfer='2'

saveinputs('fluxscale',prefix+'.saved.fluxscale')

fluxscale()

if benchmarking:
    fluxscale2time=time.time()

#=====================================================================
# Solves for bandpass, writes it to table
#=====================================================================
#
print "--bandpass--"
print ""
print " determine bandpass"

default('bandpass')

vis      = msfile
caltable = prefix + '.bcal'
field    = '1,3~4'
solint   = 'inf'
combine  = ''
solnorm  = True

saveinputs('bandpass',prefix+'.saved.bandpass')

bandpass()

if benchmarking:
    bandpass2time=time.time()

#=====================================================================
# Plot bandpass
#=====================================================================
#
print "--plotcal--"
print ""
print " First we plot solutions for antennas 9-12 for field 1 only"

default('plotcal')

vis=msfile
caltable= prefix + '.bcal'
xaxis               =     'chan'
yaxis               =      'amp'
field               =        '1'
antenna             =     '9~12'
plotrange           = [-1, -1, 0.9, 1.15]

if scriptmode:
    showgui=T
    figfile=''
    saveinputs('plotcal',prefix+'.saved.plotcal.bandpass.field1.ant9to12amp')

    plotcal()

    print ""
    user_check=raw_input('Return to continue script\n')
else:
    showgui=F
    figfile=prefix+'.plotcal.bandpass.field1.ant9to12amp.png'
    saveinputs('plotcal',prefix+'.saved.plotcal.bandpass.field1.ant9to12amp')

    plotcal()

print ""

antenna='25'

if scriptmode:
    showgui=T
    field = '1,3~4'
    iteration='field'
    figfile=''
    saveinputs('plotcal',prefix+'.saved.plotcal.bandpass.fields.ant25amp')

    plotcal()

    print " we iterate over all three fields, just for antenna 25 using"
    print " the iteration parameter"
    print ""

    print " Make sure to click Next to go through the fields before hitting return"
    print ""
    print " note the galactic absorption in the first two fields"
    print " only field 4 (1331+305) is free of absorption"
    print " for now, we will use only bandpass solutions for field 4"
    print ""
    user_check=raw_input('Return when done to run applycal\n')

else:
    showgui=F
    field = '1'
    iteration=''
    figfile=prefix+'.plotcal.bandpass.field1.ant25amp.png'
    saveinputs('plotcal',prefix+'.saved.plotcal.bandpass.field1.ant25amp')

    plotcal()

    field = '3'
    figfile=prefix+'.plotcal.bandpass.field3.ant25amp.png'
    saveinputs('plotcal',prefix+'.saved.plotcal.bandpass.field3.ant25amp')

    plotcal()

    field = '4'
    figfile=prefix+'.plotcal.bandpass.field4.ant25amp.png'
    saveinputs('plotcal',prefix+'.saved.plotcal.bandpass.field4.ant25amp')

    plotcal()

if benchmarking:
    plotbcal2time=time.time()

#=====================================================================
#Apply calibration - results go to corrected_data column
#=====================================================================
print "--applycal--"
print ""
print " applies supplied calibration tables (gain and bandpass) and"
print " writes corrected data to column labeled CORRECTED_DATA."
print " This may take a while ..."

default('applycal')

vis=msfile
spw='0:5~112'
gaintable=[prefix+'.fcal',prefix+'.bcal']
gainfield=['1', '4']

saveinputs('applycal',prefix+'.saved.applycal')

applycal()

if benchmarking:
    correct2time=time.time()


#=====================================================================
# Flag data non-interactively
#=====================================================================
#
print "--flagdata--"

default('flagdata')
vis=msfile
spw='0'
mode='manualflag'

print ""
print " flag the narrow time range around 03:53 for RR"
print ""

# NOTE: this was flagged previously in RR if not in scriptmode, but do
# it here again for consistency between modes
#
timerange='03:52:44~03:52:46'
correlation='RR'

saveinputs('flagdata',prefix+'.saved.flagdata.time0352')

flagdata()

print ""
print " flag antenna 0 for correlation LL over the whole time range"
print ""

antenna='0'
timerange=''
correlation='LL'

saveinputs('flagdata',prefix+'.saved.flagdata.ant0.ll')

flagdata()

if benchmarking:
    flagcorrect2time=time.time()

#=====================================================================
# Split
#=====================================================================
#
# selects target source data (throws away all calibrator data).
print ""
print "--split--"
print ""
print " We are done with the calibrator data so we use split to select"
print " source data only (field '0').  split will also move the"
print " CORRECTED_DATA column to the DATA column."
print ""

default('split')

splitfile = prefix + '.split.ms'

vis       = msfile
outputvis = splitfile
field     = '0'

saveinputs('split',prefix+'.saved.split')

split()

if benchmarking:
    split2time=time.time()

#=====================================================================
# Continuum subtraction
#=====================================================================
#
# subtract continuum to form a line-only data set

print "--uvcontsub--"
print ""
print " fit a continuum using line-free regions on both ends of the spectrum"
print " and subtract this continuum.  Inspection of the earlier data cube"
print " shows that a good choice for line-free channels at either end are"
print " channels 21-30 and 92-111.  We use the parameter fitspw to specify"
print " the range of channels to base the fit on"
print ""
print " Note that no output file is needed; the results are stored in the"
print " 'corrected' data column"
print ""
print ""
print " have patience - this will take a while ..."
print ""

default('uvcontsub')

vis      = splitfile
field    = '0'
fitspw   = '0:21~30;92~111' 
fitorder = 1

saveinputs('uvcontsub',prefix+'.saved.uvcontsub')

uvcontsub()

if benchmarking:
    uvcontsub2time=time.time()

uvcontsubfile = splitfile + '.contsub'

#=====================================================================
# IMAGING
# make dirty image of channel 32
#=====================================================================
#
print "--clean (dirty image)--"
print ""
print " for now, we will image just channels 32, which is line-free"
print " and last are line-free but were not part of the line-free range in"
print " uvcontsub and their rms noise is therefore indicative of the noise in"
print " the channels with line emission.  The middle channel is an example of"
print " a channel containing line emission without continuum"
print ""
print " Note by setting niter=0 we are just imaging; not cleaning"
print ""

default('clean')

outdirty = prefix + '.dirty'

vis       = uvcontsubfile
imagename = outdirty
field     = '0'
spw       = '0'
imagermode = ''
mode      = 'channel'
nchan     = 1
start     = 32
width     = 1
niter     = 0
imsize    = [512,512]
cell      = ['4arcsec','4arcsec']
weighting = 'briggs'
robust    = 0.0

clean()

if benchmarking:
    dirty2time=time.time()

#=====================================================================
# view dirty image
#=====================================================================
#
if scriptmode:
    print "--viewer (dirty image)--"
    print ""
    print " display dirty image of chan 32"
    print ""
    
    default('viewer')

    infile = outdirty+'.image'

    viewer()

    user_check=raw_input('when done viewing, Close and hit Return to continue\n')

    print ""

#=====================================================================
# Determine the rms in chan32
#=====================================================================
#
# use the task imstat to do this
print "--imstat (dirty image)--"
print ""
default('imstat')

imagename = outdirty+'.image'
box       = '0,0,511,511'
chans     = '0'

dirty_stat = imstat()
rmsjy     = dirty_stat['sigma'][0]
rmsmjy    = 1000 * rmsjy

print " Dirty image chan32 rms = ", rmsmjy, "mJy"

print ""

if benchmarking:
    dirtystat2time=time.time()

#=====================================================================
# Now image and clean all channels
#=====================================================================
#

print ""
print "--clean--"
print ""
print " we will image all channels of interest, and are requesting"
print " cleaning by setting niter to a large number and using a"
print " threshold of 1.2mJy (2 x sigma of dirty image of ch32)"
print ""

default('clean')

outname = prefix + '.final.clean'

vis       = uvcontsubfile
imagename = outname
field     = '0'
spw       = '0'
imagermode = ''
mode      = 'channel'
nchan     = 64
start     = 32
width     = 1
niter     = 100000
threshold = 1.2
psfmode   = 'clark'
mask      = [0,0,511,511]
imsize    = [512,512]
cell      = ['4arcsec','4arcsec']
weighting = 'briggs'
robust    = 0.0

saveinputs('clean',prefix+'.saved.clean')

clean()

if benchmarking:
    clean2time=time.time()

print ""

#=====================================================================
# view result of clean
#=====================================================================
#
# view all channels if you are running in scriptmode
if scriptmode:
    print "--viewer (clean cube)--"
    print ""
    print " display continuum-free channels"
    print ""
    
    default('viewer')

    infile = outname+'.image'

    viewer()

    user_check=raw_input('when done viewing, Close and hit Return to continue\n')

    print ""

#=====================================================================
# display image header
#=====================================================================
#
# 
print "--imhead--"
print ""
print " We will need to specify a subset of this cube, so let's explore the"
print " structure of the image cube first.  Use the task imhead for this"
print ""

default('imhead')

imagename = outname+'.image'

imhead()

print ""
print " Look in the log window for the output of imhead.  You will see that"
print " the axis order is RA, Dec, Stokes, Frequency.  Keep this in mind"
print " when specifying a subset of the image cube below"
print ""

#=====================================================================
# Statistics on clean image cube and moments
#=====================================================================
#
print '--imstat (clean cube)--'
default('imstat')

imagename = outname+'.image'

# determine the stats in entire cube

srcstat = imstat()

print " Found image max = "+str(srcstat['max'][0])+" Jy"

# determine the stats in a off-source box

offbox = '10,384,195,505'
box = offbox

offstat = imstat()

print " Found off-source image rms = "+str(offstat['sigma'][0])+" Jy"

# determine the stats in line-free channels (here: 0-3)

cenbox = '128,128,384,384'
offlinechan = '0,1,2,3'

box = cenbox
chans = offlinechan

offlinestat = imstat()

print " Found off-line image rms = "+str(offlinestat['sigma'][0])+" Jy"
offline_rms = offlinestat['sigma'][0]

if benchmarking:
    stat2time=time.time()

#=====================================================================
# make a total HI map
#=====================================================================
#
# use the task immoments to do this

print ""
print "--immoments--"
print ""
print " as the final step we determine the 0'th and first moment maps, aka"
print " the total HI map and the velocity field.  For want of a better method"
print " we exclude pixel values falling in the interval given by excludepix at"
print "   cutoff = "+str(3*offline_rms)+" Jy"
print " Note the use of the rms determined earlier in imstat"
print ""

default('immoments')

momfile    = outname + '.mom'

imagename  = outname + '.image'
moments    = [0,1]
chans     = '4~56'
axis       = 'spec'
excludepix = [-100, 3*offline_rms]
outfile    = momfile

saveinputs('immoments',prefix+'.saved.immoments')

immoments()

if benchmarking:
    moments2time=time.time()

#=====================================================================
# view result of immoments; first total HI, then velocity field
#=====================================================================
#
# view if in scriptmode

if scriptmode:
    print "--viewer--"
    print ""

    default('viewer')

    infile = outfile + '.integrated'

    viewer()

    print " display moment 0 of continuum-free channels"
    print ""
    user_check=raw_input('when done viewing, Close and hit Return to continue\n')

    infile = outfile + '.weighted_coord'

    viewer()

    print ""
    print " display moment 1 of continuum-free channels"
    print ""
    user_check=raw_input('when done viewing, Close and hit Return to continue\n')

    print ""


#=====================================================================
# Statistics on moment images
#=====================================================================
#
print '--imstat (moment images)--'
default('imstat')

imagename = momfile+'.integrated'

momzerostat=imstat()

try:
    print " Found moment 0 max = "+str(momzerostat['max'][0])
    print " Found moment 0 rms = "+str(momzerostat['rms'][0])
except:
    pass

imagename = momfile+'.weighted_coord'

momonestat=imstat()

try:
    print " Found moment 1 median = "+str(momonestat['median'][0])
except:
    pass

if benchmarking:
    momstat2time=time.time()

#=====================================================================
# DONE
#=====================================================================
if benchmarking:
    endProc=time.clock()
    endTime=time.time()

print ""
print "Completed processing of NGC2403 data"
print ""

#
##########################################################################
# Calculate regression values
##########################################################################
print '--Calculate Regression Results--'
print ''
#
# Save these stats
#
try:
    dirtyrms = dirty_stat['sigma'][0]
except:
    dirtyrms = 0.0

try:
    srcmax = srcstat['max'][0]
except:
    srcmax = 0.0

try:
    offrms = offstat['sigma'][0]
except:
    offrms = 0.0

try:
    offlinerms = offlinestat['sigma'][0]
except:
    offlinerms = 0.0
    
try:
    momzero_max = momzerostat['max'][0]
except:
    momzero_max = 0.0

try:
    momzero_rms = momzerostat['rms'][0]
except:
    momzero_rms = 0.0

try:
    momone_median = momonestat['median'][0]
except:
    momone_median = 0.0

#
##########################################################################
# Canonical results to be used for regression

canonical = {}
canonical['exist'] = True

canonical['date'] = '2009-12-07 (GAM)'
canonical['version'] = 'CASA Version 3.0.0 Rev 9751'
canonical['user'] = 'gmoellen'
canonical['host'] = 'penns'
canonical['cwd'] = '/home/penns/gmoellen/CASA/REG/N2403'
print "Canonical regression from "+canonical['version']+" on "+canonical['date']

canonical_results = {}
canonical_results['dirty_image_rms'] = {}
canonical_results['dirty_image_rms']['value'] = 0.000342

canonical_results['clean_image_max'] = {}
canonical_results['clean_image_max']['value'] = 0.0231359191239

canonical_results['clean_image_offsrc_rms'] = {}
canonical_results['clean_image_offsrc_rms']['value'] = 0.000533470036927

canonical_results['clean_image_offline_rms'] = {}
canonical_results['clean_image_offline_rms']['value'] = 0.000515150649237

canonical_results['clean_momentzero_max'] = {}
canonical_results['clean_momentzero_max']['value'] = 0.551860868931

canonical_results['clean_momentzero_rms'] = {}
canonical_results['clean_momentzero_rms']['value'] = 0.0878139138222

# The following value is a velocity in BARY
#  (in LSRK, the answer is ~132.47
canonical_results['clean_momentone_median'] = {}
canonical_results['clean_momentone_median']['value'] = 129.940628


#
##########################################################################
#
# Try and load previous results from regression file
#
regression = {}
regressfile = scriptprefix + '.pickle'
prev_results = {}

try:
    fr = open(regressfile,'r')
except:
    print "No previous regression results file "+regressfile
else:
    u = pickle.Unpickler(fr)
    regression = u.load()
    fr.close()
    print "Regression results filled from "+regressfile
    print "Regression from version "+regression['version']+" on "+regression['date']
    regression['exist'] = True

    prev_results = regression['results']
    
#
##########################################################################
#
# Store results in dictionary
#
new_regression = {}

# Some date and version info
import datetime
datestring=datetime.datetime.isoformat(datetime.datetime.today())

myvers = casalog.version()
myuser = os.getenv('USER')
myhost = os.uname()[1]
mycwd = os.getcwd()
myos = os.uname()

# Save info in regression dictionary
new_regression['date'] = datestring
new_regression['version'] = myvers
new_regression['user'] = myuser
new_regression['host'] = myhost
new_regression['cwd'] = mycwd
new_regression['os'] = myos

new_regression['dataset'] = 'NGC2403 02-JAN-1999 VLA HI'

# Dataset size info
datasize_raw = 564.0 # MB
datasize_ms = 5200.0 # MB
new_regression['datasize'] = {}
new_regression['datasize']['raw'] = datasize_raw
new_regression['datasize']['ms'] = datasize_ms

#
##########################################################################
#
# Timing
#
if benchmarking:
    # Save timing to regression dictionary
    new_regression['timing'] = {}

    total = {}
    total['wall'] = (endTime - startTime)
    total['cpu'] = (endProc - startProc)
    total['rate_raw'] = (datasize_raw/(endTime - startTime))
    total['rate_ms'] = (datasize_ms/(endTime - startTime))

    new_regression['timing']['total'] = total

    nstages = 19
    new_regression['timing']['nstages'] = nstages

    stages = {}
    stages[0] = ['import',(import2time-startTime)]
    stages[1] = ['listobs',(list2time-import2time)]
    stages[2] = ['flagdata',(flag2time-list2time)]
    stages[3] = ['setjy',(setjy2time-flag2time)]
    stages[4] = ['gaincal',(gaincal2time-setjy2time)]
    stages[5] = ['plotgcal',(plotgcal2time-gaincal2time)]
    stages[6] = ['fluxscale',(fluxscale2time-plotgcal2time)]
    stages[7] = ['bandpass',(bandpass2time-fluxscale2time)]
    stages[8] = ['plotbcal',(plotbcal2time-bandpass2time)]
    stages[9] = ['applycal',(correct2time-plotbcal2time)]
    stages[10] = ['flagfinal',(flagcorrect2time-correct2time)]
    stages[11] = ['split',(split2time-flagcorrect2time)]
    stages[12] = ['uvcontsub',(uvcontsub2time-split2time)]
    stages[13] = ['clean(dirty)',(dirty2time-uvcontsub2time)]
    stages[14] = ['stat(dirty)',(dirtystat2time-dirty2time)]
    stages[15] = ['clean',(clean2time-dirtystat2time)]
    stages[16] = ['stat',(stat2time-clean2time)]
    stages[17] = ['moments',(moments2time-stat2time)]
    stages[18] = ['momstat',(momstat2time-moments2time)]
    
    new_regression['timing']['stages'] = stages

#
##########################################################################
# Fill results
# Note that 'op' tells what to do for the diff :
#    'divf' = abs( new - prev )/prev
#    'diff' = new - prev

results = {}

op = 'divf'
tol = 0.08
results['dirty_image_rms'] = {}
results['dirty_image_rms']['name'] = 'Dirty image rms'
results['dirty_image_rms']['value'] = dirtyrms
results['dirty_image_rms']['op'] = op
results['dirty_image_rms']['tol'] = tol

results['clean_image_max'] = {}
results['clean_image_max']['name'] = 'Clean image max'
results['clean_image_max']['value'] = srcmax
results['clean_image_max']['op'] = op
results['clean_image_max']['tol'] = tol

results['clean_image_offsrc_rms'] = {}
results['clean_image_offsrc_rms']['name'] = 'Clean image off-src rms'
results['clean_image_offsrc_rms']['value'] = offrms
results['clean_image_offsrc_rms']['op'] = op
results['clean_image_offsrc_rms']['tol'] = tol

results['clean_image_offline_rms'] = {}
results['clean_image_offline_rms']['name'] = 'Clean image off-line rms'
results['clean_image_offline_rms']['value'] = offlinerms
results['clean_image_offline_rms']['op'] = op
results['clean_image_offline_rms']['tol'] = tol

results['clean_momentzero_max'] = {}
results['clean_momentzero_max']['name'] = 'Moment 0 image max'
results['clean_momentzero_max']['value'] = momzero_max
results['clean_momentzero_max']['op'] = op
results['clean_momentzero_max']['tol'] = tol

results['clean_momentzero_rms'] = {}
results['clean_momentzero_rms']['name'] = 'Moment 0 image rms'
results['clean_momentzero_rms']['value'] = momzero_rms
results['clean_momentzero_rms']['op'] = op
results['clean_momentzero_rms']['tol'] = tol

op = 'diff'
tol = 0.1
results['clean_momentone_median'] = {}
results['clean_momentone_median']['name'] = 'Moment 1 image median'
results['clean_momentone_median']['value'] = momone_median
results['clean_momentone_median']['op'] = op
results['clean_momentone_median']['tol'] = tol

#
##########################################################################
# Now go through and regress
resultlist = ['dirty_image_rms','clean_image_max',
              'clean_image_offsrc_rms','clean_image_offline_rms',
              'clean_momentzero_max','clean_momentzero_rms','clean_momentone_median']

for keys in resultlist:
    res = results[keys]
    if prev_results.has_key(keys):
        # This is a known regression
        prev = prev_results[keys]
        new_val = res['value']
        prev_val = prev['value']
        if res['op'] == 'divf':
            new_diff = (new_val - prev_val)/prev_val
        else:
            new_diff = new_val - prev_val

        if abs(new_diff)>res['tol']:
            new_status = 'Failed'
        else:
            new_status = 'Passed'
        
        results[keys]['prev'] = prev_val
        results[keys]['diff'] = new_diff
        results[keys]['status'] = new_status
        results[keys]['test'] = 'Last'
    elif canonical_results.has_key(keys):
        # Go back to canonical values
        prev = canonical_results[keys]
        new_val = res['value']
        prev_val = prev['value']
        if res['op'] == 'divf':
            new_diff = (new_val - prev_val)/prev_val
        else:
            new_diff = new_val - prev_val

        if abs(new_diff)>res['tol']:
            new_status = 'Failed'
        else:
            new_status = 'Passed'
        
        results[keys]['prev'] = prev_val
        results[keys]['diff'] = new_diff
        results[keys]['status'] = new_status
        results[keys]['test'] = 'Canon'
    else:
        # Unknown regression key
        results[keys]['prev'] = 0.0
        results[keys]['diff'] = 1.0
        results[keys]['status'] = 'Missed'
        results[keys]['test'] = 'none'

# Done filling results
new_regression['results'] = results

#
##########################################################################
#
# Save regression results as dictionary using Pickle
#
pickfile = 'out.'+prefix + '.regression.'+datestring+'.pickle'
f = open(pickfile,'w')
p = pickle.Pickler(f)
p.dump(new_regression)
f.close()

print ""
print "Regression result dictionary saved in "+pickfile
print ""
print "Use Pickle to retrieve these"
print ""

# e.g.
# f = open(pickfile)
# u = pickle.Unpickler(f)
# clnmodel = u.load()
# polmodel = u.load()
# f.close()

#
##########################################################################
#
# Now print out results
# The following writes a logfile for posterity
#
##########################################################################
#
outfile='out.'+prefix+'.'+datestring+'.log'
logfile=open(outfile,'w')

# Print version info to outfile
print >>logfile,'Regression = '+new_regression['dataset']
print >>logfile,'Running '+myvers+' on host '+myhost
print >>logfile,'at '+datestring
print >>logfile,''

# Print out comparison:
print >>logfile,'---'
print >>logfile,'Regression versus previous values:'
print >>logfile,'---'

res = {}
resultlist = ['dirty_image_rms','clean_image_max',
              'clean_image_offsrc_rms','clean_image_offline_rms',
              'clean_momentzero_max','clean_momentzero_rms','clean_momentone_median']

final_status = 'Passed'
for keys in resultlist:
    res = results[keys]
    print '--%30s : %12.6f was %12.6f %4s %12.6f (%6s) %s ' % ( res['name'], res['value'], res['prev'], res['op'], res['diff'], res['status'], res['test'] )
    print >>logfile,'--%30s : %12.6f was %12.6f %4s %12.6f (%6s) %s ' % ( res['name'], res['value'], res['prev'], res['op'], res['diff'], res['status'], res['test'] )
    if res['status']=='Failed':
        final_status = 'Failed'

if (final_status == 'Passed'):
    regstate=True
    print >>logfile,'---'
    print >>logfile,'Passed Regression test for NGC 2403'
    print >>logfile,'---'
    print ''
    print 'Regression PASSED'
    print ''
    print 'Passed Regression test for NGC 2403'
else:
    regstate=False
    print >>logfile,'----FAILED Regression test for NGC 2403'
    print ''
    print 'Regression FAILED'
    print ''
    print '----FAILED Regression test for NGC 2403'
    
#
##########################################################################
# Print benchmarking etc.

if benchmarking:
    print ''
    print 'Total wall clock time was: %10.3f ' % total['wall']
    print 'Total CPU        time was: %10.3f ' % total['cpu']
    print 'Raw processing rate MB/s was: %8.1f ' % total['rate_raw']
    print 'MS  processing rate MB/s was: %8.1f ' % total['rate_ms']
    print ''
    print '* Breakdown:                              *'

    print >>logfile,''
    print >>logfile,'********* Benchmarking *************************'
    print >>logfile,'*                                              *'
    print >>logfile,'Total wall clock time was: %10.3f ' % total['wall']
    print >>logfile,'Total CPU        time was: %10.3f ' % total['cpu']
    print >>logfile,'Raw processing rate MB/s was: %8.1f ' % total['rate_raw']
    print >>logfile,'MS  processing rate MB/s was: %8.1f ' % total['rate_ms']
    print >>logfile,'* Breakdown:                                   *'

    for i in range(nstages):
        print '* %16s * time was: %10.3f ' % tuple(stages[i])
        print >>logfile,'* %16s * time was: %10.3f ' % tuple(stages[i])
    
    print >>logfile,'************************************************'
    print >>logfile,'imager-b (2008-07-24) wall time was: 2740 seconds'
    print >>logfile,'imager-b (2008-07-24) CPU  time was: 1792 seconds'

logfile.close()

print "Done with NGC2403 Tutorial Regression"
#
##########################################################################
