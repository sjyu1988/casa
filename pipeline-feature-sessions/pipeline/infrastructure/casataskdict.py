import textwrap

import pipeline.h.tasks as h_tasks
import pipeline.hif.tasks as hif_tasks
import pipeline.hsd.tasks as hsd_tasks
import pipeline.hifa.tasks as hifa_tasks
import pipeline.hifv.tasks as hifv_tasks

CasaTaskDict = {
    'session_bandpass': 'SessionALMAPhcorBandpass',
    'session_gfluxscale': 'SessionGcorFluxscale',
    'session_refant': 'SessionRefAnt',
    'session_hifa_applycal': 'SessionIFApplycal',
    'session_h_applycal': 'SessionApplycal',
    # General Tasks ----------------------------------------------------------
    'h_applycal': 'Applycal',
    'h_importdata': 'ImportData',
    'h_exportdata': 'ExportData',
    'h_mssplit': 'MsSplit',
    'h_restoredata': 'RestoreData',
    'h_tsyscal': 'Tsyscal',
    # Interferometry tasks ---------------------------------------------------
    'hif_editimlist': 'Editimlist',
    'hif_linfeedpolcal': 'Linfeedpolcal',
    'hif_antpos': 'Antpos',
    'hif_atmflag': 'Atmflag',
    'hif_applycal': 'IFApplycal',
    'hif_bandpass': 'Bandpass',
    'hif_bpflagchans': 'Bandpassflagchans',
    'hif_correctedampflag': 'Correctedampflag',
    'hif_checkproductsize': 'CheckProductSize',
    'hif_rawflagchans': 'Rawflagchans',
    'hif_findcont': 'FindCont',
    'hif_flagcorrected': 'Flagcorrected',
    'hif_gaincal': 'Gaincal',
    'hif_gainflag': 'Gainflag',
    'hif_lowgainflag': 'Lowgainflag',
    'hif_makeimages': 'MakeImages',
    'hif_makeimlist': 'MakeImList',
    'hif_mstransform': 'Mstransform',
    'hif_polarization': 'Polarization',
    'hif_refant': 'RefAnt',
    'hif_setjy': 'Setjy',
    'hif_setmodels': 'SetModels',
    'hif_tclean': 'Tclean',
    'hif_uvcontfit': 'UVcontFit',
    'hif_uvcontsub': 'UVcontSub',
    # Single dish tasks ------------------------------------------------------
    'hsd_applycal': 'SDMSApplycal',
    'hsd_baseline': 'SDMSBaseline',
    'hsd_blflag': 'SDMSBLFlag',
    'hsd_exportdata': 'SDMSExportData',
    'hsd_flagdata': 'FlagDeterALMASingleDish',
    'hsd_imaging': 'SDMSImaging',
    'hsd_importdata': 'SDImportData',
    'hsd_k2jycal': 'SDK2JyCal',
    'hsd_skycal': 'SDMSSkyCal',
    'hsd_tsysflag': 'SDTsysflag',
    # ALMA interferometry tasks ---------------------------------------------
    'hifa_importdata': 'ALMAImportData',
    'hifa_antpos': 'ALMAAntpos',
    'hifa_bandpass': 'ALMAPhcorBandpass',
    'hifa_bpsolint': 'BpSolint',
    'hifa_flagdata': 'FlagDeterALMA',
    'hifa_exportdata': 'ALMAExportData',
    'hifa_flagtargets': 'FlagTargetsALMA',
    'hifa_fluxcalflag': 'FluxcalFlag',
    'hifa_fluxdb': 'Fluxdb',
    'hifa_gaincalsnr': 'GaincalSnr',
    'hifa_gfluxscale': 'GcorFluxscale',
    'hifa_linpolcal': 'Linpolcal',
    'hifa_restoredata': 'ALMARestoreData',
    'hifa_spwphaseup': 'SpwPhaseup',
    'hifa_timegaincal': 'TimeGaincal',
    'hifa_tsysflag': 'ALMATsysflag',
    'hifa_wvrgcal': 'Wvrgcal',
    'hifa_wvrgcalflag': 'Wvrgcalflag',
    # VLA tasks -----------------------------------------------------------------
    'hifv_tecmaps': 'TecMaps',
    'hifv_circfeedpolcal': 'Circfeedpolcal',
    'hifv_importdata': 'VLAImportData',
    'hifv_exportdata': 'VLAExportData',
    'hifv_hanning': 'Hanning',
    'hifv_flagdata': 'FlagDeterVLA',
    'hifv_vlasetjy': 'VLASetjy',
    'hifv_priorcals': 'Priorcals',
    'hifv_testBPdcals': 'testBPdcals',
    'hifv_flagbaddef': 'FlagBadDeformatters',
    'hifv_checkflag': 'Checkflag',
    'hifv_semiFinalBPdcals': 'semiFinalBPdcals',
    'hifv_solint': 'Solint',
    'hifv_fluxboot': 'Fluxboot',
    'hifv_finalcals': 'Finalcals',
    'hifv_applycals': 'Applycals',
    'hifv_targetflag': 'Targetflag',
    'hifv_statwt': 'Statwt',
    'hifv_plotsummary': 'PlotSummary',
    'hifv_restoredata': 'VLARestoreData'
}


classToCASATask = {
    h_tasks.SessionApplycal: 'session_h_applycal',
    hif_tasks.SessionIFApplycal: 'session_hifa_applycal',
    hifa_tasks.SessionALMAPhcorBandpass: 'session_bandpass',
    hifa_tasks.SessionGcorFluxscale: 'session_gfluxscale',
    hif_tasks.SessionRefAnt: 'session_refant',
    # ALMA interferometry tasks ---------------------------------------------
    hifa_tasks.ALMAImportData         : 'hifa_importdata',
    hifa_tasks.ALMAPhcorBandpass      : 'hifa_bandpass',
    hifa_tasks.ALMAAntpos             : 'hifa_antpos',
    hifa_tasks.BpSolint               : 'hifa_bpsolint',
    hifa_tasks.ALMAExportData         : 'hifa_exportdata',
    hifa_tasks.FlagDeterALMA          : 'hifa_flagdata',
    hifa_tasks.FlagTargetsALMA        : 'hifa_flagtargets',
    hifa_tasks.FluxcalFlag            : 'hifa_fluxcalflag',
    hifa_tasks.Fluxdb                 : 'hifa_fluxdb',
    hifa_tasks.GaincalSnr             : 'hifa_gaincalsnr',
    hifa_tasks.GcorFluxscale          : 'hifa_gfluxscale',
    hifa_tasks.Linpolcal              : 'hifa_linpolcal',
    hifa_tasks.ALMARestoreData        : 'hifa_restoredata',
    hifa_tasks.SpwPhaseup             : 'hifa_spwphaseup',
    hifa_tasks.TimeGaincal            : 'hifa_timegaincal',
    hifa_tasks.ALMATsysflag           : 'hifa_tsysflag',
    hifa_tasks.Wvrgcal                : 'hifa_wvrgcal',
    hifa_tasks.Wvrgcalflag            : 'hifa_wvrgcalflag',
    # Interferometry tasks ---------------------------------------------------
    hif_tasks.Antpos                  : 'hif_antpos',
    hif_tasks.Editimlist              : 'hif_editimlist',
    hif_tasks.IFApplycal              : 'hif_applycal',    
    hif_tasks.Atmflag                 : 'hif_atmflag',
    hif_tasks.Bandpass                : 'hif_bandpass',
    hif_tasks.Bandpassflagchans       : 'hif_bpflagchans',
    hif_tasks.Correctedampflag        : 'hif_correctedampflag',
    hif_tasks.CheckProductSize        : 'hif_checkproductsize',
    hif_tasks.FindCont                : 'hif_findcont',
    hif_tasks.Flagcorrected           : 'hif_flagcorrected',
    hif_tasks.Linfeedpolcal           : 'hif_linfeedpolcal',
    hif_tasks.Rawflagchans            : 'hif_rawflagchans',
    hif_tasks.Fluxcal                 : 'hif_fluxcal',
    hif_tasks.Fluxscale               : 'hif_fluxscale',
    hif_tasks.Gaincal                 : 'hif_gaincal',
    hif_tasks.Gainflag                : 'hif_gainflag',
    hif_tasks.Lowgainflag             : 'hif_lowgainflag',
    hif_tasks.MakeImages              : 'hif_makeimages',
    hif_tasks.MakeImList              : 'hif_makeimlist',
    hif_tasks.Mstransform             : 'hif_mstransform',
    hif_tasks.Polarization            : 'hif_polarization',
    hif_tasks.RefAnt                  : 'hif_refant',
    hif_tasks.Setjy                   : 'hif_setjy',
    hif_tasks.SetModels               : 'hif_setmodels',
    hif_tasks.Tclean                  : 'hif_tclean',
    hif_tasks.UVcontFit               : 'hif_uvcontfit',
    hif_tasks.UVcontSub               : 'hif_uvcontsub',
    # Single dish tasks ------------------------------------------------------
    hsd_tasks.SDMSApplycal            : 'hsd_applycal',
    hsd_tasks.SDMSBaseline            : 'hsd_baseline',
    hsd_tasks.SDMSBLFlag              : 'hsd_blflag',
    hsd_tasks.SDMSExportData          : 'hsd_exportdata',
    hsd_tasks.FlagDeterALMASingleDish : 'hsd_flagdata',
    hsd_tasks.SDMSImaging             : 'hsd_imaging',
    hsd_tasks.SDImportData            : 'hsd_importdata',
    hsd_tasks.SDK2JyCal               : 'hsd_k2jycal',
    hsd_tasks.SDMSSkyCal              : 'hsd_skycal',
    hsd_tasks.SDTsysflag              : 'hsd_tsysflag',
    # VLA tasks ----------------------------------------------------------------
    hifv_tasks.TecMaps                : 'hifv_tecmaps',
    hifv_tasks.Circfeedpolcal         : 'hifv_circfeedpolcal',
    hifv_tasks.VLAImportData          : 'hifv_importdata',
    hifv_tasks.VLAExportData          : 'hifv_exportdata',
    hifv_tasks.Hanning                : 'hifv_hanning',
    hifv_tasks.FlagDeterVLA           : 'hifv_flagdata',
    hifv_tasks.VLASetjy               : 'hifv_vlasetjy',
    hifv_tasks.Priorcals              : 'hifv_priorcals',
    hifv_tasks.testBPdcals            : 'hifv_testBPdcals',
    hifv_tasks.FlagBadDeformatters    : 'hifv_flagbaddef',
    hifv_tasks.Checkflag              : 'hifv_checkflag',
    hifv_tasks.semiFinalBPdcals       : 'hifv_semiFinalBPdcals',
    hifv_tasks.Solint                 : 'hifv_solint',
    hifv_tasks.Fluxboot               : 'hifv_fluxboot', 
    hifv_tasks.Finalcals              : 'hifv_finalcals',
    hifv_tasks.Applycals              : 'hifv_applycals',
    hifv_tasks.Targetflag             : 'hifv_targetflag',
    hifv_tasks.PlotSummary            : 'hifv_plotsummary',
    hifv_tasks.Statwt                 : 'hifv_statwt',
    hifv_tasks.VLARestoreData         : 'hifv_restoredata',
    # General Tasks -------------------------------------------------------------
    h_tasks.Applycal                  : 'h_applycal',
    h_tasks.ImportData                : 'h_importdata',
    h_tasks.ExportData                : 'h_exportdata',
    h_tasks.RestoreData               : 'h_restoredata',
    h_tasks.MsSplit                   : 'h_mssplit',
    h_tasks.Tsyscal                   : 'h_tsyscal'
}


SILENT_TASK_COMMENT = (
    'This stage performs a pipeline calculation without running any CASA '
    'commands to be put in this file.'
)

CASA_COMMANDS_PROLOGUE = (
    'This file contains CASA commands run by the pipeline. Although all '
    'commands required to calibrate the data are included here, this file '
    'cannot be executed, nor does it contain heuristic and flagging '
    'calculations performed by pipeline code. This file is useful to '
    'understand which CASA commands are being run by each pipeline task. If '
    'one wishes to re-run the pipeline, one should use the pipeline script '
    'linked on the front page or By Task page of the weblog. Some stages may not have any '
    'commands listed here, e.g. hifa_importdata if conversion from ASDM to MS '
    'is not required.'
)

TASK_COMMENTS = {
    (h_tasks.ImportData,
     hifa_tasks.ALMAImportData, 
     hifv_tasks.VLAImportData,
     hsd_tasks.SDImportData,): (
        'If required, ASDMs are converted to measurement sets.'
    ),
    (hifa_tasks.FlagDeterALMA,
     hsd_tasks.FlagDeterALMASingleDish,): (
        'Flags generated by the online telescope software, by the QA0 '
        'process, and manually set by the pipeline user.'
    ),
    (hifa_tasks.ALMAPhcorBandpass,): (
        'The spectral response of each antenna is calibrated. A short-solint '
        'phase gain is calculated to remove decorrelation of the bandpass '
        'calibrator before the bandpass is calculated.'
    ),
    (hifa_tasks.BpSolint,): (
        'Compute the best per spw bandpass solution intervals.'
    ),
    (hifa_tasks.FluxcalFlag,): (
        SILENT_TASK_COMMENT
    ),
    (hif_tasks.RefAnt,): (
        'Antennas are prioritized and enumerated based on fraction flagged '
        'and position in the array. The best antenna is used as a reference '
        'antenna unless it gets flagged, in which case the next-best '
        'antenna is used.\n'
        '' + SILENT_TASK_COMMENT
    ),
    (h_tasks.Tsyscal,): (
        'The Tsys calibration and spectral window map is computed.'
    ),
    (h_tasks.Tsysflag,
     hifa_tasks.ALMATsysflag,
     hsd_tasks.SDTsysflag,): (
        'The Tsys calibration table is analyzed and deviant points are flagged.'
    ),
    (hifa_tasks.Wvrgcalflag,): (
        'Water vapour radiometer corrections are calculated for each antenna. '
        'The quality of the correction is assessed by comparing a phase gain '
        'solution calculated with and without the WVR correction. This '
        'requires calculation of a temporary phase gain on the bandpass '
        'calibrator, a temporary bandpass using that temporary gain, followed '
        'by phase gains with the temporary bandpass, with and without the WVR '
        'correction. After that, some antennas are wvrflagged (so that their '
        'WVR corrections are interpolated), and then the quality of the '
        'correction recalculated.'
    ),                              
    (hif_tasks.Lowgainflag,): (
        'Sometimes antennas have significantly lower gain than nominal. Even '
        'when calibrated, it is better for ALMA data to flag these antennas. '
        'The pipeline detects this by calculating a long solint amplitude '
        'gain on the bandpass calibrator.  First, temporary phase and '
        'bandpass solutions are calculated, and then that temporary bandpass '
        'is used to calculate a short solint phase and long solint amplitude '
        'solution.'
    ),
    (hif_tasks.Gainflag,): (
        'Sometimes antennas have significantly lower gain than nominal and/or'
        'have a significantly larger spread in gain than nominal. Even '
        'when calibrated, it is better for ALMA data to flag these antennas. '
        'The pipeline detects this by calculating a short solint amplitude '
        'gain on the bandpass calibrator.  First, temporary phase and '
        'bandpass solutions are calculated, and then that temporary bandpass '
        'is used to calculate a short solint phase and short solint amplitude '
        'solution.'
    ),
    (hif_tasks.Correctedampflag,): (
        'This task identifies baselines and antennas with a significant '
        'fraction of outlier integrations, based on a comparison of the '
        'calibrated (corrected) amplitudes with the model amplitudes for one '
        'or more specified calibrator sources.'
    ),
    (hif_tasks.Setjy,): (
        'If the amplitude calibrator is a resolved solar system source, this '
        'uses a subset of antennas with short baselines (where the resolved '
        'source model is of highest quality).'
    ),
    (hif_tasks.PhcorBandpass,): (
        'The spectral response of each antenna is calibrated. A short-solint '
        'phase gain is calculated to remove decorrelation of the bandpass '
        'calibrator before the bandpass is calculated.'
    ),
    (hif_tasks.Bandpassflagchans,): (
        'Very sharp features (e.g. the cores of strong atmospheric lines, '
        'and rare instrumental artifacts) in the bandpass solution are '
        'flagged.'
    ),  
    (hifa_tasks.GcorFluxscale,): (
        'The absolute flux calibration is transferred to secondary calibrator '
        'sources.'
    ),
    (hifa_tasks.TimeGaincal,): (
        'Time dependent gain calibrations are computed. '
    ),
    (h_tasks.Applycal, hif_tasks.IFApplycal, hsd_tasks.SDMSApplycal): (
        'Calibrations are applied to the data. Final flagging summaries '
        'are computed'
    ),
    (hif_tasks.MakeImList,): (
        'A list of target sources to be imaged is constructed. '
    ),
    (hif_tasks.MakeImages,): (
        'A list of target sources is cleaned. '
    ),
    (h_tasks.MsSplit,): (
        'The parent MS is split by field, intent, or spw '
        'and / or averaged by channel and time. '
    ),
    (hif_tasks.Tclean,): (
        'A single target source is cleaned. '
    ),
    (h_tasks.ExportData,
     hifa_tasks.ALMAExportData,
     hsd_tasks.SDMSExportData): (
        'The output data products are computed. '
    ),
    # Single Dish Tasks
    (hsd_tasks.SDMSSkyCal,): (
        'Generates sky calibration table according to calibration '
        'strategy. '
    ),
    (hsd_tasks.SDMSBaseline,): (
        'Subtracts spectral baseline by least-square fitting with '
        'N-sigma clipping. Spectral lines are automatically detected '
        'and examined to determine the region that is masked to protect '
        'these features from the fit. \n'
        '' + SILENT_TASK_COMMENT
    ),
    (hsd_tasks.SDMSBLFlag,): (
        'Perform row-based flagging based on noise level and quality of '
        'spectral baseline subtraction. \n'
        '' + SILENT_TASK_COMMENT
    ),
    (hsd_tasks.SDMSImaging,): (
        'Perform single dish imaging.'
    ),
    (hsd_tasks.SDK2JyCal,): (
        'The Kelvin to Jy calibration tables are generated.'
    )
}


def get_task_comment(task):
    """
    Get the casalog comment for the given task.
    """
    comment = ''
    for task_classes, task_comment in TASK_COMMENTS.items():
        if task.__class__ in task_classes:
            wrapped = textwrap.wrap('# ' + task_comment,
                                    subsequent_indent='# ',
                                    width=78,
                                    break_long_words=False)
            comment += '%s\n#\n' % '\n'.join(wrapped)
    
    return comment
