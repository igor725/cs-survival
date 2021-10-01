#ifndef SURV_FS_H
#define SURV_FS_H
#include <core.h>
#include "survdata.h"

#define SURVFS_MAGIC 0xDEADBEEF

void SurvFS_Init(void);
cs_bool SurvFS_LoadPlayerData(SrvData *data);
cs_bool SurvFS_SavePlayerData(SrvData *data);
#endif
