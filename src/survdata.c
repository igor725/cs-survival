#include <core.h>
#include <platform.h>
#include <client.h>
#include <assoc.h>
#include <config.h>
#include "survdata.h"

AssocType SurvData_AssocType = -1;

void SurvData_Create(Client *client) {
	if(SurvData_AssocType < 0) {
		SurvData_AssocType = Assoc_NewType(ASSOC_BIND_CLIENT);
		if(SurvData_AssocType < 0) return;
	}
	SrvData *ptr = Assoc_AllocFor(client, SurvData_AssocType, 1, sizeof(SrvData));
	ptr->health = SURV_MAX_HEALTH;
	ptr->oxygen = SURV_MAX_OXYGEN;
	ptr->client = client;
}

void SurvData_Free(Client *client) {
	Assoc_Remove(client, SurvData_AssocType);
}

SrvData *SurvData_Get(Client *client) {
	return Assoc_GetPtr(client, SurvData_AssocType);
}

SrvData *SurvData_GetByID(ClientID id) {
	Client *client = Client_GetByID(id);
	return client ? Assoc_GetPtr(client, SurvData_AssocType) : NULL;
}
