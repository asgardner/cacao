/**
 * @file    AOloopControl_time_param.c
 * @brief   AO loop control - Timing Parameter
 *
 * REAL TIME COMPUTING ROUTINES
 *
 *
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <string.h>

#include "CommandLineInterface/CLIcore.h"

#include "AOloopControl/AOloopControl.h"
#include "AOloopControl_perfTest/AOloopControl_perfTest.h"
#include "COREMOD_memory/COREMOD_memory.h"

// defined in AOloopControl.c
extern AOLOOPCONTROL_CONF *AOconf; // configuration - this can be an array

// defined in AOloopControl.c
extern AOloopControl_var aoloopcontrol_var;

errno_t AOloopControl_set_loopfrequ(float loopfrequ)
{
    if(aoloopcontrol_var.AOloopcontrol_meminit == 0)
    {
        AOloopControl_InitializeMemory(1);
    }

    AOconf[aoloopcontrol_var.LOOPNUMBER].AOtiminginfo.loopfrequ = loopfrequ;
    AOloopControl_perfTest_showparams(aoloopcontrol_var.LOOPNUMBER);

    return RETURN_SUCCESS;
}

errno_t AOloopControl_set_hardwlatency_frame(float hardwlatency_frame)
{

    if(aoloopcontrol_var.AOloopcontrol_meminit == 0)
    {
        AOloopControl_InitializeMemory(1);
    }

    AOconf[aoloopcontrol_var.LOOPNUMBER].AOtiminginfo.hardwlatency_frame =
        hardwlatency_frame;
    AOloopControl_perfTest_showparams(aoloopcontrol_var.LOOPNUMBER);

    return RETURN_SUCCESS;
}

errno_t AOloopControl_set_complatency_frame(float complatency_frame)
{
    if(aoloopcontrol_var.AOloopcontrol_meminit == 0)
    {
        AOloopControl_InitializeMemory(1);
    }

    AOconf[aoloopcontrol_var.LOOPNUMBER].AOtiminginfo.complatency_frame =
        complatency_frame;
    AOloopControl_perfTest_showparams(aoloopcontrol_var.LOOPNUMBER);

    return RETURN_SUCCESS;
}

errno_t AOloopControl_set_wfsmextrlatency_frame(float wfsmextrlatency_frame)
{
    if(aoloopcontrol_var.AOloopcontrol_meminit == 0)
    {
        AOloopControl_InitializeMemory(1);
    }

    AOconf[aoloopcontrol_var.LOOPNUMBER].AOtiminginfo.wfsmextrlatency_frame =
        wfsmextrlatency_frame;
    AOloopControl_perfTest_showparams(aoloopcontrol_var.LOOPNUMBER);

    return RETURN_SUCCESS;
}
