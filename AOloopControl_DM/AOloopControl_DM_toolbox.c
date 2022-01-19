/**
 * @file    AOloopControl_DM_toolbox.c
 * @brief   DM control
 *
 *
 *
 *
 */

#include "CommandLineInterface/CLIcore.h"

#include "AOloopControl_DM/AOloopControl_DM.h"
#include "COREMOD_arith/COREMOD_arith.h"
#include "COREMOD_memory/COREMOD_memory.h"

#include "fft/fft.h"
#include "image_gen/image_gen.h"
#include "info/info.h"
#include <math.h>

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
/*  TOOLBOX */
/*                                                                                                 */
/* ===============================================================================================
 */
/* ===============================================================================================
 */

struct timespec time_diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0)
        {
            temp.tv_sec = end.tv_sec - start.tv_sec - 1;
            temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
        }
    else
        {
            temp.tv_sec = end.tv_sec - start.tv_sec;
            temp.tv_nsec = end.tv_nsec - start.tv_nsec;
        }
    return temp;
}

// local copy of function in module AtmosphericTurbulence

//
// innerscale and outerscale in pixel
// von Karman spectrum
//
errno_t make_master_turbulence_screen_local(
    const char *ID_name1, const char *ID_name2, long size, float outerscale, float innerscale)
{
    imageID ID;
    float value, C1, C2;
    long cnt;
    long Dlim = 3;
    imageID IDv;

    // int OUTERSCALE_MODE = 1; // 1 if outer scale
    double OUTERscale_f0;
    double INNERscale_f0;
    double dx, dy, r;
    double rlim = 0.0;
    int RLIMMODE = 0;
    double iscoeff;

    /*  IDv = variable_ID("OUTERSCALE");
    if(IDv!=-1)
      {
        outerscale = data.variable[IDv].value.f;
        printf("Outer scale = %f pix\n", outerscale);
      }
   */

    IDv = variable_ID("RLIM");
    if (IDv != -1)
        {
            RLIMMODE = 1;
            rlim = data.variable[IDv].value.f;
            printf("R limit = %f pix\n", rlim);
        }

    OUTERscale_f0 = 1.0 * size / outerscale; // [1/pix] in F plane
    INNERscale_f0 = (5.92 / (2.0 * M_PI)) * size / innerscale;

    make_rnd("tmppha", size, size, "");
    arith_image_cstmult("tmppha", 2.0 * PI, "tmppha1");
    delete_image_ID("tmppha", DELETE_IMAGE_ERRMODE_WARNING);
    //  make_dist("tmpd",size,size,size/2,size/2);
    create_2Dimage_ID("tmpd", size, size, &ID);
    for (uint32_t ii = 0; ii < size; ii++)
        for (uint32_t jj = 0; jj < size; jj++)
            {
                dx = 1.0 * ii - size / 2;
                dy = 1.0 * jj - size / 2;

                if (RLIMMODE == 1)
                    {
                        r = sqrt(dx * dx + dy * dy);
                        if (r < rlim)
                            {
                                data.image[ID].array.F[jj * size + ii] = 0.0;
                            }
                        else
                            {
                                data.image[ID].array.F[jj * size + ii] =
                                    sqrt(dx * dx + dy * dy + OUTERscale_f0 * OUTERscale_f0);
                            }
                    }
                else
                    {
                        data.image[ID].array.F[jj * size + ii] =
                            sqrt(dx * dx + dy * dy + OUTERscale_f0 * OUTERscale_f0);
                    }
            }
    //  data.image[ID].array.F[size/2*size+size/2+10] = 1.0;

    // period [pix] = size/sqrt(dx*dx+dy*dy)
    // f [1/pix] = sqrt(dx*dx+dy*dy)/size
    // f [1/pix] * size = sqrt(dx*dx+dy*dy)

    make_rnd("tmpg", size, size, "-gauss");
    ID = image_ID("tmpg");
    for (uint32_t ii = 0; ii < size; ii++)
        for (uint32_t jj = 0; jj < size; jj++)
            {
                dx = 1.0 * ii - size / 2;
                dy = 1.0 * jj - size / 2;
                iscoeff = exp(-(dx * dx + dy * dy) / INNERscale_f0 / INNERscale_f0);
                data.image[ID].array.F[jj * size + ii] *= sqrt(iscoeff); // power -> amplitude : sqrt
            }

    arith_image_cstpow("tmpd", 11.0 / 6.0, "tmpd1");
    delete_image_ID("tmpd", DELETE_IMAGE_ERRMODE_WARNING);
    arith_image_div("tmpg", "tmpd1", "tmpamp");
    delete_image_ID("tmpg", DELETE_IMAGE_ERRMODE_WARNING);
    delete_image_ID("tmpd1", DELETE_IMAGE_ERRMODE_WARNING);
    arith_set_pixel("tmpamp", 0.0, size / 2, size / 2);
    mk_complex_from_amph("tmpamp", "tmppha1", "tmpc", 0);
    delete_image_ID("tmpamp", DELETE_IMAGE_ERRMODE_WARNING);
    delete_image_ID("tmppha1", DELETE_IMAGE_ERRMODE_WARNING);
    permut("tmpc");
    do2dfft("tmpc", "tmpcf");
    delete_image_ID("tmpc", DELETE_IMAGE_ERRMODE_WARNING);
    mk_reim_from_complex("tmpcf", "tmpo1", "tmpo2", 0);
    delete_image_ID("tmpcf", DELETE_IMAGE_ERRMODE_WARNING);

    /* compute the scaling factor in the power law of the structure function */
    fft_structure_function("tmpo1", "strf");
    ID = image_ID("strf");
    value = 0.0;
    cnt = 0;
    for (uint32_t ii = 1; ii < Dlim; ii++)
        for (uint32_t jj = 1; jj < Dlim; jj++)
            {
                value += log10(data.image[ID].array.F[jj * size + ii]) - 5.0 / 3.0 * log10(sqrt(ii * ii + jj * jj));
                cnt++;
            }
    // save_fl_fits("strf","strf.fits");
    delete_image_ID("strf", DELETE_IMAGE_ERRMODE_WARNING);
    C1 = pow(10.0, value / cnt);

    fft_structure_function("tmpo2", "strf");
    ID = image_ID("strf");
    value = 0.0;
    cnt = 0;
    for (uint32_t ii = 1; ii < Dlim; ii++)
        for (uint32_t jj = 1; jj < Dlim; jj++)
            {
                value += log10(data.image[ID].array.F[jj * size + ii]) - 5.0 / 3.0 * log10(sqrt(ii * ii + jj * jj));
                cnt++;
            }
    delete_image_ID("strf", DELETE_IMAGE_ERRMODE_WARNING);
    C2 = pow(10.0, value / cnt);

    printf("%f %f\n", C1, C2);

    arith_image_cstmult("tmpo1", 1.0 / sqrt(C1), ID_name1);
    arith_image_cstmult("tmpo2", 1.0 / sqrt(C2), ID_name2);
    delete_image_ID("tmpo1", DELETE_IMAGE_ERRMODE_WARNING);
    delete_image_ID("tmpo2", DELETE_IMAGE_ERRMODE_WARNING);

    return RETURN_SUCCESS;
}
