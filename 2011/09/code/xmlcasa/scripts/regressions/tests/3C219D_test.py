import sys
import os
import string
from locatescript import locatescript
import inspect

a=inspect.stack()
stacklevel=0
for k in range(len(a)):
    if (string.find(a[k][1], 'ipython console') > 0):
        stacklevel=k
        break
gl=sys._getframe(stacklevel).f_globals

# Short description
def description():
    return 'Regression tests for 3C219D'

def run():
    #####locate the regression script
    lepath=locatescript('3C219D_regression.py')
    print 'Script used is ',lepath
    gl['regstate']=True
    execfile(lepath, gl)
    print 'regstate =', gl['regstate']
    if not gl['regstate']:
        raise Exception, 'regstate = False'
###return the images that will be templated and compared in future runs
    return []

def data():
    ### return the data files that is needed by the regression script
    return ['3C219D_CAL.UVFITS','rgn3C219D.rgn']
