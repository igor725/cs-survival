#ifndef SURV_GUI_H
#define SURV_GUI_H
#include <core.h>
#include <block.h>
#include "survdata.h"

void SurvGui_DrawHealth(SrvData *data);
void SurvGui_DrawBreakProgress(SrvData *data);
void SurvGui_DrawBlockInfo(SrvData *data, BlockID id);
void SurvGui_DrawAll(SrvData *data);
#endif // SURV_GUI_H
