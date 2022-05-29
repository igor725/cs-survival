#ifndef SURVCRAFT_H
#define SURVCRAFT_H
#include <types/block.h>
#include <types/event.h>

#define SURVCRAFT_MAX_RECP_LEN 6

typedef struct _SRecipe {
	cs_uint16 count;
	struct _SRItem {
		BlockID id;
		cs_uint16 count;
	} items[SURVCRAFT_MAX_RECP_LEN];
} SurvRecipe;

void SurvCraft_Init(void);
cs_bool SurvCraft_OnMessage(onMessage *obj);
void SurvCraft_OnHeldChange(onHeldBlockChange *obj);
cs_bool SurvCraft_OnBlockPlace(onBlockPlace *obj);
SurvRecipe *SurvCraft_GetRecipe(BlockID id);
#endif
