
#ifndef  MDS_USERDEF_H
#define  MDS_USERDEF_H


/******************************* �û����޸İ����ļ� *******************************/
#include "stm32f10x.h"


/******************************** �û����޸ĺ궨�� ********************************/
/* �����ӹ��ܺ궨��, ֻ�ж���ú�ſ�ʹ�� */
//#define MDS_INCLUDETASKHOOK

/* �������ȼ��̳к궨��, ֻ�ж���ú�ſ�ʹ�� */
//#define MDS_TASKPRIOINHER

/* ����ʱ��Ƭ��ת���Ⱥ궨��, ֻ�ж���ú�ſ�ʹ�� */
#define MDS_TASKROUNDROBIN

/* ��¼����Ĵ������ջ��Ϣ���ܵĺ궨��, ֻ�ж���ú�ſ�ʹ�� */
//#define MDS_DEBUGCONTEXT

#ifdef MDS_DEBUGCONTEXT
 #define MDS_CONTEXTADDR    0x2000B000  /* ��¼���ڴ���ʼ��ַ */
 #define MDS_CONTEXTLEN     0x1000      /* ��¼�ĳ���, ��λ: �ֽ� */
#endif

/* ���ջ�ռ书�ܵĺ궨��, ֻ�ж���ú�ſ�ʹ�� */
//#define MDS_DEBUGSTACKCHECK

/* CPUռ���ʹ��ܵĺ궨��, ֻ�ж���ú�ſ�ʹ�� */
//#define MDS_CPUSHARE


#define TICK                10          /* ����ϵͳ��������, ��λ: ms */
#define CORECLOCKPMS        72000000    /* оƬ�ں���Ƶ��, ��λ: Hz */


#define ROOTTASKNAME        "Root"      /* ���������� */
#define IDLETASKNAME        "Idle"      /* ������������ */

#define ROOTTASKSTACK       600         /* ������ջ��С, ��λ: �ֽ� */
#define IDLETASKSTACK       600         /* ��������ջ��С, ��λ: �ֽ� */

#define STACKALIGNMASK      ALIGN8MASK  /* ջ��������, cortex�ں�ʹ��8�ֽڶ��� */


#define PRIORITYNUM         PRIORITY8   /* ����ϵͳ֧�ֵ����ȼ���Ŀ */


#endif

