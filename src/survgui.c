#include <core.h>
#include <client.h>
#include <protocol.h>
#include <block.h>
#include <str.h>
#include "survdata.h"
#include "survgui.h"
#include "survinv.h"

void SurvGui_DrawHealth(SrvData *data) {
	char healthstr[SURV_MAX_HEALTH + 1] = {0};

	if(!data->godMode) {
		cs_byte hltf = data->health / 2;
		cs_byte empty = (SURV_MAX_HEALTH / 2) - hltf;

		String_Append(healthstr, SURV_MAX_HEALTH, "&c");
		for(cs_int32 i = 0; i < hltf; i++) {
			String_Append(healthstr, SURV_MAX_HEALTH, "\3");
		}
		if(data->health % 2) {
			String_Append(healthstr, SURV_MAX_HEALTH, "&4\3");
			empty -= 1;
		}
		if(empty > 0) {
			String_Append(healthstr, SURV_MAX_HEALTH, "&8");
			for(cs_int32 i = 0; i < empty; i++) {
				String_Append(healthstr, SURV_MAX_HEALTH, "\3");
			}
		}
	}

	Client_Chat(data->client, MESSAGE_TYPE_STATUS1, healthstr);
}

void SurvGui_DrawOxygen(SrvData *data) {
	char oxystr[SURV_MAX_OXYGEN + 4] = {0};

	if(!data->godMode && data->showOxygen) {
		String_Copy(oxystr, SURV_MAX_OXYGEN + 4, "&b");
		for(cs_byte i = 0; i < SURV_MAX_OXYGEN; i++) {
			String_Append(oxystr, SURV_MAX_OXYGEN + 4, data->oxygen > i ? "\7" : " ");
		}
	}

	Client_Chat(data->client, MESSAGE_TYPE_STATUS2, oxystr);
}

void SurvGui_DrawBreakProgress(SrvData *data) {
	char breakstr[SURV_MAX_BREAKPRG * 2] = {0};

	if(data->breakStarted) {
		String_Append(breakstr, SURV_MAX_BREAKPRG * 2, "[&a");
		for(cs_int32 i = 0; i < SURV_MAX_BREAKPRG; i++) {
			if(i == data->breakProgress)
				String_Append(breakstr, SURV_MAX_BREAKPRG * 2, "&8");
			String_Append(breakstr, SURV_MAX_BREAKPRG * 2, "|");
		}
		String_Append(breakstr, SURV_MAX_BREAKPRG * 2, "&f]");
	}

	Client_Chat(data->client, MESSAGE_TYPE_ANNOUNCE, breakstr);
}

void SurvGui_DrawBlockInfo(SrvData *data, BlockID id) {
	char blockinfo[65] = {0};

	if(id > BLOCK_AIR) {
		World *world = Client_GetWorld(data->client);
		const char *bn = Block_GetName(world, id);
		cs_uint16 bc = SurvInv_Get(data, id);
		String_FormatBuf(blockinfo, 65, "%s (%d)", bn, bc);
	}

	Client_Chat(data->client, MESSAGE_TYPE_BRIGHT1, blockinfo);
}

void SurvGui_DrawAll(SrvData *data) {
	SurvGui_DrawHealth(data);
	SurvGui_DrawOxygen(data);
	EMesgType type = data->showOxygen ? MESSAGE_TYPE_STATUS3 : MESSAGE_TYPE_STATUS2;
	if(!data->showOxygen && data->pvpMode)
		Client_Chat(data->client, MESSAGE_TYPE_STATUS3, "");
	if(data->pvpMode && !data->godMode)
		Client_Chat(data->client, type, "&cPvP");
	else
		Client_Chat(data->client, type, "");
}
