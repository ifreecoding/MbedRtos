
#ifndef  __MSG_TX_H__
#define  __MSG_TX_H__


#include "global.h"


/* tx»º³å×é°ü×´Ì¬ */
typedef enum tx_sta
{
    TX_7E = 0,
    TX_NORMAL,
    TX_TRANSFER_7E,
    TX_TRANSFER_7D,
    TX_TRANSFER_8E
}TX_STA;


extern void msg_tx(void);
extern void tx_task_send_data(BUF_NODE* pbuf);
extern BUF_NODE* fill_tx_msg(BUF_NODE* pbuf);
extern BUF_NODE* tx_message_transfer(USR_MSG* pbuf);


#endif

