from asap.scantable import scantable
from asap import rcParams
from asap import print_log
from asap import selector

def average_time(*args, **kwargs):
    """
    Return the (time) average of a scan or list of scans. [in channels only]
    The cursor of the output scan is set to 0
    Parameters:
        one scan or comma separated  scans or a list of scans
        mask:     an optional mask (only used for 'var' and 'tsys' weighting)
        scanav:   True averages each scan separately.
                  False (default) averages all scans together,
        weight:   Weighting scheme.
                    'none'     (mean no weight)
                    'var'      (1/var(spec) weighted)
                    'tsys'     (1/Tsys**2 weighted)
                    'tint'     (integration time weighted)
                    'tintsys'  (Tint/Tsys**2)
                    'median'   ( median averaging)
        align:    align the spectra in velocity before averaging. It takes
                  the time of the first spectrum in the first scantable
                  as reference time.
    Example:
        # return a time averaged scan from scana and scanb
        # without using a mask
        scanav = average_time(scana,scanb)
	# or equivalent
	# scanav = average_time([scana, scanb])
        # return the (time) averaged scan, i.e. the average of
        # all correlator cycles
        scanav = average_time(scan, scanav=True)
    """
    scanav = False
    if kwargs.has_key('scanav'):
       scanav = kwargs.get('scanav')
    weight = 'tint'
    if kwargs.has_key('weight'):
       weight = kwargs.get('weight')
    mask = ()
    if kwargs.has_key('mask'):
        mask = kwargs.get('mask')
    align = False
    if kwargs.has_key('align'):
        align = kwargs.get('align')
    compel = False
    if kwargs.has_key('compel'):
        compel = kwargs.get('compel')
    varlist = vars()
    if isinstance(args[0],list):
        lst = args[0]
    elif isinstance(args[0],tuple):
        lst = list(args[0])
    else:
        lst = list(args)

    del varlist["kwargs"]
    varlist["args"] = "%d scantables" % len(lst)
    # need special formatting here for history...

    from asap._asap import stmath
    stm = stmath()
    for s in lst:
        if not isinstance(s,scantable):
            msg = "Please give a list of scantables"
            if rcParams['verbose']:
                print msg
                return
            else:
                raise TypeError(msg)
    if scanav: scanav = "SCAN"
    else: scanav = "NONE"
    alignedlst = []
    if align:
        refepoch = lst[0].get_time(0)
        for scan in lst:
            alignedlst.append(scan.freq_align(refepoch,insitu=False))
    else:
        alignedlst = lst
    if weight.upper() == 'MEDIAN':
        # median doesn't support list of scantables - merge first
        merged = None
        if len(alignedlst) > 1:
            merged = merge(alignedlst)
        else:
            merged = alignedlst[0]
        s = scantable(stm._averagechannel(merged, 'MEDIAN', scanav))
        del merged
    else:
        #s = scantable(stm._average(alignedlst, mask, weight.upper(), scanav))
        s = scantable(stm._new_average(alignedlst, compel, mask, weight.upper(), scanav))
    s._add_history("average_time",varlist)
    print_log()
    return s

def quotient(source, reference, preserve=True):
    """
    Return the quotient of a 'source' (signal) scan and a 'reference' scan.
    The reference can have just one scan, even if the signal has many. Otherwise
    they must have the same number of scans.
    The cursor of the output scan is set to 0
    Parameters:
        source:        the 'on' scan
        reference:     the 'off' scan
        preserve:      you can preserve (default) the continuum or
                       remove it.  The equations used are
                       preserve:  Output = Toff * (on/off) - Toff
                       remove:    Output = Toff * (on/off) - Ton
    """
    varlist = vars()
    from asap._asap import stmath
    stm = stmath()
    stm._setinsitu(False)
    s = scantable(stm._quotient(source, reference, preserve))
    s._add_history("quotient",varlist)
    print_log()
    return s

def dototalpower(calon, caloff, tcalval=0.0):
    """
    Do calibration for CAL on,off signals.
    Adopted from GBTIDL dototalpower
    Parameters:
        calon:         the 'cal on' subintegration
        caloff:        the 'cal off' subintegration
        tcalval:       user supplied Tcal value
    """
    varlist = vars()
    from asap._asap import stmath
    stm = stmath()
    stm._setinsitu(False)
    s = scantable(stm._dototalpower(calon, caloff, tcalval))
    s._add_history("dototalpower",varlist)
    print_log()
    return s

def dosigref(sig, ref, smooth, tsysval=0.0, tauval=0.0):
    """
    Calculate a quotient (sig-ref/ref * Tsys)
    Adopted from GBTIDL dosigref
    Parameters:
        sig:         on source data
        ref:         reference data
        smooth:      width of box car smoothing for reference
        tsysval:     user specified Tsys (scalar only)
        tauval:      user specified Tau (required if tsysval is set)
    """
    varlist = vars()
    from asap._asap import stmath
    stm = stmath()
    stm._setinsitu(False)
    s = scantable(stm._dosigref(sig, ref, smooth, tsysval, tauval))
    s._add_history("dosigref",varlist)
    print_log()
    return s

def calps(scantab, scannos, smooth=1, tsysval=0.0, tauval=0.0, tcalval=0.0):
    """
    Calibrate GBT position switched data
    Adopted from GBTIDL getps
    Currently calps identify the scans as position switched data if they
    contain '_ps' in the source name. The data must contains 'CAL' signal
    on/off in each integration. To identify 'CAL' on state, the word, 'calon'
    need to be present in the source name field.
    (GBT MS data reading process to scantable automatically append these
    id names to the source names)

    Parameters:
        scantab:       scantable
        scannos:       list of scan numbers
        smooth:        optional box smoothing order for the reference
                       (default is 1 = no smoothing)
        tsysval:       optional user specified Tsys (default is 0.0,
                       use Tsys in the data)
        tauval:        optional user specified Tau
        tcalval:       optional user specified Tcal (default is 0.0,
                       use Tcal value in the data)
    """
    varlist = vars()
    # check for the appropriate data
    s = scantab.get_scan('*_ps*')
    if s is None:
        msg = "The input data appear to contain no position-switch mode data."
        if rcParams['verbose']:
            print msg
            return
        else:
            raise TypeError(msg)
    ssub = s.get_scan(scannos)
    if ssub is None:
        msg = "No data was found with given scan numbers!"
        if rcParams['verbose']:
            print msg
            return
        else:
            raise TypeError(msg)
    ssubon = ssub.get_scan('*calon')
    ssuboff = ssub.get_scan('*[^calon]')
    if ssubon.nrow() != ssuboff.nrow():
        msg = "mismatch in numbers of CAL on/off scans. Cannot calibrate. Check the scan numbers."
        if rcParams['verbose']:
            print msg
            return
        else:
            raise TypeError(msg)
    cals = dototalpower(ssubon, ssuboff, tcalval)
    sig = cals.get_scan('*ps')
    ref = cals.get_scan('*psr')
    if sig.nscan() != ref.nscan():
        msg = "mismatch in numbers of on/off scans. Cannot calibrate. Check the scan numbers."
        if rcParams['verbose']:
            print msg
            return
        else:
            raise TypeError(msg)

    #for user supplied Tsys
    if tsysval>0.0:
        if tauval<=0.0:
            msg = "Need to supply a valid tau to use the supplied Tsys"
            if rcParams['verbose']:
                print msg
                return
            else:
                raise TypeError(msg)
        else:
            sig.recalc_azel()
            ref.recalc_azel()
            #msg = "Use of user specified Tsys is not fully implemented yet."
            #if rcParams['verbose']:
            #    print msg
            #    return
            #else:
            #    raise TypeError(msg)
            # use get_elevation to get elevation and
            # calculate a scaling factor using the formula
            # -> tsys use to dosigref

    #ress = dosigref(sig, ref, smooth, tsysval)
    ress = dosigref(sig, ref, smooth, tsysval, tauval)
    ress._add_history("calps", varlist)
    print_log()
    return ress

def calnod(scantab, scannos=[], smooth=1, tsysval=0.0, tauval=0.0, tcalval=0.0):
    """
    Do full (but a pair of scans at time) processing of GBT Nod data
    calibration.
    Adopted from  GBTIDL's getnod
    Parameters:
        scantab:     scantable
        scannos:     a pair of scan numbers, or the first scan number of the pair
        smooth:      box car smoothing order
        tsysval:     optional user specified Tsys value
        tauval:      optional user specified tau value (not implemented yet)
        tcalval:     optional user specified Tcal value
    """
    varlist = vars()
    from asap._asap import stmath
    stm = stmath()
    stm._setinsitu(False)

    # check for the appropriate data
    s = scantab.get_scan('*_nod*')
    if s is None:
        msg = "The input data appear to contain no Nod observing mode data."
        if rcParams['verbose']:
            print msg
            return
        else:
            raise TypeError(msg)

    # need check correspondance of each beam with sig-ref ...
    # check for timestamps, scan numbers, subscan id (not available in
    # ASAP data format...). Assume 1st scan of the pair (beam 0 - sig
    # and beam 1 - ref...)
    # First scan number of paired scans or list of pairs of
    # scan numbers (has to have even number of pairs.)

    #data splitting
    scan1no = scan2no = 0

    if len(scannos)==1:
        scan1no = scannos[0]
        scan2no = scannos[0]+1
        pairScans = [scan1no, scan2no]
    else:
        #if len(scannos)>2:
        #    msg = "calnod can only process a pair of nod scans at time."
        #    if rcParams['verbose']:
        #        print msg
        #        return
        #    else:
        #        raise TypeError(msg)
        #
        #if len(scannos)==2:
        #    scan1no = scannos[0]
        #    scan2no = scannos[1]
        pairScans = list(scannos)

    if tsysval>0.0:
        if tauval<=0.0:
            msg = "Need to supply a valid tau to use the supplied Tsys"
            if rcParams['verbose']:
                print msg
                return
            else:
                raise TypeError(msg)
        else:
            scantab.recalc_azel()
    resspec = scantable(stm._donod(scantab, pairScans, smooth, tsysval,tauval,tcalval))
    resspec._add_history("calnod",varlist)
    print_log()
    return resspec

def calfs(scantab, scannos=[], smooth=1, tsysval=0.0, tauval=0.0, tcalval=0.0):
    """
    Calibrate GBT frequency switched data.
    Adopted from GBTIDL getfs.
    Currently calfs identify the scans as frequency switched data if they
    contain '_fs' in the source name. The data must contains 'CAL' signal
    on/off in each integration. To identify 'CAL' on state, the word, 'calon'
    need to be present in the source name field.
    (GBT MS data reading via scantable automatically append these
    id names to the source names)

    Parameters:
        scantab:       scantable
        scannos:       list of scan numbers
        smooth:        optional box smoothing order for the reference
                       (default is 1 = no smoothing)
        tsysval:       optional user specified Tsys (default is 0.0,
                       use Tsys in the data)
        tauval:        optional user specified Tau
    """
    varlist = vars()
    from asap._asap import stmath
    stm = stmath()
    stm._setinsitu(False)

#    check = scantab.get_scan('*_fs*')
#    if check is None:
#        msg = "The input data appear to contain no Nod observing mode data."
#        if rcParams['verbose']:
#            print msg
#            return
#        else:
#            raise TypeError(msg)
    s = scantab.get_scan(scannos)
    del scantab

    resspec = scantable(stm._dofs(s, scannos, smooth, tsysval,tauval,tcalval))
    resspec._add_history("calfs",varlist)
    print_log()
    return resspec

def simple_math(left, right, op='add', tsys=True):
    """
    Apply simple mathematical binary operations to two
    scan tables,  returning the result in a new scan table.
    The operation is applied to both the correlations and the TSys data
    The cursor of the output scan is set to 0
    Parameters:
        left:          the 'left' scan
        right:         the 'right' scan
        op:            the operation: 'add' (default), 'sub', 'mul', 'div'
        tsys:          if True (default) then apply the operation to Tsys
                       as well as the data
    """
    print "simple_math is deprecated use +=/* instead."

def merge(*args):
    """
    Merge a list of scanatables, or comma-sperated scantables into one
    scnatble.
    Parameters:
        A list [scan1, scan2] or scan1, scan2.
    Example:
        myscans = [scan1, scan2]
	allscans = merge(myscans)
	# or equivalent
	sameallscans = merge(scan1, scan2)
    """
    varlist = vars()
    if isinstance(args[0],list):
        lst = tuple(args[0])
    elif isinstance(args[0],tuple):
        lst = args[0]
    else:
        lst = tuple(args)
    varlist["args"] = "%d scantables" % len(lst)
    # need special formatting her for history...
    from asap._asap import stmath
    stm = stmath()
    for s in lst:
        if not isinstance(s,scantable):
            msg = "Please give a list of scantables"
            if rcParams['verbose']:
                print msg
                return
            else:
                raise TypeError(msg)
    s = scantable(stm._merge(lst))
    s._add_history("merge", varlist)
    print_log()
    return s

