#include <core.h>
#include <client.h>
#include "survdata.h"
#include "survgui.h"
#include "survdmg.h"

void SurvDmg_Hurt(SrvData *target, SrvData *attacker, cs_byte damage) {
	if(damage < 1 || target->godMode) return;
	(void)attacker;

	target->health -= min(damage, target->health);
	if(target->health == 0) {

	}

	SurvGui_DrawHealth(target);
}

void SurvDmg_Heal(SrvData *target, cs_byte points) {
	if(points < 1 || target->godMode) return;

	target->health += min(points, 20 - target->health);
	SurvGui_DrawHealth(target);
}
