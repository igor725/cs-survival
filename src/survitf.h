#ifndef SURV_ITF_H
#define SURV_ITF_H
#include <core.h>
#include <client.h>
#include "survinv.h"
#include "survgui.h"
#include "survdmg.h"

#define SURV_ITF_NAME "SurvivalController_v1"

typedef struct _SurvItf {
	SrvData *(*getSrvData)(Client *client);

	cs_bool (*isInGodMode)(SrvData *data);
	cs_bool (*isInPvPMode)(SrvData *data);

	void (*setGodMode)(SrvData *data, cs_bool state);
	void (*setPvPMode)(SrvData *data, cs_bool state);


	cs_uint16 (*getBlockCount)(SrvData *data, BlockID id);
	cs_uint16 (*takeFromInventory)(SrvData *data, BlockID id, cs_uint16 ammount);
	cs_uint16 (*giveToInventory)(SrvData *data, BlockID id, cs_uint16 ammount);
	void (*updateInventory)(SrvData *data);

	void (*hurt)(SrvData *target, SrvData *attacker, cs_byte damage);
	void (*heal)(SrvData *target, cs_byte points);
	void (*kill)(SrvData *target);
} SurvItf;
#endif
