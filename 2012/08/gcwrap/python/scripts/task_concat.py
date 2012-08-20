import os
import shutil
import stat
import time
from taskinit import *
import partitionhelper as ph

def concat(vislist,concatvis,freqtol,dirtol,timesort,copypointing,visweightscale,createmms):
	"""concatenate visibility datasets
	The list of data sets given in the vis argument are concatenated into an output
	data set in concatvis.  If concatvis already exists (e.g., it is the same as the
	first input data set), then the other input data sets will be appended to the
	concatvis data set.  There is no limit to the number of input data sets.

	If none of the input data sets have any scratch columns (model and corrected
	columns), none are created in the concatvis.  Otherwise these columns are
	created on output and initialized to their default value (1 in model column,
	data in corrected column) for those data with no input columns.

	Spectral windows for each data set with the same chanelization, and within a
	specified frequency tolerance of another data set will be combined into one
	spectral window.

	A field position in one data set that is within a specified direction tolerance
	of another field position in any other data set will be combined into one
	field.  The field names need not be the same---only their position is used.

	Each appended dataset is assigned a new observation id.

	Keyword arguments:
	vis -- Name of input visibility files to be combined
		default: none; example: vis = ['src2.ms','ngc5921.ms','ngc315.ms']
	concatvis -- Name of visibility file that will contain the concatenated data
		note: if this file exits on disk then the input files are 
		      added to this file.  Otherwise the new file contains  
		      the concatenated data.  Be careful here when concatenating to
		      an existing file.
		default: none; example: concatvis='src2.ms'
			 example: concatvis='outvis.ms'

		other examples: 
		   concat(vis=['src2.ms','ngc5921.ms'], concatvis='src2.ms')
		       will concatenate 'ngc5921.ms' into 'src2.ms', and the original
		       src2.ms is lost

		   concat(vis=['src2.ms','ngc5921.ms'], concatvis='out.ms') 
		       will concatenate 'ngc5921.ms' and 'src2.ms' into a file named 
		       'out.ms'; the original 'ngc5921.ms' and 'src2.ms' are untouched.

		   concat(vis=['v1.ms','v2.ms'], concatvis = 'vall.ms')
		      then
		   concat(vis=['v3.ms','v4.ms'], concatvis = 'vall.ms')
		     vall.ms will contains v1.ms+v2.ms+v3.ms+v4.ms

	     Note: run flagmanager to save flags in the concatvis

	freqtol -- Frequency shift tolerance for considering data to be in the same
		   spwid.  The number of channels must also be the same.
		default: ''  do not combine unless frequencies are equal
		example: freqtol='10MHz' will not combine spwid unless they are
		   within 10 MHz.
		Note: This option is useful to conbine spectral windows with very slight
		   frequency differences caused by Doppler tracking, for example.

	dirtol -- Direction shift tolerance for considering data as the same field
		default: '' means always combine.
		example: dirtol='1.arcsec' will not combine data for a field unless
		   their phase center differ by less than 1 arcsec.  If the field names
		   are different in the input data sets, the name in the output data
		   set will be the first relevant data set in the list.

	timesort -- If true, the output visibility table will be sorted in time.
		default: false.  Data in order as read in.
		example: timesort=true
	     Note: There is no constraint on data that is simultaneously observed for
		more than one field; for example multi-source correlation of VLBA data.

	copypointing -- Make a proper copy of the POINTING subtable (can be time consuming).
		If False, the result is an empty POINTING table.
		default: True

	visweightscale -- The weights of the individual MSs will be scaled in the concatenated
		output MS by the factors in this list. Useful for handling heterogeneous arrays.
		Use plotms to inspect the "Wt" column as a reference for determining the scaling 
		factors. See the cookbook for more details.
		example: [1.,3.,3.] - scale the weights of the second and third MS by a factor 3.
		default: [] (empty list) - no scaling

	createmms -- disabled. Please use task virtualconcat.
		 default: False

	"""

        ###
	#Python script
	try:
		casalog.origin('concat')
		t = tbtool()
		m = mstool()
		
		#break the reference between vis and vislist as we modify vis
		if(type(vislist)==str):
			vis=[vislist]
		else:
			vis=list(vislist)
		#dto. for concavis
		theconcatvis = concatvis

		doweightscale = False
		if(len(visweightscale)>0):
			if (len(visweightscale) != len(vis)):
				raise Exception, 'parameter visweightscale must have same number of elements as parameter vis'
			for factor in visweightscale:
				if factor<0.:
					raise Exception, 'parameter visweightscale must only contain positive numbers'
				elif factor!=1.:
					doweightscale=True

		if(createmms):
			raise Exception, 'createmms disabled. Please use task virtualconcat from now on.'
			
		if((type(concatvis)!=str) or (len(concatvis.split()) < 1)):
			raise Exception, 'parameter concatvis is invalid'
		if(vis.count(concatvis) > 0):
			vis.remove(concatvis)

		if(os.path.exists(concatvis)):
			casalog.post('Will be concatenating into the existing ms '+concatvis , 'WARN')
		else:
			if(len(vis) >0): # (note: in case len is 1, we only copy, essentially)
				casalog.post('copying '+vis[0]+' to '+theconcatvis , 'INFO')
				shutil.copytree(vis[0], theconcatvis)
				# note that the resulting copy is writable even if the original was read-only
				vis.remove(vis[0])

		if not copypointing: # remove the rows from the POINTING table of the first MS
			casalog.post('*** copypointing==False: resulting MS will have empty POINTING table.', 'INFO')
			tmptabname = 'TMPPOINTING'+str(time.time())
			shutil.rmtree(tmptabname, ignore_errors=True)
			shutil.move(theconcatvis+'/POINTING', tmptabname)
			t.open(tmptabname)
			if(t.nrows()>0): 
				t.copy(newtablename=theconcatvis+'/POINTING', deep=False, valuecopy=True, norows=True)
				t.close()
				shutil.rmtree(tmptabname, ignore_errors=True)
			else: # the POINTING table is already empty
				casalog.post('***    Input POINTING table was already empty.', 'INFO')
				shutil.move(tmptabname, theconcatvis+'/POINTING')
				t.close()

		# Determine if scratch columns should be considered at all
		# by checking if any of the MSs has them.
		
		considerscrcols = False
		needscrcols = []
                if ((type(theconcatvis)==str) and (os.path.exists(theconcatvis))):
			
			# check if all scratch columns are present
			t.open(theconcatvis)
			if(t.colnames().count('CORRECTED_DATA')==1 
			   or  t.colnames().count('MODEL_DATA')==1):
				considerscrcols = True  # there are scratch columns
				
			needscrcols.append(t.colnames().count('CORRECTED_DATA')==0 
					   or  t.colnames().count('MODEL_DATA')==0)
			t.close()
                else:
                        raise Exception, 'Visibility data set '+theconcatvis+' not found - please verify the name'

		for elvis in vis : 			###Oh no Elvis does not exist Mr Bill
			if(not os.path.exists(elvis)):
				raise Exception, 'Visibility data set '+elvis+' not found - please verify the name'

			# check if all scratch columns are present
			t.open(elvis)
			if(t.colnames().count('CORRECTED_DATA')==1 
                           or  t.colnames().count('MODEL_DATA')==1):
				considerscrcols = True  # there are scratch columns

			needscrcols.append(t.colnames().count('CORRECTED_DATA')==0 
					  or  t.colnames().count('MODEL_DATA')==0)
			t.close()

		# start actual work, file existence has already been checked
		i = 0
		if(considerscrcols and needscrcols[i]):
			# create scratch cols			
			casalog.post('creating scratch columns in '+theconcatvis , 'INFO')
			cb.open(theconcatvis) # calibrator-open creates scratch columns
			cb.close()

		# scale the weights of the first MS in the chain
		if doweightscale:
			wscale = visweightscale[i]
			if(wscale==1.):
				casalog.post('Will leave the weights for this MS unchanged.', 'INFO')
			else:
				casalog.post('Scaling weights for first MS by factor '+str(wscale), 'INFO')
				t.open(theconcatvis, nomodify=False)
				for colname in [ 'WEIGHT', 'WEIGHT_SPECTRUM']:
					if (colname in t.colnames()) and (t.iscelldefined(colname,0)):
						for j in xrange(0,t.nrows()):
							a = t.getcell(colname, j)
							a *= wscale
							t.putcell(colname, j, a)
				t.close()

		# determine handling switch value
		handlingswitch = 0
		if not copypointing:
			handlingswitch = 2

		m.open(theconcatvis,nomodify=False)
		mmsmembers = [theconcatvis]
	
		for elvis in vis : 
			i = i + 1
			destms = ""
			casalog.post('concatenating '+elvis+' into '+theconcatvis , 'INFO')

			wscale = 1.
			if doweightscale:
				wscale = visweightscale[i]
				if(wscale==1.):
					casalog.post('Will leave the weights for this MS unchanged.', 'INFO')
				else:
					casalog.post('Will scale weights for this MS by factor '+str(wscale) , 'INFO')

			if(considerscrcols and needscrcols[i]):
				# create scratch cols			
				casalog.post('creating scratch columns for '+elvis+' (original MS unchanged)', 'INFO')
				tempname = elvis+'_with_scrcols'
				shutil.rmtree(tempname, ignore_errors=True)
				shutil.copytree(elvis, tempname)
				cb.open(tempname) # calibrator-open creates scratch columns
				cb.close()
				# concatenate copy instead of original file
				m.concatenate(msfile=tempname,freqtol=freqtol,dirtol=dirtol,weightscale=wscale,handling=handlingswitch,
					      destmsfile=destms)
				shutil.rmtree(tempname, ignore_errors=True)
			else:
				m.concatenate(msfile=elvis,freqtol=freqtol,dirtol=dirtol,weightscale=wscale,handling=handlingswitch,
					      destmsfile=destms)

		if timesort:
			casalog.post('Sorting main table by TIME ...', 'INFO')
			m.timesort()

		m.writehistory(message='taskname=concat',origin='concat')
		m.writehistory(message='vis          = "'+str(vis)+'"',origin='concat')
		m.writehistory(message='concatvis    = "'+str(concatvis)+'"',origin='concat')
		m.writehistory(message='freqtol      = "'+str(freqtol)+'"',origin='concat')
		m.writehistory(message='dirtol       = "'+str(dirtol)+'"',origin='concat')
		m.writehistory(message='copypointing = "'+str(copypointing)+'"',origin='concat')
		m.writehistory(message='visweightscale = "'+str(visweightscale)+'"',origin='concat')

		m.close()


	except Exception, instance:
		print '*** Error ***',instance
		raise Exception, instance

