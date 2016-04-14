"""
The exportdata for SD module provides base classes for preparing data products
on disk for upload to the archive. 

To test these classes, register some data with the pipeline using ImportData,
then execute:

import pipeline
# Create a pipeline context and register some data
context = pipeline.Pipeline().context
output_dir = "."
products_dir = "./products"
inputs = pipeline.tasks.singledish.SDExportData.Inputs(context,output_dir,products_dir)
task = pipeline.tasks.singledish.SDExportData (inputs)
results = task.execute (dry_run = True)
results = task.execute (dry_run = False)

"""
from __future__ import absolute_import
import os
import errno
import tarfile
import fnmatch
import shutil
import string
import pipeline.infrastructure.api as api
import pipeline.infrastructure.basetask as basetask
from pipeline.infrastructure import casa_tasks
import pipeline.infrastructure as infrastructure
import pipeline.infrastructure.sdfilenamer as filenamer

# the logger for this module
LOG = infrastructure.get_logger(__name__)


class SDExportDataInputs(basetask.StandardInputs):
    """
    ExportDataInputs manages the inputs for the ExportData task.
    
    .. py:attribute:: context
    
    the (:class:`~pipeline.infrastructure.launcher.Context`) holding all
    pipeline state
    
    .. py:attribute:: output_dir
    
    the directory containing the output of the pipeline
    
    .. py:attribute:: products_dir
    
    the directory where the data productions will be written
    
    .. py:attribute:: targetimages
    
    the list of target source images to be saved.  Defaults to all
    target images. If defined overrides the list of target images in
    the context.
    
    .. py:attribute:: skytsyscal_bl
    the list of flag and bl coefficient to be saved.  Defaults to all
    target flag and bl coefficient. If defined overrides the list of 
    flag and bl coefficient in the context.
    
    """
    
    def __init__(self, context, output_dir=None,pprfile=None,
                 targetimages=None,skytsyscal_bl=None,products_dir=None):
        """
        Initialise the Inputs, initialising any property values to those given
        here.
            
        :param context: the pipeline Context state object
        :type context: :class:`~pipeline.infrastructure.launcher.Context`
        :param output_dir: the working directory for pipeline data
        :type output_dir: string
        :param pprfile: the pipeline processing request
        :type pprfile: a string
        :param targetimages: the list of target images to be saved
        :type targetimages: a list
        :param skytsyscal_bl: the list of skytsyscal_bl to be saved
        :type skytsyscal_bl: a list
        :param products_dir: the data products directory for pipeline data
        :type products_dir: string
        """        
        # set the properties to the values given as input arguments
        self._init_properties(vars())
     
    @property
    def products_dir(self):
        if self._products_dir is None:
            if self.context.products_dir is None:
                self._products_dir = os.path.abspath('./')
            else:
                self._products_dir = self.context.products_dir
                try:
                    LOG.trace('Creating products directory \'%s\'' % self._products_dir)
                    os.makedirs(self.products_dir)
                except OSError as exc:
                    if exc.errno == errno.EEXIST:
                        pass
                    else: 
                        raise
                return self._products_dir
        return self._products_dir
     
    @products_dir.setter
    def products_dir(self, value):
        self._products_dir = value
     
    @property
    def pprfile(self):
        if self._pprfile is None:
            self._pprfile = ''
        return self._pprfile
    
    @pprfile.setter
    def pprfile(self, value):
        self._pprfile = value
    
    @property
    def targetimages(self):
        if self._targetimages is None:
            self._targetimages = []
        return self._targetimages
     
    @targetimages.setter
    def targetimages(self, value):
        self._targetimages = value
     
    @property
    def skytsyscal_bl(self):
        if self._skytsyscal_bl is None:
            self._skytsyscal_bl = []
        return self._skytsyscal_bl
     
    @skytsyscal_bl.setter
    def skytsyscal_bl(self, value):
        self._skytsyscal_bl = value
 
class SDExportDataResults(basetask.Results):
    def __init__(self, jobs=[]):
        super(SDExportDataResults,self).__init__()
        """
        Initialise the results object with the given list of JobRequests.
        """
        self.jobs = jobs
    
    def __repr__(self):
        s = 'SDExportData results:\n'
        for job in self.jobs:
            s += '%s performed.' % str(job)
        return s 

class SDExportData(basetask.StandardTaskTemplate):
    """
    SDExportData is the base class for exporting data to the products
    subdirectory. It performs the following operations:
    
    - Saves the pipeline processing request in an XML file
    - Saves the images in FITS cubes one per target and spectral window
    - Saves the final flags and bl coefficient per ASDM in a compressed / tarred CASA flag
      versions file
    - Saves the final web log in a compressed / tarred file
    - Saves the text formatted list of contents of products directory
    """

    
    # link the accompanying inputs to this task 
    Inputs = SDExportDataInputs
    
    # Override the default behavior for multi-vis tasks
    def is_multi_vis_task(self):
        return True
    
    def prepare(self):
        """
        Prepare and execute an export data job appropriate to the
        task inputs.
        """
        # Create a local alias for inputs, so we're not saying
        # 'self.inputs' everywhere
        inputs = self.inputs
         
        if os.path.exists(inputs.products_dir):
            # Create fits files from CASA images
            fitsfiles = self._export_images (inputs.context, inputs.products_dir, inputs.targetimages)
            # Export a tar file of skycal , tsyscal and bl-subtracted which is created by asap
            skytsyscal_bl = self._export_skytsyscal_bl(inputs.context, inputs.products_dir, inputs.skytsyscal_bl)
            # Export a tar file of the web log
            weblog = self._export_weblog (inputs.context, inputs.products_dir)
            
            # Locate and copy the pipeline processing request.
            pprfiles = self._export_pprfile (inputs.context,inputs.products_dir, inputs.pprfile)
        
            # Export the processing log independently of the web log
            casa_commands_file = self._export_casa_commands_log (inputs.context,
            'casa_commands.log', inputs.products_dir)
    
            # Export a text format list of files whithin a products directory
            newlist = [fitsfiles,skytsyscal_bl,weblog,pprfiles,casa_commands_file]
            list_of_locallists = [key for key,value in locals().iteritems()
                                if type(value) == list]
            self._export_list_txt(inputs.context,inputs.products_dir,newlist,list_of_locallists)
            #LOG.info('contents of product direoctory is %s' % os.listdir(inputs.products_dir))
        else:
            LOG.info('There is no product direoctory, please input !mkdir products')
         
        return SDExportDataResults(jobs=[])
     
    def analyse(self, results):
        """
        Analyse the results of the export data operation.
        This method does not perform any analysis, so the results object is
        returned exactly as-is, with no data massaging or results items
        added.
        :rtype: :class:~`SDExportDataResults`
        """
        return results
     
    def _export_pprfile (self, context, products_dir, pprfile):
        """
        Export the pipeline processing request to the output products directory as is.
        """
        os.chdir(context.output_dir)
        # Prepare the search template for the pipeline processing request file.
        if pprfile == '':
            ps = context.project_structure
            if ps is None:
                pprtemplate = 'PPR_*.xml'
            elif ps.ppr_file == '':
                pprtemplate = 'PPR_*.xml'
            else:
                pprtemplate = os.path.basename(ps.ppr_file)
        else:
            pprtemplate = os.path.basename(pprfile)
        
        # Locate the pipeline processing request(s) and  generate a list
        # to be copied to the data products directory. Normally there
        # should be only one match but if there are more copy them all.
        pprmatches = []
        for file in os.listdir(context.output_dir):
            if fnmatch.fnmatch (file, pprtemplate):
                LOG.debug('Located pipeline processing request(PPR) xmlfile %s' % (file))
                pprmatches.append (os.path.join(context.output_dir, file))
        
        # Copy the pipeline processing request files.
        for file in pprmatches: 
            LOG.info('Copying pipeline processing request(PPR) xmlfile %s to %s' % \
                (os.path.basename(file), products_dir))
            if not self._executor._dry_run:
                shutil.copy (file, products_dir)
        return pprmatches

    def _export_casa_commands_log (self, context, casalog_name, products_dir):
        """
        Save the CASA commands file.
        """
        ps = context.project_structure
        if ps is None:
            casalog_file = os.path.join (context.report_dir, casalog_name)
            out_casalog_file = os.path.join (products_dir, casalog_name) 
        elif ps.ousstatus_entity_id == 'unknown':
            casalog_file = os.path.join (context.report_dir, casalog_name)
            out_casalog_file = os.path.join (products_dir, casalog_name) 
        else:
            ousid = ps.ousstatus_entity_id.translate(string.maketrans(':/', '__'))
            casalog_file = os.path.join (context.report_dir, casalog_name)
            out_casalog_file = os.path.join (products_dir, ousid + '.' + casalog_name)
        
        LOG.info('Copying casa commands log %s to %s' % \
                (casalog_file, out_casalog_file))
        if not self._executor._dry_run:
            shutil.copy (casalog_file, out_casalog_file)
        
        #return os.path.basename(out_casalog_file)
        return out_casalog_file
    
    def _export_images (self, context, products_dir, images):
        #cwd = os.getcwd()
        os.chdir(context.output_dir)
        producted_filename = '*.im'
        
        if len(images) == 0:
            xx_dot_im = [fname for fname in os.listdir(context.output_dir)
                if fnmatch.fnmatch(fname, producted_filename)]
        else:
            xx_dot_im = images
        
        fits_list = []
        if len(xx_dot_im) != 0:
            # split by dot
            splitwith_dot = [name.split('.') for name in xx_dot_im]
            
            # pop by 'line'
            tmplate_line = "line*"
            num_selected_im = [ i for i in range(len(splitwith_dot))
                for words in splitwith_dot[i]
                    if fnmatch.fnmatch(words,tmplate_line)]
            
            xx_dot_im_next = [xx_dot_im[i] for i in range(len(xx_dot_im))
                if not (i in num_selected_im)]
            
            splitted_path = []
            for fname in xx_dot_im_next:
                root, ext = os.path.splitext(fname)
                splitted_path.append(root)
             
            images_list = splitted_path
             
            # showing target CASA image
            if self._executor._dry_run:
                for imfile in xx_dot_im_next:
                    LOG.info('FITS: Target CASA image is %s' % imfile)
            else:
                pass
             
            # Convert to FITS 
            fits_list = []
            for image in images_list:
                fitsfile = os.path.join (products_dir, image + '.fits')
                if not self._executor._dry_run:
                    task = casa_tasks.exportfits (imagename=image + '.im',
                    fitsimage = fitsfile,  velocity=False, optical=False,
                    bitpix=-32, minpix=0, maxpix=-1, overwrite=True,
                    dropstokes=False, stokeslast=True)
                    self._executor.execute (task)
                elif self._executor._dry_run:
                    LOG.info('FITS: Saving final FITS file is %s' % fitsfile)
                fits_list.append(fitsfile)
        else:
            LOG.info('FITS: There are no CASA image files here')
        return fits_list
     
    def _export_skytsyscal_bl (self, context, products_dir, skytsyscal_bl):
        """
        Save flag_bl / Antenna are to be tar file
        """
        cwd = os.getcwd()
        os.chdir(context.output_dir)
        producted_filename = 'uid___*_*_*.*'
        
        asdm_uidxx = []
        if len(skytsyscal_bl) == 0:
            asdm_uidxx = [ fname for fname in os.listdir(context.output_dir)
                if fnmatch.fnmatch(fname, producted_filename)]
        elif len(skytsyscal_bl) != 0 :
            asdm_uidxx = skytsyscal_bl
            
        list_of_tarname=[]
        tar_filename=[]
        if len(asdm_uidxx) != 0:
            splitwith_dot = [name.split('.')for name in asdm_uidxx]
            
            #identifier setting
            identif_sky_tsys_bl =['skycal','tsyscal','bl']
            
            #selection
            Xasap_outX =[splitwith_dot[i] for i in range(len(splitwith_dot))
                if identif_sky_tsys_bl[0] == splitwith_dot[i][-2] or identif_sky_tsys_bl[1] == splitwith_dot[i][-2] or identif_sky_tsys_bl[2] == splitwith_dot[i][-2] ]
            
            # create tar name
            outname = []
            for i in range(len(Xasap_outX)):
                savename = []
                savename.append(Xasap_outX[i][0])
                for nn in savename:
                    if not nn in outname:
                        outname.append(nn)
            
            # create match number 
            snum2 =[]
            for k in range(len(outname)):
                snum =[i for i in range(len(Xasap_outX))
                    if fnmatch.fnmatch(Xasap_outX[i][0],outname[k])]
                snum2.append(snum)
            
            # re join Xasa_outX
            for i in range(len(Xasap_outX)):
                Xasap_outX[i] = ".".join(Xasap_outX[i])
                
            #tar
            tar_filename = ["".join(outname[i]) + "." + "skycal_tsyscal_bl" + ".tar.gz" for i in range(len(snum2))]
            list_of_tarname=[]
            if not self._executor._dry_run and len(Xasap_outX)!=0:
                for i in range(len(snum2)):
                    LOG.info ('SKY_TSYSCAL_BL: Copying final tar file in %s ' % os.path.join (products_dir,tar_filename[i]))
                    tar = tarfile.open (os.path.join(products_dir, tar_filename[i]), "w:gz")
                    for num in snum2[i]:
                        tar.add (Xasap_outX[num])
                    list_of_tarname.append(tar_filename[i])
                tar.close()
            elif self._executor._dry_run and len(Xasap_outX)!=0:
                for i in range(len(snum2)):
                    for num in snum2[i]:
                        LOG.info('SKY_TSYSCAL_BL: Target Calibrated and Flag_BL is %s' % Xasap_outX[num])
                    LOG.info('SKY_TSYSCAL_BL: Saving final tar file is %s ' % os.path.join (products_dir,tar_filename[i]))
                    list_of_tarname.append(tar_filename[i])
                LOG.info('SKY_TSYSCAL_BL: identifier is %s' % "skycal.tbl/tsyscal.tbl/bl.tbl")
            elif len(Xasap_outX)==0:
                LOG.info('SKY_TSYSCAL_BL: There are no Cal_Flag_bl_coeff(*.skycal.tbl/*.tsyscal.tbl/*.bl.tbl) in output_dir')
        else:
            LOG.info('SKY_TSYSCAL_BL: There are no target files here')
        return list_of_tarname
    
    def _export_weblog (self, context, products_dir):
        """
        Save the processing web log to a tarfile
        """
        # Save the current working directory and move to the pipeline
        # working directory. This is required for tarfile IO
        cwd = os.getcwd()
        os.chdir (context.output_dir)
        
        product_tar_list = []
        if os.path.exists(context.report_dir):
            # Define the name of the output tarfile
            product_tarfilename = 'weblogSD.tar.gz'
            # Create tar file
            product_tar_list = []
            if not self._executor._dry_run:
                LOG.info('WEBLOG: Copying final weblog of SD in %s' % os.path.join(products_dir,product_tarfilename))
                tar = tarfile.open (os.path.join(products_dir, product_tarfilename), "w:gz")
                os.chdir(context.report_dir)
                os.chdir('../')
                tar.add ("html")
                tar.close()
                product_tar_list = [product_tarfilename]
            elif self._executor._dry_run:
                LOG.info('WEBLOG: Target path is  %s' % context.report_dir)
                if os.path.isdir(context.report_dir):
                    LOG.info('WEBLOG: Located html directory is %s' % os.path.relpath(context.report_dir,context.output_dir))
                else:
                    LOG.info('WEBLOG: no html directory is found')
                LOG.info('WEBLOG: Saving final weblog of SD in %s' % os.path.join (products_dir,product_tarfilename))
        else:
            LOG.info('WEBLOG: There is no html directory in report_dir')
        # Restore the original current working directory
        os.chdir(cwd)
        return product_tar_list
      
    def _export_list_txt (self,context, products_dir, inputlist=[],name=[]):
        if not self._executor._dry_run:
            f = open(products_dir + '/' + 'list_of_exported_files.txt','a+')
            for n in name:
                if fnmatch.fnmatch(n, "fitsfiles"):
                    listname_txt = "\n %s : \n --------\n" % n
                    f.write(listname_txt)
                    for i in range(len(inputlist)):
                        for ln in inputlist[i]:
                            if fnmatch.fnmatch(ln,"*fits*"):
                                output_txt = " %s \n" % os.path.basename(ln)
                                f.write(output_txt)
                if fnmatch.fnmatch(n, "skytsyscal_bl"):
                    listname_txt = "\n %s : \n --------\n" % n
                    f.write(listname_txt)
                    for i in range(len(inputlist)):
                        for ln in inputlist[i]:
                            if fnmatch.fnmatch(ln,"*" + "skycal_tsyscal_bl" + "*"):
                                output_txt = " %s \n" % os.path.basename(ln)
                                f.write(output_txt)
                if fnmatch.fnmatch(n, "weblog"):
                    listname_txt = "\n %s : \n --------\n" % n
                    f.write(listname_txt)
                    for i in range(len(inputlist)):
                        for ln in inputlist[i]:
                            if fnmatch.fnmatch(ln,"*weblog*"):
                                output_txt = " %s \n" % os.path.basename(ln)
                                f.write(output_txt)
                if fnmatch.fnmatch(n, "pprfiles"):
                    listname_txt = "\n %s : \n --------\n" % n
                    f.write(listname_txt)
                    for i in range(len(inputlist)):
                        for ln in inputlist[i]:
                            if fnmatch.fnmatch(ln,"*PPR*"):
                                output_txt = " %s \n" % os.path.basename(ln)
                                f.write(output_txt)
                if fnmatch.fnmatch(n, "casa_commands_file"):
                    listname_txt = "\n %s : \n --------\n" % n
                    f.write(listname_txt)
                    for i in range(len(inputlist)):
                        for ln in inputlist[i]:
                            if fnmatch.fnmatch(ln,"*casa_commands*"):
                                output_txt = " %s \n" % os.path.basename(ln)
                                f.write(output_txt)
            f.close()