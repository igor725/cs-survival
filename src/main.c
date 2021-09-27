#include <core.h>
#include <str.h>
#include <log.h>
#include <client.h>
#include <csmath.h>
#include <event.h>
#include <block.h>
#include <command.h>
#include <protocol.h>
#include <server.h>
#include <timer.h>
#include "data.h"
#include "damage.h"
#include "gui.h"
#include "hacks.h"
#include "break.h"
#include "inventory.h"

#define MODE(a) (a) ? "&aenabled" : "&cdisabled"

static void Survival_OnHandshake(void *param) {
	Client *client = (Client *)param;
	if(!Client_GetExtVer(client, EXT_HACKCTRL) ||
	!Client_GetExtVer(client, EXT_MESSAGETYPE) ||
	!Client_GetExtVer(client, EXT_PLAYERCLICK) ||
	!Client_GetExtVer(client, EXT_HELDBLOCK)) {
		Client_Kick(client, "Your client doesn't support necessary CPE extensions.");
		return;
	}
	SurvData_Create(client);
}

static void Survival_OnSpawn(void *param) {
	Client *cl = (Client *)param;
	SrvData *data = SurvData_Get(cl);
	data->lastPos = cl->playerData->position;
	SurvGui_DrawAll(data);
	SurvHacks_Set(data);
	SurvInv_Init(data);
}

static cs_bool Survival_OnBlockPlace(void *param) {
	onBlockPlace *a = (onBlockPlace *)param;
	SrvData *data = SurvData_Get(a->client);
	if(data->godMode) return true;

	cs_byte mode = a->mode;
	BlockID id = a->id;

	if(mode == 0x00) {
		Client_Kick(a->client, "Your client seems to be ignoring the setBlockPermission packet.");
		return false;
	}

	if(mode == 0x01 && SurvInv_Take(data, id, 1)) {
		if(SurvInv_Get(data, id) < 1) {
			SurvGui_DrawBlockInfo(data, 0);
			return true;
		}
		SurvGui_DrawBlockInfo(data, id);
		return true;
	}

	return false;
}

static void Survival_OnHeldChange(void *param) {
	onHeldBlockChange *a = (onHeldBlockChange *)param;
	SrvData *data = SurvData_Get(a->client);
	if(!data->godMode)
		SurvGui_DrawBlockInfo(data, a->curr);
}

static void Survival_OnTick(void *param) {
	cs_int32 delta = *(cs_int32 *)param;
	for(ClientID i = 0; i < MAX_CLIENTS; i++) {
		SrvData *data = SurvData_GetByID(i);
		if(data) {
			if(data->breakStarted)
				SurvBrk_Tick(data, delta);
			SurvHacks_Update(data);
		}
	}
}

static void Survival_OnDisconnect(void *param) {
	SurvData_Free((Client *)param);
}

static void Survival_OnClick(void *param) {
	onPlayerClick *a = (onPlayerClick *)param;
	if(a->button != 0) return;

	SrvData *data = SurvData_Get(a->client);
	if(data->godMode) return;

	if(a->action == 1) {
		SurvBrk_Stop(data);
		return;
	}

	if(!SurvHacks_ValidateClick(a, data)) {
		Client_Kick(a->client, "Click hack detected!");
		return;
	}

	Vec kb;
	SVec *blockPos = &a->tgpos;
	Client *target = Client_GetByID(a->tgid);
	SrvData *dataTg = NULL;
	if(target) dataTg = SurvData_Get(target);

	float dist_entity = 32768.0f;
	float dist_block = 32768.0f;

	if(!Vec_IsInvalid(blockPos)) {
		Vec blockcenter;
		blockcenter.x = blockPos->x + .5f;
		blockcenter.y = blockPos->y + .5f;
		blockcenter.z = blockPos->z + .5f;
		dist_block = Math_Distance(&blockcenter, &a->client->playerData->position);
	}

	if(target) {
		Vec tgcampos = target->playerData->position,
		plcampos = a->client->playerData->position;
		kb = a->client->playerData->position;
		kb.x = -(kb.x - tgcampos.x) * 500.0f;
		kb.y = -(kb.y - tgcampos.y) * 500.0f;
		kb.z = -(kb.z - tgcampos.z) * 500.0f;
		tgcampos.y += 1.59375f;
		plcampos.y += 1.59375f;
		dist_entity = Math_Distance(&tgcampos, &plcampos);
	}

	if(data->breakStarted && !SVec_Compare(&data->lastClick, blockPos)) {
		SurvBrk_Stop(data);
		return;
	}

	if(dist_block < dist_entity && dist_block < 4) {
		if(!data->breakStarted) {
			BlockID bid = World_GetBlock(a->client->playerData->world, blockPos);
			if(bid > BLOCK_AIR) SurvBrk_Start(data, bid);
		}
		data->lastClick = *blockPos;
	} else if(dist_entity < dist_block && dist_entity < 3.5) {
		if(data->breakStarted) {
			SurvBrk_Stop(data);
			return;
		}
		if(data->pvpMode && dataTg->pvpMode) {
			SurvDmg_Hurt(dataTg, data, 1);
			Client_SetVelocity(target, &kb, true);
		} else {
			if(!data->pvpMode)
				Client_Chat(a->client, 0, "Enable pvp mode (/pvp) first.");
		}
	}
}

COMMAND_FUNC(God) {
	SrvData *data = SurvData_Get(ccdata->caller);
	data->godMode ^= 1;
	SurvHacks_Set(data);
	SurvGui_DrawAll(data);
	SurvInv_UpdateInventory(data);
	SurvGui_DrawBlockInfo(data, data->godMode ? 0 : Client_GetHeldBlock(ccdata->caller));
	COMMAND_PRINTF("God mode %s", MODE(data->godMode));
}

COMMAND_FUNC(Hurt) {
	char damage[32];
	if(COMMAND_GETARG(damage, 32, 0)){
		cs_byte dmg = (cs_byte)(String_ToFloat(damage) * 2);
		SurvDmg_Hurt(SurvData_Get(ccdata->caller), NULL, dmg);
	}

	return false;
}

COMMAND_FUNC(PvP) {
	SrvData *data = SurvData_Get(ccdata->caller);
	if(data->godMode) {
		COMMAND_PRINT("This command can't be used from god mode.");
	}

	data->pvpMode ^= 1;
	COMMAND_PRINTF("PvP mode %s", MODE(data->pvpMode));
}

TIMER_FUNC(FluidTester) {
	(void)left; (void)ticks; (void)ud;
	for(ClientID id = 0; id < MAX_CLIENTS; id++) {
		Client *client = Clients_List[id];
		if(!client || !Client_CheckState(client, PLAYER_STATE_INGAME)) continue;
		SrvData *data = SurvData_Get(client);
		if(!data) continue;
		cs_byte waterLevel = Client_GetFluidLevel(client);

		if(data->showOxygen) {
			if(waterLevel > 1) {
				if(data->oxygen > 0)
					data->oxygen--;
				else
					SurvDmg_Hurt(data, NULL, 2);
			} else {
				if(data->oxygen < 10)
					data->oxygen++;
			}
			SurvGui_DrawAll(data);
		}

		if(waterLevel > 0)
			data->showOxygen = true;
		else if(data->oxygen == 10) {
			data->showOxygen = false;
			SurvGui_DrawAll(data);
		}
	}
}

Plugin_SetVersion(1)

cs_bool Plugin_Load(void) {
	if(Server_Ready) {
		Log_Error("Survival plugin can be loaded only at server startup.");
		return false;
	}

	COMMAND_ADD(God, CMDF_OP | CMDF_CLIENT);
	COMMAND_ADD(Hurt, CMDF_CLIENT);
	COMMAND_ADD(PvP, CMDF_CLIENT);

	SurvHacks_Init();
	SurvData_AssocType = Assoc_NewType();
	Timer_Add(-1, 1000, FluidTester, NULL);
	Event_RegisterVoid(EVT_ONTICK, Survival_OnTick);
	Event_RegisterVoid(EVT_ONSPAWN, Survival_OnSpawn);
	Event_RegisterVoid(EVT_ONHELDBLOCKCHNG, Survival_OnHeldChange);
	Event_RegisterBool(EVT_ONBLOCKPLACE, Survival_OnBlockPlace);
	Event_RegisterVoid(EVT_ONDISCONNECT, Survival_OnDisconnect);
	Event_RegisterVoid(EVT_ONHANDSHAKEDONE, Survival_OnHandshake);
	Event_RegisterVoid(EVT_ONCLICK, Survival_OnClick);
	return true;
}

cs_bool Plugin_Unload(cs_bool force) {
	return force;
}
