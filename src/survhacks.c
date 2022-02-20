#include <core.h>
#include <client.h>
#include <timer.h>
#include <csmath.h>
#include "survdata.h"
#include "survhacks.h"

TIMER_FUNC(HackScore) {
	(void)left; (void)ticks; (void)ud;
	for(ClientID id = 0; id < MAX_CLIENTS; id++) {
		Client *client = Clients_List[id];
		if(client && Client_CheckState(client, PLAYER_STATE_INGAME)) {
			SrvData *data = SurvData_Get(client);
			if(data && data->hackScore > 0) data->hackScore--;
		}
	}
}

void SurvHacks_Init(void) {
	Timer_Add(-1, 2000, HackScore, NULL);
}

void SurvHacks_Update(SrvData *data) {
	CPEHacks hacks;

	hacks.tpv = true;
	hacks.spawnControl = false;
	hacks.jumpHeight = -1;

	if(data->godMode) {
		hacks.flying = true;
		hacks.noclip = true;
		hacks.speeding = true;
	} else {
		hacks.flying = false;
		hacks.noclip = false;
		hacks.speeding = false;
	}

	Client_SendHacks(data->client, &hacks);
}

static cs_float DistanceXZ(Vec *v1, Vec *v2) {
	cs_float dx = (v1->x - v2->x),
	dz = (v1->z - v2->z);
	return Math_Sqrt((dx * dx) + (dz * dz));
}

void SurvHacks_Test(SrvData *data, Vec *playerPos) {
	if(data->hackScore < 10) {
		cs_float *ppt = (cs_float *)playerPos,
		*lpt = (cs_float *)&data->lastPos;
		for(cs_uint32 i = 0; i < 3; i++) {
			cs_float tmp = ppt[i] - lpt[i];
			if(tmp < 0 && i != 1) tmp *= -1;
			if(tmp > 1.5f) {
				data->hackScore += 1;
				break;
			}
		}

		if(data->freeFall && DistanceXZ(playerPos, &data->fallStart) > 10)
			data->hackScore += 4;

		data->lastPos = *playerPos;
	}

	if(data->hackScore > 10)
		Client_Kick(data->client, "Hacked client detected");
}
