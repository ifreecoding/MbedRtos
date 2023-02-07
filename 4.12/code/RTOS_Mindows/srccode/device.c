
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "device.h"


BUFPOOL gstrBufPool;                /* ��Ϣ����� */

U16 gusPenColor;                    /* ������ɫ */
U16 gusBackGroundColor;             /* ����ɫ */
/***********************************************************************************
��������: ��ʼ�����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_SoftwareInit(void)
{
    /* ��ʼ����Ϣ����� */
    DEV_BufferInit(&gstrBufPool);

    /* ��ʼ������ */
    gpstrSerialMsgQue = MDS_QueCreate((M_QUE*)NULL, QUEPRIO);   /* ������ӡ��Ϣ���� */
    gpstrKeyMsgQue = MDS_QueCreate((M_QUE*)NULL, QUEPRIO);      /* ����������Ϣ���� */

    /* ��ʼ���ź��� */
    gpstrFigSem = MDS_SemCreate((M_SEM*)NULL, SEMBIN | SEMPRIO, SEMFULL);

    /* ��Ϸ״̬ */
    gucGameSta = GAMESTA_STOP;

#ifdef MDS_INCLUDETASKHOOK

    /* �ҽ������Ӻ��� */
    MDS_TaskCreateHookAdd(TEST_TaskCreatePrint);
    MDS_TaskSwitchHookAdd(TEST_TaskSwitchPrint);
    MDS_TaskDeleteHookAdd(TEST_TaskDeletePrint);

#endif
}

/***********************************************************************************
��������: ��ʼ��Ӳ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_HardwareInit(void)
{
    /* ��ʼ��Ӳ���汾��IO */
    DEV_HardwareVersionInit();

    /* ��ʼ������ */
    DEV_UartInit();

    /* ��ʼ������ */
    DEV_KeyConfiguration();

    /* ��ʼ��LCDҺ���� */
    DEV_LcdInit();
}

/***********************************************************************************
��������: ��ʼ������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_KeyConfiguration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* ���ð����Ĺܽ�Ϊ����״̬, ������ */
    GPIO_InitStructure.GPIO_Pin = IOKEYUP | IOKEYDOWN | IOKEYLEFT | IOKEYRIGHT  | IOKEYRESTART;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/***********************************************************************************
��������: ��ȡ����ֵ.
��ڲ���: none.
�� �� ֵ: KEYNULL: û�а�����Ӧ.
          ����ֵ: ÿbit��Ӧһ������.
                  KEYUP: ���ϰ���.
                  KEYDOWN: ���°���.
                  KEYLEFT: ���󰴼�.
                  KEYRIGHT: ���Ұ���.
                  KEYRESTART: ���¿�ʼ����.
***********************************************************************************/
U8 DEV_ReadKey(void)
{
    U8 ucKey;
    U16 usTemp;

    ucKey = KEYNULL;

    /* ��ȡ������Ӧ��GPIO�ܽ� */
    usTemp = GPIO_ReadInputData(GPIOB);

    /* �жϰ����Ƿ񱻰���, ����״̬Ϊ0, δ����״̬Ϊ1 */
    if(0 == (((usTemp & IOKEYUP) >> KEYUPOFFSET) & 1))
    {
        /* ���ϰ��������� */
        ucKey |= KEYUP;
    }

    if(0 == (((usTemp & IOKEYDOWN) >> KEYDOWNOFFSET) & 1))
    {
        /* ���°��������� */
        ucKey |= KEYDOWN;
    }

    if((0 == (((usTemp & IOKEYLEFT) >> KEYLEFTOFFSET) & 1)))
    {
        /* ���󰴼������� */
        ucKey |= KEYLEFT;
    }

    if((0 == (((usTemp & IOKEYRIGHT) >> KEYRIGHTOFFSET) & 1)))
    {
        /* ���Ұ��������� */
        ucKey |= KEYRIGHT;
    }

    if(0 == (((usTemp & IOKEYRESTART) >> KEYRESTARTOFFSET) & 1))
    {
        /* ���¿�ʼ���������� */
        ucKey |= KEYRESTART;
    }

    return ucKey;
}

/***********************************************************************************
��������: ��ʼ������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_UartInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* Enable USART1 Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1
                           | RCC_APB2Periph_AFIO, ENABLE);

    /* Configure USART1 Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART1 configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Receive and transmit enabled
        - Hardware flow control disabled (RTS and CTS signals)
    */
    USART_StructInit(&USART_InitStructure);

    /* Configure USART1 */
    USART_Init(USART1, &USART_InitStructure);

    /* Enable the USART1 */
    USART_Cmd(USART1, ENABLE);
}

/***********************************************************************************
��������: �򴮿ڷ���һ���ַ�.
��ڲ���: ucChar: ������ַ�.
�� �� ֵ: none.
***********************************************************************************/
void DEV_PutChar(U8 ucChar)
{
    /* Loop until USART1 DR register is empty */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
        ;
    }

    USART_SendData(USART1, ucChar);
}

/***********************************************************************************
��������: ���ַ�����ӡ���ڴ�. �ú�����ڲ�����printf����һ��.
��ڲ���: pvStringPt: ��һ��������ָ��, ����Ϊ�ַ���ָ��.
          ...: ��������.
�� �� ֵ: none.
***********************************************************************************/
void DEV_PutStrToMem(U8* pvStringPt, ...)
{
    MSGBUF* pstrMsgBuf;
    va_list args;

    /* ����buf, ���������Ҫ��ӡ���ַ� */
    pstrMsgBuf = (MSGBUF*)DEV_BufferAlloc(&gstrBufPool);
    if(NULL == pstrMsgBuf)
    {
        return;
    }

    /* ���ַ�����ӡ���ڴ� */
    va_start(args, pvStringPt);
    (void)vsprintf(pstrMsgBuf->aucBuf, pvStringPt, args);
    va_end(args);

    /* ���buf���� */
    pstrMsgBuf->ucLength = strlen(pstrMsgBuf->aucBuf);

    /* ��buf������� */
    (void)MDS_QuePut(gpstrSerialMsgQue, &pstrMsgBuf->strList);
}

/***********************************************************************************
��������: �򴮿ڴ�ӡ��Ϣ. ���ڴ���ָ����ַ�򴮿�������ӡһ�����ȵ�����, ���ô��ڲ�ѯ
          ��ʽ��ӡ.
��ڲ���: pucBuf: ��Ҫ��ӡ���ݴ�ŵĻ����ַ.
          uiLen: ��Ҫ��ӡ���ݵĳ���, ��λ: �ֽ�.
�� �� ֵ: none.
***********************************************************************************/
void DEV_PrintMsg(U8* pucBuf, U32 uiLen)
{
    U32 uiCounter;

    uiCounter = 0;

    /* ���ò�ѯ�ķ�ʽ�������� */
    while(uiCounter < uiLen)
    {
        /* �򴮿ڷ�һ���ַ� */
        (void)DEV_PutChar(pucBuf[uiCounter++]);
    }
}

/***********************************************************************************
��������: ��ʼ��LCDҺ����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdInit(void)
{
    U8 ucHwVer;

    /* ��ȡӲ���汾�� */
    ucHwVer = DEV_ReadHardwareVersion();

    DEV_LcdGpioCfg();
    DEV_LcdFsmcCfg();
    DEV_LcdReset();
    DEV_LcdLightOn();

    DEV_LcdWriteCmd(0x11);      //SLP OUT
    DEV_DelayMs(1);

    DEV_LcdWriteCmd(0x3A);      //Set_pixel_format
    DEV_LcdWriteData(0x55);     //0x55: 16bit/pixel, 65k; 0x66: 18bit/pixel, 262k;

    DEV_LcdWriteCmd(0x36);      //Set_address_mode

    if(0 == ucHwVer)            //Һ����ʹ��S6D04D1X21������
    {
        DEV_LcdWriteData(0x78);
    }
    else if(1 == ucHwVer)       //Һ����ʹ��HX8352C������
    {
        DEV_LcdWriteData(0x3A);
        DEV_LcdWriteCmd(0x55);
    }

    DEV_LcdWriteCmd(0x29);      //Set_display_on
    DEV_DelayMs(1);
}

/***********************************************************************************
��������: ��ʼ������LCD��GPIO.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdGpioCfg(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /*  LED��, RESET����ܽ� */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_13;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* FLM����ܽ� */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/***********************************************************************************
��������: ��ʼ��LCDʹ�õ�FSMC�ܽź�FSMCʱ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdFsmcCfg(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef p;

    /* Enable FSMC pin Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE
                           | RCC_APB2Periph_AFIO, ENABLE);

    /* config tft data lines base on FSMC data lines,
     * FSMC-D0~D15: PD 0 1 4 5 7 8 9 10 11 14 15, PE 7 8 9 10 11 12 13 14 15
     */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7
                                  | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_14
                                  | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11
                                  | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* Enable the FSMC Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

    p.FSMC_AddressSetupTime = 0x02;
    p.FSMC_AddressHoldTime = 0x00;
    p.FSMC_DataSetupTime = 0x05;
    p.FSMC_BusTurnAroundDuration = 0x00;
    p.FSMC_CLKDivision = 0x00;
    p.FSMC_DataLatency = 0x00;

    p.FSMC_AccessMode = FSMC_AccessMode_B;

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

    /* Enable FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

/***********************************************************************************
��������: ��λLCDҺ����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdReset(void)
{
    GPIO_ResetBits(GPIOD, GPIO_Pin_13);
    DEV_DelayMs(1);
    GPIO_SetBits(GPIOD, GPIO_Pin_13);
    DEV_DelayMs(1);
}

/***********************************************************************************
��������: ����LCDҺ���������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdLightOn(void)
{
    /* ��LCD����� */
    GPIO_ResetBits(GPIOD, GPIO_Pin_3);
}

/***********************************************************************************
��������: �ر�LCDҺ���������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdLightOff(void)
{
    /* �ر�LCD����� */
    GPIO_SetBits(GPIOD, GPIO_Pin_3);
}

/***********************************************************************************
��������: ��LCDд����.
��ڲ���: usCmd: д�������.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdWriteCmd(U16 usCmd)
{
    *(volatile U16*)(FSMCCMDADDR) = usCmd;
}

/***********************************************************************************
��������: ��LCDд����.
��ڲ���: usData: д�������.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdWriteData(U16 usData)
{
    *(volatile U16*)(FSMCDATAADDR) = usData;
}

/***********************************************************************************
��������: �趨LCDһ������, ����д�������������д������, ��λ: ����.
��ڲ���: usXStart: X�Ὺʼ�����.
          usXEnd: X����������.
          usYStart: Y�Ὺʼ�����.
          usYEnd: Y����������.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdSetBlock(U16 usXStart, U16 usXEnd, U16 usYStart, U16 usYEnd)
{
    /* �趨X�᷶Χ */
    DEV_LcdWriteCmd(0x2B);
    DEV_LcdWriteData(usXStart >> 8);
    DEV_LcdWriteData(usXStart & 0xFF);
    DEV_LcdWriteData(usXEnd >> 8);
    DEV_LcdWriteData(usXEnd & 0xFF);

    /* �趨Y�᷶Χ */
    DEV_LcdWriteCmd(0x2A);
    DEV_LcdWriteData(usYStart >> 8);
    DEV_LcdWriteData(usYStart & 0xFF);
    DEV_LcdWriteData(usYEnd >> 8);
    DEV_LcdWriteData(usYEnd & 0xFF);

    /* д��д������, ����д������ݻ���ʾ������ */
    DEV_LcdWriteCmd(0x2C);
}

/***********************************************************************************
��������: ��LCD��ָ����λ��������ͼ��, ʹ�û�����ɫ, ��λ: ����.
��ڲ���: usXStart: X�Ὺʼ�����.
          usYStart: Y�Ὺʼ�����.
          usXLen: Y�᳤��.
          usYLen: Y�᳤��.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdDrawBox(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen)
{
    U32 i;

    /* �趨д��ķ�Χ */
    DEV_LcdSetBlock(usXStart, usXStart + usXLen - 1, usYStart, usYStart + usYLen - 1);

    /* д������ */
    for(i = 0; i < usXLen * usYLen; i++)
    {
        *(volatile U16*)(FSMCDATAADDR) = LCDGETPENCOLOR;
    }
}

/***********************************************************************************
��������: ��X�᷽��һ����X��ƽ�е�ֱ��, ʹ�û�����ɫ, ��λ: ����.
��ڲ���: usXStart: X�Ὺʼ�����.
          usYStart: Y�Ὺʼ�����, Ϊ�߿�ȵ��м���.
          usLen: ����, X�᷽��.
          usWid: ���, Y�᷽��, Ӧ��������.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdDrawXLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid)
{
    DEV_LcdDrawBox(usXStart, usYStart - usWid / 2, usLen, usWid);
}

/***********************************************************************************
��������: ��Y�᷽��һ����Y��ƽ�е�ֱ��, ʹ�û�����ɫ, ��λ: ����.
��ڲ���: usXStart: X�Ὺʼ�����, Ϊ�߿�ȵ��м���.
          usYStart: Y�Ὺʼ�����.
          usLen: ����, Y�᷽��.
          usWid: ���, X�᷽��, Ӧ��������.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdDrawYLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid)
{
    DEV_LcdDrawBox(usXStart - usWid / 2, usYStart, usWid, usLen);
}

/***********************************************************************************
��������: ��LCD��ָ����λ������ַ�, �����Ǿ��ε�ͼ��, X�������8�ı���, ��λ: ����.
��ڲ���: usXStart: X�Ὺʼ�����.
          usYStart: Y�Ὺʼ�����.
          usXLen: Y�᳤��.
          usYLen: Y�᳤��.
          pucPic: ���ͼ����ŵ��ڴ��ַ, �ڴ��е�1����ʹ�û�����ɫ, 0���ֱ���ɫ.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdDrawTxt(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen, U8* pucPic)
{
    U32 i;
    S32 j;

    /* �趨д��ķ�Χ */
    DEV_LcdSetBlock(usXStart, usXStart + usXLen - 1, usYStart, usYStart + usYLen - 1);

    /* ���ֽ�д������ */
    for(i = 0; i < usXLen * usYLen / 8; i++)
    {
        /* �ж��ֽ���ÿ��bit */
        for(j = 7; j >= 0; j--)
        {
            /* ֻ��bitΪ1��д�뻭����ɫ */
            if(0x01 == ((pucPic[i] >> j) & 0x01))
            {
                *(volatile U16*)(FSMCDATAADDR) = LCDGETPENCOLOR;
            }
            else /* д�뱳��ɫ */
            {
                *(volatile U16*)(FSMCDATAADDR) = LCDGETBACKGROUNDCOLOR;
            }
        }
    }
}

/***********************************************************************************
��������: ��ʼ����Ϣ�����.
��ڲ���: pstrBufPool: ��Ϣ�����ָ��.
�� �� ֵ: none.
***********************************************************************************/
void DEV_BufferInit(BUFPOOL* pstrBufPool)
{
    U32 i;

    /* ��ʼ������ؿ������� */
    MDS_DlistInit(&pstrBufPool->strFreeList);

    /* ����������ź���, ���ڱ�֤�Ի��������Ĵ��в��� */
    (void)MDS_SemCreate(&pstrBufPool->strSem, SEMBIN | SEMPRIO, SEMFULL);

    /* �������������������� */
    for(i = 0; i < BUFPOOLNUM; i++)
    {
        MDS_DlistNodeAdd(&pstrBufPool->strFreeList,
                         &pstrBufPool->astrBufPool[i].strList);
    }
}

/***********************************************************************************
��������: ����Ϣ�������������Ϣ����.
��ڲ���: pstrBufPool: ��Ϣ�����ָ��.
�� �� ֵ: �������Ϣ����ָ��.
***********************************************************************************/
MSGBUF* DEV_BufferAlloc(BUFPOOL* pstrBufPool)
{
    MSGBUF* pstrBuf;

    /* ���ж��������ڴ� */
    if(RTN_SUCD == MDS_RunInInt())
    {
        /* �ӿ����������һ������ */
        pstrBuf = (MSGBUF*)MDS_DlistNodeDelete(&pstrBufPool->strFreeList);
    }
    else /* �������������ڴ� */
    {
        /* ��ȡ�ź���, ��֤�Ի��������Ĵ��в��� */
        (void)MDS_SemTake(&pstrBufPool->strSem, SEMWAITFEV);

        /* �ӿ����������һ������ */
        pstrBuf = (MSGBUF*)MDS_DlistNodeDelete(&pstrBufPool->strFreeList);

        /* �ͷ��ź��� */
        (void)MDS_SemGive(&pstrBufPool->strSem);
    }

    return pstrBuf;
}

/***********************************************************************************
��������: �ͷŽ���Ϣ�����ͷŻ���Ϣ�����.
��ڲ���: pstrBufPool: ��Ϣ�����ָ��.
          pstrQueNode: �ͷŵĻ���ڵ�ָ��.
�� �� ֵ: none.
***********************************************************************************/
void DEV_BufferFree(BUFPOOL* pstrBufPool, M_DLIST* pstrQueNode)
{
    /* ��ȡ�ź���, ��֤�Ի��������Ĵ��в��� */
    (void)MDS_SemTake(&pstrBufPool->strSem, SEMWAITFEV);

    /* ���ͷŵĻ���ҵ��������� */
    MDS_DlistNodeAdd(&pstrBufPool->strFreeList, pstrQueNode);

    /* �ͷ��ź��� */
    (void)MDS_SemGive(&pstrBufPool->strSem);
}

/***********************************************************************************
��������: ��ȡ���ͼ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
U32 DEV_GetRandomFig(void)
{
    static U32 i = 0;

    i += (MDS_GetSystemTick() + 3);

    /* �������ͼ�� */
    return (i % FIGNUM);
}

/***********************************************************************************
��������: ��ʼ��Ӳ���汾��IO.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_HardwareVersionInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* ����Ӳ���汾��IO�Ĺܽ�Ϊ����״̬, ������ */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/***********************************************************************************
��������: ��ȡӲ���汾��.
��ڲ���: none.
�� �� ֵ: Ӳ���汾��, 0~7.
***********************************************************************************/
U8 DEV_ReadHardwareVersion(void)
{
    U16 usHwVer;

    /* ��ȡӲ���汾��IO */
    usHwVer = GPIO_ReadInputData(GPIOC);

    /* PC12~PC10��ӦӲ���汾�ŵ�3��IO */
    return (usHwVer >> 10) & 0x07;
}

