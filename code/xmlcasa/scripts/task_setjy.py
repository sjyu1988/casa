import os
import sys
from taskinit import *

def setjy(vis=None, field=None, spw=None, modimage=None, listmodimages=None,
          scalebychan=None, fluxdensity=None, standard=None):
       """ Fills the model column for flux density calibrators:

       The task places the model visibility amp and phase associated
       with a specified clean components image into the model column
       of the data set.  The simplest way is to enter the flux density
       (I, Q, U, V) explicitly.

       Models are available for 3C48, 3C138, and 3C286 between
       1.4 and 43 GHz.  3C147 is available above 13 GHz.  These models
       are scaled to the precise frequency of the data.  Only I models are
       presently available.

       The location of the models are system dependent:  At the AOC, the
       models are in the directory::/usr/lib/casapy/data/nrao/VLA/CalModels

       Setjy need only be run on the calibrator sources with a know flux
       density and/or model.

       Keyword arguments:
       vis -- Name of input visibility file
               default: none.  example: vis='ngc5921.ms'
       field -- Select field using field id(s) or field name(s).
                 [run listobs to obtain the list id's or names of calibrators]
              default: ''=all fields
              Only one field can be used if a source model or specific fluxdensity
                 is inserted.
              If field string is a non-negative integer, it is assumed a field index
                otherwise, it is assumed a field name
              field='0~2'; field ids 0,1,2
              field='0,4,5~7'; field ids 0,4,5,6,7
              field='3C286,3C295'; fields named 3C286 and 3C295
              field = '3,4C*'; field id 3, all names starting with 4C
       spw -- Select spectral window/channels
              default: ''=all spectral windows and channels
              spw='0~2,4'; spectral windows 0,1,2,4 (all channels)
              spw='<2';  spectral windows less than 2 (i.e. 0,1)
              spw='0:5~61'; spw 0, channels 5 to 61
              spw='0,10,3:3~45'; spw 0,10 all channels, spw 3, channels 3 to 45.
              spw='0~2:2~6'; spw 0,1,2 with channels 2 through 6 in each.
              spw='0:0~10;15~60'; spectral window 0 with channels 0-10,15-60
              spw='0:0~10,1:20~30,2:1;2;4'; spw 0, channels 0-10,
                       spw 1, channels 20-30, and spw 2, channels, 1,2 and 4
       modimage -- Optional model image (I only) from which the model visibilities
              are calculated and placed in the model column.  Each field must
              be done separately.  The image flux density will be scaled from the
              frequency in the model to that actually used.
              In CV and the AOC, the models are located in the
              /usr/lib/casapy/data/nrao/VLA/CalModels directory
       fluxdensity -- Specified flux density [I,Q,U,V] in Jy
               default => -1, puts total flux density for known standard source
               places [1,0,0,0] for any other source.
               Otherwise,  places the specified flux density for the source.  This
               is the only way to insert a polarized flux density model at the
               present time (March 2008).
               example:  fluxdensity=[2.63,0.21,-0.33,0.02]
       standard -- Flux density standard, used if fluxdensity<0
               default: 'Perley-Taylor 99'; example: standard='Baars'
               Options: 'Baars','Perley 90','Perley-Taylor 95',
                  'Perley-Taylor 99'
       """
       retval = True
       try:
         casalog.origin('setjy')

         if listmodimages:
             casalog.post("Listing modimage candidates (listmodimages == True).")
             casalog.post("%s is NOT being modified." % vis)

             def lsmodims(path, doanyway=False, modimdir='CalModels',
                          modpat='*', header='Candidate modimages'):
                 """
                 Does an ls -d of files or directories in path matching modpat,
                 but only if path ends in modimdir or doanyway is True.

                 header describes what is being listed.
                 """
                 if os.path.isdir(path):
                     path = path.rstrip('/')
                     if doanyway or path.split('/')[-1] == modimdir:
                         print "\n%s (%s) in %s:" % (header, modpat, path)
                         sys.stdout.flush()
                         os.system('cd ' + path + ';ls -d ' + modpat)

             lsmodims('.', True, modpat='*.im* *.mod*')   
             lsmodims('CalModels')
             
             # Do a walk to find CalModels directories in casa['dirs']['data'].
             # Because data/ contains a lot, and CASA tables are directories,
             # heavy filtering is needed for speed.
             # 7/5/2011:
             # glob('/export/data_1/casa/gnuactive/data/*/CalModels/*') doesn't
             # work.
             permexcludes = ['.svn', 'regression', 'ephemerides', 'geodetic',
                             'gui']
             for path, dirs, fnames in os.walk(casa['dirs']['data'],
                                               followlinks=True):
                 excludes = permexcludes[:]
                 for ext in ['.ms', '.im', '.tab']:
                     excludes += [d for d in dirs if ext in d]
                 for d in excludes:
                     if d in dirs:
                         dirs.remove(d)
                 lsmodims(path)
         else:
             myim = imtool.create()
             myms = mstool.create()

             if type(vis) == str and os.path.isdir(vis):
                    myim.open(vis, usescratch=True)
             else:
                    raise Exception, 'Visibility data set not found - please verify the name'

             myim.setjy(field=field, spw=spw, modimage=modimage,
                        fluxdensity=fluxdensity, standard=standard,
                        scalebychan=scalebychan)
             myim.close()

             # Write history
             try:
                    param_names = setjy.func_code.co_varnames[:setjy.func_code.co_argcount]
                    param_vals = [eval(p) for p in param_names]   
                    retval &= write_history(myms, vis, 'setjy', param_names,
                                            param_vals, casalog)
             except Exception, instance:
                    casalog.post("*** Error \'%s\' updating HISTORY" % (instance),
                                 'WARN')
       except Exception, instance:
              print '*** Error ***',instance
       return retval

