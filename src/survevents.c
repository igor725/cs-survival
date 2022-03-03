#include <core.h>
#include <str.h>
#include <block.h>
#include <world.h>
#include <event.h>
#include <client.h>
#include <platform.h>
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

static cs_bool Survival_OnHandshake(void *param) {
	onHandshakeDone *a = (onHandshakeDone *)param;
	if(!Client_GetExtVer(a->client, EXT_HACKCTRL) ||
	!Client_GetExtVer(a->client, EXT_MESSAGETYPE) ||
	!Client_GetExtVer(a->client, EXT_PLAYERCLICK) ||
	!Client_GetExtVer(a->client, EXT_HELDBLOCK)) {
		Client_Kick(a->client, "Your client doesn't support necessary CPE extensions.");
		return false;
	}

	SrvData *data = SurvData_Create(a->client);
	if(data == NULL) return false;

	if(!SurvFS_LoadPlayerData(data)) {
		Client_Chat(a->client, MESSAGE_TYPE_CHAT, "&cLooks like your survival saved data is corrupted.");
		SurvData_Reset(data);
	} else {
		World *world = World_GetByName(data->lastWorld);
		if(world) a->world = world;
		else data->loadSucc = false;
	}

	return true;
}

static void Survival_OnSpawn(void *param) {
	onSpawn *a = (onSpawn *)param;
	SrvData *data = SurvData_Get(a->client);

	if(data) {
		cs_str wname = World_GetName(Client_GetWorld(data->client));
		if(data->loadSucc && String_Compare(wname, data->lastWorld)) {
			*a->position = data->lastPos;
			*a->angle = data->lastAng;
		} else
			String_Copy(data->lastWorld, 65, wname);
		SurvGui_DrawAll(data);
		SurvHacks_Update(data);
		SurvInv_Init(data);
	}
}

static void Survival_OnDespawn(void *param) {
	SrvData *data = SurvData_Get((Client *)param);
	if(data) SurvFS_SavePlayerData(data);
}

static cs_bool leaves[4][5][5] = {
	{
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 1, 1, 1, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
	},
	{
		{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 1, 0, 1, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 0, 0, 0},
	},
	{
		{1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1},
		{1, 1, 0, 1, 1},
		{1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1},
	},
	{
		{1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1},
		{1, 1, 0, 1, 1},
		{1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1},
	},
};

// TODO: Упростить эту штуку
static void PlaceTree(cs_int16 treeHeight, World *world, SVec pos) {
	BulkBlockUpdate bbu = {
		.world = world,
		.autosend = true
	};

	Block_BulkUpdateClean(&bbu);
	SVec tmp = pos;
	for(; tmp.y <= pos.y + treeHeight; tmp.y++) {
		cs_uint32 offset = World_GetOffset(world, &tmp);
		if(offset == WORLD_INVALID_OFFSET) continue; // Попытка установить блок на кривом оффсете
		if(World_GetBlock(world, &tmp)) continue; // Трогаем только воздух
		Block_BulkUpdateAdd(&bbu, offset, BLOCK_LOG);
		World_SetBlockO(world, offset, BLOCK_LOG);
	}
	for(cs_int16 y = 0; y < 4; y++) {
		for(cs_int16 x = 0; x < 5; x++) {
			for(cs_int16 z = 0; z < 5; z++) {
				if(leaves[y][x][z]) {
					tmp.x = x + pos.x - 2; tmp.z = z + pos.z - 2;
					cs_uint32 offset = World_GetOffset(world, &tmp);
					if(offset == WORLD_INVALID_OFFSET) continue;
					if(World_GetBlock(world, &tmp)) continue;
					Block_BulkUpdateAdd(&bbu, offset, BLOCK_LEAVES);
					World_SetBlockO(world, offset, BLOCK_LEAVES);
				}
			}
		}
		tmp.y -= 1;
	}
	Block_BulkUpdateSend(&bbu);
}

static cs_bool Survival_OnBlockPlace(void *param) {
	onBlockPlace *a = (onBlockPlace *)param;
	SrvData *data = SurvData_Get(a->client);
	if(!data || data->godMode) return true;

	if(a->mode == 0x00 || a->mode != 0x01) {
		Client_Kick(a->client, "Hacked client detected.");
		return false;
	}

	World *world = Client_GetWorld(a->client);

	if(a->id == BLOCK_SAPLING) {
		SVec under = a->pos;
		under.y -= 1;
		BlockID ublock = World_GetBlock(world, &under);
		if(ublock < BLOCK_GRASS || ublock > BLOCK_DIRT)
			return false;
	}

	if(SurvInv_Take(data, a->id, 1)) {
		SurvGui_DrawBlockInfo(data,
			SurvInv_Get(data, a->id) > 0 ? a->id : BLOCK_AIR
		);
		if(a->id == BLOCK_SAPLING) {
			cs_int16 treeHeight = (cs_int16)Random_Range(&data->rnd, 4, 6);
			PlaceTree(treeHeight, world, a->pos);
			return false;
		}
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
		if(data && Client_CheckState(data->client, PLAYER_STATE_INGAME))
			if(data->breakStarted) SurvBrk_Tick(data, delta);
	}
}

static void Survival_OnMove(void *param) {
	Client *client = (Client *)param;
	if(Client_IsOP(client)) return;
	SrvData *data = SurvData_Get(client);
	if(!data || data->godMode) return;

	Vec ppos; Ang pang;
	cs_float falldamage;
	if(Client_GetPosition(client, &ppos, &pang)) {
		SurvHacks_Test(data, &ppos);
		switch(Client_GetStandBlock(client)) {
			case BLOCK_AIR:
				if(!data->freeFall) {
					data->fallStart = ppos;
					data->freeFall = true;
				} else if(ppos.y - data->fallStart.y > 3.0f) {
					data->fallStart.y = ppos.y;
					data->hackScore += 6;
				}
				break;
			case BLOCK_WATER:
			case BLOCK_WATER_STILL:
				data->freeFall = false;
				break;
			default:
				if(data->freeFall) {
					falldamage = (data->fallStart.y - ppos.y) / 22.0f;
					data->freeFall = false;
					if(falldamage > 0.19f && Client_GetFluidLevel(client, NULL) < 1)
						SurvDmg_Hurt(data, NULL, (cs_byte)(falldamage * SURV_MAX_HEALTH));
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

	Vec playerpos;
	if(!Client_GetPosition(a->client, &playerpos, NULL)) {
		Client_Kick(a->client, "Internal error");
		return;
	}

	Vec knockback = {0.0f, 0.0f, 0.0f};
	SVec *blockPos = &a->tgpos;
	Client *target = Client_GetByID(a->tgid);
	SrvData *dataTg = NULL;
	cs_float dist_max = Client_GetClickDistanceInBlocks(a->client);

	float dist_entity = 32768.0f;
	float dist_block = 32768.0f;

	if(!Vec_IsNegative(*blockPos)) {
		Vec blockcenter;
		blockcenter.x = blockPos->x + 0.5f;
		blockcenter.y = blockPos->y - 0.5f;
		blockcenter.z = blockPos->z + 0.5f;
		dist_block = Math_Distance(&blockcenter, &playerpos);
		if(dist_block - dist_max > 1.5f) goto hackdetected;
	}

	if(target) {
		Vec tgcampos;
		dataTg = SurvData_Get(target);
		if(Client_GetPosition(target, &tgcampos, NULL)) {
			knockback.x = -(playerpos.x - tgcampos.x) * 350.0f;
			knockback.y = 150.0f - (playerpos.y - tgcampos.y) * 350.0f;
			knockback.z = -(playerpos.z - tgcampos.z) * 350.0f;
			dist_entity = Math_Distance(&tgcampos, &playerpos);
			if(dist_entity > dist_max) dist_entity = 32768.0f;
		}
	}

	if(data->breakStarted && !SVec_Compare(&data->lastClick, blockPos)) {
		SurvBrk_Stop(data);
		return;
	}

	if(dist_block < dist_entity && dist_block < dist_max) {
		if(!data->breakStarted) {
			BlockID bid = World_GetBlock(Client_GetWorld(a->client), blockPos);
			if(bid > BLOCK_AIR) SurvBrk_Start(data, bid);
		}
		data->lastClick = *blockPos;
	} else if(dist_entity < dist_block && dist_entity < dist_max && dataTg) {
		if(data->breakStarted) {
			SurvBrk_Stop(data);
			return;
		}
		if(data->pvpMode && dataTg->pvpMode) {
			if(!dataTg->godMode) {
				cs_uint64 ctime = Time_GetMSec();
				if(ctime - data->lastHit < 600ull)
					return;
				data->lastHit = ctime;
				dataTg->hackScore = 0;
				SurvDmg_Hurt(dataTg, data, 1);
				Client_SetVelocity(target, &knockback, true);
			} else
				Client_Chat(a->client, MESSAGE_TYPE_CHAT, "&cYou cannot hit a player in god mode!");
		} else {
			if(!data->pvpMode)
				Client_Chat(a->client, MESSAGE_TYPE_CHAT, "&cEnable PvP mode (/pvp) first!");
			else
				Client_Chat(a->client, MESSAGE_TYPE_CHAT, "&cThis player is not in PvP mode!");
		}
	}

	return;
	hackdetected:
	Client_Kick(a->client, "Click hack detected!");
}

Event_DeclareBunch (events) {
	EVENT_BUNCH_ADD('v', EVT_ONTICK, Survival_OnTick)
	EVENT_BUNCH_ADD('v', EVT_ONSPAWN, Survival_OnSpawn)
	EVENT_BUNCH_ADD('v', EVT_ONDESPAWN, Survival_OnDespawn)
	EVENT_BUNCH_ADD('v', EVT_ONHELDBLOCKCHNG, Survival_OnHeldChange)
	EVENT_BUNCH_ADD('b', EVT_ONBLOCKPLACE, Survival_OnBlockPlace)
	EVENT_BUNCH_ADD('v', EVT_ONMOVE, Survival_OnMove)
	EVENT_BUNCH_ADD('v', EVT_ONDISCONNECT, SurvData_Free)
	EVENT_BUNCH_ADD('b', EVT_ONHANDSHAKEDONE, Survival_OnHandshake)
	EVENT_BUNCH_ADD('v', EVT_ONCLICK, Survival_OnClick)

	EVENT_BUNCH_END
};

void SurvEvents_Init(void) {
	Event_RegisterBunch(events);
}
