#ifndef SURV_DAMAGE_H
#define SURV_DAMAGE_H
#include "core.h"
#include "survdata.h"

void SurvDmg_Hurt(SrvData *target, SrvData *attacker, cs_byte damage);
void SurvDmg_Heal(SrvData *target, cs_byte damage);
#endif // SURV_DAMAGE_H
