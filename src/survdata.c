#include <core.h>
#include <client.h>
#include <assoc.h>
#include <csmath.h>
#include "survdata.h"
#include "survinv.h"

AssocType SurvData_AssocType = -1;

SrvData *SurvData_Create(Client *client) {
	if(SurvData_AssocType < 0) {
		SurvData_AssocType = Assoc_NewType(ASSOC_BIND_CLIENT);
		if(SurvData_AssocType < 0) return NULL;
	}

	SrvData *data = Assoc_AllocFor(client, SurvData_AssocType, 1, sizeof(SrvData));
	if(data) {
		Random_SeedFromTime(&data->rnd);
		data->health = SURV_MAX_HEALTH;
		data->oxygen = SURV_MAX_OXYGEN;
		data->craftHelp = true;
		data->client = client;
		return data;
	}

	return NULL;
}

void SurvData_Reset(SrvData *data) {
	data->health = SURV_MAX_HEALTH;
	data->oxygen = SURV_MAX_OXYGEN;
	*data->lastWorld = '\0';
	data->craftHelp = true;
	data->pvpMode = false;
	data->godMode = false;
	SurvInv_Empty(data);
}

void SurvData_Free(Client *client) {
	Assoc_Remove(client, SurvData_AssocType);
}

SrvData *SurvData_Get(Client *client) {
	if(!client) return NULL;
	return Assoc_GetPtr(client, SurvData_AssocType);
}

SrvData *SurvData_GetByID(ClientID id) {
	Client *client = Client_GetByID(id);
	return client ? Assoc_GetPtr(client, SurvData_AssocType) : NULL;
}
