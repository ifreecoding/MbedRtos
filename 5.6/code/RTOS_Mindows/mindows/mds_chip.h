
#ifndef  MDS_CHIP_H
#define  MDS_CHIP_H


/* ���жϷ����, 0x00000000~0x1FFFFFFFΪ����ϵͳ���� */
#define SWI_TASKSCHED           0x10001001  /* ������� */


/*********************************** �������� *************************************/
extern void MDS_IntPendSvSet(void);
extern U32 MDS_RunInInt(void);
extern U32 MDS_IntLock(void);
extern U32 MDS_IntUnlock(void);


#endif

