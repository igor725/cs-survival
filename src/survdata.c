#include <core.h>
#include <client.h>
#include <assoc.h>
#include "survdata.h"

AssocType SurvData_AssocType = -1;

cs_bool SurvData_Create(Client *client) {
	if(SurvData_AssocType < 0) {
		SurvData_AssocType = Assoc_NewType(ASSOC_BIND_CLIENT);
		if(SurvData_AssocType < 0) return false;
	}

	SrvData *ptr = Assoc_AllocFor(client, SurvData_AssocType, 1, sizeof(SrvData));
	if(ptr) {
		ptr->health = SURV_MAX_HEALTH;
		ptr->oxygen = SURV_MAX_OXYGEN;
		ptr->client = client;
		return true;
	}

	return false;
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
