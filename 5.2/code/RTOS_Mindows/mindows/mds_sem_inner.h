
#ifndef MDS_SEM_INNER_H
#define MDS_SEM_INNER_H


/* �ź����������� */
#define SEMSCHEDOPTMASK     0x00000003      /* �ź��������е��ȷ�ʽ������ */
#define SEMTYPEMASK         0x0000001C      /* �ź����������ź������͵����� */
#define SEMPRIINHMASK       0x00000020      /* �ź����������������ȼ��̳е����� */


/*********************************** �������� *************************************/
extern U32 MDS_SemFlushValue(M_SEM* pstrSem, U32 uiRtnValue);


#endif

