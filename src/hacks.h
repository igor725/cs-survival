#ifndef SURV_HACKS_H
#define SURV_HACKS_H
#include <event.h>
void SurvHacks_Init(void);
void SurvHacks_Set(SrvData *data);
void SurvHacks_Update(SrvData *data);
cs_bool SurvHacks_ValidateClick(onPlayerClick *click, SrvData *data);
#endif // SURV_HACKS_H
