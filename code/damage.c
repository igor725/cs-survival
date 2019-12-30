#include <core.h>
#include <client.h>

#include "data.h"
#include "gui.h"
#include "damage.h"

void SurvDmg_Hurt(SurvivalData* target, SurvivalData* attacker, cs_uint8 damage) {
	if(damage < 1 || target->godMode) return;
	(void)attacker;

	target->health -= min(damage, target->health);
	SurvGui_DrawHealth(target);
}

void SurvDmg_Heal(SurvivalData* target, cs_uint8 points) {
	if(points < 1 || target->godMode) return;

	target->health += min(points, 20 - target->health);
	SurvGui_DrawHealth(target);
}
