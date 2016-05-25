<%!
rsc_path = ""
import os
%>
<%inherit file="t2-4m_details-base.html"/>

<%block name="title">Flag Flux Calibrator</%block>

<p>Known spectral lines in flux calibrators are flagged and the reference spwmap for flux scaling is computed</p>

<div class="row-fluid">
    <table class="table table-bordered table-striped" summary="Targetflag flagging results">
	<caption>Reference spwmap per measurement set.</caption>
        <thead>
	    <tr>
	        <th>Measurement Set</th>
	        <th>Reference Spwmap</th>
	        <th>Detected Lines</th>
	        <th>Flagging Commands</th>
	    </tr>
	</thead>
	<tbody>
            %for r in result:
		<tr>
			<td>${os.path.basename(r._vis)}</td>
			<td>${','.join([str(spwid) for spwid in r._refspwmap]).replace(',', ', ')}</td>
			<td>${'<br>'.join(['field=%s line=%s spw=%d:%d~%d' % (line.fieldname, line.species, line.spwid, line.chanrange[0], line.chanrange[1]) for line in r._fluxcal_linelist])}</td>
			<td>${'<br>'.join([str(cmd) for cmd in r._fluxcal_flagcmds])}</td>
		</tr>
            %endfor
	</tbody>
    </table>
</div>