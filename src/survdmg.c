#include <core.h>
#include <client.h>
#include "survdata.h"
#include "survgui.h"
#include "survdmg.h"
#include "survinv.h"

void SurvDmg_Hurt(SrvData *target, SrvData *attacker, cs_byte damage) {
	if(damage < 1 || target->godMode) return;
	(void)attacker;

	target->health -= min(damage, target->health);
	if(target->health == 0)
		SurvDmg_Die(target);
	else
		SurvGui_DrawHealth(target);
}

void SurvDmg_Heal(SrvData *target, cs_byte points) {
	if(points < 1 || target->godMode) return;

	target->health += min(points, SURV_MAX_HEALTH - target->health);
	SurvGui_DrawHealth(target);
}

void SurvDmg_Die(SrvData *target) {
	Client_TeleportToSpawn(target->client);
	SurvInv_Empty(target);
	SurvInv_Init(target);
	target->health = SURV_MAX_HEALTH;
	target->oxygen = SURV_MAX_OXYGEN;
	target->showOxygen = false;
	SurvGui_DrawAll(target);
}
