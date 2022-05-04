#include <core.h>
#include <world.h>
#include <client.h>
#include <event.h>
#include <types/block.h>
#include <csmath.h>
#include "survdata.h"
#include "survbrk.h"
#include "survgui.h"
#include "survinv.h"

static const cs_int32 BreakTimings[256] = {
	0, 3200, 500, 500, 4000, 1100, 0, -1, -1, -1, -1,
	-1, 500, 500, 4000, 4000, 4000, 600, 250, 250, 0,
	1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500,
	1500, 1500, 1500, 1500, 1500, 1500, 0, 0, 0, 0, 7000, 7000,
	7000, 6400, 7000, 400, 1100, 4000, 80000, 7000, 500, 4800,
	500, 0, 1500, 1500, 1500, 1500, 1500, 890, 1500, 70000,
	4000, 2000, 7000
};

static void UpdateBlock(World *world, SVec *pos, BlockID bid) {
	for(ClientID i = 0; i < MAX_CLIENTS; i++) {
		Client *client = Client_GetByID(i);
		if(client && Client_IsInWorld(client, world))
			Client_SetBlock(client, pos, bid);
	}
}

void SurvBrk_Start(SrvData *data, BlockID block) {
	if(BreakTimings[block] != -1) {
		data->breakStarted = true;
		data->breakBlock = block;
		data->breakTimer = 0;
		SurvGui_DrawBreakProgress(data);
	}
}

void SurvBrk_Stop(SrvData *data) {
	data->breakProgress = 0;
	data->breakStarted = false;
	SurvGui_DrawBreakProgress(data);
}

BlockID SurvBrk_GetDrop(SrvData *data) {
	switch(data->breakBlock) {
		case BLOCK_LEAVES:
			return Random_Range(&data->rnd, 0, 100) > 10 ? BLOCK_LEAVES : BLOCK_SAPLING;
		case BLOCK_GOLD_ORE:
			return Random_Range(&data->rnd, 0, 100) > 1 ? BLOCK_GOLD_ORE : BLOCK_GOLD;
		case BLOCK_IRON_ORE:
			return Random_Range(&data->rnd, 0, 50) > 1 ? BLOCK_IRON_ORE: BLOCK_IRON;
	}

	return data->breakBlock;
}

void SurvBrk_Done(SrvData *data) {
	SVec *pos = &data->lastClick;
	World *world = Client_GetWorld(data->client);
	onBlockPlace params = {
		.mode = SETBLOCK_MODE_DESTROY,
		.client = data->client,
		.id = BLOCK_AIR,
		.pos = *pos
	};
	if(Event_Call(EVT_ONBLOCKPLACE, &params)) {
		BlockID id = SurvBrk_GetDrop(data),
		held = Client_GetHeldBlock(data->client);

		SurvInv_Add(data, id, 1);
		if(held == BLOCK_AIR)
			Client_SetHeldBlock(data->client, id, false);
		if(held == id)
			SurvGui_DrawBlockInfo(data, id);
		World_SetBlock(world, pos, BLOCK_AIR);
		UpdateBlock(world, pos, BLOCK_AIR);
	}
	SurvBrk_Stop(data);
}

void SurvBrk_Tick(SrvData *data, cs_uint32 delta) {
	if(data->craftMode) return;

	cs_int32 breakTime = BreakTimings[data->breakBlock];
	if(breakTime == -1) {
		SurvBrk_Stop(data);
		return;
	} else if(breakTime == 0) {
		SurvBrk_Done(data);
		return;
	}

	data->breakTimer += (cs_uint16)delta;
	cs_byte newProgress = (cs_byte)((data->breakTimer / (float)breakTime) * SURV_MAX_BRK);
	if(newProgress > data->breakProgress) {
		data->breakProgress = newProgress;
		SurvGui_DrawBreakProgress(data);
	}
	if(data->breakTimer >= breakTime) SurvBrk_Done(data);
}
