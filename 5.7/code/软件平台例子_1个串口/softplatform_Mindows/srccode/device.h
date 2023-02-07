
#ifndef  __DEVICE_H__
#define  __DEVICE_H__


#include "global.h"


#define FSMCCMDADDR         ((U32)0x60000000)           /* FSMC �����ַ */
#define FSMCDATAADDR        ((U32)0x60020000)           /* FSMC ���ݵ�ַ */


/* LCD��ʾ�궨��, �������Ͻ�Ϊ����ԭ��, X�᷽������, Y�᷽������ */
#define LCDXLEN             240                         /* LCD X�᳤��, ��λ: ���� */
#define LCDYLEN             400                         /* LCD Y�᳤��, ��λ: ���� */


extern void DEV_LcdInit(void);
extern void DEV_LcdGpioCfg(void);
extern void DEV_LcdFsmcCfg(void);
extern void DEV_LcdReset(void);
extern void DEV_LcdLightOn(void);
extern void DEV_LcdLightOff(void);
extern void DEV_LcdWriteCmd(U16 usCmd);
extern void DEV_LcdWriteData(U16 usData);
extern void DEV_LcdSetBlock(U16 usXStart, U16 usXEnd, U16 usYStart, U16 usYEnd);
extern U8* DEV_CalcFontAddr(U8 ucAscii);
extern U8* DEV_CalcInvertFontAddr(U8 ucAscii);
extern U8 DEV_ReadHardwareVersion(void);


#endif

