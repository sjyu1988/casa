<%!
import os
%>

<%
# these functions are defined in template scope.
def num_lines(relpath):
	abspath = os.path.join(pcontext.report_dir, relpath)
	if os.path.exists(abspath):
		return sum(1 for line in open(abspath) if not line.startswith('#'))
	else:
		return 'N/A'
%>

<%inherit file="t2-4m_details-base.mako"/>
<%block name="header" />

<%block name="title">Bandpass Calibration and Flagging</%block>

<p>
    This task creates bandpass solutions for each measurement set, and
    flags outliers based on a comparison of the calibrated (corrected)
    amplitudes with the model amplitudes for the bandpass calibrator source.
</p>

% if htmlreports:
    <h2>Flagging</h2>
    <table class="table table-bordered table-striped">
        <caption>Report Files</caption>
        <thead>
            <tr>
                <th>Measurement Set</th>
                <th>Flagging Commands</th>
                <th>Number of Statements</th>
            </tr>
        </thead>
        <tbody>
        % for msname, relpath in htmlreports.items():
            <tr>
                <td>${msname}</td>
                <td><a class="replace-pre" href="${relpath}">${os.path.basename(relpath)}</a></td>
                <td>${num_lines(relpath)}</td>
            </tr>
        % endfor
        </tbody>
    </table>
% endif


<h2>Bandpass results</h2>

% if phaseup_applications:
    <h4>Phase-up on bandpass calibrator</h4>
    <table class="table table-bordered" summary="Application Results">
        <caption>Applied calibrations and parameters used for phase-up calibration</caption>
        <thead>
            <tr>
                <th scope="col" rowspan="2">Measurement Set</th>
                <th scope="col" colspan="5">Phase-up Solution Parameters</th>
            </tr>
            <tr>
                <th>Type</th>
                <th>Interval</th>
                <th>Min Baselines per Antenna</th>
                <th>Min SNR</th>
                <th>Phase-up Bandwidth</th>
            </tr>
        </thead>
        <tbody>
    % for application in phaseup_applications:
            <tr>
                <td>${application.ms}</td>
                <td>${application.calmode}</td>
                <td>${application.solint}</td>
                <td>${application.minblperant}</td>
                <td>${application.minsnr}</td>
                <td>${application.phaseupbw}</td>
            </tr>
    % endfor
        </tbody>
    </table>
% endif

<h4>Bandpass calibration</h4>

<table class="table table-bordered" summary="Application Results">
	<caption>Parameters used for bandpass calibration</caption>
    <thead>
        <tr>
            <th scope="col" rowspan="2">Measurement Set</th>
			<th scope="col" colspan="2">Solution Parameters</th>
			<th scope="col" colspan="2">Applied To</th>
            <th scope="col" rowspan="2">Calibration Table</th>
		</tr>
		<tr>
			<th>Type</th>
            <th>Interval</th>
			<th>Scan Intent</th>
			<th>Spectral Windows</th>
        </tr>
    </thead>
	<tbody>
	% for tr in bandpass_table_rows:
		<tr>
		% for td in tr:
			${td}
		% endfor
		</tr>
	%endfor
	</tbody>
</table>

% if amp_refant or amp_mode:
    <h2>Plots</h2>

    <p>Plots show the bandpass correction applied to the target source.
    The first two plots show amplitude vs frequency; one for the reference antenna
    and one for a typical antenna, identified the antenna with mode score.
    The third plot shows phase vs frequency for the typical antenna.
    </p>

    <p>Click the summary plots to enlarge them, or the plot title to see
    see detailed plots per spectral window and antenna.</p>

    % for ms in amp_refant:

        <h4>${ms}</h4>
        % if amp_refant[ms] or amp_mode[ms]:
        <div class="col-md-8">
            <div class="thumbnail">
                <div class="caption">
                    <%
                        plot = amp_refant[ms]
                    %>
                    <h4>Amplitude vs frequency
                        (<a class="replace"
                            data-vis="${ms}"
                            href="${os.path.relpath(os.path.join(dirname, amp_subpages[ms]), pcontext.report_dir)}">show ${ms}</a>)
                    </h4>
                    <p>The plots below show amplitude vs frequency for the
                    bandpass correction, overlayed for all spectral windows
                    and correlations. Click on the link above to show
                    show detailed plots for all antennas, or on the links
                    below to show plots with specific antennas preselected.</p>
                </div>
                <div class="row">
                    <div class="col-md-6">
                        % if plot is None or not os.path.exists(plot.thumbnail):
                        <a href="${os.path.relpath(os.path.join(dirname, amp_subpages[ms]), pcontext.report_dir)}"
                           class="fancybox"
                           rel="plots-${ms}">
                            <img data-src="holder.js/255x188/text:Not Available">
                        </a>
                        <div class="caption">
                            <h5>Reference antenna
                                (<a href="${os.path.relpath(os.path.join(dirname, amp_subpages[ms]), pcontext.report_dir)}"
                                    class="replace"
                                    data-vis="${ms}">
                                    show all detail plots</a>)
                            </h5>
                            <p>The amplitude vs frequency plot for the reference antenna
                            is not available.</p>
                        </div>
                        % else:
                        <a href="${os.path.relpath(plot.abspath, pcontext.report_dir)}"
                           class="fancybox"
                           rel="plots-${ms}">
                            <img src="${os.path.relpath(plot.thumbnail, pcontext.report_dir)}"
                                 title="Click to show amplitude vs time plot"
                                 data-thumbnail="${os.path.relpath(plot.thumbnail, pcontext.report_dir)}">
                        </a>
                        <div class="caption">
                            <h5>
                                Reference antenna (${plot.parameters['ant']})
                                (<a href="${os.path.relpath(os.path.join(dirname, amp_subpages[ms]), pcontext.report_dir)}"
                                   class="replace"
                                   data-ant="${plot.parameters['ant']}"
                                   data-vis="${plot.parameters['vis']}">
                                   show ${plot.parameters['ant']}</a>)
                            </h5>
                            <p>Amplitude vs frequency for the reference antenna
                            (${plot.parameters['ant']}). Click the link above to show
                            detailed plots for ${plot.parameters['ant']}.</p>
                        </div>
                        % endif
                    </div>

                    <%
                        plot = amp_mode[ms]
                    %>
                    <div class="col-md-6">
                        % if plot is None or not os.path.exists(plot.thumbnail):
                        <a href="${os.path.relpath(os.path.join(dirname, amp_subpages[ms]), pcontext.report_dir)}"
                           class="fancybox"
                           rel="plots-${ms}">
                            <img data-src="holder.js/255x188/text:Not Available">
                        </a>
                        <div class="caption">
                            <h5>Typical antenna
                                (<a href="${os.path.relpath(os.path.join(dirname, amp_subpages[ms]), pcontext.report_dir)}"
                                    class="replace"
                                    data-vis="${ms}">
                                    show all detail plots</a>)
                            </h5>
                            <p>The amplitude vs frequency plot for a typical antenna
                            is not available.</p>
                        </div>
                        % else:
                        <a href="${os.path.relpath(plot.abspath, pcontext.report_dir)}"
                           class="fancybox"
                           rel="plots-${ms}">
                            <img src="${os.path.relpath(plot.thumbnail, pcontext.report_dir)}"
                                 title="Click to show amplitude vs time plot"
                                 data-thumbnail="${os.path.relpath(plot.thumbnail, pcontext.report_dir)}">
                        </a>
                        <div class="caption">
                            <h5>Typical antenna (${plot.parameters['ant']})
                                (<a href="${os.path.relpath(os.path.join(dirname, amp_subpages[ms]), pcontext.report_dir)}"
                                   class="replace"
                                   data-ant="${plot.parameters['ant']}"
                                   data-vis="${plot.parameters['vis']}">
                                   show ${plot.parameters['ant']}
                                </a>)
                            </h5>
                            <p>Amplitude vs frequency for a typical antenna
                            (${plot.parameters['ant']}). Click the link above to show
                            detailed plots for ${plot.parameters['ant']}.</p>
                            <p>NB. random antenna until scores are working</p>
                        </div>
                        % endif
                    </div>
                </div>
            </div>
        </div>
        % endif

        % if phase_mode[ms]:
        <div class="col-md-4">
            <div class="thumbnail">
                <div class="caption">
                    <%
                        plot = phase_mode[ms]
                    %>
                    <h4>Phase vs frequency
                        (<a class="replace"
                            data-vis="${ms}"
                            href="${os.path.relpath(os.path.join(dirname, phase_subpages[ms]), pcontext.report_dir)}">show ${ms}</a>)
                    </h4>
                    <p>The plot below shows phase vs frequency for the
                    bandpass correction, overlayed for all spectral windows
                    and correlations. Click on the link above to show
                    show phase vs frequency plots for all antennas, or on the
                    link for just the typical antenna.</p>
                </div>
                <div class="row">
                    <div class="col-md-12">
                        % if plot is None or not os.path.exists(plot.thumbnail):
                        <a href="${os.path.relpath(os.path.join(dirname, phase_subpages[ms]), pcontext.report_dir)}"
                           class="fancybox"
                           rel="plots-${ms}">
                            <img data-src="holder.js/255x188/text:Not Available">
                        </a>
                        <div class="caption">
                            <h5>Typical antenna
                                (<a href="${os.path.relpath(os.path.join(dirname, phase_subpages[ms]), pcontext.report_dir)}"
                                    class="replace"
                                    data-vis="${ms}">
                                    show all detail plots</a>)
                            </h5>
                            <p>The phase vs frequency plot is not available.</p>
                        </div>
                        % else:
                        <a href="${os.path.relpath(plot.abspath, pcontext.report_dir)}"
                              class="fancybox"
                              rel="plots-${ms}">
                            <img src="${os.path.relpath(plot.thumbnail, pcontext.report_dir)}"
                                    title="Click to show phase vs frequency plot"
                                    data-thumbnail="${os.path.relpath(plot.thumbnail, pcontext.report_dir)}">
                        </a>
                        <div class="caption">
                            <h5>Typical antenna (${plot.parameters['ant']})
                                (<a href="${os.path.relpath(os.path.join(dirname, phase_subpages[ms]), pcontext.report_dir)}"
                                   class="replace"
                                   data-ant="${plot.parameters['ant']}"
                                   data-vis="${plot.parameters['vis']}">
                                   show ${plot.parameters['ant']}
                                </a>)
                            </h5>
                            <p>Phase vs frequency for a typical antenna
                            (${plot.parameters['ant']}). Click the link above to show
                            detailed plots for ${plot.parameters['ant']}.</p>
                        </div>
                        % endif
                    </div>
                </div>
            </div>
        </div>
        % endif

    % endfor

% endif
