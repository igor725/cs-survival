#include <core.h>
#include <world.h>
#include <client.h>
#include <block.h>
#include "survdata.h"
#include "survbrk.h"
#include "survgui.h"
#include "survinv.h"

static const cs_int32 BreakTimings[256] = {
	0, 4000, 500, 500, 4000, 1100, 0, -1, -1, -1, -1,
	-1, 500, 500, 4000, 4000, 4000, 600, 250, 250, 0,
	1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500,
	1500, 1500, 1500, 1500, 1500, 1500, 0, 0, 0, 0, 7000, 7000,
	7000, 6400, 7000, 400, 1100, 4000, 20000
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

void SurvBrk_Done(SrvData *data) {
	SVec *pos = &data->lastClick;
	World *world = Client_GetWorld(data->client);
	BlockID id = data->breakBlock;

	SurvInv_Add(data, id, 1);
	if(!Client_GetHeldBlock(data->client))
		Client_SetHeldBlock(data->client, id, false);
	SurvGui_DrawBlockInfo(data, id);
	World_SetBlock(world, pos, BLOCK_AIR);
	UpdateBlock(world, pos, BLOCK_AIR);
	SurvBrk_Stop(data);
}

void SurvBrk_Tick(SrvData *data, cs_uint32 delta) {
	cs_int32 breakTime = BreakTimings[data->breakBlock];
	if(breakTime == -1) {
		SurvBrk_Stop(data);
		return;
	} else if(breakTime == 0) {
		SurvBrk_Done(data);
		return;
	}

	data->breakTimer += (cs_uint16)delta;
	float df = (data->breakTimer / (float)breakTime);
	cs_byte newProgress = (cs_byte)(df * SURV_MAX_BRK);
	if(newProgress > data->breakProgress) {
		data->breakProgress = newProgress;
		SurvGui_DrawBreakProgress(data);
	}
	if(data->breakTimer >= breakTime) SurvBrk_Done(data);
}
