
#ifndef  WLX_CORE_C_H
#define  WLX_CORE_C_H


#include "wanlix.h"


/* THUMBָ�USR����ģʽ���� */
#define MODE_USR            0x01000000


/*********************************** �������� *************************************/
extern U32 guiCurTask;


/*********************************** �������� *************************************/
extern void WLX_ContextSwitch(STACKREG* pstrCurTaskStackRegAddr,
                              STACKREG* pstrNextTaskStackRegAddr);
extern void WLX_SwitchToTask(STACKREG* pstrNextTaskStackRegAddr);


#endif

