#include <core.h>
#include <str.h>
#include <world.h>
#include <client.h>
#include <platform.h>
#include "survdata.h"
#include "survfs.h"

cs_str pdat_path = "survdata" PATH_DELIM "players" PATH_DELIM "%s.dat";

void SurvFS_Init(void) {
	Directory_Ensure("survdata");
	Directory_Ensure("survdata" PATH_DELIM "players");
}

INL static cs_bool GetFilePathFor(Client *client, cs_char *path, cs_size len) {
	cs_char sanpname[65];
	if(String_Copy(sanpname, 65, Client_GetName(client)) > 0) {
		for(cs_byte i = 0; i < 65; i++) {
			cs_char cc = sanpname[i];
			if(cc == '\0') break;
			if((cc < '0' || cc > '9') && (cc < 'A' || cc > 'Z') && (cc < 'a' || cc > 'z'))
				sanpname[i] = '_';
		}

		return String_FormatBuf(path, len, pdat_path, sanpname) > 0;
	}

	return false;
}

#define READ_ENTRY(e) if(allok) allok = File_Read(&e, sizeof(e), 1, handle) == 1

INL static cs_bool ReadPlayerData(SrvData *data, cs_file handle) {
	cs_bool allok = true;
	cs_uint32 header = 0;

	READ_ENTRY(header);
	allok = (header == SURVFS_MAGIC);
	READ_ENTRY(data->lastWorld);
	READ_ENTRY(data->pvpMode);
	READ_ENTRY(data->godMode);
	READ_ENTRY(data->health);
	READ_ENTRY(data->oxygen);
	READ_ENTRY(data->lastPos);
	READ_ENTRY(data->lastAng);
	READ_ENTRY(data->inventory);
	data->loadSucc = allok;

	File_Close(handle);
	return allok;
}

cs_bool SurvFS_LoadPlayerData(SrvData *data) {
	cs_char filepath[FILENAME_MAX];
	if(GetFilePathFor(data->client, filepath, FILENAME_MAX)) {
		cs_file handle = File_Open(filepath, "rb");
		if(handle)
			return ReadPlayerData(data, handle);
		else
			return Thread_GetError() == ENOENT;
	}

	return false;
}

#define WRITE_ENTRY(e) if(allok) allok = File_Write(&e, sizeof(e), 1, handle) == 1

INL static cs_bool WritePlayerData(SrvData *data, cs_file handle) {
	cs_bool allok = true;
	cs_uint32 header = SURVFS_MAGIC;
	Vec position; Ang angle;
	Client_GetPosition(data->client, &position, &angle);

	WRITE_ENTRY(header);
	WRITE_ENTRY(data->lastWorld);
	WRITE_ENTRY(data->pvpMode);
	WRITE_ENTRY(data->godMode);
	WRITE_ENTRY(data->health);
	WRITE_ENTRY(data->oxygen);
	WRITE_ENTRY(position);
	WRITE_ENTRY(angle);
	WRITE_ENTRY(data->inventory);

	File_Close(handle);
	return allok;
}

cs_bool SurvFS_SavePlayerData(SrvData *data) {
	cs_char filepath[FILENAME_MAX], tmppath[FILENAME_MAX];
	if(GetFilePathFor(data->client, filepath, FILENAME_MAX)) {
		String_Copy(tmppath, FILENAME_MAX, filepath);
		String_Append(tmppath, FILENAME_MAX, ".tmp");
		cs_file handle = File_Open(tmppath, "wb");
		if(WritePlayerData(data, handle))
			return File_Rename(tmppath, filepath);
	}

	return false;
}
