
#ifndef  DEVICE_H
#define  DEVICE_H


#include "global.h"


#define TICK                100                         /* Tickʱ��, ��λ: ms */


#define FSMCCMDADDR         ((U32)0x60000000)           /* FSMC �����ַ */
#define FSMCDATAADDR        ((U32)0x60020000)           /* FSMC ���ݵ�ַ */


/* LCD��ʾ�궨��, �������Ͻ�Ϊ����ԭ��, X�᷽������, Y�᷽������ */
#define LCDXLEN             240                         /* LCD X�᳤��, ��λ: ���� */
#define LCDYLEN             400                         /* LCD Y�᳤��, ��λ: ���� */


/* �������� */
#define PSGKEY1             0x1                         /* ���˰���1 */
#define PSGKEY2             0x2                         /* ���˰���2 */

/* ����IO���� */
#define IOPSGKEY1           GPIO_Pin_14                 /* ���˰���1 */
#define IOPSGKEY2           GPIO_Pin_15                 /* ���˰���2 */

/* �����ܽ���GPIO�˿ڵ�ƫ��ֵ */
#define PSGKEY1OFFSET       14                          /* ���˰���1 */
#define PSGKEY2OFFSET       15                          /* ���˰���2 */


/*********************************** �������� *************************************/
extern U8 gaucFont[];
extern U8 gaucInvertFont[];
extern U8 gaucRound[];
extern U8 gaucArrow1[];
extern U8 gaucArrow2[];


/*********************************** �������� *************************************/
extern void DEV_PsgKeyInit(void);
extern void DEV_TickTimerInit(void);
extern void DEV_LcdInit(void);
extern void DEV_LcdGpioCfg(void);
extern void DEV_LcdFsmcCfg(void);
extern void DEV_LcdReset(void);
extern void DEV_LcdLightOn(void);
extern void DEV_LcdLightOff(void);
extern void DEV_LcdWriteCmd(U16 usCmd);
extern void DEV_LcdWriteData(U16 usData);
extern void DEV_LcdSetBlock(U16 usXStart, U16 usXEnd, U16 usYStart, U16 usYEnd);
extern void DEV_LcdCrossInit(void);
extern U8 DEV_ReadHardwareVersion(void);


#endif

