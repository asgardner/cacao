/**
 * @file    AOloopControl_autotune.c
 * @brief   AO loop control - INTEGRATOR AUTO TUNING
 *
 * REAL TIME COMPUTING ROUTINES
 *
 *
 */

#define _GNU_SOURCE

#include "AOloopControl/AOloopControl.h"
#include "AOloopControl_perfTest/AOloopControl_perfTest.h"
#include "COREMOD_memory/COREMOD_memory.h"
#include "CommandLineInterface/CLIcore.h"

// defined in AOloopControl.c
extern AOLOOPCONTROL_CONF *AOconf; // configuration - this can be an array

// defined in AOloopControl.c
extern AOloopControl_var aoloopcontrol_var;

/* =============================================================================================== */
/** @name AOloopControl - 8.4. LOOP CONTROL INTERFACE - INTEGRATOR AUTO TUNING                     */
/* =============================================================================================== */

errno_t AOloopControl_AUTOTUNE_LIMITS_on()
{
    if (aoloopcontrol_var.AOloopcontrol_meminit == 0)
        AOloopControl_InitializeMemory(1);

    AOconf[aoloopcontrol_var.LOOPNUMBER].AOAutoTune.AUTOTUNE_LIMITS_ON = 1;
    AOloopControl_perfTest_showparams(aoloopcontrol_var.LOOPNUMBER);

    return RETURN_SUCCESS;
}

errno_t AOloopControl_AUTOTUNE_LIMITS_off()
{
    int block;
    int NBblock;

    if (aoloopcontrol_var.AOloopcontrol_meminit == 0)
        AOloopControl_InitializeMemory(1);

    AOconf[aoloopcontrol_var.LOOPNUMBER].AOAutoTune.AUTOTUNE_LIMITS_ON = 0;
    AOloopControl_perfTest_showparams(aoloopcontrol_var.LOOPNUMBER);

    if (aoloopcontrol_var.aoconfID_limitb == -1)
    {
        char imname[200];

        if (sprintf(imname, "aol%ld_limitb", aoloopcontrol_var.LOOPNUMBER) < 1)
            PRINT_ERROR("sprintf wrote <1 char");

        aoloopcontrol_var.aoconfID_limitb = read_sharedmem_image(imname);
    }

    NBblock = data.image[aoloopcontrol_var.aoconfID_limitb].md[0].size[0];

    // Save Limits
    for (block = 0; block < NBblock; block++)
    {
        FILE *fp;
        char fname[200];

        sprintf(fname, "conf/param_limitb%02d.txt", block);

        if ((fp = fopen(fname, "w")) == NULL)
            PRINT_ERROR("Cannot open file");
        else
        {
            fprintf(fp, "%7.5f\n", data.image[aoloopcontrol_var.aoconfID_limitb].array.F[block]);
        }

        fclose(fp);
    }

    return RETURN_SUCCESS;
}

errno_t AOloopControl_set_AUTOTUNE_LIMITS_delta(float AUTOTUNE_LIMITS_delta)
{
    if (aoloopcontrol_var.AOloopcontrol_meminit == 0)
        AOloopControl_InitializeMemory(1);

    AOconf[aoloopcontrol_var.LOOPNUMBER].AOAutoTune.AUTOTUNE_LIMITS_delta = AUTOTUNE_LIMITS_delta;
    AOloopControl_perfTest_showparams(aoloopcontrol_var.LOOPNUMBER);

    return RETURN_SUCCESS;
}

errno_t AOloopControl_set_AUTOTUNE_LIMITS_perc(float AUTOTUNE_LIMITS_perc)
{
    if (aoloopcontrol_var.AOloopcontrol_meminit == 0)
        AOloopControl_InitializeMemory(1);

    AOconf[aoloopcontrol_var.LOOPNUMBER].AOAutoTune.AUTOTUNE_LIMITS_perc = AUTOTUNE_LIMITS_perc;
    AOloopControl_perfTest_showparams(aoloopcontrol_var.LOOPNUMBER);

    return RETURN_SUCCESS;
}

errno_t AOloopControl_set_AUTOTUNE_LIMITS_mcoeff(float AUTOTUNE_LIMITS_mcoeff)
{
    if (aoloopcontrol_var.AOloopcontrol_meminit == 0)
        AOloopControl_InitializeMemory(1);

    AOconf[aoloopcontrol_var.LOOPNUMBER].AOAutoTune.AUTOTUNE_LIMITS_mcoeff = AUTOTUNE_LIMITS_mcoeff;
    AOloopControl_perfTest_showparams(aoloopcontrol_var.LOOPNUMBER);

    return RETURN_SUCCESS;
}

errno_t AOloopControl_AUTOTUNE_GAINS_on()
{
    if (aoloopcontrol_var.AOloopcontrol_meminit == 0)
        AOloopControl_InitializeMemory(1);

    AOconf[aoloopcontrol_var.LOOPNUMBER].AOAutoTune.AUTOTUNE_GAINS_ON = 1;
    AOloopControl_perfTest_showparams(aoloopcontrol_var.LOOPNUMBER);

    return RETURN_SUCCESS;
}

errno_t AOloopControl_AUTOTUNE_GAINS_off()
{
    if (aoloopcontrol_var.AOloopcontrol_meminit == 0)
        AOloopControl_InitializeMemory(1);

    AOconf[aoloopcontrol_var.LOOPNUMBER].AOAutoTune.AUTOTUNE_GAINS_ON = 0;
    AOloopControl_perfTest_showparams(aoloopcontrol_var.LOOPNUMBER);

    return RETURN_SUCCESS;
}
