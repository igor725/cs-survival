#include <core.h>
#include <str.h>
#include <client.h>
#include <platform.h>
#include "survdata.h"

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

// TODO: Do not read/save whole SrvData structure
INL static cs_bool ReadPlayerData(SrvData *data, cs_file handle) {
	cs_bool rd = File_Read(data, sizeof(SrvData), 1, handle) == 1;
	File_Close(handle);
	return rd;
}

cs_bool SurvFS_LoadPlayerData(SrvData *data) {
	cs_char filepath[FILENAME_MAX];
	if(GetFilePathFor(data->client, filepath, FILENAME_MAX)) {
		cs_file handle = File_Open(filepath, "rb");
		if(handle) return ReadPlayerData(data, handle);
	}

	return false;
}

INL static cs_bool WritePlayerData(SrvData *data, cs_file handle) {
	cs_bool rd = File_Write(data, sizeof(SrvData), 1, handle) == 1;
	File_Close(handle);
	return rd;
}

cs_bool SurvFS_SavePlayerData(SrvData *data) {
	cs_char filepath[FILENAME_MAX];
	if(GetFilePathFor(data->client, filepath, FILENAME_MAX)) {
		cs_file handle = File_Open(filepath, "wb");
		if(handle) return WritePlayerData(data, handle);
	}

	return false;
}
