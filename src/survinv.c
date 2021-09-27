#include <core.h>
#include <client.h>
#include <block.h>
#include "survdata.h"
#include "survinv.h"
#include "survgui.h"

void SurvInv_Init(SrvData *data) {
	SurvInv_UpdateInventory(data);
	for(cs_byte i = 0; i < 9; i++) {
		Client_SetHotbar(data->client, i, BLOCK_AIR);
	}
}

void SurvInv_Empty(SrvData *data) {
	for(BlockID i = 1; i < 254; i++)
		data->inventory[i] = 0;
}

void SurvInv_UpdateInventory(SrvData *data) {
	cs_byte invIdx = 0;

	for(BlockID i = 1; i < 254; i++) {
		cs_bool mz = data->inventory[i] > 0 || data->godMode;
		Client_SetBlockPerm(data->client, i, mz, data->godMode);
		if(mz)
			Client_SetInvOrder(data->client, ++invIdx, i);
		else
			Client_SetInvOrder(data->client, 0, i);
	}
}

cs_uint16 SurvInv_Add(SrvData *data, BlockID id, cs_uint16 count) {
	cs_uint16 *inv = data->inventory;
	cs_uint16 old = inv[id];

	if(old < SURV_MAX_BLOCKS) {
		cs_uint16 newC = min(SURV_MAX_BLOCKS, old + count);
		inv[id] = newC;
		if(old < 1) {
			SurvInv_UpdateInventory(data);
			Client_SetHeld(data->client, id, false);
		}
		return newC - old;
	}
	return 0;
}

cs_uint16 SurvInv_Get(SrvData *data, BlockID id) {
	return data->inventory[id];
}

cs_uint16 SurvInv_Take(SrvData *data, BlockID id, cs_uint16 count) {
	cs_uint16 *inv = data->inventory;
	cs_uint16 old = inv[id];

	if(old > 0) {
		cs_uint16 newC = old - min(old, count);
		inv[id] = newC;
		if(newC == 0) {
			SurvInv_UpdateInventory(data);
			Client_SetHeld(data->client, BLOCK_AIR, false);
		}
		return old - newC;
	}
	return 0;
}
