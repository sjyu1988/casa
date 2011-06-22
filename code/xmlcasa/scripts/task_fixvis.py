from taskinit import *
import shutil

def fixvis(vis, outputvis='',field='', refcode='', reuse=True, phasecenter=''):
    """
    Input Parameters
    vis        -- Name of the input visibility set
    
    outputvis  -- Name of the output visibility set, default: same as vis

    field      -- field selection string

    refcode    -- Reference frame to convert to, default: the refcode of PHASE_DIR in the FIELD table
                  example: 'B1950'
    
    reuse      -- base recalculation on existing UVW coordinates? default=True
                  ignored if parameter 'phasecenter' is set

    phasecenter  --  if set to a valid direction: change the phase center for the given field
                     to this value
                     example: 'J2000 9h25m00s 05d12m00s'
                     If given without the equinox, e.g. '0h01m00s 00d12m00s', the parameter
                     is interpreted as a pair of offsets in RA and DEC to the present phasecenter.

    Examples:

    fixvis('NGC3256.ms','NGC3256-fixed.ms')
          will recalculate the UVW coordinates for all fields based on the existing
          phase center information in the FIELD table.

    fixvis('0925+0512.ms','0925+0512-fixed.ms','0925+0512', '', 'J2000 9h25m00s 05d12m00s')
          will set the phase center for field '0925+0512' to the given direction and recalculate
          the UVW coordinates.
    """

    tbt = casac.homefinder.find_home_by_name('tableHome').create()

    casalog.origin('fixvis')
    try:
        if(vis==outputvis or outputvis==''):
            casalog.post('Will overwrite original MS ...', 'NORMAL')
            outputvis = vis
        else:
            casalog.post('Copying original MS to outputvis ...', 'NORMAL')
            shutil.rmtree(outputvis, ignore_errors=True)
            shutil.copytree(vis, outputvis)

        if(field==''):
            field='*'
            
        fields = ms.msseltoindex(vis=outputvis,field=field)['field']

        if(len(fields) == 0):
            casalog.post( "Field selection returned zero results.", 'WARN')
            return False
        
        #determine therefcode, the reference frame to be used for the output UVWs
        tbt.open(outputvis+"/FIELD")
        numfields = tbt.nrows()
        therefcode = 'J2000'
        ckwdict = tbt.getcolkeyword('PHASE_DIR', 'MEASINFO')
        tbt.close()
        if(refcode==''):
            if(ckwdict.has_key('TabRefTypes')): # we have a variable reference column
                therefcode = 'J2000' # always use "J2000"
            else: # not a variable reference column
                therefcode = ckwdict['Ref']
        else: # a refcode was given, enforce validity
            if not (type(refcode)==str):
                casalog.post('Invalid refcode '+str(refcode), 'SEVERE')
                return False                
            if(ckwdict.has_key('TabRefTypes')): # variable ref column
                refcodelist = ckwdict['TabRefTypes'].tolist()
                ref = 0
                if not (refcode in refcodelist):
                    casalog.post('Invalid refcode '+refcode, 'SEVERE')
                    return False
            else: # not a variable reference column
                validcodes = me.listcodes(me.direction('J2000', '0','0'))['normal']
                if not (refcode in validcodes):
                    casalog.post('Invalid refcode '+refcode, 'SEVERE')
                    return False
            #endif
            therefcode = refcode
        #end if

        if(phasecenter==''): # we are only modifying the UVW coordinates        
            casalog.post('Will leave phase centers unchanged.', 'NORMAL')
                
            casalog.post("Recalculating the UVW coordinates ...", 'NORMAL')

            fldids = []
            for i in xrange(numfields):
                if (i in fields):
                    fldids.append(i)

            im.open(outputvis, usescratch=True) # usescratch=True needed in order to have writable ms
            im.calcuvw(fields=fldids, refcode=therefcode, reuse=reuse)
            im.close()

        else: # we are modifying UVWs and visibilities

            for fld in fields:
                viaoffset = False
                thenewra_rad = 0.
                thenewdec_rad = 0.
                thenewref = -1
                theoldref = -1
                theoldrefstr = ''
                thenewrefstr = ''
                isvarref = False
                tbt.open(outputvis+"/FIELD")
                ckwdict = tbt.getcolkeyword('PHASE_DIR', 'MEASINFO')
                theolddir = tbt.getcell('PHASE_DIR',fld)
                fieldname = tbt.getcell('NAME',fld)
                if(ckwdict.has_key('TabRefTypes') and ckwdict.has_key('TabRefCodes')):
                    isvarref = True
                    # determine string name of the phase dir reference frame
                    theoldref = tbt.getcol('PhaseDir_Ref')[fld]
                    refcodestrlist = ckwdict['TabRefTypes'].tolist()
                    refcodelist = ckwdict['TabRefCodes'].tolist()
                    if not (theoldref in refcodelist):
                        casalog.post('Invalid refcode in FIELD column PhaseDir_Ref: '+str(theoldref), 'SEVERE')
                        return False
                    tindex = refcodelist.index(theoldref)
                    theoldrefstr = refcodestrlist[tindex]
                else:
                    tmprec = tbt.getcolkeyword('PHASE_DIR', 'MEASINFO')
                    theoldrefstr = tmprec['Ref']
                tbt.close()

                theoldphasecenter = theoldrefstr+' '+qa.time(qa.quantity(theolddir[0],'rad'),14)+' '+qa.angle(qa.quantity(theolddir[1],'rad'),14)

                if (theoldref<32 and not theoldrefstr in ['J2000', 'B1950', 'B1950_VLA', 'HADEC']):
                    casalog.post('Refcode for FIELD column PHASE_DIR is valid but not supported here: '+theoldrefstr, 'SEVERE')
                    return False                            

                casalog.post( 'field: '+fieldname, 'NORMAL')
                casalog.post( 'old phasecenter RA, DEC '+theoldrefstr+' '+qa.time(qa.quantity(theolddir[0],'rad'),10) # 10 digits precision
                              +" "+ qa.angle(qa.quantity(theolddir[1],'rad'),10), 'NORMAL')
                casalog.post( '          RA, DEC (rad) '+theoldrefstr+' '+str(theolddir[0])+" " +str(theolddir[1]), 'NORMAL')

                
                if(therefcode!='J2000'):
                    casalog.post("When changing phase center, can only write new UVW coordinates in J2000.", 'WARN')
                    therefcode='J2000'
                if(reuse):
                    casalog.post("When changing phase center, UVW coordinates will be recalculated.", 'NORMAL')
                    reuse=False
                if(type(phasecenter)!=str):
                    casalog.post("Invalid phase center.", 'SEVERE')
                    return False
                else:
                    dirstr = phasecenter.split(' ')
                    if len(dirstr)==2: # interpret phasecenter as an offset
                        casalog.post("No equinox given in parameter \'phasecenter\': "+phasecenter, 'NORMAL')         
                        casalog.post("Interpreting it as pair of offsets in (RA,DEC) ...", 'NORMAL')

                        if (isvarref and theoldref>31):
                            casalog.post('*** Refcode in FIELD column PhaseDir_Ref is a solar system object: '+theoldrefstr, 'NORMAL')
                            casalog.post('*** Will use the nominal entry in the PHASE_DIR column to calculate new phase center', 'NORMAL')
                            
                        qra = qa.quantity(theolddir[0], 'rad') 
                        qdec = qa.quantity(theolddir[1], 'rad')
                        qraoffset = qa.quantity(dirstr[0])
                        qdecoffset = qa.quantity(dirstr[1])
                        if not qa.isangle(qdecoffset):
                            casalog.post("Invalid phasecenter parameter. DEC offset must be an angle.", 'SEVERE')
                            return False
                        qnewdec = qa.add(qdec,qdecoffset)
                        qnewra = qra
                        if qa.canonical(qraoffset)['unit']=='rad':
                            casalog.post("RA offset is an angle (not a time). Will divide by cos(DEC) to compute time offset.")
                            if(qa.cos(qnewdec)['value']==0.):
                                casalog.post("Resulting DEC is at celestial pole. Will ignore RA offset.", 'WARN')
                            else:
                                qraoffset = qa.div(qraoffset, qa.cos(qnewdec))
                                qnewra = qa.add(qnewra, qraoffset)
                        else:
                            if not (qa.canonical(qraoffset)['unit']=='s'):
                                casalog.post("Invalid phasecenter parameter. RA offset must be an angle or a time.", 'SEVERE')
                                return False
                            qraoffset = qa.convert(qraoffset, 'deg')
                            qnewra = qa.add(qnewra, qraoffset)
                        dirstr = [theoldrefstr, qa.time(qnewra,12), qa.angle(qnewdec,12)]
                    elif not len(dirstr)==3:
                        casalog.post('Incorrectly formatted parameter \'phasecenter\': '+phasecenter, 'SEVERE')
                        return False
                    
                    try:
                        thedir = me.direction(dirstr[0], dirstr[1], dirstr[2])
                        thenewra_rad = thedir['m0']['value']
                        thenewdec_rad = thedir['m1']['value']
                    except Exception, instance:
                        casalog.post("*** Error \'%s\' when interpreting parameter \'phasecenter\': " % (instance), 'SEVERE')
                        return False
                    if(isvarref):
                        thenewrefindex = ckwdict['TabRefTypes'].tolist().index(dirstr[0])
                        thenewref = ckwdict['TabRefCodes'][thenewrefindex]
                        thenewrefstr = dirstr[0]
                    else: # not a variable ref col
                        validcodes = me.listcodes(me.direction('J2000', '0','0'))['normal']
                        if(dirstr[0] in validcodes):
                            thenewref = validcodes.tolist().index(dirstr[0])
                            thenewrefstr = dirstr[0]
                        else:
                            casalog.post('Invalid refcode '+refcode, 'SEVERE')
                            return False
                        if(dirstr[0] != ckwdict['Ref']):
                            allselected = True
                            for i in range(0, len(refcol)):
                                if not (i in fields):
                                    allselected = False
                            if not allselected:
                                casalog.post("PHASE_DIR is not a variable reference column. Please provide phase dir in "+ckwdict['Ref'], 'SEVERE')
                                return False
                            else:
                                casalog.post("The direction column reference frame in the FIELD table will be changed from "
                                             +ckwdict['Ref']+" to "+dirstr[0], 'NORMAL')
                    #endif
                                
                #endif
                    
                casalog.post( 'new phasecenter RA, DEC '+thenewrefstr+' '+qa.time(qa.quantity(thenewra_rad,'rad'),10)
                              +" "+ qa.angle(qa.quantity(thenewdec_rad,'rad'),10), 'NORMAL')
                casalog.post( '          RA, DEC (rad) '+thenewrefstr+' '+str(thenewra_rad)+" "+ str(thenewdec_rad), 'NORMAL')

                # modify FIELD table                
                tbt.open(outputvis+'/FIELD', nomodify=False)
                pcol = tbt.getcol('PHASE_DIR')
                pcol[0][0][fld] = thenewra_rad
                pcol[1][0][fld] = thenewdec_rad
                tbt.putcol('PHASE_DIR',pcol)
                
                casalog.post("FIELD table PHASE_DIR column changed for field "+str(fld)+".", 'NORMAL')

                if(thenewref!=-1):
                    # modify reference of the phase dir column; check also the other direction columns
                    theoldref2 = -1
                    theoldref3 = -1
                    if(isvarref):
                        pcol = tbt.getcol('PhaseDir_Ref')
                        #theoldref was already determined further above
                        #theoldref = pcol[fld]
                        pcol[fld] = thenewref
                        
                        pcol2 = tbt.getcol('DelayDir_Ref')
                        theoldref2 = pcol2[fld]
                        
                        pcol3 = tbt.getcol('RefDir_Ref')
                        theoldref3 = pcol3[fld]
                            
                        if(theoldref != thenewref):
                            tbt.putcol('PhaseDir_Ref', pcol)
                            casalog.post("FIELD table phase center direction reference frame for field "+str(fld)
                                         +" set to "+str(thenewref)+" ("+thenewrefstr+")", 'NORMAL')
                            if not (thenewref==theoldref2 and thenewref==theoldref3):
                                casalog.post("*** The three FIELD table direction reference frame entries for field "+str(fld)
                                             +" will not be identical in the output data: "
                                             +str(thenewref)+", "+str(theoldref2)+", "+str(theoldref3), 'NORMAL')
                                if not (theoldref==theoldref2 and theoldref==theoldref3):
                                    casalog.post("*** The three FIELD table direction reference frame entries for field "+str(fld)
                                                 +" were not identical in the input data either: "
                                                 +str(theoldref)+", "+str(theoldref2)+", "+str(theoldref3), 'NORMAL')
                        else:
                            casalog.post("FIELD table direction reference frame entries for field "+str(fld)
                                         +" unchanged.", 'NORMAL')

                    else: # not a variable reference column
                        tmprec = tbt.getcolkeyword('PHASE_DIR', 'MEASINFO')
                        if(theoldrefstr != thenewrefstr):
                            tmprec['Ref'] = thenewrefstr
                            tbt.putcolkeyword('PHASE_DIR', 'MEASINFO', tmprec) 
                            casalog.post("FIELD table phase center direction reference frame changed from "
                                         +oldref+" to "+thenewrefstr, 'NORMAL')

                tbt.close()

                fldids = []
                phdirs = []
                for i in xrange(numfields):
                    if (i==fld):
                        fldids.append(i)
                        phdirs.append(theoldphasecenter)

                im.open(outputvis, usescratch=True) # usescratch=True needed in order to have writable ms
                im.fixvis(fields=fldids, phasedirs=phdirs, refcode=therefcode)
                im.close()

            #end for

        #endif change phasecenter

        return True

    except Exception, instance:
        casalog.post("*** Error \'%s\' " % (instance), 'SEVERE')
        return False

