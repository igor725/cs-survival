#ifndef SURV_FS_H
#define SURV_FS_H
#include <core.h>
#include "survdata.h"

#ifdef CORE_USE_LITTLE
#	define SURVFS_MAGIC 0xEFBEADDEu
#else
#	define SURVFS_MAGIC 0xDEADBEEFu
#endif

void SurvFS_Init(void);
cs_bool SurvFS_LoadPlayerData(SrvData *data);
cs_bool SurvFS_SavePlayerData(SrvData *data);
#endif
