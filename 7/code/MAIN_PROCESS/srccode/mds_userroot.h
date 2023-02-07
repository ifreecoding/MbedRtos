
#ifndef  MDS_USERROOT_H
#define  MDS_USERROOT_H


#include <string.h>
#include "mindows.h"


#define TASK_STASK_LEN          400         /* ������ʹ�õ�ջ����, ��λ: �ֽ� */

#define BUF_LEN                 100         /* ��Ϣ���峤��, ��λ: �ֽ� */

#define TEST_PROCESS_NUM        3           /* ���Խ����� */


#define MSG_FLAG_NONE           0           /* û����Ϣ */
#define MSG_FLAG_NEED_SEND      1           /* ����Ϣ��Ҫ���� */


#define TEST_PROCESS_ADDR       0x08010000  /* ���Խ��̴�ŵ���ʼ��ַ */
#define RESET_VECTOR_ADDR       0x04        /* ��λ������ַ */
#define TEST_PROCESS_MAX_SIZE   0x2000      /* ���Խ�����ռ���ռ�, ��λ: �ֽ� */


/* ��Ϣ����ṹ */
typedef struct msg_buf
{
    U32 flag;           /* �����־ */
    U8 buf[BUF_LEN];    /* ���� */
}MSG_BUF;


extern MSG_BUF gtx_buf[3];


extern void rx_thread(void* pdata);
extern void tx_thread(void* pdata);
extern void uart1_init(void);
extern void put_char(U8 send_char);
extern void put_string(U8* pstring);


#endif

