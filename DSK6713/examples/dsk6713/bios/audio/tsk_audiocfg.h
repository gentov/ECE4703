/*   Do *not* directly modify this file.  It was    */
/*   generated by the Configuration Tool; any  */
/*   changes risk being overwritten.                */

/* INPUT tsk_audio.cdb */

#define CHIP_6713 1

/*  Include Header Files  */
#include <std.h>
#include <prd.h>
#include <hst.h>
#include <swi.h>
#include <tsk.h>
#include <log.h>
#include <sts.h>

#ifdef __cplusplus
extern "C" {
#endif

extern far PRD_Obj prd10000;
extern far HST_Obj RTA_fromHost;
extern far HST_Obj RTA_toHost;
extern far SWI_Obj PRD_swi;
extern far SWI_Obj KNL_swi;
extern far TSK_Obj TSK_idle;
extern far TSK_Obj tskEcho;
extern far LOG_Obj LOG_system;
extern far LOG_Obj trace;
extern far STS_Obj IDL_busyObj;
extern far void CSL_cfgInit();

#ifdef __cplusplus
}
#endif /* extern "C" */
