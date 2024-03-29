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
#include "survcraft.h"

Plugin_SetVersion(1);
#if PLUGIN_API_NUM > 1
Plugin_SetURL("https://github.com/igor725/cs-survival");
#endif

static SurvItf SuvivalController = {
	.getSrvData = SurvData_Get,

	.updateInventory = SurvInv_UpdateInventory,
	.getBlockCount = SurvInv_Get,
	.giveToInventory = SurvInv_Add,
	.takeFromInventory = SurvInv_Take,

	.isInGodMode = SurvDmg_GetGod,
	.isInPvPMode = SurvDmg_GetPvP,

	.setPvPMode = SurvDmg_SetPvP,
	.setGodMode = SurvDmg_SetGod,

	.hurt = SurvDmg_Hurt,
	.heal = SurvDmg_Heal,
	.kill = SurvDmg_Kill,

	.getRecipe = SurvCraft_GetRecipe
};

Plugin_DeclareInterfaces {
	PLUGIN_IFACE_ADD(SURV_ITF_NAME, SuvivalController),

	PLUGIN_IFACE_END
};

cs_bool Plugin_Load(void) {
	if(Server_Ready) {
		Log_Error("Survival plugin can be loaded only at server startup.");
		return false;
	}

	SurvFS_Init();
	SurvCmds_Init();
	SurvHacks_Init();
	SurvCraft_Init();
	SurvEvents_Init();
	SurvTimers_Init();

	return true;
}

cs_bool Plugin_Unload(cs_bool force) {
	return force;
}
