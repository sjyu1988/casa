from __future__ import absolute_import

import os

import pipeline.infrastructure.logging as logging
import pipeline.infrastructure.utils as utils
import pipeline.infrastructure.pipelineqa as pqa
import pipeline.qa.scorecalculator as qacalc

from . import plotsummary


LOG = logging.get_logger(__name__)

class PlotSummaryQAHandler(pqa.QAResultHandler):
    result_cls = plotsummary.PlotSummaryResults
    child_cls = None
    generating_task = plotsummary.PlotSummary

    def handle(self, context, result):

        # Check for existence of the the target MS.
        score1 = self._ms_exists(os.path.dirname(result.inputs['vis']), os.path.basename(result.inputs['vis']))
        scores = [score1]

        result.qa.pool.extend(scores)

    def _ms_exists(self, output_dir, ms):
        '''
        Check for the existence of the target MS
        '''
        return qacalc.score_path_exists(output_dir, ms, 'Plotsummary')

class PlotSummaryListQAHandler(pqa.QAResultHandler):
    """
    QA handler for a list containing PlotSummaryResults.
    """
    result_cls = list
    child_cls = plotsummary.PlotSummaryResults
    generating_task = plotsummary.PlotSummary

    def handle(self, context, result):
        # collate the QAScores from each child result, pulling them into our
        # own QAscore list
        collated = utils.flatten([r.qa.pool for r in result])
        result.qa.pool[:] = collated
        mses = [r.inputs['vis'] for r in result]
        longmsg = 'No missing target MS(s) for %s' % utils.commafy(mses, quotes=False, conjunction='or')
        result.qa.all_unity_longmsg = longmsg

