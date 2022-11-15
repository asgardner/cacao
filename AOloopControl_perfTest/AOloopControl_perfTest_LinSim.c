/**
 * @file    AOloopControl_perfTest_LinSim.c
 * @brief   Adaptive Optics Control loop linear simulator
 *
 * Uses response matrix for linear simulation
 *
 *
 *
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// uncomment for test print statements to stdout
//#define _PRINT_TEST

/* ===============================================================================================
 */
/* ===============================================================================================
 */
/*                                        HEADER FILES */
/* ===============================================================================================
 */
/* ===============================================================================================
 */
#include <math.h>
#include <string.h>

#include "CommandLineInterface/CLIcore.h"

#include "COREMOD_iofits/COREMOD_iofits.h"
#include "COREMOD_memory/COREMOD_memory.h"

#include "AOloopControl/AOloopControl.h"
#include "info/info.h"
#include "linopt_imtools/linopt_imtools.h"

#include "AOloopControl_perfTest/AOloopControl_perfTest.h"

/* ===============================================================================================
 */
/* ===============================================================================================
 */
/*                                      DEFINES, MACROS */
/* ===============================================================================================
 */
/* ===============================================================================================
 */

#ifdef _OPENMP
#include <omp.h>
#define OMP_NELEMENT_LIMIT 1000000
#endif

/* ===============================================================================================
 */
/* ===============================================================================================
 */
/*                                  GLOBAL DATA DECLARATION */
/* ===============================================================================================
 */
/* ===============================================================================================
 */

/* ===============================================================================================
 */
/*                                     MAIN DATA STRUCTURES */
/* ===============================================================================================
 */

errno_t AOcontrolLoop_perfTest_LinearSimulator_FPCONF(const char *fpsname,
        uint32_t    CMDmode)
{
    FPS_SETUP_INIT(fpsname, CMDmode);

    function_parameter_add_entry(&fps,
                                 ".DMxsize",
                                 "Deformable mirror X size",
                                 FPTYPE_INT64,
                                 FPFLAG_DEFAULT_INPUT,
                                 NULL,
                                 NULL);

    function_parameter_add_entry(&fps,
                                 ".DMysize",
                                 "Deformable mirror Y size",
                                 FPTYPE_INT64,
                                 FPFLAG_DEFAULT_INPUT,
                                 NULL,
                                 NULL);

    FPS_CONFLOOP_START // macro in function_parameter.h

    FPS_CONFLOOP_END // macro in function_parameter.h

    return RETURN_SUCCESS;
}

errno_t AOcontrolLoop_perfTest_LinearSimulator_RUN(const char *fpsname)
{
    FUNCTION_PARAMETER_STRUCT fps;
    // int SMfd = -1;

    int FPSINTERFACE = 1;

    if(function_parameter_struct_connect(fpsname, &fps, FPSCONNECT_RUN) == -1)
    {
        printf(
            "ERROR: fps \"%s\" does not exist -> running without FPS "
            "interface\n",
            fpsname);
        FPSINTERFACE = 0;
    }
    else
    {
        FPSINTERFACE = 1;
    }

    if(FPSINTERFACE == 1)
    {
        function_parameter_struct_disconnect(&fps);
    }

    return (0);
}
