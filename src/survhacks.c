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

void SurvHacks_Set(SrvData *data) {
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

void SurvHacks_Update(SrvData *data) {
	if(data->godMode) return;
	if(data->hackScore < 10) {
		Vec playerPos;
		if(Client_GetPosition(data->client, &playerPos, NULL)) {
			cs_float *ppt = (cs_float *)&playerPos,
			*lpt = (cs_float *)&data->lastPos;
			for(cs_uint32 i = 0; i < 3; i++) {
				cs_float tmp = ppt[i] - lpt[i];
				if(tmp < 0) tmp *= -1;
				if(tmp > 1.5f) {
					data->hackScore += 1;
					break;
				}
			}
			data->lastPos = playerPos;
		}
	}

	if(data->hackScore >= 8) {
		Client_Kick(data->client, "Hacked client detected.");
		data->hackScore = 0;
	}
}

cs_bool SurvHacks_ValidateClick(onPlayerClick *click, SrvData *data) {
	if(Vec_IsInvalid(&click->tgpos)) return true;
	Vec clickfvec, playerpos;
	clickfvec.x = (cs_float)click->tgpos.x;
	clickfvec.y = (cs_float)click->tgpos.y;
	clickfvec.z = (cs_float)click->tgpos.z;
	if(Client_GetPosition(data->client, &playerpos, NULL))
		return Math_Distance(&playerpos, &clickfvec) < 6.5f;
	else return false;
}
