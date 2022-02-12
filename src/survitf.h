#ifndef SURV_ITF_H
#define SURV_ITF_H
#include <core.h>
#include <client.h>
#include "survinv.h"
#include "survgui.h"
#include "survdmg.h"

typedef struct _SurvItf {
	SrvData *(*getSrvData)(Client *client);

	void (*redrawGUI)(SrvData *data);

	cs_uint16 (*getBlockCount)(SrvData *data, BlockID id);
	cs_uint16 (*takeFromInventory)(SrvData *data, BlockID id, cs_uint16 ammount);
	cs_uint16 (*giveToInventory)(SrvData *data, BlockID id, cs_uint16 ammount);

	void (*hurt)(SrvData *target, SrvData *attacker, cs_byte damage);
	void (*heal)(SrvData *target, cs_byte points);
	void (*kill)(SrvData *target);
} SurvItf;
#endif
