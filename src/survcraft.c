#include <core.h>
#include <log.h>
#include <event.h>
#include <client.h>
#include <command.h>
#include <types/block.h>
#include "survdata.h"
#include "survinv.h"
#include "survcraft.h"

static SurvRecipe recipes[256] = {
	[BLOCK_STONE] = {
		.count = 1,
		.citems = 2,
		.items = {
			{.id = BLOCK_FIRE, .count = 1},
			{.id = BLOCK_COBBLE, .count = 1}
		}
	},
	[BLOCK_GRASS] = {
		.count = 1,
		.citems = 2,
		.items = {
			{.id = BLOCK_DIRT, .count = 1},
			{.id = BLOCK_LEAVES, .count = 1}
		}
	},
	[BLOCK_COBBLE] = {
		.count = 1,
		.citems = 1,
		.items = {
			{.id = BLOCK_STONE, .count = 1}
		}
	},
	[BLOCK_WOOD] = {
		.count = 4,
		.citems = 1,
		.items = {
			{.id = BLOCK_LOG, .count = 1}
		}
	},
	[BLOCK_GRAVEL] = {
		.count = 2,
		.citems = 2,
		.items = {
			{.id = BLOCK_SAND, .count = 1},
			{.id = BLOCK_DIRT, .count = 1}
		}
	},
	[BLOCK_GLASS] = {
		.count = 1,
		.citems = 2,
		.items = {
			{.id = BLOCK_FIRE, .count = 1},
			{.id = BLOCK_SAND, .count = 1}
		}
	},
	[BLOCK_GOLD] = {
		.count = 1,
		.citems = 1,
		.items = {
			{.id = BLOCK_GOLD_ORE, .count = 9}
		}
	},
	[BLOCK_IRON] = {
		.count = 1,
		.citems = 1,
		.items = {
			{.id = BLOCK_IRON_ORE, .count = 9}
		}
	},
	[BLOCK_DOUBLE_SLAB] = {
		.count = 1,
		.citems = 1,
		.items = {
			{.id = BLOCK_SLAB, .count = 1}
		}
	},
	[BLOCK_SLAB] = {
		.count = 1,
		.citems = 1,
		.items = {
			{.id = BLOCK_STONE, .count = 2}
		}
	},
	[BLOCK_BOOKSHELF] = {
		.count = 1,
		.citems = 1,
		.items = {
			{.id = BLOCK_WOOD, .count = 2}
		}
	},
	[BLOCK_OBSIDIAN] = {
		.count = 1,
		.citems = 2,
		.items = {
			{.id = BLOCK_FIRE, .count = 6},
			{.id = BLOCK_COBBLE, .count = 4}
		}
	},
	[BLOCK_SANDSTONE] = {
		.count = 2,
		.citems = 1,
		.items = {
			{.id = BLOCK_SAND, .count = 3}
		}
	},
	[BLOCK_FIRE] = {
		.count = 1,
		.citems = 2,
		.items = {
			{.id = BLOCK_COAL_ORE, .count = 1},
			{.id = BLOCK_WOOD, .count = 1}
		}
	}
};

static void ShowFullInventory(SrvData *data) {
	cs_byte invIdx = 0;

	for(cs_uint16 i = 1; i < 256; i++)
		Client_SetInvOrder(data->client, ++invIdx, (BlockID)i);
}

COMMAND_FUNC(Craft) {
	SrvData *data = SurvData_Get(ccdata->caller);
	if(!data) {
		COMMAND_PRINT("Something went wrong");
	}

	if(data->godMode) {
		COMMAND_PRINT("Crafting is not available in god mode");
	}

	data->craftMode ^= 1;
	if(data->craftMode) {
		ShowFullInventory(data);
		Client_SetHeldBlock(ccdata->caller, BLOCK_AIR, false);
		COMMAND_PRINTLINE("&2Entering craft mode...");
		COMMAND_PRINT("&2Open your inventory and pick the item you want to craft.");
	} else {
		SurvInv_UpdateInventory(data);
		COMMAND_PRINT("&2Exiting craft mode...");
	}
}

static void printreceipe(Client *client, BlockID id, struct _SRecipe *recp) {
	cs_char recipe_text[512] = {0}, tmp[32] = {0};
	World *world = Client_GetWorld(client);
	cs_size strlen = String_Copy(recipe_text, 512, "&eRecipe&f:");

	for(cs_byte i = 0; i < min(recp->citems, 6); i++) {
		struct _SRItem *item = &recp->items[i];
		if(String_FormatBuf(tmp, 32, " %dx%s +", item->count, Block_GetName(world, item->id)))
			strlen += String_Append(recipe_text, 512, tmp);
		else goto ierror;
	}

	if(strlen > 0 && strlen + 32 < 512) {
		String_Copy(recipe_text + (strlen - 1), 510 - strlen, "= ");
		if(String_FormatBuf(tmp, 32, "%dx%s", recp->count, Block_GetName(world, id)))
			strlen += String_Append(recipe_text, 512, tmp);
		else goto ierror;

		Client_Chat(client, MESSAGE_TYPE_CHAT, recipe_text);
		return;
	}

	ierror:
	Client_Chat(client, MESSAGE_TYPE_CHAT, "&cInternal error");
}

static void evtblockchange(void *param) {
	onHeldBlockChange *a = (onHeldBlockChange *)param;
	SrvData *data = SurvData_Get(a->client);

	if(data && data->craftMode) {
		if(a->curr != BLOCK_AIR) {
			struct _SRecipe *recp = &recipes[a->curr];
			if(recp->citems == 0) {
				Client_Chat(a->client, MESSAGE_TYPE_CHAT, "&cThis item cannot be crafted!");
				Client_SetHeldBlock(a->client, BLOCK_AIR, false);
				return;
			}
			if(data->craftHelp) {
				Client_Chat(a->client, MESSAGE_TYPE_CHAT, "&2Alright, now send to the chat the number of recipe repetitions.");
				Client_Chat(a->client, MESSAGE_TYPE_CHAT, "&aE.g. if you enter \"4\" for the planks recipe, you will get 16 planks.");
				Client_Chat(a->client, MESSAGE_TYPE_CHAT, "&aSince 1 log block results 4 planks.");
				data->craftHelp = false;
			}
			printreceipe(a->client, a->curr, recp);
		}
	}
}

static cs_bool evtonmessage(void *param) {
	onMessage *a = (onMessage *)param;
	SrvData *data = SurvData_Get(a->client);

	if(data && data->craftMode) {
		BlockID held = Client_GetHeldBlock(a->client);
		if(held != BLOCK_AIR) {
			struct _SRecipe *recp = &recipes[held];
			if(recp->citems == 0) return true;

			cs_int32 reps = String_ToInt(a->message);
			if(reps > 0) {
				for(cs_byte i = 0; i < recp->citems; i++) {
					struct _SRItem *item = &recp->items[i];
					if(SurvInv_Get(data, item->id) < (cs_uint16)reps * item->count) {
						Client_Chat(a->client, MESSAGE_TYPE_CHAT, "&cYou do not have enough resources to craft this item.");
						return false;
					}
				}
				SurvInv_Add(data, held, (cs_uint16)reps * recp->count);
				for(cs_byte i = 0; i < recp->citems; i++) {
					struct _SRItem *item = &recp->items[i];
					SurvInv_Take(data, item->id, (cs_uint16)reps * item->count);
				}
				data->craftMode = false;
				SurvInv_UpdateInventory(data);
				Client_Chat(a->client, MESSAGE_TYPE_CHAT, "&aItem successfully crafted.");
				return false;
			}
		}
	}

	return true;
}

static cs_bool evtblockplace(void *param) {
	onBlockPlace *a = (onBlockPlace *)param;
	SrvData *data = SurvData_Get(a->client);
	return data ? !data->craftMode : true;
}

static EventRegBunch events[] = {
	{'v', EVT_ONHELDBLOCKCHNG, (void *)evtblockchange},
	{'b', EVT_ONMESSAGE, (void *)evtonmessage},
	{'b', EVT_ONBLOCKPLACE, (void *)evtblockplace},
	{0, 0, NULL}
};

SurvRecipe *SurvCraft_GetRecipe(BlockID id) {
	return &recipes[id];
}

void SurvCraft_Init(void) {
	COMMAND_ADD(Craft, CMDF_CLIENT, "Enter crafting mode");
	Event_RegisterBunch(events);
}
