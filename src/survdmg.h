#ifndef SURV_DAMAGE_H
#define SURV_DAMAGE_H
#include "core.h"
#include "survdata.h"

cs_bool SurvDmg_GetGod(SrvData *data);
cs_bool SurvDmg_GetPvP(SrvData *data);
void SurvDmg_SetGod(SrvData *data, cs_bool state);
void SurvDmg_SetPvP(SrvData *data, cs_bool state);
void SurvDmg_Hurt(SrvData *target, SrvData *attacker, cs_byte damage);
void SurvDmg_Heal(SrvData *target, cs_byte damage);
void SurvDmg_Kill(SrvData *target);
#endif // SURV_DAMAGE_H
