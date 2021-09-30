#include <core.h>
#include <log.h>
#include <client.h>
#include <server.h>
#include "survdata.h"
#include "survdmg.h"
#include "survgui.h"
#include "survhacks.h"
#include "survbrk.h"
#include "survinv.h"
#include "survcmds.h"
#include "survevents.h"
#include "survtimers.h"
#include "survfs.h"

Plugin_SetVersion(1)

cs_bool Plugin_Load(void) {
	if(Server_Ready) {
		Log_Error("Survival plugin can be loaded only at server startup.");
		return false;
	}

	SurvData_AssocType = Assoc_NewType();
	SurvFS_Init();
	SurvCmds_Init();
	SurvHacks_Init();
	SurvEvents_Init();
	SurvTimers_Init();

	return true;
}

cs_bool Plugin_Unload(cs_bool force) {
	return force;
}
