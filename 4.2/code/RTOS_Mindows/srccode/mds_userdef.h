
#ifndef  MDS_USERDEF_H
#define  MDS_USERDEF_H


/******************************* �û����޸İ����ļ� *******************************/
#include "stm32f10x.h"


/******************************** �û����޸ĺ궨�� ********************************/
#define TICK                10          /* ����ϵͳ��������, ��λ: ms */
#define CORECLOCKPMS        72000000    /* оƬ�ں���Ƶ��, ��λ: Hz */


#define ROOTTASKSTACK       600         /* ������ջ��С, ��λ: �ֽ� */

#define STACKALIGNMASK      ALIGN8MASK  /* ջ��������, cortex�ں�ʹ��8�ֽڶ��� */


#define PRIORITYNUM         PRIORITY8   /* ����ϵͳ֧�ֵ����ȼ���Ŀ */


#endif

