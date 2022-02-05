#include <core.h>
#include <timer.h>
#include <client.h>
#include "survdata.h"
#include "survtimers.h"
#include "survdmg.h"
#include "survgui.h"

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

TIMER_FUNC(HealthRegen) {
	(void)left; (void)ticks; (void)ud;
	for(ClientID id = 0; id < MAX_CLIENTS; id++) {
		Client *client = Clients_List[id];
		if(!client || !Client_CheckState(client, PLAYER_STATE_INGAME)) continue;
		SrvData *data = SurvData_Get(client);
		if(!data || data->godMode || data->health == SURV_MAX_HEALTH) continue;
		SurvDmg_Heal(data, 1);
	}
}

void SurvTimers_Init(void) {
	Timer_Add(-1, 1000, FluidTester, NULL);
	Timer_Add(-1, 2000, HealthRegen, NULL);
}
