#ifndef SURV_HACKS_H
#define SURV_HACKS_H
#include <core.h>
#include <vector.h>
#include "survdata.h"

#define SURV_HACKS_MESSAGE "Hacked client detected"

void SurvHacks_Init(void);
void SurvHacks_Update(SrvData *data);
void SurvHacks_Test(SrvData *data, Vec *playerPos);
#endif // SURV_HACKS_H
