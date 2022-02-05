#include <core.h>
#include <command.h>
#include "survcmds.h"
#include "survdata.h"
#include "survhacks.h"
#include "survgui.h"
#include "survinv.h"
#include "survdmg.h"

COMMAND_FUNC(God) {
	SrvData *data = SurvData_Get(ccdata->caller);
	if(!data) {
		COMMAND_PRINT("Something went wrong");
	}
	data->godMode ^= 1;
	SurvHacks_Set(data);
	SurvGui_DrawAll(data);
	SurvInv_UpdateInventory(data);
	SurvGui_DrawBlockInfo(data, data->godMode ? 0 : Client_GetHeldBlock(ccdata->caller));
	COMMAND_PRINTF("God mode %s", MODE(data->godMode));
}

COMMAND_FUNC(Hurt) {
	char damage[32];
	SrvData *data = SurvData_Get(ccdata->caller);
	if(!data) {
		COMMAND_PRINT("Something went wrong");
	}
	if(COMMAND_GETARG(damage, 32, 0)){
		cs_byte dmg = (cs_byte)(String_ToFloat(damage) * 2);
		SurvDmg_Hurt(data, NULL, dmg);
	}

	return false;
}

COMMAND_FUNC(PvP) {
	SrvData *data = SurvData_Get(ccdata->caller);
	if(!data) {
		COMMAND_PRINT("Something went wrong");
	}
	if(data->godMode) {
		COMMAND_PRINT("This command can't be used from god mode.");
	}

	data->pvpMode ^= 1;
	COMMAND_PRINTF("PvP mode %s", MODE(data->pvpMode));
}

COMMAND_FUNC(Suicide) {
	SrvData *data = SurvData_Get(ccdata->caller);
	if(!data) {
		COMMAND_PRINT("Something went wrong");
	}

	SurvDmg_Kill(data);
	return false;
}

void SurvCmds_Init(void) {
	COMMAND_ADD(God, CMDF_OP | CMDF_CLIENT);
	COMMAND_ADD(Hurt, CMDF_CLIENT);
	COMMAND_ADD(PvP, CMDF_CLIENT);
	COMMAND_ADD(Suicide, CMDF_CLIENT);
}
