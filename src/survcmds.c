#include <core.h>
#include <command.h>
#include "survcmds.h"
#include "survdata.h"
#include "survhacks.h"
#include "survgui.h"
#include "survinv.h"
#include "survdmg.h"

COMMAND_FUNC(God) {
	cs_char username[64];
	Client *target = ccdata->caller;
	SrvData *data = SurvData_Get(target);

	if(COMMAND_GETARG(username, 64, 0)) {
		target = Client_GetByName(username);
		data = SurvData_Get(target);
	}

	if(data) {
		data->godMode ^= 1;
		SurvHacks_Set(data);
		SurvGui_DrawAll(data);
		SurvInv_UpdateInventory(data);
		SurvGui_DrawBlockInfo(data, data->godMode ? 0 : Client_GetHeldBlock(target));
		COMMAND_PRINTF("God mode %s", MODE(data->godMode));
	}

	COMMAND_PRINT("User not found");
}

COMMAND_FUNC(Hurt) {
	cs_char username[64], damage[32];
	SrvData *data = SurvData_Get(ccdata->caller);
	cs_uint32 argoffset = 0;

	if(String_CountArguments(ccdata->args) > 1) {
		COMMAND_TESTOP();

		if(COMMAND_GETARG(username, 64, argoffset)) {
			Client *target = Client_GetByName(username);
			data = SurvData_Get(target);
			argoffset++;
		}
	}

	if(data && COMMAND_GETARG(damage, 32, argoffset)){
		cs_byte dmg = (cs_byte)(String_ToFloat(damage) * 2);
		SurvDmg_Hurt(data, NULL, dmg);
	}

	COMMAND_PRINT("User not found");
}

COMMAND_FUNC(PvP) {
	SrvData *data = SurvData_Get(ccdata->caller);
	if(!data) {
		COMMAND_PRINT("Something went wrong");
	}
	if(data->godMode) {
		COMMAND_PRINT("You cannot use this command in god mode");
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
	COMMAND_ADD(God, CMDF_OP, "Toggle god mode");
	COMMAND_ADD(Hurt, CMDF_NONE, "Takes specified ammount of health");
	COMMAND_ADD(PvP, CMDF_CLIENT, "Toggle PvP mode");
	COMMAND_ADD(Suicide, CMDF_CLIENT, "Commit suicide");
}
