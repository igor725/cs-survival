#include <core.h>
#include <log.h>
#include <server.h>
#include <plugin.h>
#include "survdata.h"
#include "survhacks.h"
#include "survcmds.h"
#include "survevents.h"
#include "survtimers.h"
#include "survfs.h"
#include "survitf.h"
#include "str.h"

Plugin_SetVersion(1)

static SurvItf SuvivalController = {
	.getSrvData = SurvData_Get,
	.redrawGUI = SurvGui_DrawAll,
	.getBlockCount = SurvInv_Get,
	.giveToInventory = SurvInv_Add,
	.takeFromInventory = SurvInv_Take,
	.hurt = SurvDmg_Hurt,
	.heal = SurvDmg_Heal,
	.kill = SurvDmg_Kill
};

Plugin_DeclareInterfaces(2) {
	PLUGIN_IFACE_ADD(SURV_ITF_NAME, SuvivalController)

	PLUGIN_IFACE_DONE
};

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
