/* ###*B*###
 * Erika Enterprise, version 3
 * 
 * Copyright (C) 2017 - 2018 Evidence s.r.l.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License, version 2, for more details.
 * 
 * You should have received a copy of the GNU General Public License,
 * version 2, along with this program; if not, see
 * < www.gnu.org/licenses/old-licenses/gpl-2.0.html >.
 * 
 * This program is distributed to you subject to the following
 * clarifications and special exceptions to the GNU General Public
 * License, version 2.
 * 
 * THIRD PARTIES' MATERIALS
 * 
 * Certain materials included in this library are provided by third
 * parties under licenses other than the GNU General Public License. You
 * may only use, copy, link to, modify and redistribute this library
 * following the terms of license indicated below for third parties'
 * materials.
 * 
 * In case you make modified versions of this library which still include
 * said third parties' materials, you are obligated to grant this special
 * exception.
 * 
 * The complete list of Third party materials allowed with ERIKA
 * Enterprise version 3, together with the terms and conditions of each
 * license, is present in the file THIRDPARTY.TXT in the root of the
 * project.
 * ###*E*### */

#include "disparity.h"

#include <architecture/tsc.h>
#include <real-time.h>

using namespace EPOS;

OStream cout;

#define MAX_EXECS 1000

// img1 and img2 size dependent
#define PERIOD   6000000// IA32 == 160000 
#define DEADLINE 6000000// IA32 == 160000
#define WCET     5700000// IA32 == 155000


extern signed char img1[];
extern signed char img2[];

/* First task */
int FuncTask1()
{
    int rows = 32;
    int cols = 32;
I2D *imleft, *imright, *retDisparity;

    //char * im2, * im1;
    int WIN_SZ=8, SHIFT=64;

    // Reading BMP image
    char signature[2]= {66, 77};



    short int bits_per_pixel = 24;

    unsigned long long startCycles;
    unsigned long long endCycles;
    unsigned long long elapsed;

    u32 sctlr;


    I2D* srcImage;

    int iterations;
    for(iterations = 0; iterations < MAX_EXECS; iterations++) {
        Periodic_Thread::wait_next();
        
        srcImage = (I2D*)img1;

        if(srcImage->height <= 0 || srcImage->width <= 0 || signature[0] != 'B' || signature[1] != 'M'  || ( bits_per_pixel !=24 && bits_per_pixel !=8 ) )
        {
                cout << "ERROR in BMP read: The input file is not in standard BMP format" << endl;
                return 0;
        }


        srcImage = (I2D*)img2;

        if(srcImage->height <= 0 || srcImage->width <= 0 || signature[0] != 'B' || signature[1] != 'M'  || ( bits_per_pixel !=24 && bits_per_pixel !=8 ) )
        {
            cout << "ERROR in BMP read: The input file is not in standard BMP format" << endl;
            return 0;
        }



        imleft  = (I2D *) img1;
        imright = (I2D *) img2;


        rows = imleft->height;
        cols = imleft->width;

    #ifdef test
        WIN_SZ = 2;
        SHIFT = 1;
    #endif
    #ifdef sim_fast
        WIN_SZ = 4;
        SHIFT = 4;
    #endif
    #ifdef sim
        WIN_SZ = 4;
        SHIFT = 8;
    #endif

        (void)SHIFT;
        (void)WIN_SZ;

        startCycles = TSC::time_stamp();

        retDisparity = getDisparity(imleft, imright, WIN_SZ, SHIFT);
        endCycles = TSC::time_stamp();

        unsigned long long diff = endCycles - startCycles;

        cout << "Elapsed " << (diff* 1000000)/TSC::frequency() << "usec, Cycles" << diff << endl;

    }

    return 0;
}

/******************************************************************************
 *  Main
 *****************************************************************************/

int main(void)
{
    Thread *t = new Periodic_Thread(RTConf(PERIOD, MAX_EXECS, Thread::READY ,
            Thread::Criterion((RTC::Microsecond) PERIOD, DEADLINE,
            WCET, 0)), &FuncTask1);

    cout << "Disparity Periodic: All TASKs created with result (>0 is OK): " << t << endl;

    t->join();

    cout << "Disparity Periodic | Returned from application main" << endl; 

    delete t;

    return 0;
}
