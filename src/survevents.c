#include <core.h>
#include <event.h>
#include <client.h>
#include <protocol.h>
#include <csmath.h>
#include "survdata.h"
#include "survevents.h"
#include "survgui.h"
#include "survhacks.h"
#include "survinv.h"
#include "survdmg.h"
#include "survbrk.h"
#include "survfs.h"

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
	if(!data) return;
	Client_GetPosition(cl, &data->lastPos, NULL);
	SurvFS_LoadPlayerData(data);
	SurvGui_DrawAll(data);
	SurvHacks_Set(data);
	SurvInv_Init(data);
}

static cs_bool Survival_OnBlockPlace(void *param) {
	onBlockPlace *a = (onBlockPlace *)param;
	SrvData *data = SurvData_Get(a->client);
	if(!data || data->godMode) return true;

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
	if(data && !data->godMode)
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
	SrvData *data = SurvData_Get((Client *)param);
	if(data) SurvFS_SavePlayerData(data);
	SurvData_Free((Client *)param);
}

static void Survival_OnMove(void *param) {
	Client *client = (Client *)param;
	SrvData *data = SurvData_Get(client);
	if(!data || data->godMode) return;

	Vec ppos;
	cs_float falldamage;
	if(Client_GetPosition(client, &ppos, NULL)) {
		switch(Client_GetStandBlock(client)) {
			case BLOCK_AIR:
				if(!data->freeFall) {
					data->fallStart = ppos.y;
					data->freeFall = true;
				}
				break;
			default:
				if(data->freeFall) {
					falldamage = (data->fallStart - ppos.y) / 2.0f;
					data->freeFall = false;
					if(falldamage > 1.0f && Client_GetFluidLevel(client, NULL) < 1)
						SurvDmg_Hurt(data, NULL, (cs_byte)falldamage);
				}
				break;
		}
	}
}

static void Survival_OnClick(void *param) {
	onPlayerClick *a = (onPlayerClick *)param;
	if(a->button != 0) return;

	SrvData *data = SurvData_Get(a->client);
	if(!data || data->godMode) return;

	if(a->action == 1) {
		SurvBrk_Stop(data);
		return;
	}

	if(!SurvHacks_ValidateClick(a, data)) {
		Client_Kick(a->client, "Click hack detected!");
		return;
	}

	Vec knockback = {0.0f, 0.0f, 0.0f};
	SVec *blockPos = &a->tgpos;
	Client *target = Client_GetByID(a->tgid);
	SrvData *dataTg = NULL;
	if(target) dataTg = SurvData_Get(target);

	float dist_entity = 32768.0f;
	float dist_block = 32768.0f;

	if(!Vec_IsInvalid(blockPos)) {
		Vec blockcenter, playerpos;
		blockcenter.x = blockPos->x + .5f;
		blockcenter.y = blockPos->y + .5f;
		blockcenter.z = blockPos->z + .5f;
		if(Client_GetPosition(a->client, &playerpos, NULL))
			dist_block = Math_Distance(&blockcenter, &playerpos);
	}

	if(target) {
		Vec tgcampos, plcampos;
		if(Client_GetPosition(target, &tgcampos, NULL) && Client_GetPosition(a->client, &plcampos, NULL)) {
			knockback.x = -(plcampos.x - tgcampos.x) * 350.0f;
			knockback.y = -(plcampos.y - tgcampos.y) * 350.0f;
			knockback.z = -(plcampos.z - tgcampos.z) * 350.0f;
			tgcampos.y += 1.59375f;
			plcampos.y += 1.59375f;
			dist_entity = Math_Distance(&tgcampos, &plcampos);
		}
	}

	if(data->breakStarted && !SVec_Compare(&data->lastClick, blockPos)) {
		SurvBrk_Stop(data);
		return;
	}

	if(dist_block < dist_entity && dist_block < 4) {
		if(!data->breakStarted) {
			BlockID bid = World_GetBlock(Client_GetWorld(a->client), blockPos);
			if(bid > BLOCK_AIR) SurvBrk_Start(data, bid);
		}
		data->lastClick = *blockPos;
	} else if(dist_entity < dist_block && dist_entity < 3.5) {
		if(data->breakStarted) {
			SurvBrk_Stop(data);
			return;
		}
		if(data->pvpMode && dataTg->pvpMode) {
			if(!dataTg->godMode) {
				SurvDmg_Hurt(dataTg, data, 1);
				Client_SetVelocity(target, &knockback, true);
			}
		} else {
			if(!data->pvpMode)
				Client_Chat(a->client, 0, "Enable pvp mode (/pvp) first.");
		}
	}
}

void SurvEvents_Init(void) {
	Event_RegisterVoid(EVT_ONTICK, Survival_OnTick);
	Event_RegisterVoid(EVT_ONSPAWN, Survival_OnSpawn);
	Event_RegisterVoid(EVT_ONHELDBLOCKCHNG, Survival_OnHeldChange);
	Event_RegisterBool(EVT_ONBLOCKPLACE, Survival_OnBlockPlace);
	Event_RegisterVoid(EVT_ONMOVE, Survival_OnMove);
	Event_RegisterVoid(EVT_ONDISCONNECT, Survival_OnDisconnect);
	Event_RegisterVoid(EVT_ONHANDSHAKEDONE, Survival_OnHandshake);
	Event_RegisterVoid(EVT_ONCLICK, Survival_OnClick);
}
