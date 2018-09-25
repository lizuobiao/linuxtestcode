#ifndef __MYPRINTF_H__
#define __MYPRINTF_H__

#include <stdio.h>
#include <time.h>

#define PRINT   printf

#define PERR(...)                                        \
do{                                                     \
		PRINT("<%ld> %s %s %d ERR : ", time(NULL),      \
               __FILE__, __FUNCTION__, __LINE__);       \
  		PRINT(__VA_ARGS__);                             \
		PRINT("\r\n");									\
} while (0)

#define PINFO(...)  										\
do{														\
	PRINT("<%ld> %s %d INFO : ", time(NULL),            \
		   __FILE__, __LINE__);                         \
	PRINT(__VA_ARGS__);									\
	PRINT("\r\n");										\
} while (0)
	
#define DLS_PrintInfo  PINFO
#define DLS_PrintErr   PERR

#endif
