#include <core.h>
#include <log.h>
#include <server.h>
#include "survdata.h"
#include "survhacks.h"
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
