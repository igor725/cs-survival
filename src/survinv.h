#ifndef SURV_INV_H
#define SURV_INV_H
#include <core.h>
#include <block.h>
#include "survdata.h"

#define SURV_MAX_BLOCKS 999

void SurvInv_Init(SrvData *data);
void SurvInv_UpdateInventory(SrvData *data);
cs_uint16 SurvInv_Get(SrvData *data, BlockID id);
cs_uint16 SurvInv_Take(SrvData *data, BlockID id, cs_uint16 count);
cs_uint16 SurvInv_Add(SrvData *data, BlockID id, cs_uint16 count);
#endif // SURV_INV_H
