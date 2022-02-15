#include <core.h>
#include <client.h>
#include "survdata.h"
#include "survgui.h"
#include "survdmg.h"
#include "survinv.h"
#include "survhacks.h"

cs_bool SurvDmg_GetGod(SrvData *data) {
	return data->godMode;
}

cs_bool SurvDmg_GetPvP(SrvData *data) {
	return data->pvpMode;
}

void SurvDmg_SetGod(SrvData *data, cs_bool state) {
	data->godMode = state;
	SurvHacks_Update(data);
	SurvGui_DrawAll(data);
	SurvInv_UpdateInventory(data);
	SurvGui_DrawBlockInfo(data, data->godMode ? 0 : Client_GetHeldBlock(data->client));
}

void SurvDmg_SetPvP(SrvData *data, cs_bool state) {
	data->pvpMode = state;
	SurvGui_DrawAll(data);
}

void SurvDmg_Hurt(SrvData *target, SrvData *attacker, cs_byte damage) {
	if(damage < 1 || target->godMode) return;
	(void)attacker;

	target->health -= min(damage, target->health);
	if(target->health == 0)
		SurvDmg_Kill(target);
	else
		SurvGui_DrawHealth(target);
}

void SurvDmg_Heal(SrvData *target, cs_byte points) {
	if(points < 1 || target->godMode) return;

	target->health += min(points, SURV_MAX_HEALTH - target->health);
	SurvGui_DrawHealth(target);
}

void SurvDmg_Kill(SrvData *target) {
	Client_TeleportToSpawn(target->client);
	SurvInv_Empty(target);
	SurvInv_Init(target);
	target->health = SURV_MAX_HEALTH;
	target->oxygen = SURV_MAX_OXYGEN;
	target->showOxygen = false;
	SurvGui_DrawAll(target);
}
