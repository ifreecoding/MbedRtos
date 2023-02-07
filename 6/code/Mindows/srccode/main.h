
#include <string.h>
#include "mindows.h"


#define TASK_STASK_LEN          400     /* ������ʹ�õ�ջ����, ��λ: �ֽ� */

#define BUF_LEN                 1000    /* ��Ϣ���峤��, ��λ: �ֽ� */


/* ��Ϣ����ṹ */
typedef struct msg_buf
{
    M_DLIST list;       /* �����ӽڵ� */
    U32 len;            /* ���ݳ��� */
    U8 buf[BUF_LEN];    /* ���� */
}MSG_BUF;


extern M_SEM* gprx_sem;
extern M_QUE* gprx_que;
extern M_QUE* gptx_que;
extern MSG_BUF grx_buf;
extern MSG_BUF gtx_buf;


extern U32 socket_tcp_connect(U8* pserver_ip, U32 server_port, U32 client_port);
extern void rx_thread(void* pdata);
extern void handle_thread(void* pdata);
extern void tx_thread(void* pdata);
extern void msg_rx(void);
extern void msg_process(void);
extern void msg_tx(void);
extern void uart1_init(void);
extern void start_to_send_data(void);
extern void msg_rx_isr(void);
extern void msg_tx_isr(void);

