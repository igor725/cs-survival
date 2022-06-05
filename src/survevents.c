#include <core.h>
#include <str.h>
#include <block.h>
#include <world.h>
#include <event.h>
#include <client.h>
#include <platform.h>
#include <protocol.h>
#include <vector.h>
#include "survdata.h"
#include "survevents.h"
#include "survgui.h"
#include "survhacks.h"
#include "survinv.h"
#include "survdmg.h"
#include "survbrk.h"
#include "survfs.h"
#include "survcraft.h"

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
			String_Copy(data->lastWorld, MAX_STR_LEN, wname);
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

static cs_bool Survival_OnBlockPlace(onBlockPlace *obj) {
	if(!SurvCraft_OnBlockPlace(obj)) return false;
	SrvData *data = SurvData_Get(obj->client);
	if(!data || data->godMode) return true;
	if(obj->mode != SETBLOCK_MODE_CREATE && !data->breakStarted) {
		Client_Kick(obj->client, SURV_HACKS_MESSAGE);
		return false;
	} else if(obj->mode == SETBLOCK_MODE_DESTROY) return true;

	World *world = Client_GetWorld(obj->client);

	if(obj->id == BLOCK_SAPLING) {
		SVec under = obj->pos;
		under.y -= 1;
		BlockID ublock = World_GetBlock(world, &under);
		if(ublock < BLOCK_GRASS || ublock > BLOCK_DIRT)
			return false;
	}

	if(SurvInv_Take(data, obj->id, 1)) {
		SurvGui_DrawBlockInfo(data,
			SurvInv_Get(data, obj->id) > 0 ? obj->id : BLOCK_AIR
		);
		if(obj->id == BLOCK_SAPLING) {
			cs_int16 treeHeight = (cs_int16)Random_Range(&data->rnd, 4, 6);
			PlaceTree(treeHeight, world, obj->pos);
			return false;
		}
		return true;
	}

	return false;
}

static void Survival_OnHeldChange(onHeldBlockChange *obj) {
	SurvCraft_OnHeldChange(obj);
	SrvData *data = SurvData_Get(obj->client);
	if(data && !data->godMode)
		SurvGui_DrawBlockInfo(data, obj->curr);
}

static void Survival_OnTick(cs_int32 *obj) {
	for(ClientID i = 0; i < MAX_CLIENTS; i++) {
		SrvData *data = SurvData_GetByID(i);
		if(data && Client_CheckState(data->client, CLIENT_STATE_INGAME))
			if(data->breakStarted) SurvBrk_Tick(data, *obj);
	}
}

static void Survival_OnMove(Client *obj) {
	if(Client_IsOP(obj)) return;
	SrvData *data = SurvData_Get(obj);
	if(!data || data->godMode) return;

	Vec ppos; Ang pang;
	cs_float falldamage;
	Client_GetPosition(obj, &ppos, &pang);
	SurvHacks_Test(data, &ppos);
	switch(Client_GetStandBlock(obj)) {
		case BLOCK_AIR:
			if(!data->freeFall) {
				data->fallStart = ppos;
				data->freeFall = true;
				data->hackScore = 0;
			} else if(ppos.y - data->fallStart.y > (2.0f + data->pingBlocks)) {
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
				data->hackScore = 0;
				if(falldamage > 0.19f && Client_GetFluidLevel(obj, NULL) < 1)
					SurvDmg_Hurt(data, NULL, (cs_byte)(falldamage * SURV_MAX_HEALTH));
			}
			break;
	}
}

static void Survival_OnPing(Client *obj) {
	SrvData *data = SurvData_Get(obj);
	if(!data || data->godMode) return;
	cs_float ping = Client_GetAvgPing(obj);
	data->pingBlocks = ping / 120.0f;
	if(ping > 200.0f && !data->pingWarned) {
		Client_Chat(obj, MESSAGE_TYPE_CHAT,
			"&cYour ping is almost unplayable!\r\n"
			"Some actions will take longer than expected."
		);
		data->pingWarned = true;
	}
}

static void Survival_OnClick(onPlayerClick *obj) {
	if(obj->button != 0) return;

	SrvData *data = SurvData_Get(obj->client);
	if(!data || data->godMode) return;

	if(obj->action == 1) {
		SurvBrk_Stop(data);
		return;
	}

	Vec playerpos;
	Client_GetPosition(obj->client, &playerpos, NULL);

	Vec knockback = {0.0f, 0.0f, 0.0f};
	SVec *blockPos = &obj->tgpos;
	Client *target = Client_GetByID(obj->tgid);
	SrvData *dataTg = NULL;
	cs_float dist_max = Client_GetClickDistanceInBlocks(obj->client);

	cs_float dist_entity = 32768.0f;
	cs_float dist_block = 32768.0f;

	if(!Vec_IsNegative(*blockPos)) {
		Vec blockcenter;
		blockcenter.x = blockPos->x + 0.5f;
		blockcenter.y = blockPos->y - 0.5f;
		blockcenter.z = blockPos->z + 0.5f;
		dist_block = Vec_Distance(blockcenter, playerpos);
		if(dist_block - dist_max > 1.5f) goto hackdetected;
	}

	if(target) {
		Vec tgcampos;
		dataTg = SurvData_Get(target);
		Client_GetPosition(target, &tgcampos, NULL);
		knockback.x = -(playerpos.x - tgcampos.x) * 0.7f;
		knockback.y = 0.9f - (playerpos.y - tgcampos.y) * 0.7f;
		knockback.z = -(playerpos.z - tgcampos.z) * 0.7f;
		dist_entity = Vec_Distance(tgcampos, playerpos);
		if(dist_entity > dist_max) dist_entity = 32768.0f;
	}

	if(data->breakStarted && !SVec_Compare(&data->lastClick, blockPos)) {
		SurvBrk_Stop(data);
		return;
	}

	if(dist_block < dist_entity && dist_block < dist_max) {
		if(!data->breakStarted) {
			BlockID bid = World_GetBlock(Client_GetWorld(obj->client), blockPos);
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
				Client_SetVelocity(target, &knockback, PVC_ADDALL);
			} else
				Client_Chat(obj->client, MESSAGE_TYPE_CHAT, "&cYou cannot hit a player in god mode!");
		} else {
			if(!data->pvpMode)
				Client_Chat(obj->client, MESSAGE_TYPE_CHAT, "&cEnable PvP mode (/pvp) first!");
			else
				Client_Chat(obj->client, MESSAGE_TYPE_CHAT, "&cThis player is not in PvP mode!");
		}
	}

	return;
	hackdetected:
	Client_Kick(obj->client, SURV_HACKS_MESSAGE);
}

Event_DeclareBunch (events) {
	EVENT_BUNCH_ADD('v', EVT_ONTICK, Survival_OnTick)
	EVENT_BUNCH_ADD('b', EVT_ONHANDSHAKEDONE, Survival_OnHandshake)
	EVENT_BUNCH_ADD('b', EVT_ONMESSAGE, SurvCraft_OnMessage)
	EVENT_BUNCH_ADD('v', EVT_ONDISCONNECT, SurvData_Free)
	EVENT_BUNCH_ADD('v', EVT_ONSPAWN, Survival_OnSpawn)
	EVENT_BUNCH_ADD('v', EVT_ONDESPAWN, Survival_OnDespawn)
	EVENT_BUNCH_ADD('v', EVT_ONHELDBLOCKCHNG, Survival_OnHeldChange)
	EVENT_BUNCH_ADD('b', EVT_ONBLOCKPLACE, Survival_OnBlockPlace)
	EVENT_BUNCH_ADD('v', EVT_ONCLICK, Survival_OnClick)
	EVENT_BUNCH_ADD('v', EVT_ONMOVE, Survival_OnMove)
	EVENT_BUNCH_ADD('v', EVT_ONPING, Survival_OnPing)

	EVENT_BUNCH_END
};

void SurvEvents_Init(void) {
	Event_RegisterBunch(events);
}
