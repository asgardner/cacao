/**
 * @file    AOloopControl_DM_init.c
 * @brief   DM control
 *
 * To be used for AOloopControl module
 *
 *
 *
 */

#include <fcntl.h>
#include <math.h>
#include <sys/mman.h>

#include "CommandLineInterface/CLIcore.h"

#include "AOloopControl_DM/AOloopControl_DM.h"

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
/* 1. INITIALIZATION, LOAD/CREATE */
/*                                                                                                 */
/* ===============================================================================================
 */
/* ===============================================================================================
 */

static errno_t AOloopControl_DM_createconf()
{
    int result;
    int ch;
    char fname[STRINGMAXLEN_FULLFILENAME];
    long DMindex;
    char errstr[STRINGMAXLEN_ERRORMSG];

    sprintf(fname, "%s/dmdispcombconf.conf.shm", data.shmdir);

    if (dmdispcomb_loaded == 0)
        {
            printf("Create/read DM configuration, %ld entries\n", NB_DMindex);

            SMfd = open(fname, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
            if (SMfd == -1)
                {
                    int slen = snprintf(errstr,
                                        STRINGMAXLEN_ERRORMSG,
                                        "Error opening (O_RDWR | O_CREAT | O_TRUNC) file "
                                        "\"%s\", function AOloopControl_DM_createconf",
                                        fname);
                    if (slen < 1)
                        {
                            PRINT_ERROR("snprintf wrote <1 char");
                            abort(); // can't handle this error any other way
                        }
                    if (slen >= STRINGMAXLEN_ERRORMSG)
                        {
                            PRINT_ERROR("snprintf string truncation");
                            abort(); // can't handle this error any other way
                        }
                    perror(errstr);
                    exit(EXIT_FAILURE);
                }

            result = lseek(SMfd, sizeof(AOLOOPCONTROL_DM_DISPCOMB_CONF) * NB_DMindex - 1, SEEK_SET);
            if (result == -1)
                {
                    close(SMfd);
                    perror("Error calling lseek() to 'stretch' the file");
                    exit(EXIT_FAILURE);
                }

            result = write(SMfd, "", 1);
            if (result != 1)
                {
                    close(SMfd);
                    perror("Error writing last byte of the file");
                    exit(EXIT_FAILURE);
                }

            dmdispcombconf = (AOLOOPCONTROL_DM_DISPCOMB_CONF *)mmap(
                0, sizeof(AOLOOPCONTROL_DM_DISPCOMB_CONF) * NB_DMindex, PROT_READ | PROT_WRITE, MAP_SHARED, SMfd, 0);
            if (dmdispcombconf == MAP_FAILED)
                {
                    close(SMfd);
                    perror("Error mmapping the file");
                    exit(EXIT_FAILURE);
                }

            for (DMindex = 0; DMindex < NB_DMindex; DMindex++)
                {
                    dmdispcombconf[DMindex].ON = 0;
                    dmdispcombconf[DMindex].xsize = 0;
                    dmdispcombconf[DMindex].ysize = 0;
                    dmdispcombconf[DMindex].xysize = 0;
                    dmdispcombconf[DMindex].NBchannel = 0;
                    dmdispcombconf[DMindex].busy = 0;
                    dmdispcombconf[DMindex].volttype = 0;
                    dmdispcombconf[DMindex].stroke100 = 1.0;
                    dmdispcombconf[DMindex].MAXVOLT = 150.0;
                    dmdispcombconf[DMindex].moninterval = 30000; // 33Hz
                    dmdispcombconf[DMindex].status = 0;
                    dmdispcombconf[DMindex].nsecwait = 1000; // 1 us

                    dmdispcombconf[DMindex].TrigMode = 0;
                    dmdispcombconf[DMindex].TrigChan = 0;
                    dmdispcombconf[DMindex].TrigSem = 0;

                    dmdispcombconf[DMindex].IDdisp = -1;
                    dmdispcombconf[DMindex].IDvolt = -1;
                    sprintf(dmdispcombconf[DMindex].voltname, " ");

                    for (ch = 0; ch < DM_NUMBER_CHANMAX; ch++)
                        {
                            dmdispcombconf[DMindex].dmdispID[ch] = -1;
                            dmdispcombconf[DMindex].dmdispgain[ch] = 1.0;
                            dmdispcombconf[DMindex].dmdispcnt[ch] = 0;
                        }
                }
            dmdispcomb_loaded = 1;
        }
    AOloopControl_printDMconf();

    return RETURN_SUCCESS;
}

errno_t AOloopControl_DM_loadconf()
{
    char fname[STRINGMAXLEN_FULLFILENAME];
    ;

    WRITE_FULLFILENAME(fname, "%s/dmdispcombconf.conf.shm", data.shmdir);

    if (dmdispcomb_loaded == 0)
        {
            printf("Create/read DM configuration\n");

            SMfd = open(fname, O_RDWR, (mode_t)0600);
            if (SMfd == -1)
                {
                    AOloopControl_DM_createconf();
                }
            else
                {
                    dmdispcombconf =
                        (AOLOOPCONTROL_DM_DISPCOMB_CONF *)mmap(0,
                                                               sizeof(AOLOOPCONTROL_DM_DISPCOMB_CONF) * NB_DMindex,
                                                               PROT_READ | PROT_WRITE,
                                                               MAP_SHARED,
                                                               SMfd,
                                                               0);
                    if (dmdispcombconf == MAP_FAILED)
                        {
                            close(SMfd);
                            printf("Error mmapping the file -> creating it\n");
                            AOloopControl_DM_createconf();
                            //            exit(EXIT_FAILURE);
                        }
                }

            dmdispcomb_loaded = 1;
        }
    AOloopControl_printDMconf();

    return RETURN_SUCCESS;
}

errno_t AOloopControl_DM_unloadconf()
{
    if (dmdispcomb_loaded == 1)
        {
            if (munmap(dmdispcombconf, sizeof(AOLOOPCONTROL_DM_DISPCOMB_CONF) * NB_DMindex) == -1)
                {
                    perror("Error un-mmapping the file");
                }
            close(SMfd);
            dmdispcomb_loaded = 0;
        }
    return RETURN_SUCCESS;
}
