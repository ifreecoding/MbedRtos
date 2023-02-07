
#ifndef MDS_SEM_H
#define MDS_SEM_H


/* ��ȡ�ź����ķ���ֵ */
#define RTN_SMTKTO          2           /* �ȴ��ź�����ʱ��ľ�, ��ʱ���� */
#define RTN_SMTKRT          3           /* û�л�ȡ���ź���, ֱ�ӷ��� */
#define RTN_SMTKDL          4           /* �ź�����ɾ�� */

/* �ͷ��ź����ķ���ֵ */
#define RTN_SMGVOV          2           /* �ͷ��ź������ */


/* �ź�������ƫ���� */
#define SEMSCHOF            0           /* �ź��������е��ȷ�ʽ��ƫ���� */
#define SEMTYPOF            2           /* �ź����������ź������͵�ƫ���� */

/* �ź������ȷ�ʽ */
#define SEMFIFO             (1 << SEMSCHOF) /* �ź��������Ƚ��ȳ����� */
#define SEMPRIO             (2 << SEMSCHOF) /* �ź����������ȼ����� */

/* �ź������� */
#define SEMBIN              (1 << SEMTYPOF) /* �������ź��� */
#define SEMCNT              (2 << SEMTYPOF) /* �����ź��� */

/* �ź�����ʼֵ */
#define SEMEMPTY            0           /* �ź���Ϊ��״̬ */
#define SEMFULL             0xFFFFFFFF  /* �ź���Ϊ��״̬ */


/* �ӳٵȴ���ʱ�� */
#define SEMNOWAIT           0           /* ���ȴ� */
#define SEMWAITFEV          0xFFFFFFFF  /* ���õȴ� */


/*********************************** �������� *************************************/
extern M_SEM* MDS_SemCreate(M_SEM* pstrSem, U32 uiSemOpt, U32 uiInitVal);
extern U32 MDS_SemTake(M_SEM* pstrSem, U32 uiDelayTick);
extern U32 MDS_SemGive(M_SEM* pstrSem);
extern U32 MDS_SemFlush(M_SEM* pstrSem);
extern U32 MDS_SemDelete(M_SEM* pstrSem);


#endif

