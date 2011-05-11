//# tPrecTimer.cc: Test program for class PrecTimer
//# Copyright (C) 2006
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: tPrecTimer.cc 19252 2006-02-10 08:14:10Z gvandiep $

#include <cmath>
#include <iostream>
#include <casa/OS/PrecTimer.h>
#include <casa/OS/Timer.h>
#include <casa/BasicSL/String.h>

using namespace casa;

int main()
{
  PrecTimer timer;
  Timer ttimer;

  double a = 1;
  for (int i=0; i<1000000; i++) {
    timer.start();
    a = pow(a, a);
    timer.stop();
  }

  ttimer.show ("Timer    ");
  timer.show  ("PrecTimer");
  return 0;
}
