#include <core.h>
#include <platform.h>
#include <client.h>
#include <config.h>
#include "survdata.h"

cs_uint16 SurvData_AssocType = 0;

void SurvData_Create(Client *client) {
	SrvData *ptr = Memory_Alloc(1, sizeof(SrvData));
	Assoc_Set(client, SurvData_AssocType, (void *)ptr);
	ptr->client = client;
	ptr->health = SURV_MAX_HEALTH;
	ptr->oxygen = SURV_MAX_OXYGEN;
}

void SurvData_Free(Client *client) {
	Assoc_Remove(client, SurvData_AssocType, true);
}

SrvData *SurvData_Get(Client *client) {
	return Assoc_GetPtr(client, SurvData_AssocType);
}

SrvData *SurvData_GetByID(ClientID id) {
	Client *client = Client_GetByID(id);
	return client ? Assoc_GetPtr(client, SurvData_AssocType) : NULL;
}
