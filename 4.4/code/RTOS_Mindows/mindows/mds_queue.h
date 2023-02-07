
#ifndef MDS_QUEUE_H
#define MDS_QUEUE_H


#define RTN_NULL            2       /* ����Ϊ�� */


typedef struct m_que    /* ���нṹ */
{
    M_DLIST strList;    /* �������� */
}M_QUE;


/*********************************** �������� *************************************/
extern U32 MDS_QueCreate(M_QUE* pstrQue);
extern U32 MDS_QuePut(M_QUE* pstrQue, M_DLIST* pstrQueNode);
extern U32 MDS_QueGet(M_QUE* pstrQue, M_DLIST** ppstrQueNode);


#endif

