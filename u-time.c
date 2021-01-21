// #include <stdlib.h>
// #include <stdio.h>
// #include <cbm.h>
//#include <string.h>
#include <time.h>

// #ifdef __C128__
// #include <c128.h>
// #endif

// #ifdef __C64__
// #include <c64.h>
// #endif

#include "./ultimate-dos-lib/src/lib/ultimate_lib.h"

#define NTP_TIMESTAMP_DELTA 2208988800ul //0x83AA7E80
/* //the data in the ntp packet
  typedef struct { // https://github.com/lettier/ntpclient/blob/master/source/c/main.c
    unsigned char li_vn_mode;      // Eight bits. li, vn, and mode.
                             // li.   Two bits.   Leap indicator.
                             // vn.   Three bits. Version number of the protocol.
                             // mode. Three bits. Client will pick mode 3 for client.

    unsigned char stratum;         // Eight bits. Stratum level of the local clock.
    unsigned char poll;            // Eight bits. Maximum interval between successive messages.
    unsigned char precision;       // Eight bits. Precision of the local clock.

    unsigned long rootDelay;      // 32 bits. Total round trip delay time.
    unsigned long rootDispersion; // 32 bits. Max error aloud from primary clock source.
    unsigned long refId;          // 32 bits. Reference clock identifier.

    unsigned long refTm_s;        // 32 bits. Reference time-stamp seconds.
    unsigned long refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

    unsigned long origTm_s;       // 32 bits. Originate time-stamp seconds.
    unsigned long origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

    unsigned long rxTm_s;         // 32 bits. Received time-stamp seconds.
    unsigned long rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

    unsigned long txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
    unsigned long txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.
  } ntp_packet;              // Total: 384 bits or 48 bytes.
*/

unsigned char CheckStatus() {
    if (uii_status[0] != '0' || uii_status[1] != '0') {
        printf("\nStatus: %s Data:%s", uii_status, uii_data);
        return 1;
    }
    return 0;
}

char host[] = "pool.ntp.org";
struct tm *datetime;
extern struct _timezone _tz;
void main(void) 
{
    unsigned char settime[6];
    unsigned char fullcmd[] = { 0x00, NET_CMD_SOCKET_WRITE, 0x00, \
                               0x1b, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    unsigned char socket = 0;
    time_t t;
    char res[32];
    uii_get_time();
    printf("\nUltimate datetime: %s", uii_data);
    
    printf("\nConnecting to: %s", host);
	socket = uii_udpconnect(host, 123); //https://github.com/markusC64/1541ultimate2/blob/master/software/io/network/network_target.cc
    if(CheckStatus()) return;

    printf("\nSending data");
	fullcmd[2] = socket;
    uii_settarget(TARGET_NETWORK);
    uii_sendcommand(fullcmd, 51);//3 + sizeof( ntp_packet ));
	uii_readstatus();
	uii_accept();
    if(CheckStatus()) return;
    
    printf("\nReading result");
    uii_socketread(socket, 50);// 2 + sizeof( ntp_packet ));
    if(CheckStatus()) return;
    uii_socketclose(socket);

    t = uii_data[37] | (((unsigned long)uii_data[36])<<8)| (((unsigned long)uii_data[35])<<16)| (((unsigned long)uii_data[34])<<24);
    t -= NTP_TIMESTAMP_DELTA;
    printf("\nUnix epoch %lu", t);
    _tz.timezone = 3600; // TODO: Input for local timezone
    datetime = localtime(&t);
    if (strftime(res, sizeof(res), "%F %H:%M:%S", datetime) == 0){
        printf("\nError cannot parse date");
        return;
    }
    printf("\nNTP datetime: %s", res);
    settime[0]=datetime->tm_year;
    settime[1]=datetime->tm_mon + 1;
    settime[2]=datetime->tm_mday;
    settime[3]=datetime->tm_hour;
    settime[4]=datetime->tm_min;
    settime[5]=datetime->tm_sec;
    uii_set_time(settime);
    printf("\nStatus: %s", uii_status);
}

