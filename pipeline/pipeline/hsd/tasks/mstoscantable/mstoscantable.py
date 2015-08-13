from __future__ import absolute_import
import os
import re
import numpy

import pipeline.infrastructure as infrastructure
import pipeline.infrastructure.basetask as basetask
import pipeline.infrastructure.callibrary as callibrary
import pipeline.infrastructure.casatools as casatools
from pipeline.infrastructure import casa_tasks
import pipeline.domain as domain
from pipeline.infrastructure import sdtablereader
from ... import heuristics
from .. import common
from ..common import utils
from . import tsystablemapper

import asap as sd

LOG = infrastructure.get_logger(__name__)


class SDMsToScantableInputs(common.SingleDishInputs):
    @basetask.log_equivalent_CASA_call
    def __init__(self, context=None, infiles=None, output_dir=None, 
                 session=None, overwrite=None):
        self._init_properties(vars())

    @property
    def overwrite(self):
        return self._overwrite

    @overwrite.setter
    def overwrite(self, value):
        if value is None:
            value = False
        else:
            value = utils.to_bool(value)
        self._overwrite = value

    def to_casa_args(self):
        raise NotImplementedError

    @property
    def infiles(self):
        return self._infiles
 
    @infiles.setter
    def infiles(self, value):
        if value is None:
            self._infiles = []
        else:
            self._infiles = value

class SDMsToScantableResults(common.SingleDishResults):
    def __init__(self, mses=[], scantables=[]):
        super(SDMsToScantableResults, self).__init__()
        self.mses = mses
        self.scantables = scantables
        self.mappedcaltables = None
        
    def merge_with_context(self, context):
        if isinstance(context.observing_run, domain.ScantableList):
            self.replace_callibrary(context)
        target = context.observing_run
        for st in self.scantables:
            LOG.info('Adding {0} to context'.format(st.name))            
            target.add_scantable(st)

    def replace_callibrary(self, context):
        callib = context.callibrary
        mycallib = callibrary.SDCalLibrary(context)

        if self.mappedcaltables is not None:
            LOG.debug('register mapped Tsys caltables to callibrary')
            for st in self.scantables:
                ms = st.ms
                vis = ms.name
                basename = ms.basename
                antenna = st.antenna.name
                caltable_list = self.mappedcaltables[basename]['mapped']

                calto = callibrary.CalTo(vis=vis)
                calstate = callib.get_calstate(calto)
                calfrom = calstate.merged().values()[0][0]
                spwmap = calfrom.spwmap

                LOG.info('caltable_list.keys()=%s'%(caltable_list.keys()))
                for (spwid, caltable_per_antenna) in caltable_list.items():
                    LOG.info('spwid %s: caltable list %s'%(spwid, caltable_per_antenna))
                    myspwmap = {spwmap[spwid]: [spwid]}
                    mycalto = callibrary.CalTo(vis=vis, 
                                               spw='%s'%(spwid), 
                                               antenna=antenna, 
                                               intent='TARGET,REFERENCE')
                    mycalfrom = callibrary.SDCalFrom(gaintable=caltable_per_antenna[antenna],
                                                     interp='',
                                                     spwmap=myspwmap,
                                                     caltype='tsys')
                    mycallib.add(mycalto, mycalfrom)
#                 myspwmap = remap_spwmap(spwmap)
#                 for key in myspwmap.keys():
#                     filtered_value = [spw for spw in myspwmap[key] if st.spectral_window[spw].nchan > 1 and st.spectral_window[spw].is_target]
#                     myspwmap[key] = filtered_value
#                 mycalto = callibrary.CalTo(vis=vis,
#                                            spw='',
#                                            antenna=antenna,
#                                            intent='TARGET,REFERENCE')
#                 mycalfrom = callibrary.SDCalFrom(gaintable=caltable_list[antenna],
#                                                  interp='',
#                                                  spwmap=myspwmap,
#                                                  caltype='tsys')
#                 mycallib.add(mycalto, mycalfrom)

        context.callibrary = mycallib
            
    def __repr__(self):
        return 'SDMsToScantableResults:\n\t{0}\n\t{1}'.format(
            '\n\t'.join([ms.name for ms in self.mses]),
            '\n\t'.join([st.name for st in self.scantables]))


class SDMsToScantable(common.SingleDishTaskTemplate):
    Inputs = SDMsToScantableInputs

    def _ms_directories(self, names):
        """
        Inspect a list of file entries, finding the root directory of any
        measurement sets present via a set of identifying files and
        directories.
        """
        identifiers = ('SOURCE', 'FIELD', 'ANTENNA', 'DATA_DESCRIPTION')
        
        matching = [os.path.dirname(n) for n in names 
                    if os.path.basename(n) in identifiers]

        return set([m for m in matching 
                    if matching.count(m) == len(identifiers)])

    @common.datatable_setter
    def prepare(self, **parameters):
        inputs = self.inputs
        context = inputs.context
        infiles = inputs.infiles
        vis = [v.name for v in inputs.context.observing_run.measurement_sets]
        #to_import = set(vis)
        to_import = vis

        to_import_sd = set()
        to_convert_sd = set()


        if True:
            # get a list of all the files in the given directory
            h = heuristics.DataTypeHeuristics()
            for v in infiles:
                data_type = h(v).upper()
                if data_type == 'ASDM' or data_type == 'MS2':
                    raise TypeError, '%s should not be an ASDM nor MS'%(v)
                elif data_type == 'ASAP':
                    to_import_sd.add(v)
                elif data_type in ['FITS','NRO']:
                    to_convert_sd.add(v)

            to_import_sd = map(os.path.abspath, to_import_sd)

        LOG.debug('to_import=%s'%(to_import))
            
        # execute applycal to apply flags in Tsys caltable
        #jobs = self._applycal_list(to_import)
        #for job in jobs:
        #    self._executor.execute(job)
        #    
        #if self._executor._dry_run:
        #    return SDMsToScantableResults()

        ms_reader = sdtablereader.ObservingRunReader

        # convert MS to Scantable
        scantable_list = []
        st_ms_map = []
        idx = 0
        for ms in to_import:
            LOG.info('Import %s as Scantable'%(ms))
            prefix = ms.rstrip('/')
            if re.match('.*\.(ms|MS)$',prefix):
                prefix = prefix[:-3]
            prefix = os.path.join(context.output_dir, prefix)
            LOG.debug('prefix: %s'%(prefix))
            scantables = sd.splitant(filename=ms,#ms.name,
                                     outprefix=prefix,
                                     overwrite=True)
            scantable_list.extend(scantables)
            st_ms_map.extend([idx]*len(scantables))
            
            idx += 1
        
        # launch an import job for each non-Scantable, non-MS single-dish data
        for any_data in to_convert_sd:
            LOG.info('Importing %s as Scantable'%(any_data))
            self._import_to_scantable(any_data)
            
        # calculate the filenames of the resultant Scantable
        to_import_sd.extend([self._any_data_to_scantable_name(name) for name in to_convert_sd])

        # There are no associating MS data for non-MS inputs
        scantable_list.extend(to_import_sd)
        if len(st_ms_map) > 0:
            st_ms_map.extend([-1]*len(to_import_sd))
            
        # fix for bdfflags issue
        self.fix_flagrow(scantable_list)

        # Now everything is in Scnatable format, import them
        LOG.debug('scantable_list=%s'%(scantable_list))
        LOG.debug('to_import=%s'%(to_import))
        LOG.debug('st_ms_map=%s'%(st_ms_map))
        ms_objects = context.observing_run.measurement_sets
        observing_run_sd = ms_reader.get_observing_run_for_sd2(scantable_list,
                                                               ms_objects,
                                                               st_ms_map)
    
        for st in observing_run_sd:
            LOG.debug('Setting session to %s for %s' % (inputs.session, 
                                                        st.basename))
            st.session = inputs.session
            
        for ms in observing_run_sd.measurement_sets:
            LOG.debug('Setting session to %s for %s' % (inputs.session,
                                                        ms.basename))
            ms.session = inputs.session

        results = SDMsToScantableResults(observing_run_sd.measurement_sets,
                                      observing_run_sd)
        return results
    
    def analyse(self, results):
        # convert interferometric Tsys caltable to single dish ones
        sdtsystables = {}
        for ms in results.mses:
            callib = self.inputs.context.callibrary
            reftable = None
            for st in results.scantables:
                if st.ms == ms:
                    reftable = st.basename
                    break
            assert reftable is not None
            science_window_ids = list(set([spw.id for spw in st.spectral_window.values() if not (spw.nchan==1 or spw.type=='WVR' or 'TARGET' not in spw.intents)]))
            
            vis = ms.basename
            prefix = vis.replace('.ms','')
            calto = callibrary.CalTo(vis=vis)
            calstate = callib.get_calstate(calto)
            allcaltables = calstate.get_caltable()
            if len(allcaltables) == 0:
                continue
            
            tsyscaltables = [name for name in allcaltables if re.search('.*\.tsyscal\.tbl$', name) is not None]
            assert len(tsyscaltables) == 1

            caltable = tsyscaltables[0]
            
            # tsys caltable must be created for each spectral window
            merged = calstate.merged()
            assert len(merged) == 1
            mycalto = merged.keys()[0]
            mycalfroms = merged[mycalto]
            calapp = callibrary.CalApplication(mycalto, mycalfroms)
            spwmap = calapp.spwmap
            sdspwmap = remap_spwmap(spwmap)
            for key in sdspwmap.keys():
                # Remove non-science spw from calibration target (key of spwmap).
#                 filtered_value = [spw for spw in sdspwmap[key] if st.spectral_window[spw].nchan > 1 and st.spectral_window[spw].is_target]
                # NOTE: WVR band is also detected as TARGET.
                filtered_value = [spw for spw in sdspwmap[key] if spw in science_window_ids]
                sdspwmap[key] = filtered_value
            # names = {spw: {antenna: caltable_name}}
            names = {}
            for (aspwid, science_spwid_list) in sdspwmap.items():
                atm_spw = st.spectral_window[aspwid]
                for sspwid in science_spwid_list: # nothing will be done if science spw (target) is empty
                    science_spw = st.spectral_window[sspwid]
                    _names = tsystablemapper.map_with_average(prefix, caltable, reftable, atm_spw, science_spw)
                    LOG.info('_names=%s'%(_names))
                    names.update(_names)
                    LOG.info('names=%s'%(names))
            #names = tsystablemapper.map(prefix, caltable, reftable)
            sdtsystables[vis] = {'mapped': names, 'original': caltable}
        LOG.info('sdtsystables=%s'%(sdtsystables))

        if len(sdtsystables) > 0:
            LOG.debug('setting mappedcaltables attribute to result object')
            results.mappedcaltables = sdtsystables
                                                 
        return results

    def _import_to_scantable(self, data):
        outfile = self._any_data_to_scantable_name(data)
        s = sd.scantable(data, avearge=False)
        s.save(outfile, format='ASAP', overwrite=self.inputs.overwrite)
        

    def _any_data_to_scantable_name(self, name):
        return '{0}.asap'.format(os.path.join(self.inputs.output_dir,
                                              os.path.basename(name.rstrip('/'))))

    def _applycal_list(self, vislist):
        context = self.inputs.context
        if hasattr(context, 'callibrary'):
            callib = context.callibrary
            for vis in vislist:
                if vis not in callib.applied.keys():
                    msobj = context.observing_run.get_measurement_sets(vis)[0]
                    spwlist = [spw.id for spw in msobj.spectral_windows 
                               if spw.num_channels != 4]
                    spw = ','.join(map(str,spwlist))
                    mycalto = callibrary.CalTo(vis, spw=spw)
                    calstate = callib.get_calstate(mycalto)
                    merged = calstate.merged()
                    for (calto, calfroms) in merged.items():
                        args = {'vis': vis,
                                'spw': spw}
                        calapp = callibrary.CalApplication(calto, calfroms)
                        args['gaintable'] = calapp.gaintable
                        args['gainfield'] = calapp.gainfield
                        args['spwmap']    = calapp.spwmap
                        args['interp']    = calapp.interp
                        args['calwt']     = calapp.calwt
                        args['applymode'] = 'flagonly'
                        if not self._executor._dry_run:
                            for calfrom in calfroms:
                                callib.mark_as_applied(calto, calfrom)
                        yield casa_tasks.applycal(**args) 
    
    def fix_flagrow(self, scantable_list):
        for name in scantable_list:
            with casatools.TableReader(name, nomodify=False) as tb:
                nrow = tb.nrows()
                for irow in xrange(nrow):
                    channel_flag = tb.getcell('FLAGTRA', irow)
                    row_flag = tb.getcell('FLAGROW', irow)
                    if row_flag == 0 and numpy.all(channel_flag != 0):
                        tb.putcell('FLAGROW', irow, 128)
                        
                            
def remap_spwmap(spwmap):
    remapped = {}
    for (i,spw) in enumerate(spwmap):
#         if i != spw: #non-science spw is filtered in the next step
        if remapped.has_key(spw):
            remapped[spw].append(i)
        else:
            remapped[spw] = [i]
    return remapped
