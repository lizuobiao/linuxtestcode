#ifndef __TSPEED_H__
#define __TSPEED_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <error.h>

    /* For "open" function */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
struct if_data
{
    /*  generic interface information */
    u_long ifi_opackets;    /*  packets sent on interface */
    u_long ifi_ipackets;    /*  packets received on interface */
    u_long ifi_obytes;      /*  total number of octets sent */
    u_long ifi_ibytes;      /*  total number of octets received */
};

struct if_info
{
    char ifi_name[16];
    unsigned long ifi_ibytes;
    unsigned long ifi_obytes;
};
struct if_speed
{
    char ifs_name[16];
    unsigned long ifs_ispeed;
    unsigned long ifs_ospeed;
    unsigned long ifs_us;
};

extern int get_if_dbytes(struct if_info *ndev);
extern int get_if_speed(struct if_speed *ndev);
extern double *my_ipconfig(char *ath0);
extern double get_traffic(char *ath0,int direction);
#ifdef __cplusplus
}
#endif

#endif
