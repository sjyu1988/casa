import os, re
import string
from taskinit import casalog, mstool, qa, tbtool
from update_spw import update_spwchan

def split(vis, outputvis, datacolumn, field, spw, width, antenna,
          timebin, timerange, scan, array, uvrange, correlation,
          combine, keepflags):
    """Create a visibility subset from an existing visibility set:

    Keyword arguments:
    vis -- Name of input visibility file (MS)
            default: none; example: vis='ngc5921.ms'
    outputvis -- Name of output visibility file (MS)
                  default: none; example: outputvis='ngc5921_src.ms'
    datacolumn -- Which data column to split out
                  default='corrected'; example: datacolumn='data'
                  Options: 'data', 'corrected', 'model', 'all',
                  'float_data', 'lag_data', 'float_data,data', and
                  'lag_data,data'.
                  note: 'all' = whichever of the above that are present.
    field -- Field name
              default: field = '' means  use all sources
              field = 1 # will get field_id=1 (if you give it an
                          integer, it will retrieve the source with that index)
              field = '1328+307' specifies source '1328+307'.
                 Minimum match can be used, egs  field = '13*' will
                 retrieve '1328+307' if it is unique or exists.
                 Source names with imbedded blanks cannot be included.
    spw -- Spectral window index identifier
            default=-1 (all); example: spw=1
    antenna -- antenna names
               default '' (all),
               antenna = '3 & 7' gives one baseline with antennaid = 3,7.
    timebin -- Interval width for time averaging.
               default: '0s' or '-1s' (no averaging)
               example: timebin='30s'
    timerange -- Time range
                 default='' means all times.  examples:
                 timerange = 'YYYY/MM/DD/hh:mm:ss~YYYY/MM/DD/hh:mm:ss'
                 timerange='< YYYY/MM/DD/HH:MM:SS.sss'
                 timerange='> YYYY/MM/DD/HH:MM:SS.sss'
                 timerange='< ddd/HH:MM:SS.sss'
                 timerange='> ddd/HH:MM:SS.sss'
    scan -- Scan numbers to select.
            default '' (all).
    array -- (Sub)array IDs to select.     
             default '' (all).
    uvrange -- uv distance range to select.
               default '' (all).
    correlation -- Select correlations, e.g. 'rr, ll' or ['XY', 'YX'].
                   default '' (all).
    combine -- Data descriptors that time averaging can ignore:
                  scan, and/or state
                  Default '' (none)
    keepflags -- Keep flagged data, if possible
                 Default True
    """
    retval = True
    casalog.origin('split')

    if not outputvis or outputvis.isspace():
        raise ValueError, 'Please specify outputvis'

    myms = mstool.create()
    if ((type(vis)==str) & (os.path.exists(vis))):
        myms.open(vis, nomodify=True)
    else:
        raise ValueError, 'Visibility data set not found - please verify the name'
    if os.path.exists(outputvis):
        myms.close()
        raise ValueError, "Output MS %s already exists - will not overwrite." % outputvis

    # No longer needed.  When did it get put in?  Note that the default
    # spw='*' in myms.split ends up as '' since the default type for a variant
    # is BOOLVEC.  (Of course!)  Therefore both split and myms.split must
    # work properly when spw=''.
    #if(spw == ''):
    #    spw = '*'
    
    if(type(antenna) == list):
        antenna = ', '.join([str(ant) for ant in antenna])

    ## Accept digits without units ...assume seconds
    timebin = qa.convert(qa.quantity(timebin), 's')['value']
    timebin = str(timebin) + 's'
    
    if timebin == '0s':
        timebin = '-1s'

    if '^' in spw:
        casalog.post("The interpretation of ^n in split's spw strings has changed from 'average n' to 'skip n' channels!", 'WARN')
        casalog.post("Watch for Slicer errors", 'WARN')
        
    if type(width) == str:
        try:
            if(width.isdigit()):
                width=[string.atoi(width)]
            elif(width.count('[') == 1 and width.count(']') == 1):
                width = width.replace('[', '')
                width = width.replace(']', '')
                splitwidth = width.split(',')
                width = []
                for ws in splitwidth:
                    if(ws.isdigit()):
                        width.append(string.atoi(ws)) 
            else:
                width = [1]
        except:
            raise TypeError, 'parameter width is invalid...using 1'

    if type(correlation) == list:
        correlation = ', '.join(correlation)
    correlation = correlation.upper()

    if hasattr(combine, '__iter__'):
        combine = ', '.join(combine)

    if type(spw) == list:
        spw = ','.join([str(s) for s in spw])
    elif type(spw) == int:
        spw = str(spw)
    do_chan_mod = spw.find('^') > -1     # '0:2~11^1' would be pointless.
    if not do_chan_mod:                  # ...look in width.
        if type(width) == int and width > 1:
            do_chan_mod = True
        elif hasattr(width, '__iter__'):
            for w in width:
                if w > 1:
                    do_chan_mod = True
                    break

    do_both_chan_and_time_mod = (do_chan_mod and
                                 string.atof(timebin[:-1]) > 0.0)
    if do_both_chan_and_time_mod:
        # Do channel averaging first because it might be included in the spw
        # string.
        import tempfile
        # We want the directory outputvis is in, not /tmp, because /tmp
        # might not have enough space.
        # outputvis is itself a directory, so strip off a trailing slash if
        # it is present.
        # I don't know if giving tempfile an absolute directory is necessary -
        # dir='' is effectively '.' in Ubuntu.
        workingdir = os.path.abspath(os.path.dirname(outputvis.rstrip('/')))
        cavms = tempfile.mkdtemp(suffix=outputvis, dir=workingdir)

        casalog.post('Channel averaging to ' + cavms)
        myms.split(outputms=cavms,     field=field,
                   spw=spw,            step=width,
                   baseline=antenna,   subarray=array,
                   timebin='',         time=timerange,
                   whichcol=datacolumn,
                   scan=scan,          uvrange=uvrange,
                   combine=combine,
                   correlation=correlation)
        
        # The selection was already made, so blank them before time averaging.
        field = ''
        spw = ''
        width = [1]
        antenna = ''
        array = ''
        timerange = ''
        datacolumn = 'all'
        scan = ''
        uvrange = ''

        myms.close()
        myms.open(cavms)
        casalog.post('Starting time averaging')

    if keepflags:
        taqlstr = ''
    else:
        taqlstr = 'NOT (FLAG_ROW OR ALL(FLAG))'

    myms.split(outputms=outputvis,  field=field,
             spw=spw,             step=width,
             baseline=antenna,    subarray=array,
             timebin=timebin,     time=timerange,
             whichcol=datacolumn,
             scan=scan,           uvrange=uvrange,
             combine=combine,
             correlation=correlation,
             taql=taqlstr)
    myms.close()

    if do_both_chan_and_time_mod:
        import shutil
        shutil.rmtree(cavms)

    # Update FLAG_CMD if necessary.
    if ((spw != '') and (spw != '*')) or do_chan_mod:
        isopen = False
        try:
            mytb = tbtool.create()
            mytb.open(outputvis + '/FLAG_CMD', nomodify=False)
            isopen = True
            #print "is open"
            nflgcmds = mytb.nrows()
            #print "nflgcmds =", nflgcmds
            if nflgcmds > 0:
                mademod = False
                cmds = mytb.getcol('COMMAND')
                for rownum in xrange(nflgcmds):
                    # Matches a bare number or a string quoted any way.
                    spwmatch = re.search(r'spw\s*=\s*(\S+)', cmds[rownum])
                    if spwmatch:
                        sch1 = spwmatch.groups()[0]
                        sch1 = re.sub(r"[\'\"]", '', sch1)  # Dequote
                        # Provide a default in case the split selection excludes
                        # cmds[rownum].  update_spwchan() will throw an exception
                        # in that case.
                        cmd = ''
                        try:
                            widths = {}
                            if hasattr(width, 'has_key'):
                                widths = width
                            else:
                                if hasattr(width, '__iter__'):
                                    for i in xrange(len(width)):
                                        widths[i] = width[i]
                                elif width != 1:
                                    #print 'using myms.msseltoindex + a scalar width'
                                    nspw = len(myms.msseltoindex(vis=vis,
                                                                 spw='*')['spw'])
                                    for i in xrange(nspw):
                                        widths[i] = width
                            #print 'sch1 =', sch1
                            sch2 = update_spwchan(vis, spw, sch1, truncate=True,
                                                  widths=widths)
                            #print 'sch2 =', sch2
                            #print 'spwmatch.group() =', spwmatch.group()
                            if sch2:
                                repl = ''
                                if sch2 != '*':
                                    repl = "spw='" + sch2 + "'"
                                cmd = cmds[rownum].replace(spwmatch.group(), repl)
                        #except: # cmd[rownum] no longer applies.
                        except Exception, e:
                            casalog.post(
                                "Error %s updating row %d of FLAG_CMD" % (e,
                                                                          rownum),
                                         'WARN')
                            casalog.post('sch1 = ' + sch1, 'DEBUG1')
                            casalog.post('cmd = ' + cmd, 'DEBUG1')
                        if cmd != cmds[rownum]:
                            mademod = True
                            cmds[rownum] = cmd
                if mademod:
                    casalog.post('Updating FLAG_CMD', 'INFO')
                    mytb.putcol('COMMAND', cmds)
        except Exception, instance:
            casalog.post("*** Error \'%s\' updating FLAG_CMD" % (instance),
                         'SEVERE')
            retval = False
        finally:
            if isopen:
                casalog.post('Closing FLAG_CMD', 'DEBUG1')
                mytb.close()
    
    # Write history to output MS, not the input ms.
    isopen = False
    try:
        myms.open(outputvis, nomodify=False)
        isopen = True
        myms.writehistory(message='taskname=split', origin='split')
        # Write the arguments.
        for arg in split.func_code.co_varnames[:split.func_code.co_argcount]:
            msg = "%-11s = " % arg
            val = eval(arg)
            if type(val) == str:
                msg += '"'
            msg += str(val)
            if type(val) == str:
                msg += '"'
            myms.writehistory(message=msg, origin='split')
    except Exception, instance:
        casalog.post("*** Error \'%s\' updating HISTORY" % (instance),
                     'SEVERE')
        retval = False
    finally:
        if isopen:
            myms.close()

    return retval
