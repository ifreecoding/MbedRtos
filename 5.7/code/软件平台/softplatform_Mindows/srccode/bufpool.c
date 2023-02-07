
#include <stdlib.h>
#include "bufpool.h"


U32 gabuf_len[] = BUF_CONFIG_VAL;
BUF_POOL gabuf_pool[BUF_TYPE_NUM];  /* �����*/


/***********************************************************************************
��������: ��ʼ����Ϣ�����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void buf_pool_init(void)
{
    U32 i;

    /* ��黺�峤���Ƿ��ǰ��յ����ķ�ʽ���� */
    if(BUF_TYPE_NUM > 1)
    {
        for(i = 0; i < BUF_TYPE_NUM - 1; i++)
        {
            /* ���ǰ��յ�����ʽ����, �˳� */
            if(gabuf_len[i] > gabuf_len[i + 1])
            {
                return;
            }
        }
    }

    /* ��ʼ������� */
    for(i = 0; i < BUF_TYPE_NUM; i++)
    {
        /* ��ʼ��ÿ�ֻ����������ڵ� */
        slist_init(&gabuf_pool[i].list);

        /* ��ʼ��ÿ�ֻ�����ź��� */
        gabuf_pool[i].psem = MDS_SemCreate(NULL, SEMCNT | SEMPRIO, SEMFULL);

        /* ��ʼ�����峤�ȼ�ͳ��ֵ */
        gabuf_pool[i].len = gabuf_len[i];
        gabuf_pool[i].s_num = 0;
        gabuf_pool[i].d_num = 0;
    }
}

/***********************************************************************************
��������: ����Ϣ��������뻺��.
��ڲ���: len: ����Ļ���ĳ���, ��λ: �ֽ�. ������BUF_NODE���ֵĳ���.
�� �� ֵ: ����Ļ���ָ��, �����벻�������򷵻�NULL.
***********************************************************************************/
BUF_NODE* buf_malloc(U32 len)
{
    BUF_POOL* pbuf_pool;
    BUF_NODE* pnode;
    U32 i;
    U32 j;

    /* �ӻ������Ѱ�������������볤�ȵĻ��� */
    for(i = 0; i < BUF_TYPE_NUM; i++)
    {
        if(len <= gabuf_len[i])
        {
            pbuf_pool = &gabuf_pool[i];

            break;
        }
    }

    /* ������֧�ֵĻ��峤�ȷ�Χ */
    if(i >= BUF_TYPE_NUM)
    {
        print_msg(PRINT_IMPORTANT, PRINT_BUF,
                  "\r\ncan't support length of %d buf, max length is %d. (%d, %s)",
                  len, BUF_MAX_LEN, __LINE__, __FILE__);

        return NULL;
    }

    /* ��ȡ�ź���, ��֤��������Ĵ����� */
    if(RTN_SUCD != MDS_SemTake(pbuf_pool->psem, SEMWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_BUF,
                  "\r\nbuf length %d wait sem failed! buf pool is 0x%x. (%d, %s)",
                  len, pbuf_pool, __LINE__, __FILE__);

        return NULL;
    }

    /* �ӻ���������һ������ڵ� */
    pnode = (BUF_NODE*)slist_node_delete(&pbuf_pool->list);

    /* �����������뵽�ڴ� */
    if(NULL != pnode)
    {
        /* ���Ӵ�����̬����ļ���ͳ�� */
        pbuf_pool->d_num++;

        print_msg(PRINT_SUGGEST, PRINT_BUF,
                  "\r\nmalloc %d bytes 0x%x from buf pool successfully. (%d, %s)",
                  len, pnode, __LINE__, __FILE__);
    }
    /* ���������û�п��õĻ�����ʹ��malloc��������һ�黺�� */
    else
    {
        /* ��ϵͳ�����ڴ�ɹ� */
        if(NULL != (pnode = (BUF_NODE*)malloc(gabuf_len[i] + sizeof(BUF_NODE))))
        {
            /* ���Ӵ�ϵͳ��̬����ļ���ͳ�� */
            pbuf_pool->s_num++;

            /* ���Ӵ�����̬����ļ���ͳ�� */
            pbuf_pool->d_num++;

            /* ������뵽�Ļ����������ڵ�ָ�� */
            pnode->phead = pbuf_pool;

            print_msg(PRINT_SUGGEST, PRINT_BUF,
                      "\r\nmalloc %d(+%d) bytes 0x%x from system successfully."
                      " (%d, %s)",
                      gabuf_len[i], sizeof(BUF_NODE), pnode, __LINE__, __FILE__);
        }
        else /*  ��ϵͳ�����ڴ�ʧ�� */
        {
            print_msg(PRINT_IMPORTANT, PRINT_BUF,
                      "\r\nmalloc %d(+%d) bytes from system failed. (%d, %s)",
                      gabuf_len[i], sizeof(BUF_NODE), __LINE__, __FILE__);

            /* ��ӡ������Ļ��� */
            for(j = 0; j < BUF_TYPE_NUM; j++)
            {
                print_msg(PRINT_IMPORTANT, PRINT_BUF,
                          "\r\nalready malloc %d buf: length is %d(+%d). (%s, %d)",
                          gabuf_pool[j].s_num, gabuf_pool[j].len, sizeof(BUF_NODE),
                          __FILE__, __LINE__);
            }

        }
    }

    /* �ͷ��ź��� */
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
��������: �ͷ���Ϣ����. ����Ϣ��������Ӧ������, ���´�����ʹ��.
��ڲ���: pbuf: �ͷŵĻ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void buf_free(BUF_NODE* pbuf)
{
    U32 len;
    U32 i;

    if(NULL == pbuf)
    {
        return;
    }

    /* ��ȡ��Ҫ�ͷŵĻ������ڻ�����еĳ��� */
    len = pbuf->phead->len;

    /* �ӻ������Ѱ�������ͷŻ��峤����ȵĻ��� */
    for(i = 0; i < BUF_TYPE_NUM; i++)
    {
        if(len == gabuf_len[i])
        {
            break;
        }
    }

    /* ���峤�Ȳ���, �޷��ͷ� */
    if(i >= BUF_TYPE_NUM)
    {
        print_msg(PRINT_IMPORTANT, PRINT_BUF,
                  "\r\ncan't free the buf of %d length. (%d, %s)", len,
                  __LINE__, __FILE__);

        return;
    }

    /* �Ի����������ǰ��ȡ�ź���, ��֤��������Ĵ����� */
    if(RTN_SUCD != MDS_SemTake(gabuf_pool[i].psem, SEMWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_BUF,
                  "\r\nbuf length %d wait sem failed. (%d, %s)", len,
                  __LINE__, __FILE__);

        return;
    }

    /* ���ͷŵĻ�����뵽���������� */
    slist_node_add((SLIST*)&gabuf_pool[i].list, (SLIST*)&pbuf->list);

    gabuf_pool[i].d_num--;

    /* �ͷ��ź��� */
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
��������: ����Ϣ��������뻺��, ����ӡ������Ϣ.
��ڲ���: len: ����Ļ���ĳ���, ��λ: �ֽ�. ������BUF_NODE���ֵĳ���.
�� �� ֵ: ����Ļ���ָ��, �����벻�������򷵻�NULL.
***********************************************************************************/
BUF_NODE* buf_malloc_without_print(U32 len)
{
    BUF_POOL* pbuf_pool;
    BUF_NODE* pnode;
    U32 i;

    /* �ӻ������Ѱ�������������볤�ȵĻ��� */
    for(i = 0; i < BUF_TYPE_NUM; i++)
    {
        if(len <= gabuf_len[i])
        {
            pbuf_pool = &gabuf_pool[i];

            break;
        }
    }

    /* ������֧�ֵĻ��峤�ȷ�Χ */
    if(i >= BUF_TYPE_NUM)
    {
        return NULL;
    }

    /* ��ȡ�ź���, ��֤��������Ĵ����� */
    if(RTN_SUCD != MDS_SemTake(pbuf_pool->psem, SEMWAITFEV))
    {
        return NULL;
    }

    /* �ӻ���������һ������ڵ� */
    pnode = (BUF_NODE*)slist_node_delete(&pbuf_pool->list);

    /* �����������뵽�ڴ� */
    if(NULL != pnode)
    {
        /* ���Ӵ�����̬����ļ���ͳ�� */
        pbuf_pool->d_num++;
    }
    /* ���������û�п��õĻ�����ʹ��malloc��������һ�黺�� */
    else
    {
        /* ��ϵͳ�����ڴ�ɹ� */
        if(NULL != (pnode = (BUF_NODE*)malloc(gabuf_len[i] + sizeof(BUF_NODE))))
        {
            /* ���Ӵ�ϵͳ��̬����ļ���ͳ�� */
            pbuf_pool->s_num++;

            /* ���Ӵ�����̬����ļ���ͳ�� */
            pbuf_pool->d_num++;

            /* ������뵽�Ļ����������ڵ�ָ�� */
            pnode->phead = pbuf_pool;
        }
    }

    /* �ͷ��ź��� */
    if(RTN_SUCD != MDS_SemGive(pbuf_pool->psem))
    {
        return NULL;
    }

    return pnode;
}

/***********************************************************************************
��������: �ͷ���Ϣ����. ����Ϣ��������Ӧ������, ���´�����ʹ��. ����ӡ������Ϣ.
��ڲ���: pbuf: �ͷŵĻ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void buf_free_without_print(BUF_NODE* pbuf)
{
    U32 len;
    U32 i;

    if(NULL == pbuf)
    {
        return;
    }

    /* ��ȡ��Ҫ�ͷŵĻ������ڻ�����еĳ��� */
    len = pbuf->phead->len;

    /* �ӻ������Ѱ�������ͷŻ��峤����ȵĻ��� */
    for(i = 0; i < BUF_TYPE_NUM; i++)
    {
        if(len == gabuf_len[i])
        {
            break;
        }
    }

    /* ���峤�Ȳ���, �޷��ͷ� */
    if(i >= BUF_TYPE_NUM)
    {
        return;
    }

    /* �Ի����������ǰ��ȡ�ź���, ��֤��������Ĵ����� */
    if(RTN_SUCD != MDS_SemTake(gabuf_pool[i].psem, SEMWAITFEV))
    {
        return;
    }

    /* ���ͷŵĻ�����뵽���������� */
    slist_node_add((SLIST*)&gabuf_pool[i].list, (SLIST*)&pbuf->list);

    gabuf_pool[i].d_num--;

    /* �ͷ��ź��� */
    (void)MDS_SemGive(gabuf_pool[i].psem);
}

/***********************************************************************************
��������: ��ʼ����������.
��ڲ���: plist: ������ڵ�ָ��.
�� �� ֵ: none.
***********************************************************************************/
void slist_init(SLIST* plist)
{
    plist->next = (SLIST*)NULL;
}

/***********************************************************************************
��������: �����������һ���ڵ�, ��ӵ����ڵ�֮��.
��ڲ���: plist: ������ڵ�ָ��.
          pnode: ����Ľڵ�ָ��.
�� �� ֵ: none.
***********************************************************************************/
void slist_node_add(SLIST* plist, SLIST* pnode)
{
    pnode->next = plist->next;  /* ���ڵ�ָ���½ڵ� */
    plist->next = pnode;        /* �½ڵ�ָ����ڵ�ԭ��ָ��Ľڵ� */
}

/***********************************************************************************
��������: �ӵ�������ɾ��һ���ڵ�, �Ӹ��ڵ�֮��ɾ��.
��ڲ���: plist: ������ڵ�ָ��.
�� �� ֵ: ɾ���Ľڵ�ָ��, ������Ϊ���򷵻�NULL.
***********************************************************************************/
SLIST* slist_node_delete(SLIST* plist)
{
    SLIST* ptemp;

    ptemp = plist->next;

    /* �������Ϊ��, �����ڵ�ָ��ɾ���ڵ�֮��Ľڵ� */
    if(NULL != plist->next)
    {
        plist->next = plist->next->next;
    }

    /* ���ر�ɾ���Ľڵ� */
    return ptemp;
}

