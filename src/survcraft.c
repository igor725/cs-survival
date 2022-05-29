#include <core.h>
#include <log.h>
#include <event.h>
#include <client.h>
#include <command.h>
#include <block.h>
#include "survdata.h"
#include "survinv.h"
#include "survcraft.h"

static SurvRecipe recipes[256] = {
	[BLOCK_STONE] = {
		.count = 1,
		.items = {
			{.id = BLOCK_COBBLE, .count = 1}
		}
	},
	[BLOCK_GRASS] = {
		.count = 1,
		.items = {
			{.id = BLOCK_DIRT, .count = 1},
			{.id = BLOCK_LEAVES, .count = 1}
		}
	},
	[BLOCK_COBBLE] = {
		.count = 1,
		.items = {
			{.id = BLOCK_SAND, .count = 8},
			{.id = BLOCK_GRAVEL, .count = 2}
		}
	},
	[BLOCK_WOOD] = {
		.count = 4,
		.items = {
			{.id = BLOCK_LOG, .count = 1}
		}
	},
	[BLOCK_GRAVEL] = {
		.count = 2,
		.items = {
			{.id = BLOCK_SAND, .count = 1},
			{.id = BLOCK_DIRT, .count = 1}
		}
	},
	[BLOCK_SPONGE] = {
		.count = 2,
		.items = {
			{.id = BLOCK_SAND, .count = 1},
			{.id = BLOCK_GRAVEL, .count = 1},
			{.id = BLOCK_FIRE, .count = 1}
		}
	},
	[BLOCK_GLASS] = {
		.count = 1,
		.items = {
			{.id = BLOCK_FIRE, .count = 1},
			{.id = BLOCK_SAND, .count = 1}
		}
	},
	[BLOCK_RED] = {
		.count = 1,
		.items = {
			{.id = BLOCK_WHITE, .count = 1},
			{.id = BLOCK_ROSE, .count = 4}
		}
	},
	[BLOCK_ORANGE] = {
		.count = 1,
		.items = {
			{.id = BLOCK_RED, .count = 1},
			{.id = BLOCK_YELLOW, .count = 1}
		}
	},
	[BLOCK_YELLOW] = {
		.count = 1,
		.items = {
			{.id = BLOCK_WHITE, .count = 1},
			{.id = BLOCK_DANDELION, .count = 8}
		}
	},
	[BLOCK_LIME] = {
		.count = 1,
		.items = {
			{.id = BLOCK_GREEN, .count = 1},
			{.id = BLOCK_YELLOW, .count = 1}
		}
	},
	[BLOCK_GREEN] = {
		.count = 1,
		.items = {
			{.id = BLOCK_GRASS, .count = 2},
			{.id = BLOCK_BLACK, .count = 1}
		}
	},
	[BLOCK_TEAL] = {
		.count = 1,
		.items = {
			{.id = BLOCK_YELLOW, .count = 1},
			{.id = BLOCK_CYAN, .count = 3}
		}
	},
	[BLOCK_AQUA] = {
		.count = 1,
		.items = {
			{.id = BLOCK_BLUE, .count = 1},
			{.id = BLOCK_GREEN, .count = 1}
		}
	},
	[BLOCK_CYAN] = {
		.count = 1,
		.items = {
			{.id = BLOCK_BLUE, .count = 1},
			{.id = BLOCK_WHITE, .count = 2}
		}
	},
	[BLOCK_BLUE] = {
		.count = 1,
		.items = {
			{.id = BLOCK_OBSIDIAN, .count = 1},
			{.id = BLOCK_WHITE, .count = 1},
			{.id = BLOCK_FIRE, .count = 1}
		}
	},
	[BLOCK_INDIGO] = {
		.count = 1,
		.items = {
			{.id = BLOCK_VIOLET, .count = 2},
			{.id = BLOCK_BLUE, .count = 1}
		}
	},
	[BLOCK_VIOLET] = {
		.count = 1,
		.items = {
			{.id = BLOCK_BLUE, .count = 2},
			{.id = BLOCK_RED, .count = 1}
		}
	},
	[BLOCK_MAGENTA] = {
		.count = 1,
		.items = {
			{.id = BLOCK_YELLOW, .count = 1},
			{.id = BLOCK_RED, .count = 1},
			{.id = BLOCK_BLUE, .count = 1}
		}
	},
	[BLOCK_PINK] = {
		.count = 1,
		.items = {
			{.id = BLOCK_RED, .count = 1},
			{.id = BLOCK_WHITE, .count = 1}
		}
	},
	[BLOCK_BLACK] = {
		.count = 1,
		.items = {
			{.id = BLOCK_WHITE, .count = 1},
			{.id = BLOCK_DIRT, .count = 2}
		}
	},
	[BLOCK_GRAY] = {
		.count = 1,
		.items = {
			{.id = BLOCK_WHITE, .count = 1},
			{.id = BLOCK_DIRT, .count = 1}
		}
	},
	[BLOCK_WHITE] = {
		.count = 8,
		.items = {
			{.id = BLOCK_FIRE, .count = 1},
			{.id = BLOCK_LEAVES, .count = 6}
		}
	},
	[BLOCK_GOLD] = {
		.count = 1,
		.items = {
			{.id = BLOCK_GOLD_ORE, .count = 9}
		}
	},
	[BLOCK_IRON] = {
		.count = 1,
		.items = {
			{.id = BLOCK_IRON_ORE, .count = 9}
		}
	},
	[BLOCK_DOUBLE_SLAB] = {
		.count = 1,
		.items = {
			{.id = BLOCK_SLAB, .count = 2}
		}
	},
	[BLOCK_SLAB] = {
		.count = 1,
		.items = {
			{.id = BLOCK_STONE, .count = 2}
		}
	},
	[BLOCK_BRICK] = {
		.count = 2,
		.items = {
			{.id = BLOCK_COBBLE, .count = 1},
			{.id = BLOCK_ORANGE, .count = 1},
			{.id = BLOCK_FIRE, .count = 2}
		}
	},
	[BLOCK_TNT] = {
		.count = 1,
		.items = {
			{.id = BLOCK_GRAVEL, .count = 16},
			{.id = BLOCK_SAND, .count = 8},
			{.id = BLOCK_FIRE, .count = 4}
		}
	},
	[BLOCK_BOOKSHELF] = {
		.count = 1,
		.items = {
			{.id = BLOCK_WOOD, .count = 2}
		}
	},
	[BLOCK_OBSIDIAN] = {
		.count = 1,
		.items = {
			{.id = BLOCK_FIRE, .count = 6},
			{.id = BLOCK_COBBLE, .count = 4}
		}
	},
	[BLOCK_COBBLESLAB] = {
		.count = 2,
		.items = {
			{.id = BLOCK_COBBLE, .count = 1}
		}
	},
	[BLOCK_ROPE] = {
		.count = 4,
		.items = {
			{.id = BLOCK_WOOD, .count = 2},
			{.id = BLOCK_FIRE, .count = 1}
		}
	},
	[BLOCK_SANDSTONE] = {
		.count = 2,
		.items = {
			{.id = BLOCK_SAND, .count = 3}
		}
	},
	[BLOCK_FIRE] = {
		.count = 1,
		.items = {
			{.id = BLOCK_COAL_ORE, .count = 1},
			{.id = BLOCK_WOOD, .count = 1}
		}
	},
	[BLOCK_LIGHTPINK] = {
		.count = 1,
		.items = {
			{.id = BLOCK_PINK, .count = 2},
			{.id = BLOCK_WHITE, .count = 1}
		}
	},
	[BLOCK_FORESTGREEN] = {
		.count = 1,
		.items = {
			{.id = BLOCK_GREEN, .count = 1},
			{.id = BLOCK_DIRT, .count = 1}
		}
	},
	[BLOCK_BROWN] = {
		.count = 1,
		.items = {
			{.id = BLOCK_RED, .count = 1},
			{.id = BLOCK_GREEN, .count = 1}
		}
	},
	[BLOCK_DEEPBLUE] = {
		.count = 1,
		.items = {
			{.id = BLOCK_BLUE, .count = 1},
			{.id = BLOCK_DIRT, .count = 1}
		}
	},
	[BLOCK_TURQUOISE] = {
		.count = 1,
		.items = {
			{.id = BLOCK_YELLOW, .count = 1},
			{.id = BLOCK_CYAN, .count = 5}
		}
	},
	[BLOCK_MAGMA] = {
		.count = 1,
		.items = {
			{.id = BLOCK_COBBLE, .count = 1},
			{.id = BLOCK_FIRE, .count = 16}
		}
	},
	[BLOCK_CRATE] = {
		.count = 1,
		.items = {
			{.id = BLOCK_WOOD, .count = 4},
			{.id = BLOCK_FIRE, .count = 1}
		}
	},
	[BLOCK_STONEBRICK] = {
		.count = 2,
		.items = {
			{.id = BLOCK_STONE, .count = 1},
			{.id = BLOCK_BRICK, .count = 1}
		}
	}
};

static void ShowFullInventory(SrvData *data) {
	World *world = Client_GetWorld(data->client);
	cs_byte invIdx = 0;

	for(cs_uint16 i = 1; i < 256; i++) {
		if(Block_IsValid(world, (BlockID)i) && recipes[i].count > 0)
			Client_SetInvOrder(data->client, ++invIdx, (BlockID)i);
	}
}

COMMAND_FUNC(Craft) {
	SrvData *data = SurvData_Get(ccdata->caller);
	if(!data)
		COMMAND_PRINT("Something went wrong");

	if(data->godMode)
		COMMAND_PRINT("Crafting is not available in god mode");

	data->craftMode ^= 1;
	if(data->craftMode) {
		ShowFullInventory(data);
		Client_SetHeldBlock(ccdata->caller, BLOCK_AIR, false);
		COMMAND_PRINT(
			"&2Entering craft mode...\r\n"
			"&2Open your inventory and pick the item you want to craft."
		);
	} else {
		SurvInv_UpdateInventory(data);
		COMMAND_PRINT("&2Exiting craft mode...");
	}
}

static void printreceipe(Client *client, BlockID id, struct _SRecipe *recp) {
	cs_char recipe_text[512] = {0}, tmp[32] = {0};
	World *world = Client_GetWorld(client);
	cs_size strlen = String_Copy(recipe_text, 512, "&eRecipe&f:");

	for(cs_byte i = 0; i < SURVCRAFT_MAX_RECP_LEN && recp->items[i].id; i++) {
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

void SurvCraft_OnHeldChange(onHeldBlockChange *obj) {
	SrvData *data = SurvData_Get(obj->client);

	if(data && data->craftMode) {
		if(obj->curr != BLOCK_AIR) {
			struct _SRecipe *recp = &recipes[obj->curr];
			if(recp->items[0].id == BLOCK_AIR) {
				Client_Chat(obj->client, MESSAGE_TYPE_CHAT, "&cThis item cannot be crafted!");
				Client_SetHeldBlock(obj->client, BLOCK_AIR, false);
				return;
			}
			if(data->craftHelp) {
				Client_Chat(obj->client, MESSAGE_TYPE_CHAT, "&2Alright, now send to the chat the number of recipe repetitions.");
				Client_Chat(obj->client, MESSAGE_TYPE_CHAT, "&aE.g. if you enter \"4\" for the planks recipe, you will get 16 planks.");
				Client_Chat(obj->client, MESSAGE_TYPE_CHAT, "&aSince 1 log block results 4 planks.");
				data->craftHelp = false;
			}
			printreceipe(obj->client, obj->curr, recp);
		}
	}
}

cs_bool SurvCraft_OnMessage(onMessage *obj) {
	SrvData *data = SurvData_Get(obj->client);

	if(data && data->craftMode) {
		BlockID held = Client_GetHeldBlock(obj->client);
		if(held != BLOCK_AIR) {
			struct _SRecipe *recp = &recipes[held];
			if(recp->items[0].id == BLOCK_AIR) return true;

			cs_int32 reps = String_ToInt(obj->message);
			if(reps > 0) {
				for(cs_byte i = 0; i < SURVCRAFT_MAX_RECP_LEN && recp->items[i].id != BLOCK_AIR; i++) {
					struct _SRItem *item = &recp->items[i];
					if(SurvInv_Get(data, item->id) < (cs_uint16)reps * item->count) {
						Client_Chat(obj->client, MESSAGE_TYPE_CHAT, "&cYou do not have enough resources to craft this item.");
						return false;
					}
				}
				cs_uint16 expected = (cs_uint16)reps * recp->count;
				expected = SurvInv_Add(data, held, expected);
				if(expected == 0) {
					Client_Chat(obj->client, MESSAGE_TYPE_CHAT, "&cYou don't have enough space for this block.");
					return false;
				}
				for(cs_byte i = 0; i < SURVCRAFT_MAX_RECP_LEN && recp->items[i].id != BLOCK_AIR; i++) {
					struct _SRItem *item = &recp->items[i];
					SurvInv_Take(data, item->id, expected);
				}
				data->craftMode = false;
				SurvInv_UpdateInventory(data);
				Client_Chat(obj->client, MESSAGE_TYPE_CHAT, "&aItem successfully crafted.");
				return false;
			}
		}
	}

	return true;
}

cs_bool SurvCraft_OnBlockPlace(onBlockPlace *obj) {
	SrvData *data = SurvData_Get(obj->client);
	return data ? !data->craftMode : true;
}

SurvRecipe *SurvCraft_GetRecipe(BlockID id) {
	return &recipes[id];
}

void SurvCraft_Init(void) {
	COMMAND_ADD(Craft, CMDF_CLIENT, "Enter crafting mode");
}
