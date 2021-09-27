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
	if(target->health == 0) {
		World *world = Client_GetWorld(target->client);
		Client_TeleportTo(target->client, &world->info.spawnVec, &world->info.spawnAng);
		SurvInv_Empty(target);
		SurvInv_Init(target);
		target->health = SURV_MAX_HEALTH;
		target->oxygen = SURV_MAX_OXYGEN;
		SurvGui_DrawAll(target);
	} else SurvGui_DrawHealth(target);
}

void SurvDmg_Heal(SrvData *target, cs_byte points) {
	if(points < 1 || target->godMode) return;

	target->health += min(points, SURV_MAX_HEALTH - target->health);
	SurvGui_DrawHealth(target);
}
