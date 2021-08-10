#ifndef SURV_BREAK_H
#define SURV_BREAK_H
#include <core.h>
#include "data.h"

#define SURV_MAX_BRK 10

void SurvBrk_Start(SrvData *data, BlockID bid);
void SurvBrk_Stop(SrvData *data);
void SurvBrk_Tick(SrvData *data, cs_uint32 delta);
#endif // SURV_BREAK_H
