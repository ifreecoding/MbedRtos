
#ifndef MDS_QUEUE_H
#define MDS_QUEUE_H


#define RTN_NULL            2       /* ����Ϊ�� */


typedef struct m_que    /* ���нṹ */
{
    M_DLIST strList;    /* �������� */
    U8* pucQueMem;      /* ��������ʱ���ڴ��ַ */
}M_QUE;


/*********************************** �������� *************************************/
extern M_QUE* MDS_QueCreate(M_QUE* pstrQue);
extern U32 MDS_QuePut(M_QUE* pstrQue, M_DLIST* pstrQueNode);
extern U32 MDS_QueGet(M_QUE* pstrQue, M_DLIST** ppstrQueNode);
extern U32 MDS_QueDelete(M_QUE* pstrQue);


#endif

