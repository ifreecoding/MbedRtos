
#ifndef  __MSG_RX_H__
#define  __MSG_RX_H__


#include "global.h"


extern void msg_rx(void);
extern U32 rx_message_packet(U8* pbuf, MSG_STR* ppacket_buf, U32 data_len,
                             U32* pused_len);


#endif

