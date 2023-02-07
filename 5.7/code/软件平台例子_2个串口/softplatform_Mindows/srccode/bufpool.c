
#include <stdlib.h>
#include "bufpool.h"


U32 gabuf_len[] = BUF_CONFIG_VAL;
BUF_POOL gabuf_pool[BUF_TYPE_NUM];  /* 缓冲池*/


/***********************************************************************************
函数功能: 初始化消息缓冲池.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void buf_pool_init(void)
{
    U32 i;

    /* 检查缓冲长度是否是按照递增的方式排列 */
    if(BUF_TYPE_NUM > 1)
    {
        for(i = 0; i < BUF_TYPE_NUM - 1; i++)
        {
            /* 不是按照递增方式排列, 退出 */
            if(gabuf_len[i] > gabuf_len[i + 1])
            {
                return;
            }
        }
    }

    /* 初始化缓冲池 */
    for(i = 0; i < BUF_TYPE_NUM; i++)
    {
        /* 初始化每种缓冲的链表根节点 */
        slist_init(&gabuf_pool[i].list);

        /* 初始化每种缓冲的信号量 */
        gabuf_pool[i].psem = MDS_SemCreate(NULL, SEMCNT | SEMPRIO, SEMFULL);

        /* 初始化缓冲长度及统计值 */
        gabuf_pool[i].len = gabuf_len[i];
        gabuf_pool[i].s_num = 0;
        gabuf_pool[i].d_num = 0;
    }
}

/***********************************************************************************
函数功能: 从消息缓冲池申请缓冲.
入口参数: len: 申请的缓冲的长度, 单位: 字节. 不包含BUF_NODE部分的长度.
返 回 值: 申请的缓冲指针, 若申请不到缓冲则返回NULL.
***********************************************************************************/
BUF_NODE* buf_malloc(U32 len)
{
    BUF_POOL* pbuf_pool;
    BUF_NODE* pnode;
    U32 i;
    U32 j;

    /* 从缓冲池中寻找能容纳下申请长度的缓冲 */
    for(i = 0; i < BUF_TYPE_NUM; i++)
    {
        if(len <= gabuf_len[i])
        {
            pbuf_pool = &gabuf_pool[i];

            break;
        }
    }

    /* 超出所支持的缓冲长度范围 */
    if(i >= BUF_TYPE_NUM)
    {
        print_msg(PRINT_IMPORTANT, PRINT_BUF,
                  "\r\ncan't support length of %d buf, max length is %d. (%d, %s)",
                  len, BUF_MAX_LEN, __LINE__, __FILE__);

        return NULL;
    }

    /* 获取信号量, 保证链表操作的串行性 */
    if(RTN_SUCD != MDS_SemTake(pbuf_pool->psem, SEMWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_BUF,
                  "\r\nbuf length %d wait sem failed! buf pool is 0x%x. (%d, %s)",
                  len, pbuf_pool, __LINE__, __FILE__);

        return NULL;
    }

    /* 从缓冲链表拆除一个缓冲节点 */
    pnode = (BUF_NODE*)slist_node_delete(&pbuf_pool->list);

    /* 从链表上申请到内存 */
    if(NULL != pnode)
    {
        /* 增加从链表动态申请的计数统计 */
        pbuf_pool->d_num++;

        print_msg(PRINT_SUGGEST, PRINT_BUF,
                  "\r\nmalloc %d bytes 0x%x from buf pool successfully. (%d, %s)",
                  len, pnode, __LINE__, __FILE__);
    }
    /* 如果链表上没有可用的缓冲则使用malloc函数申请一块缓冲 */
    else
    {
        /* 从系统申请内存成功 */
        if(NULL != (pnode = (BUF_NODE*)malloc(gabuf_len[i] + sizeof(BUF_NODE))))
        {
            /* 增加从系统静态申请的计数统计 */
            pbuf_pool->s_num++;

            /* 增加从链表动态申请的计数统计 */
            pbuf_pool->d_num++;

            /* 填充申请到的缓冲的链表根节点指针 */
            pnode->phead = pbuf_pool;

            print_msg(PRINT_SUGGEST, PRINT_BUF,
                      "\r\nmalloc %d(+%d) bytes 0x%x from system successfully."
                      " (%d, %s)",
                      gabuf_len[i], sizeof(BUF_NODE), pnode, __LINE__, __FILE__);
        }
        else /*  从系统申请内存失败 */
        {
            print_msg(PRINT_IMPORTANT, PRINT_BUF,
                      "\r\nmalloc %d(+%d) bytes from system failed. (%d, %s)",
                      gabuf_len[i], sizeof(BUF_NODE), __LINE__, __FILE__);

            /* 打印已申请的缓冲 */
            for(j = 0; j < BUF_TYPE_NUM; j++)
            {
                print_msg(PRINT_IMPORTANT, PRINT_BUF,
                          "\r\nalready malloc %d buf: length is %d(+%d). (%s, %d)",
                          gabuf_pool[j].s_num, gabuf_pool[j].len, sizeof(BUF_NODE),
                          __FILE__, __LINE__);
            }

        }
    }

    /* 释放信号量 */
    if(RTN_SUCD != MDS_SemGive(pbuf_pool->psem))
    {
        print_msg(PRINT_IMPORTANT, PRINT_BUF,
                  "\r\nbuf length %d post sem failed! buf pool is 0x%x. (%d, %s)",
                  len, pbuf_pool, __LINE__, __FILE__);

        return NULL;
    }

    return pnode;
}

/***********************************************************************************
函数功能: 释放消息缓冲. 将消息缓冲挂入对应的链表, 供下次申请使用.
入口参数: pbuf: 释放的缓冲指针.
返 回 值: none.
***********************************************************************************/
void buf_free(BUF_NODE* pbuf)
{
    U32 len;
    U32 i;

    if(NULL == pbuf)
    {
        return;
    }

    /* 获取需要释放的缓冲所在缓冲池中的长度 */
    len = pbuf->phead->len;

    /* 从缓冲池中寻找与所释放缓冲长度相等的缓冲 */
    for(i = 0; i < BUF_TYPE_NUM; i++)
    {
        if(len == gabuf_len[i])
        {
            break;
        }
    }

    /* 缓冲长度不符, 无法释放 */
    if(i >= BUF_TYPE_NUM)
    {
        print_msg(PRINT_IMPORTANT, PRINT_BUF,
                  "\r\ncan't free the buf of %d length. (%d, %s)", len,
                  __LINE__, __FILE__);

        return;
    }

    /* 对缓冲链表操作前获取信号量, 保证链表操作的串行性 */
    if(RTN_SUCD != MDS_SemTake(gabuf_pool[i].psem, SEMWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_BUF,
                  "\r\nbuf length %d wait sem failed. (%d, %s)", len,
                  __LINE__, __FILE__);

        return;
    }

    /* 将释放的缓冲加入到缓冲链表中 */
    slist_node_add((SLIST*)&gabuf_pool[i].list, (SLIST*)&pbuf->list);

    gabuf_pool[i].d_num--;

    /* 释放信号量 */
    if(RTN_SUCD != MDS_SemGive(gabuf_pool[i].psem))
    {
        print_msg(PRINT_IMPORTANT, PRINT_BUF,
                  "\r\nbuf length %d post sem failed! error is 0x%x. (%d, %s)",
                  len, __LINE__, __FILE__);
    }

    print_msg(PRINT_SUGGEST, PRINT_BUF,
              "\r\nbuf length %d free successfully. (%d, %s)", len,
              __LINE__, __FILE__);
}

/***********************************************************************************
函数功能: 从消息缓冲池申请缓冲, 不打印调试消息.
入口参数: len: 申请的缓冲的长度, 单位: 字节. 不包含BUF_NODE部分的长度.
返 回 值: 申请的缓冲指针, 若申请不到缓冲则返回NULL.
***********************************************************************************/
BUF_NODE* buf_malloc_without_print(U32 len)
{
    BUF_POOL* pbuf_pool;
    BUF_NODE* pnode;
    U32 i;

    /* 从缓冲池中寻找能容纳下申请长度的缓冲 */
    for(i = 0; i < BUF_TYPE_NUM; i++)
    {
        if(len <= gabuf_len[i])
        {
            pbuf_pool = &gabuf_pool[i];

            break;
        }
    }

    /* 超出所支持的缓冲长度范围 */
    if(i >= BUF_TYPE_NUM)
    {
        return NULL;
    }

    /* 获取信号量, 保证链表操作的串行性 */
    if(RTN_SUCD != MDS_SemTake(pbuf_pool->psem, SEMWAITFEV))
    {
        return NULL;
    }

    /* 从缓冲链表拆除一个缓冲节点 */
    pnode = (BUF_NODE*)slist_node_delete(&pbuf_pool->list);

    /* 从链表上申请到内存 */
    if(NULL != pnode)
    {
        /* 增加从链表动态申请的计数统计 */
        pbuf_pool->d_num++;
    }
    /* 如果链表上没有可用的缓冲则使用malloc函数申请一块缓冲 */
    else
    {
        /* 从系统申请内存成功 */
        if(NULL != (pnode = (BUF_NODE*)malloc(gabuf_len[i] + sizeof(BUF_NODE))))
        {
            /* 增加从系统静态申请的计数统计 */
            pbuf_pool->s_num++;

            /* 增加从链表动态申请的计数统计 */
            pbuf_pool->d_num++;

            /* 填充申请到的缓冲的链表根节点指针 */
            pnode->phead = pbuf_pool;
        }
    }

    /* 释放信号量 */
    if(RTN_SUCD != MDS_SemGive(pbuf_pool->psem))
    {
        return NULL;
    }

    return pnode;
}

/***********************************************************************************
函数功能: 释放消息缓冲. 将消息缓冲挂入对应的链表, 供下次申请使用. 不打印调试消息.
入口参数: pbuf: 释放的缓冲指针.
返 回 值: none.
***********************************************************************************/
void buf_free_without_print(BUF_NODE* pbuf)
{
    U32 len;
    U32 i;

    if(NULL == pbuf)
    {
        return;
    }

    /* 获取需要释放的缓冲所在缓冲池中的长度 */
    len = pbuf->phead->len;

    /* 从缓冲池中寻找与所释放缓冲长度相等的缓冲 */
    for(i = 0; i < BUF_TYPE_NUM; i++)
    {
        if(len == gabuf_len[i])
        {
            break;
        }
    }

    /* 缓冲长度不符, 无法释放 */
    if(i >= BUF_TYPE_NUM)
    {
        return;
    }

    /* 对缓冲链表操作前获取信号量, 保证链表操作的串行性 */
    if(RTN_SUCD != MDS_SemTake(gabuf_pool[i].psem, SEMWAITFEV))
    {
        return;
    }

    /* 将释放的缓冲加入到缓冲链表中 */
    slist_node_add((SLIST*)&gabuf_pool[i].list, (SLIST*)&pbuf->list);

    gabuf_pool[i].d_num--;

    /* 释放信号量 */
    (void)MDS_SemGive(gabuf_pool[i].psem);
}

/***********************************************************************************
函数功能: 初始化单向链表.
入口参数: plist: 链表根节点指针.
返 回 值: none.
***********************************************************************************/
void slist_init(SLIST* plist)
{
    plist->next = (SLIST*)NULL;
}

/***********************************************************************************
函数功能: 向单向链表添加一个节点, 添加到根节点之后.
入口参数: plist: 链表根节点指针.
          pnode: 加入的节点指针.
返 回 值: none.
***********************************************************************************/
void slist_node_add(SLIST* plist, SLIST* pnode)
{
    pnode->next = plist->next;  /* 根节点指向新节点 */
    plist->next = pnode;        /* 新节点指向根节点原来指向的节点 */
}

/***********************************************************************************
函数功能: 从单向链表删除一个节点, 从根节点之后删除.
入口参数: plist: 链表根节点指针.
返 回 值: 删除的节点指针, 若链表为空则返回NULL.
***********************************************************************************/
SLIST* slist_node_delete(SLIST* plist)
{
    SLIST* ptemp;

    ptemp = plist->next;

    /* 如果链表不为空, 将根节点指向被删除节点之后的节点 */
    if(NULL != plist->next)
    {
        plist->next = plist->next->next;
    }

    /* 返回被删除的节点 */
    return ptemp;
}

