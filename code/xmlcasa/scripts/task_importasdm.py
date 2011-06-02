import os
from taskinit import *

def importasdm(asdm=None, vis=None, singledish=None, antenna=None, corr_mode=None, srt=None, time_sampling=None, ocorr_mode=None, compression=None, asis=None, wvr_corrected_data=None, scans=None, ignore_time=None, process_syspower=None, process_caldevice=None, verbose=None, overwrite=None, showversion=None):
	""" Convert an ALMA Science Data Model observation into a CASA visibility file (MS)
	The conversion of the ALMA SDM archive format into a measurement set.  This version
	is under development and is geared to handling many spectral windows of different
	shapes.
	
	Keyword arguments:
	asdm -- Name of input ASDM file (directory)
		default: none; example: asdm='ExecBlock3'

	"""
	#Python script

	# make fg tool local 
	fg = casac.homefinder.find_home_by_name('flaggerHome').create()	

	try:
                casalog.origin('importasdm')
		viso = ''
		visoc = '' # for the wvr corrected version, if needed
                if singledish:
                   ocorr_mode = 'ao'
                   corr_mode = 'ao'
                   casalog.post('corr_mode and ocorr_mode is forcibly set to ao.')
                   if compression:
                      casalog.post('compression=True has no effect for single-dish format.')
		if(len(vis) > 0) :
		   viso = vis
		   tmps = vis.rstrip('.ms')
		   if(tmps==vis):
			   visoc = vis+"-wvr-corrected"
		   else:
			   visoc = tmps+"-wvr-corrected.ms"			   
                   if singledish:
                      viso = vis.rstrip('/') + '.importasdm.tmp.ms'
		else :
		   viso = asdm + '.ms'
		   visoc = asdm + '-wvr-corrected.ms'
		   vis = asdm
                   if singledish:
                      viso = asdm.rstrip('/') + '.importasdm.tmp.ms'
                      vis = asdm.rstrip('/') + '.asap'
		execute_string='asdm2MS  --icm \"' +corr_mode + '\" --isrt \"' + srt+ '\" --its \"' + time_sampling+ '\" --ocm \"' + ocorr_mode + '\" --wvr-corrected-data \"' + wvr_corrected_data + '\" --asis \"' + asis + '\" --logfile \"' +casalog.logfile() +'\"'
		if(len(scans) > 0) :
		   execute_string= execute_string +' --scans ' + scans
		if (ignore_time) :
			execute_string= execute_string +' --ignore-time'
		if (process_syspower) :
			execute_string = execute_string +' --process-syspower'
		if (process_caldevice) :
			execute_string = execute_string +' --process-caldevice'
		if(compression) :
		   execute_string= execute_string +' --compression'
		if(verbose) :
		   execute_string= execute_string +' --verbose'
		if(showversion) :
		   execute_string= execute_string +' --revision'
		if not overwrite and os.path.exists(viso):
		   raise Exception, "You have specified and existing ms and have indicated you do not wish to overwrite it"

		#
		# If viso+".flagversions" then process differently depending on the value of overwrite..
		#
		dotFlagversion = viso + ".flagversions"
		if os.path.exists(dotFlagversion):
			if overwrite :
				casalog.post("Found '"+dotFlagversion+"' . It'll be deleted before running the filler.")
				os.system('rm -rf %s'%dotFlagversion)
			else :
				casalog.post("Found '%s' but can't overwrite it."%dotFlagversion)
				raise Exception, "Found '%s' but can't overwrite it."%dotFlagversion
	   
		execute_string = execute_string + ' ' + asdm + ' ' + viso
		casalog.post('Running the asdm2MS standalone invoked as:')
		#print execute_string
		casalog.post(execute_string)
        	exitcode=os.system(execute_string)
                if exitcode!=0:
                   casalog.post('The asdm2MS terminated','SEVERE')
                   raise Exception, "asdm coversion error, please check if it is a valid ASDM"
		if compression :
                   #viso = viso + '.compressed'
                   viso = viso.rstrip('.ms') + '.compressed.ms'
                   visoc = visoc.rstrip('.ms') + '.compressed.ms'

		if(wvr_corrected_data=='no' or wvr_corrected_data=='both'):
			if os.path.exists(viso):
			   fg.open(viso)
			   fg.saveflagversion('Original',comment='Original flags at import into CASA',merge='save')
			   fg.done();
		elif(wvr_corrected_data=='yes' or wvr_corrected_data=='both'):
			if os.path.exists(visoc):
			   fg.open(visoc)
			   fg.saveflagversion('Original',comment='Original flags at import into CASA',merge='save')
			   fg.done();			

                if singledish:
                   casalog.post('temporary MS file: %s'%viso)
                   casalog.post('        ASAP file: %s'%vis)
                   casalog.post(' selected antenna: %s'%antenna)
                   try:
                      from asap import scantable
                      s = scantable(viso,average=False,getpt=True,antenna=antenna)
                      s.save(vis,format='ASAP',overwrite=overwrite)
                      # remove intermediate products
                      if os.path.exists(viso):
                         os.system('rm -rf %s'%viso)
                         os.system('rm -rf %s.flagversions'%viso)
                   except Exception, instance:
                      if os.path.exists(viso):
                         os.system('rm -rf %s'%viso)
                         os.system('rm -rf %s.flagversions'%viso)  
                      if type(instance) == ImportError and (str(instance)).find('asap') != -1:
                         casalog.post(str(instance),'SEVERE')
                         casalog.post('You should build ASAP to be able to create single-dish data.','SEVERE')
                      else:
                         raise Exception, instance
	except Exception, instance:
		print '*** Error ***',instance

