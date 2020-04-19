#ifndef SURV_DATA_H
#define SURV_DATA_H
cs_uint16 SurvData_AssocType;

typedef struct {
	Client *client;
	cs_uint16 inventory[256];
	SVec lastClick;
	cs_byte health, oxygen;
	cs_bool showOxygen, godMode, pvpMode;
	cs_uint16 regenTimer, breakTimer;
	cs_bool breakStarted;
	cs_byte breakProgress;
	BlockID breakBlock;
} SurvivalData;

void SurvData_Create(Client *client);
void SurvData_Free(Client *client);
SurvivalData *SurvData_Get(Client *client);
SurvivalData *SurvData_GetByID(ClientID id);
#endif // SURV_DATA_H
