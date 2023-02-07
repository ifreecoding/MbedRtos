
#ifndef  __UART_H__
#define  __UART_H__

#include "global.h"


#define RING_BUF_LEN                512     /* ���ν��ջ��峤��, ��λ: �ֽ� */


#define UART_TX_IDLE                 0      /* ����Ӳ�����Ϳ���̬, ���Է������� */
#define UART_TX_BUSY                 1      /* ����Ӳ������æ̬, �޷��������� */


/* ���λ���, ͷָ��ָ���һ����Ч�ַ�, βָ��ָ���һ����Ч�ַ�, ͷβָ�����ʱ��ʾ
   ����Ϊ�� */
typedef struct ring_buf
{
    U32 head;                       /* ���λ���ͷ���� */
    U32 tail;                       /* ���λ���β���� */
    U8 buf[RING_BUF_LEN];           /* ���λ��� */
}RING_BUF;


extern void copy_data_from_ring_rx_buf(U8* pbuf, U32 len);
extern void copy_data_to_ring_tx_buf(U8* pbuf, U32 len);


#endif

