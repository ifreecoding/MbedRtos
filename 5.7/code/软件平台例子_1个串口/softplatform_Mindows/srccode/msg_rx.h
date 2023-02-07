
#ifndef  __MSG_RX_H__
#define  __MSG_RX_H__


#include "global.h"


/* rx»º³å×é°ü×´Ì¬ */
typedef enum rx_sta
{
    RX_IDLE = 0,
    RX_NORMAL,
    RX_TRANSFER
}RX_STA;


extern void msg_rx(void);
extern U32 rx_message_packet(U8* pbuf, MSG_STR* ppacket_buf, U32 data_len,
                             U32* pused_len);


#endif

