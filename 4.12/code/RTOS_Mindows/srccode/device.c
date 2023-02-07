
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "device.h"


BUFPOOL gstrBufPool;                /* 消息缓冲池 */

U16 gusPenColor;                    /* 画笔颜色 */
U16 gusBackGroundColor;             /* 背景色 */
/***********************************************************************************
函数功能: 初始化软件.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_SoftwareInit(void)
{
    /* 初始化消息缓冲池 */
    DEV_BufferInit(&gstrBufPool);

    /* 初始化队列 */
    gpstrSerialMsgQue = MDS_QueCreate((M_QUE*)NULL, QUEPRIO);   /* 创建打印消息队列 */
    gpstrKeyMsgQue = MDS_QueCreate((M_QUE*)NULL, QUEPRIO);      /* 创建按键消息队列 */

    /* 初始化信号量 */
    gpstrFigSem = MDS_SemCreate((M_SEM*)NULL, SEMBIN | SEMPRIO, SEMFULL);

    /* 游戏状态 */
    gucGameSta = GAMESTA_STOP;

#ifdef MDS_INCLUDETASKHOOK

    /* 挂接任务钩子函数 */
    MDS_TaskCreateHookAdd(TEST_TaskCreatePrint);
    MDS_TaskSwitchHookAdd(TEST_TaskSwitchPrint);
    MDS_TaskDeleteHookAdd(TEST_TaskDeletePrint);

#endif
}

/***********************************************************************************
函数功能: 初始化硬件.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_HardwareInit(void)
{
    /* 初始化硬件版本号IO */
    DEV_HardwareVersionInit();

    /* 初始化串口 */
    DEV_UartInit();

    /* 初始化按键 */
    DEV_KeyConfiguration();

    /* 初始化LCD液晶屏 */
    DEV_LcdInit();
}

/***********************************************************************************
函数功能: 初始化按键.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_KeyConfiguration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* 配置按键的管脚为输入状态, 弱上拉 */
    GPIO_InitStructure.GPIO_Pin = IOKEYUP | IOKEYDOWN | IOKEYLEFT | IOKEYRIGHT  | IOKEYRESTART;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/***********************************************************************************
函数功能: 读取按键值.
入口参数: none.
返 回 值: KEYNULL: 没有按键响应.
          其它值: 每bit对应一个按键.
                  KEYUP: 向上按键.
                  KEYDOWN: 向下按键.
                  KEYLEFT: 向左按键.
                  KEYRIGHT: 向右按键.
                  KEYRESTART: 重新开始按键.
***********************************************************************************/
U8 DEV_ReadKey(void)
{
    U8 ucKey;
    U16 usTemp;

    ucKey = KEYNULL;

    /* 读取按键对应的GPIO管脚 */
    usTemp = GPIO_ReadInputData(GPIOB);

    /* 判断按键是否被按下, 按下状态为0, 未按下状态为1 */
    if(0 == (((usTemp & IOKEYUP) >> KEYUPOFFSET) & 1))
    {
        /* 向上按键被按下 */
        ucKey |= KEYUP;
    }

    if(0 == (((usTemp & IOKEYDOWN) >> KEYDOWNOFFSET) & 1))
    {
        /* 向下按键被按下 */
        ucKey |= KEYDOWN;
    }

    if((0 == (((usTemp & IOKEYLEFT) >> KEYLEFTOFFSET) & 1)))
    {
        /* 向左按键被按下 */
        ucKey |= KEYLEFT;
    }

    if((0 == (((usTemp & IOKEYRIGHT) >> KEYRIGHTOFFSET) & 1)))
    {
        /* 向右按键被按下 */
        ucKey |= KEYRIGHT;
    }

    if(0 == (((usTemp & IOKEYRESTART) >> KEYRESTARTOFFSET) & 1))
    {
        /* 重新开始按键被按下 */
        ucKey |= KEYRESTART;
    }

    return ucKey;
}

/***********************************************************************************
函数功能: 初始化串口.
入口参数: none.
返 回 值: none.
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
函数功能: 向串口发送一个字符.
入口参数: ucChar: 输出的字符.
返 回 值: none.
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
函数功能: 将字符串打印到内存. 该函数入口参数与printf函数一致.
入口参数: pvStringPt: 第一个参数的指针, 必须为字符串指针.
          ...: 其它参数.
返 回 值: none.
***********************************************************************************/
void DEV_PutStrToMem(U8* pvStringPt, ...)
{
    MSGBUF* pstrMsgBuf;
    va_list args;

    /* 申请buf, 用来存放需要打印的字符 */
    pstrMsgBuf = (MSGBUF*)DEV_BufferAlloc(&gstrBufPool);
    if(NULL == pstrMsgBuf)
    {
        return;
    }

    /* 将字符串打印到内存 */
    va_start(args, pvStringPt);
    (void)vsprintf(pstrMsgBuf->aucBuf, pvStringPt, args);
    va_end(args);

    /* 填充buf参数 */
    pstrMsgBuf->ucLength = strlen(pstrMsgBuf->aucBuf);

    /* 将buf挂入队列 */
    (void)MDS_QuePut(gpstrSerialMsgQue, &pstrMsgBuf->strList);
}

/***********************************************************************************
函数功能: 向串口打印消息. 从内存中指定地址向串口连续打印一定长度的数据, 采用串口查询
          方式打印.
入口参数: pucBuf: 需要打印数据存放的缓冲地址.
          uiLen: 需要打印数据的长度, 单位: 字节.
返 回 值: none.
***********************************************************************************/
void DEV_PrintMsg(U8* pucBuf, U32 uiLen)
{
    U32 uiCounter;

    uiCounter = 0;

    /* 采用查询的方式发送数据 */
    while(uiCounter < uiLen)
    {
        /* 向串口发一个字符 */
        (void)DEV_PutChar(pucBuf[uiCounter++]);
    }
}

/***********************************************************************************
函数功能: 初始化LCD液晶屏.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_LcdInit(void)
{
    U8 ucHwVer;

    /* 读取硬件版本号 */
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

    if(0 == ucHwVer)            //液晶屏使用S6D04D1X21驱动器
    {
        DEV_LcdWriteData(0x78);
    }
    else if(1 == ucHwVer)       //液晶屏使用HX8352C驱动器
    {
        DEV_LcdWriteData(0x3A);
        DEV_LcdWriteCmd(0x55);
    }

    DEV_LcdWriteCmd(0x29);      //Set_display_on
    DEV_DelayMs(1);
}

/***********************************************************************************
函数功能: 初始化控制LCD的GPIO.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_LcdGpioCfg(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /*  LED灯, RESET输出管脚 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_13;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* FLM输入管脚 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/***********************************************************************************
函数功能: 初始化LCD使用的FSMC管脚和FSMC时序.
入口参数: none.
返 回 值: none.
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
函数功能: 复位LCD液晶屏.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_LcdReset(void)
{
    GPIO_ResetBits(GPIOD, GPIO_Pin_13);
    DEV_DelayMs(1);
    GPIO_SetBits(GPIOD, GPIO_Pin_13);
    DEV_DelayMs(1);
}

/***********************************************************************************
函数功能: 点亮LCD液晶屏背光灯.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_LcdLightOn(void)
{
    /* 打开LCD背光灯 */
    GPIO_ResetBits(GPIOD, GPIO_Pin_3);
}

/***********************************************************************************
函数功能: 关闭LCD液晶屏背光灯.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_LcdLightOff(void)
{
    /* 关闭LCD背光灯 */
    GPIO_SetBits(GPIOD, GPIO_Pin_3);
}

/***********************************************************************************
函数功能: 向LCD写命令.
入口参数: usCmd: 写入的命令.
返 回 值: none.
***********************************************************************************/
void DEV_LcdWriteCmd(U16 usCmd)
{
    *(volatile U16*)(FSMCCMDADDR) = usCmd;
}

/***********************************************************************************
函数功能: 向LCD写数据.
入口参数: usData: 写入的数据.
返 回 值: none.
***********************************************************************************/
void DEV_LcdWriteData(U16 usData)
{
    *(volatile U16*)(FSMCDATAADDR) = usData;
}

/***********************************************************************************
函数功能: 设定LCD一块区域, 后续写操作将向该区域写入数据, 单位: 像素.
入口参数: usXStart: X轴开始坐标点.
          usXEnd: X轴结束坐标点.
          usYStart: Y轴开始坐标点.
          usYEnd: Y轴结束坐标点.
返 回 值: none.
***********************************************************************************/
void DEV_LcdSetBlock(U16 usXStart, U16 usXEnd, U16 usYStart, U16 usYEnd)
{
    /* 设定X轴范围 */
    DEV_LcdWriteCmd(0x2B);
    DEV_LcdWriteData(usXStart >> 8);
    DEV_LcdWriteData(usXStart & 0xFF);
    DEV_LcdWriteData(usXEnd >> 8);
    DEV_LcdWriteData(usXEnd & 0xFF);

    /* 设定Y轴范围 */
    DEV_LcdWriteCmd(0x2A);
    DEV_LcdWriteData(usYStart >> 8);
    DEV_LcdWriteData(usYStart & 0xFF);
    DEV_LcdWriteData(usYEnd >> 8);
    DEV_LcdWriteData(usYEnd & 0xFF);

    /* 写入写屏命令, 后续写入的数据会显示在屏上 */
    DEV_LcdWriteCmd(0x2C);
}

/***********************************************************************************
函数功能: 在LCD中指定的位置填充矩形图案, 使用画笔颜色, 单位: 像素.
入口参数: usXStart: X轴开始坐标点.
          usYStart: Y轴开始坐标点.
          usXLen: Y轴长度.
          usYLen: Y轴长度.
返 回 值: none.
***********************************************************************************/
void DEV_LcdDrawBox(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen)
{
    U32 i;

    /* 设定写入的范围 */
    DEV_LcdSetBlock(usXStart, usXStart + usXLen - 1, usYStart, usYStart + usYLen - 1);

    /* 写入数据 */
    for(i = 0; i < usXLen * usYLen; i++)
    {
        *(volatile U16*)(FSMCDATAADDR) = LCDGETPENCOLOR;
    }
}

/***********************************************************************************
函数功能: 沿X轴方向画一条与X轴平行的直线, 使用画笔颜色, 单位: 像素.
入口参数: usXStart: X轴开始坐标点.
          usYStart: Y轴开始坐标点, 为线宽度的中间线.
          usLen: 长度, X轴方向.
          usWid: 宽度, Y轴方向, 应该是奇数.
返 回 值: none.
***********************************************************************************/
void DEV_LcdDrawXLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid)
{
    DEV_LcdDrawBox(usXStart, usYStart - usWid / 2, usLen, usWid);
}

/***********************************************************************************
函数功能: 沿Y轴方向画一条与Y轴平行的直线, 使用画笔颜色, 单位: 像素.
入口参数: usXStart: X轴开始坐标点, 为线宽度的中间线.
          usYStart: Y轴开始坐标点.
          usLen: 长度, Y轴方向.
          usWid: 宽度, X轴方向, 应该是奇数.
返 回 值: none.
***********************************************************************************/
void DEV_LcdDrawYLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid)
{
    DEV_LcdDrawBox(usXStart - usWid / 2, usYStart, usWid, usLen);
}

/***********************************************************************************
函数功能: 在LCD中指定的位置填充字符, 必须是矩形的图案, X轴必须是8的倍数, 单位: 像素.
入口参数: usXStart: X轴开始坐标点.
          usYStart: Y轴开始坐标点.
          usXLen: Y轴长度.
          usYLen: Y轴长度.
          pucPic: 填充图案存放的内存地址, 内存中的1代表使用画笔颜色, 0保持背景色.
返 回 值: none.
***********************************************************************************/
void DEV_LcdDrawTxt(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen, U8* pucPic)
{
    U32 i;
    S32 j;

    /* 设定写入的范围 */
    DEV_LcdSetBlock(usXStart, usXStart + usXLen - 1, usYStart, usYStart + usYLen - 1);

    /* 按字节写入数据 */
    for(i = 0; i < usXLen * usYLen / 8; i++)
    {
        /* 判断字节中每个bit */
        for(j = 7; j >= 0; j--)
        {
            /* 只有bit为1才写入画笔颜色 */
            if(0x01 == ((pucPic[i] >> j) & 0x01))
            {
                *(volatile U16*)(FSMCDATAADDR) = LCDGETPENCOLOR;
            }
            else /* 写入背景色 */
            {
                *(volatile U16*)(FSMCDATAADDR) = LCDGETBACKGROUNDCOLOR;
            }
        }
    }
}

/***********************************************************************************
函数功能: 初始化消息缓冲池.
入口参数: pstrBufPool: 消息缓冲池指针.
返 回 值: none.
***********************************************************************************/
void DEV_BufferInit(BUFPOOL* pstrBufPool)
{
    U32 i;

    /* 初始化缓冲池空闲链表 */
    MDS_DlistInit(&pstrBufPool->strFreeList);

    /* 创建缓冲池信号量, 用于保证对缓冲池链表的串行操作 */
    (void)MDS_SemCreate(&pstrBufPool->strSem, SEMBIN | SEMPRIO, SEMFULL);

    /* 将各个缓冲挂入空闲链表 */
    for(i = 0; i < BUFPOOLNUM; i++)
    {
        MDS_DlistNodeAdd(&pstrBufPool->strFreeList,
                         &pstrBufPool->astrBufPool[i].strList);
    }
}

/***********************************************************************************
函数功能: 从消息缓冲池中申请消息缓冲.
入口参数: pstrBufPool: 消息缓冲池指针.
返 回 值: 分配的消息缓冲指针.
***********************************************************************************/
MSGBUF* DEV_BufferAlloc(BUFPOOL* pstrBufPool)
{
    MSGBUF* pstrBuf;

    /* 在中断中申请内存 */
    if(RTN_SUCD == MDS_RunInInt())
    {
        /* 从空闲链表分配一个缓冲 */
        pstrBuf = (MSGBUF*)MDS_DlistNodeDelete(&pstrBufPool->strFreeList);
    }
    else /* 在任务中申请内存 */
    {
        /* 获取信号量, 保证对缓冲池链表的串行操作 */
        (void)MDS_SemTake(&pstrBufPool->strSem, SEMWAITFEV);

        /* 从空闲链表分配一个缓冲 */
        pstrBuf = (MSGBUF*)MDS_DlistNodeDelete(&pstrBufPool->strFreeList);

        /* 释放信号量 */
        (void)MDS_SemGive(&pstrBufPool->strSem);
    }

    return pstrBuf;
}

/***********************************************************************************
函数功能: 释放将消息缓冲释放回消息缓冲池.
入口参数: pstrBufPool: 消息缓冲池指针.
          pstrQueNode: 释放的缓冲节点指针.
返 回 值: none.
***********************************************************************************/
void DEV_BufferFree(BUFPOOL* pstrBufPool, M_DLIST* pstrQueNode)
{
    /* 获取信号量, 保证对缓冲池链表的串行操作 */
    (void)MDS_SemTake(&pstrBufPool->strSem, SEMWAITFEV);

    /* 将释放的缓冲挂到空闲链表 */
    MDS_DlistNodeAdd(&pstrBufPool->strFreeList, pstrQueNode);

    /* 释放信号量 */
    (void)MDS_SemGive(&pstrBufPool->strSem);
}

/***********************************************************************************
函数功能: 获取随机图形.
入口参数: none.
返 回 值: none.
***********************************************************************************/
U32 DEV_GetRandomFig(void)
{
    static U32 i = 0;

    i += (MDS_GetSystemTick() + 3);

    /* 随机产生图形 */
    return (i % FIGNUM);
}

/***********************************************************************************
函数功能: 初始化硬件版本号IO.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_HardwareVersionInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* 配置硬件版本号IO的管脚为输入状态, 弱下拉 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/***********************************************************************************
函数功能: 读取硬件版本号.
入口参数: none.
返 回 值: 硬件版本号, 0~7.
***********************************************************************************/
U8 DEV_ReadHardwareVersion(void)
{
    U16 usHwVer;

    /* 读取硬件版本号IO */
    usHwVer = GPIO_ReadInputData(GPIOC);

    /* PC12~PC10对应硬件版本号的3个IO */
    return (usHwVer >> 10) & 0x07;
}

