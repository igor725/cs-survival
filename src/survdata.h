#ifndef SURV_DATA_H
#define SURV_DATA_H
#include <core.h>
#include <vector.h>
#include <client.h>
#include <csmath.h>

#define SURV_MAX_HEALTH 20
#define SURV_MAX_OXYGEN 10
#define SURV_MAX_BREAKPRG 10

typedef struct {
	Client *client;
	RNGState rnd;
	cs_uint64 lastHit;
	cs_char lastWorld[65];
	cs_uint16 inventory[256];
	SVec lastClick;
	Ang lastAng;
	Vec lastPos;
	cs_bool loadSucc;
	cs_bool craftMode;
	cs_bool craftHelp;
	cs_bool freeFall;
	Vec fallStart;
	cs_byte health, oxygen, hackScore;
	cs_bool showOxygen, godMode, pvpMode;
	cs_uint16 regenTimer, breakTimer;
	cs_bool breakStarted;
	cs_byte breakProgress;
	BlockID breakBlock;
} SrvData;

SrvData *SurvData_Create(Client *client);
void SurvData_Reset(SrvData *data);
void SurvData_Free(Client *client);
SrvData *SurvData_Get(Client *client);
SrvData *SurvData_GetByID(ClientID id);
#endif // SURV_DATA_H
