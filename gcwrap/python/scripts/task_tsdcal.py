import sys
import os
import numpy

from taskinit import *
from applycal import applycal
import types
import sdutil

(cb,) = gentools(['cb'])
def tsdcal(infile=None, calmode='tsys', fraction='10%', noff=-1,
           width=0.5, elongated=False, applytable='',interp='', spwmap={},
           field='', spw='', scan='', pol='', outfile='', overwrite=False): 
       
    """ Externally specify calibration solutions af various types
    """

    #Python script
    try:

        if ((type(calmode)==str) and calmode=='tsys'):
            if scan!='':
                raise Exception,'Scan input must be \'\'(\'\'=all) in calmode=\'tsys\'.'

        if ((type(infile)==str) and (os.path.exists(infile))):
            # don't need scr col for this
            cb.open(filename=infile,compress=False,addcorr=True,addmodel=False)
            cb.selectvis(spw=spw, scan=scan, field=field)
            #cb.setsolve(type=calmodemap[calmode], table=outfile)
            #cb.solve()
        else:
            raise Exception, 'Infile data set not found - please verify the name'

        if((type(calmode)==str) and calmode.lower() not in ['tsys', 'ps', 'otfraster', 'apply']): 
            raise Exception, 'Calmode must be either \'ps\' or \'tsys\' or \'apply\''

        if (not overwrite) and (os.path.exists(outfile)):
            raise RuntimeError, 'Output file \'%s\' exists.'%(outfile)


        if ((type(calmode)==str) and (calmode=='apply')):
            # single table
            if isinstance(applytable, str):
                _table_list = [applytable]

            # multiple tables
            if isinstance(applytable, list) or isinstance(applytable, numpy.ndarray):
                _table_list = applytable

            if len(_table_list) == 0:
                raise Exception, 'Applytable name must be specified.'

            for table in _table_list:
                # empty string
                if len(table) == 0:
                    raise Exception, 'Applytable name must be specified.'
                # unexisting table
                if not os.path.exists(table):
                    raise Exception, 'Table "%s" doesn\'t exist.'%(table)
                

            #applycal(vis=infile, docallib=False, gaintable=applytable, applymode='calonly')
            if (outfile != ''):
                raise UserWarning, 'Outfile is not generated but is added to MS as a new table, namely corrected data'

            if(type(spwmap)!=types.ListType and (type(spwmap)!=types.DictType)):
                raise Exception, 'Spwmap type must be list or dictionary.'

            #if(len(spwmap)==0):
            #    raise Exception, 'Spwmap must be specified.'
			
            #if (type(spwmap)==types.ListType):
            #	applycal(vis=infile, docallib=False, gaintable=applytable, applymode='calonly')
                #raise UserWarning, 'spwmap is list'
            if (type(spwmap)==types.DictType):

                MS = infile
                tb.open(MS+'/SPECTRAL_WINDOW')
                total_spwID=tb.nrows()
                tb.close()
                spwmap_dict = spwmap
                #spwmap_list=[]
                #for num in range(total_spwID):
                #    spwmap_list.append(num)
                spwmap_list = range(total_spwID)

                for key, value in spwmap_dict.items():
                    for v in value:
                        if v in spwmap_list:
                            index = spwmap_list.index(v)
                            spwmap_list[index]=int(key)

                spwmap = spwmap_list

            for _table in _table_list:
                caltype = inspect_caltype(_table)
                if caltype == 'UNKNOWN':
                    raise RuntimeError('Applytable \'%s\' is not a caltable format'%(_table))
                elif caltype == 'B TSYS':
                    cb.setapply(table=_table, spwmap=spwmap, interp=interp, calwt=True)
                else:
                    # no spw mapping is needed for sky calibration
                    cb.setapply(table=_table, interp=interp, calwt=True)
            cb.correct(applymode='calflag')
            #applycal(vis=infile, spwmap=spwmap, docallib=False, gaintable=applytable, applymode='calflag')	

        else:
            # non-apply modes (ps, tsys)
            calmodemap = {'tsys': 'tsys','ps': 'sdsky_ps',
                          'otfraster': 'sdsky_raster'}

        #if(calmode!='apply'):
            if len(outfile) == 0:
                raise RuntimeError, 'Output file name must be specified.'
            if calmode=='tsys':
                cb.specifycal(caltable=outfile,time="",spw=spw,pol=pol,caltype=calmodemap[calmode])
            else:
                fraction_numeric = to_numeric_fraction(fraction)
                if noff <= 0 and fraction_numeric >= 0.5:
                    raise ValueError, 'Too many edge points. fraction must be < 0.5.'
                cb.selectvis(spw=spw, scan=scan, field=field)
                cb.setsolve(type=calmodemap[calmode], table=outfile, fraction=fraction_numeric, numedge=noff)
                cb.solve()

    except UserWarning, instance:
        print '*** Warning ***',instance

    except Exception, instance:
        print '*** Error ***',instance
        raise Exception, instance

    finally:
        cb.close()

def inspect_caltype(table):
    caltype = 'UNKNOWN'
    with sdutil.tbmanager(table) as tb:
        if 'VisCal' in tb.keywordnames():
            caltype = tb.getkeyword('VisCal')
    return caltype

def to_numeric_fraction(fraction):
    try:
        if isinstance(fraction, str):
            if len(fraction.strip()) == 0:
                # invalid, use default
                fraction_numeric = 0.1
            else:
                pos = fraction.strip().find('%')
                if pos != -1:
                    # percentage 
                    fraction_numeric = float(fraction[:pos]) * 0.01
                else:
                    # direct value
                    fraction_numeric = float(fraction)
        else:
            fraction_numeric = float(fraction)
    except Exception, e:
        casalog.post(str(e), priority='SEVERE', origin='tsdcal')
        raise RuntimeError('Invalid fraction value (original error message: "%s")'%(str(e)))

    return fraction_numeric
    
