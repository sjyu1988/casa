/*
 * Copyright (c) 2003, 2007-8 Matteo Frigo
 * Copyright (c) 2003, 2007-8 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Mon Feb  9 19:54:11 EST 2009 */

#include "codelet-rdft.h"

#ifdef HAVE_FMA

/* Generated by: ../../../genfft/gen_r2cf -fma -reorder-insns -schedule-for-pipeline -compact -variables 4 -pipeline-latency 4 -n 4 -name r2cfII_4 -dft-II -include r2cfII.h */

/*
 * This function contains 6 FP additions, 4 FP multiplications,
 * (or, 2 additions, 0 multiplications, 4 fused multiply/add),
 * 8 stack variables, 1 constants, and 8 memory accesses
 */
#include "r2cfII.h"

static void r2cfII_4(R *R0, R *R1, R *Cr, R *Ci, stride rs, stride csr, stride csi, INT v, INT ivs, INT ovs)
{
     DK(KP707106781, +0.707106781186547524400844362104849039284835938);
     INT i;
     for (i = v; i > 0; i = i - 1, R0 = R0 + ivs, R1 = R1 + ivs, Cr = Cr + ovs, Ci = Ci + ovs, MAKE_VOLATILE_STRIDE(rs), MAKE_VOLATILE_STRIDE(csr), MAKE_VOLATILE_STRIDE(csi)) {
	  E T1, T5, T2, T3, T4, T6;
	  T1 = R0[0];
	  T5 = R0[WS(rs, 1)];
	  T2 = R1[0];
	  T3 = R1[WS(rs, 1)];
	  T4 = T2 - T3;
	  T6 = T2 + T3;
	  Ci[0] = -(FMA(KP707106781, T6, T5));
	  Ci[WS(csi, 1)] = FNMS(KP707106781, T6, T5);
	  Cr[0] = FMA(KP707106781, T4, T1);
	  Cr[WS(csr, 1)] = FNMS(KP707106781, T4, T1);
     }
}

static const kr2c_desc desc = { 4, "r2cfII_4", {2, 0, 4, 0}, &GENUS };

void X(codelet_r2cfII_4) (planner *p) {
     X(kr2c_register) (p, r2cfII_4, &desc);
}

#else				/* HAVE_FMA */

/* Generated by: ../../../genfft/gen_r2cf -compact -variables 4 -pipeline-latency 4 -n 4 -name r2cfII_4 -dft-II -include r2cfII.h */

/*
 * This function contains 6 FP additions, 2 FP multiplications,
 * (or, 6 additions, 2 multiplications, 0 fused multiply/add),
 * 8 stack variables, 1 constants, and 8 memory accesses
 */
#include "r2cfII.h"

static void r2cfII_4(R *R0, R *R1, R *Cr, R *Ci, stride rs, stride csr, stride csi, INT v, INT ivs, INT ovs)
{
     DK(KP707106781, +0.707106781186547524400844362104849039284835938);
     INT i;
     for (i = v; i > 0; i = i - 1, R0 = R0 + ivs, R1 = R1 + ivs, Cr = Cr + ovs, Ci = Ci + ovs, MAKE_VOLATILE_STRIDE(rs), MAKE_VOLATILE_STRIDE(csr), MAKE_VOLATILE_STRIDE(csi)) {
	  E T1, T6, T4, T5, T2, T3;
	  T1 = R0[0];
	  T6 = R0[WS(rs, 1)];
	  T2 = R1[0];
	  T3 = R1[WS(rs, 1)];
	  T4 = KP707106781 * (T2 - T3);
	  T5 = KP707106781 * (T2 + T3);
	  Cr[WS(csr, 1)] = T1 - T4;
	  Ci[WS(csi, 1)] = T6 - T5;
	  Cr[0] = T1 + T4;
	  Ci[0] = -(T5 + T6);
     }
}

static const kr2c_desc desc = { 4, "r2cfII_4", {6, 2, 0, 0}, &GENUS };

void X(codelet_r2cfII_4) (planner *p) {
     X(kr2c_register) (p, r2cfII_4, &desc);
}

#endif				/* HAVE_FMA */
