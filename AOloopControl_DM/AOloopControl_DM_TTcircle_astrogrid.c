/**
 * @file    AOloopControl_DM_TTcircle_astrogrid.c
 * @brief   DM control
 *
 * To be used for AOloopControl module
 *
 * @author  O. Guyon
 * @date    22 Dec 2017
 *
 *
 *
 */
#include <math.h>

#include "CommandLineInterface/CLIcore.h"

#include "AOloopControl_DM/AOloopControl_DM.h"
#include "COREMOD_memory/COREMOD_memory.h"

#include "image_basic/image_basic.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419716939937510
#endif

extern long NB_DMindex;

extern AOLOOPCONTROL_DM_DISPCOMB_CONF *dmdispcombconf; // configuration
extern int dmdispcomb_loaded;
extern int SMfd;

extern AOLOOPCONTROL_DMTURBCONF *dmturbconf; // DM turbulence configuration
extern int dmturb_loaded;
extern int SMturbfd;

/* ===============================================================================================
 */
/* ===============================================================================================
 */
/*                                                                                                 */
/* 5. MISC TESTS & UTILS */
/*                                                                                                 */
/* ===============================================================================================
 */
/* ===============================================================================================
 */

long AOloopControl_mkDM_TT_circle(char *IDoutname, long DMindex, long NBpts, float ampl)
{
    long xsize, ysize, zsize, xysize;
    long IDout;
    long ii, jj, kk;
    float x, y, xslope, yslope;

    AOloopControl_DM_loadconf();
    xsize = dmdispcombconf[DMindex].xsize;
    ysize = dmdispcombconf[DMindex].ysize;
    zsize = NBpts;
    xysize = xsize * ysize;

    create_3Dimage_ID(IDoutname, xsize, ysize, zsize, &IDout);

    for (kk = 0; kk < zsize; kk++)
        {
            xslope = ampl * cos(2.0 * M_PI * kk / zsize);
            yslope = ampl * sin(2.0 * M_PI * kk / zsize);

            for (ii = 0; ii < xsize; ii++)
                {
                    x = 2.0 * ii / xsize - 1.0;
                    for (jj = 0; jj < ysize; jj++)
                        {
                            y = 2.0 * jj / ysize - 1.0;
                            data.image[IDout].array.F[kk * xysize + jj * xsize + ii] = x * xslope + y * yslope;
                        }
                }
        }

    return (IDout);
}

//
// XYmode
//	0 : single XYdiag pattern
//	1 : single X pattern
//	2 : single Y pattern
//	3 : single Xdiag pattern
//	4 : single Ydiag pattern
//
//	5 : XYdiag -> OFF ->
//	6 : X -> OFF ->
//	7 : Y -> OFF ->
//	8 : X -> Y ->
//	9 : Xdiag -> OFF ->
// 10 : Ydiag -> OFF ->
// 11 : Xdiag -> Ydiag ->

long AOloopControl_DM_mkAstroGrid_seq(char *IDoutname, long DMindex, int XYmode, int bin, long NBcycle)
{
    long xsize, ysize, zsize, xysize;
    long IDout;
    long ii, jj, kk, kk1;
    long IDx, IDy, IDxy, IDxd, IDyd;
    int sign = 1;

    AOloopControl_DM_loadconf();
    xsize = dmdispcombconf[DMindex].xsize;
    ysize = dmdispcombconf[DMindex].ysize;
    xysize = xsize * ysize;

    create_2Dimage_ID("_tmpX", xsize, ysize, &IDx);
    for (ii = 0; ii < xsize; ii++)
        for (jj = 0; jj < ysize; jj++)
            {
                if ((ii / bin) % 2 == 0)
                    {
                        data.image[IDx].array.F[jj * xsize + ii] = 1.0;
                    }
                else
                    {
                        data.image[IDx].array.F[jj * xsize + ii] = -1.0;
                    }
            }

    create_2Dimage_ID("_tmpY", xsize, ysize, &IDy);
    for (ii = 0; ii < xsize; ii++)
        for (jj = 0; jj < ysize; jj++)
            {
                if ((jj / bin) % 2 == 0)
                    {
                        data.image[IDy].array.F[jj * xsize + ii] = 1.0;
                    }
                else
                    {
                        data.image[IDy].array.F[jj * xsize + ii] = -1.0;
                    }
            }

    create_2Dimage_ID("_tmpXY", xsize, ysize, &IDxy);
    for (ii = 0; ii < xsize; ii++)
        for (jj = 0; jj < ysize; jj++)
            {
                data.image[IDxy].array.F[jj * xsize + ii] =
                    data.image[IDx].array.F[jj * xsize + ii] * data.image[IDy].array.F[jj * xsize + ii];
            }

    create_2Dimage_ID("_tmpXd", xsize, ysize, &IDxd);
    for (ii = 0; ii < xsize; ii++)
        for (jj = 0; jj < ysize; jj++)
            {
                if ((ii / bin + jj / bin) % 4 == 0)
                    {
                        data.image[IDxd].array.F[jj * xsize + ii] = 1.0;
                    }
                if ((ii / bin + jj / bin) % 4 == 2)
                    {
                        data.image[IDxd].array.F[jj * xsize + ii] = -1.0;
                    }
            }

    create_2Dimage_ID("_tmpYd", xsize, ysize, &IDyd);
    for (ii = 0; ii < xsize; ii++)
        for (jj = 0; jj < ysize; jj++)
            {
                if ((ii / bin - jj / bin + ysize / bin) % 4 == 0)
                    {
                        data.image[IDyd].array.F[jj * xsize + ii] = 1.0;
                    }
                if ((ii / bin - jj / bin + ysize / bin) % 4 == 2)
                    {
                        data.image[IDyd].array.F[jj * xsize + ii] = -1.0;
                    }
            }

    switch (XYmode)
        {

        case 0:        // single XY-diag pattern
            zsize = 2; // only 2 frames
            create_3Dimage_ID(IDoutname, xsize, ysize, zsize, &IDout);
            kk = 0;
            for (ii = 0; ii < xsize; ii++)
                for (jj = 0; jj < ysize; jj++)
                    {
                        data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                            data.image[IDxy].array.F[jj * xsize + ii];
                    }
            kk = 1;
            for (ii = 0; ii < xsize; ii++)
                for (jj = 0; jj < ysize; jj++)
                    {
                        data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                            -data.image[IDxy].array.F[jj * xsize + ii];
                    }
            break;

        case 1:        // single X pattern
            zsize = 2; // only 2 frames
            create_3Dimage_ID(IDoutname, xsize, ysize, zsize, &IDout);
            kk = 0;
            for (ii = 0; ii < xsize; ii++)
                for (jj = 0; jj < ysize; jj++)
                    {
                        data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                            data.image[IDx].array.F[jj * xsize + ii];
                    }
            kk = 1;
            for (ii = 0; ii < xsize; ii++)
                for (jj = 0; jj < ysize; jj++)
                    {
                        data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                            -data.image[IDx].array.F[jj * xsize + ii];
                    }
            break;

        case 2:        // single Y pattern
            zsize = 2; // only 2 frames
            create_3Dimage_ID(IDoutname, xsize, ysize, zsize, &IDout);
            kk = 0;
            for (ii = 0; ii < xsize; ii++)
                for (jj = 0; jj < ysize; jj++)
                    {
                        data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                            data.image[IDy].array.F[jj * xsize + ii];
                    }
            kk = 1;
            for (ii = 0; ii < xsize; ii++)
                for (jj = 0; jj < ysize; jj++)
                    {
                        data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                            -data.image[IDy].array.F[jj * xsize + ii];
                    }
            break;

        case 3:        // single Xdiag pattern
            zsize = 2; // only 2 frames
            create_3Dimage_ID(IDoutname, xsize, ysize, zsize, &IDout);
            kk = 0;
            for (ii = 0; ii < xsize; ii++)
                for (jj = 0; jj < ysize; jj++)
                    {
                        data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                            data.image[IDxd].array.F[jj * xsize + ii];
                    }
            kk = 1;
            for (ii = 0; ii < xsize; ii++)
                for (jj = 0; jj < ysize; jj++)
                    {
                        data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                            -data.image[IDxd].array.F[jj * xsize + ii];
                    }
            break;

        case 4:        // single Ydiag pattern
            zsize = 2; // only 2 frames
            create_3Dimage_ID(IDoutname, xsize, ysize, zsize, &IDout);
            kk = 0;
            for (ii = 0; ii < xsize; ii++)
                for (jj = 0; jj < ysize; jj++)
                    {
                        data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                            data.image[IDyd].array.F[jj * xsize + ii];
                    }
            kk = 1;
            for (ii = 0; ii < xsize; ii++)
                for (jj = 0; jj < ysize; jj++)
                    {
                        data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                            -data.image[IDyd].array.F[jj * xsize + ii];
                    }
            break;

        case 5: // XYdiag -> OFF ->
            zsize = 2 * NBcycle * 2;
            create_3Dimage_ID(IDoutname, xsize, ysize, zsize, &IDout);
            sign = 1;
            for (kk = 0; kk < 2 * NBcycle; kk++)
                {
                    for (ii = 0; ii < xsize; ii++)
                        for (jj = 0; jj < ysize; jj++)
                            {
                                data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                                    sign * data.image[IDxy].array.F[jj * xsize + ii];
                            }
                    sign *= -1;
                }
            break;

        case 6: // X -> OFF ->
            zsize = 2 * NBcycle * 2;
            create_3Dimage_ID(IDoutname, xsize, ysize, zsize, &IDout);
            sign = 1;
            for (kk = 0; kk < 2 * NBcycle; kk++)
                {
                    for (ii = 0; ii < xsize; ii++)
                        for (jj = 0; jj < ysize; jj++)
                            {
                                data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                                    sign * data.image[IDx].array.F[jj * xsize + ii];
                            }
                    sign *= -1;
                }
            break;

        case 7: // Y -> OFF ->
            zsize = 2 * NBcycle * 2;
            create_3Dimage_ID(IDoutname, xsize, ysize, zsize, &IDout);
            sign = 1;
            for (kk = 0; kk < 2 * NBcycle; kk++)
                {
                    for (ii = 0; ii < xsize; ii++)
                        for (jj = 0; jj < ysize; jj++)
                            {
                                data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                                    sign * data.image[IDy].array.F[jj * xsize + ii];
                            }
                    sign *= -1;
                }
            break;

        case 8: // X -> Y ->
            zsize = 2 * NBcycle * 2;
            create_3Dimage_ID(IDoutname, xsize, ysize, zsize, &IDout);
            sign = 1;
            kk = 0;
            for (kk1 = 0; kk1 < 2 * NBcycle; kk1++)
                {
                    for (ii = 0; ii < xsize; ii++)
                        for (jj = 0; jj < ysize; jj++)
                            {
                                data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                                    sign * data.image[IDx].array.F[jj * xsize + ii];
                            }
                    sign *= -1;
                    kk++;
                }
            for (kk1 = 0; kk1 < 2 * NBcycle; kk1++)
                {
                    for (ii = 0; ii < xsize; ii++)
                        for (jj = 0; jj < ysize; jj++)
                            {
                                data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                                    sign * data.image[IDy].array.F[jj * xsize + ii];
                            }
                    sign *= -1;
                    kk++;
                }
            break;

        case 9: // Xdiag -> OFF ->
            zsize = 2 * NBcycle * 2;
            create_3Dimage_ID(IDoutname, xsize, ysize, zsize, &IDout);
            sign = 1;
            for (kk = 0; kk < 2 * NBcycle; kk++)
                {
                    for (ii = 0; ii < xsize; ii++)
                        for (jj = 0; jj < ysize; jj++)
                            {
                                data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                                    sign * data.image[IDxd].array.F[jj * xsize + ii];
                            }
                    sign *= -1;
                }
            break;

        case 10: // Ydiag -> OFF ->
            zsize = 2 * NBcycle * 2;
            create_3Dimage_ID(IDoutname, xsize, ysize, zsize, &IDout);
            sign = 1;
            for (kk = 0; kk < 2 * NBcycle; kk++)
                {
                    for (ii = 0; ii < xsize; ii++)
                        for (jj = 0; jj < ysize; jj++)
                            {
                                data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                                    sign * data.image[IDyd].array.F[jj * xsize + ii];
                            }
                    sign *= -1;
                }
            break;

        case 11: // Xdiag -> Ydiag ->
            zsize = 2 * NBcycle * 2;
            create_3Dimage_ID(IDoutname, xsize, ysize, zsize, &IDout);
            sign = 1;
            kk = 0;
            for (kk1 = 0; kk1 < 2 * NBcycle; kk1++)
                {
                    for (ii = 0; ii < xsize; ii++)
                        for (jj = 0; jj < ysize; jj++)
                            {
                                data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                                    sign * data.image[IDxd].array.F[jj * xsize + ii];
                            }
                    sign *= -1;
                    kk++;
                }
            for (kk1 = 0; kk1 < 2 * NBcycle; kk1++)
                {
                    for (ii = 0; ii < xsize; ii++)
                        for (jj = 0; jj < ysize; jj++)
                            {
                                data.image[IDout].array.F[kk * xysize + jj * ysize + ii] =
                                    sign * data.image[IDyd].array.F[jj * xsize + ii];
                            }
                    sign *= -1;
                    kk++;
                }
            break;
        }

    return (IDout);
}
