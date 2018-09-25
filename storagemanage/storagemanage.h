#ifndef __STORAGE_MANAGE_H__
#define __STORAGE_MANAGE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define STOCMD_BUF_LEN 128

enum STOCMD{
	EXTERNAL_ALL = 0,
	INTER_ALL    = 1,
	EXTERNAL_AVA = 2,
	INTER_AVA    = 3,
	FORMAT_SD    = 4,
};

#define EXTERNAL_ALL_CMD "df -m | awk '/sdb1/{print $2}'"
#define INTER_CMD_ALL    "df -m | awk '/sda1/{print $2}'"
#define EXTERNAL_AVA_CMD "df -m | awk '/sdb1/{print $4}'"
#define INTER_AVA_CMD    "df -m | awk '/sda1/{print $4}'"
#define FORMAT_SD_CMD    "mkfs.vfat /dev/mmcblk0p16"

typedef struct 
{
	const char *systemcmd;
	uint8_t stoinfo_cmd;
}storagecmd_t;


uint32_t storage_cmd_exec(uint8_t type);

#endif
