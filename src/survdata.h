#ifndef SURV_DATA_H
#define SURV_DATA_H
#include <core.h>
#include <vector.h>
#include <client.h>

#define SURV_MAX_HEALTH 20
#define SURV_MAX_OXYGEN 10
#define SURV_MAX_BREAKPRG 10

typedef struct {
	Client *client;
	cs_uint16 inventory[256];
	SVec lastClick;
	Vec lastPos;
	cs_bool freeFall;
	cs_float fallStart;
	cs_byte health, oxygen, hackScore;
	cs_bool showOxygen, godMode, pvpMode;
	cs_uint16 regenTimer, breakTimer;
	cs_bool breakStarted;
	cs_byte breakProgress;
	BlockID breakBlock;
} SrvData;

cs_bool SurvData_Create(Client *client);
void SurvData_Free(Client *client);
SrvData *SurvData_Get(Client *client);
SrvData *SurvData_GetByID(ClientID id);
#endif // SURV_DATA_H
