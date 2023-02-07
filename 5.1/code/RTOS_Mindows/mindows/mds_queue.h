
#ifndef MDS_QUEUE_H
#define MDS_QUEUE_H


/* �ڵ������еķ���ֵ */
#define RTN_QUPTOV      RTN_SMGVOV      /* ���ڵ������е��¶����ź������ */

/* ����ȡ���ڵ�ķ���ֵ */
#define RTN_QUGTTO      RTN_SMTKTO      /* �ȴ�������Ϣ��ʱ��ľ�, ��ʱ���� */
#define RTN_QUGTRT      RTN_SMTKRT      /* û�л�ȡ��������Ϣ, ֱ�ӷ��� */
#define RTN_QUGTDL      RTN_SMTKDL      /* ���б�ɾ�� */

/* ���е��ȷ�ʽ */
#define QUEFIFO         SEMFIFO         /* ���в����Ƚ��ȳ����� */
#define QUEPRIO         SEMPRIO         /* ���в������ȼ����� */

/* ���г�ʼֵ */
#define QUEEMPTY        SEMEMPTY        /* ����Ϊ��״̬ */
#define QUEFULL         SEMFULL         /* ����Ϊ��״̬ */

/* �ӳٵȴ���ʱ�� */
#define QUENOWAIT       SEMNOWAIT       /* ���ȴ� */
#define QUEWAITFEV      SEMWAITFEV      /* ���õȴ� */


typedef struct m_que    /* ���нṹ */
{
    M_DLIST strList;    /* �������� */
    M_SEM strSem;       /* �����ź��� */
    U8* pucQueMem;      /* ��������ʱ���ڴ��ַ */
}M_QUE;


/*********************************** �������� *************************************/
extern M_QUE* MDS_QueCreate(M_QUE* pstrQue, U32 uiSemOpt);
extern U32 MDS_QuePut(M_QUE* pstrQue, M_DLIST* pstrQueNode);
extern U32 MDS_QueGet(M_QUE* pstrQue, M_DLIST** ppstrQueNode, U32 uiDelayTick);
extern U32 MDS_QueDelete(M_QUE* pstrQue);


#endif

