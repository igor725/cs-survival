#ifndef SURVCRAFT_H
#define SURVCRAFT_H
#include <types/block.h>

typedef struct _SRecipe {
	cs_uint16 count;
	cs_byte citems;
	struct _SRItem {
		BlockID id;
		cs_uint16 count;
	} items[6];
} SurvRecipe;

void SurvCraft_Init(void);
SurvRecipe *SurvCraft_GetRecipe(BlockID id);
#endif
