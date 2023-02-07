
#include "message.h"


/***********************************************************************************
函数功能: 对接收的数据长度进行校验. 入口参数仅包含用户层数据域的长度, 整条消息的长度
          需要加上1字节消息头, 1字节消息尾, 1字节CRC, 1字节源地址, 1字节目的地址, 1
          字节命令, 4字节长度域, 共10字节.
入口参数: msg_len: 整条消息长度.
          data_len: 消息中数据域长度.
返 回 值: RTN_SUCD: 长度正确.
          RTN_FAIL: 长度超长.
***********************************************************************************/
U32 check_msg_len(U32 msg_len, U32 data_len)
{
    /* 消息中的长度小于等于缓冲的长度则返回正确 */
    if((USR_DATA_LEN_2_USR_TOTAL_LEN(data_len) <= GET_BUF_LEN(BUF_MAX_LEN))
       || (msg_len != USR_DATA_LEN_2_USR_TOTAL_LEN(data_len)))
    {
        return RTN_SUCD;
    }
    else
    {
        return RTN_FAIL;
    }
}

/***********************************************************************************
函数功能: 校验消息数据, 采用累加和校验.
入口参数: pbuf: 需要计算的数据的缓冲指针.
          len: 需要校验的数据长度, 单位: 字节.
返 回 值: 计算的校验和.
***********************************************************************************/
U8 calc_msg_crc(U8* pbuf, U32 len)
{
    U32 i;
    U8 checkout;

    checkout = 0;

    /* 采用数据相加的校验方式 */
    for(i = 0; i < len; i++)
    {
        checkout += pbuf[i];
    }

    return checkout;
}

/***********************************************************************************
函数功能: 校验消息地址.
入口参数: addr: 消息的目的地址.
返 回 值: RTN_SUCD: 地址正确.
          RTN_FAIL: 地址错误.
***********************************************************************************/
U8 check_msg_addr(U8 addr)
{
    if(LOCAL_ADDR == addr)
    {
        return RTN_SUCD;
    }
    else
    {
        return RTN_FAIL;
    }
}

