
#include <stdlib.h>
#include "task.h"


TASK_POOL gtask_pool;           /* ����� */


/***********************************************************************************
��������: �������ƽ̨��ʹ�õ�����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void task_create(void)
{
    /**************************************************************************/
    /*            �������������    www.ifreecoding.com                       */
    /*                              bbs.ifreecoding.com                       */
    /*                              blog.sina.com.cn/ifreecoding              */
    /**************************************************************************/

    /* ����print_task����, ��Ҫ�ȴ���print����, Ϊ���������ṩ��ӡ���� */
    create_print_task();

    /* ����sched_task���� */
    create_sched_task();

    /* ����rx_task���� */
    create_rx_task();

    /* ����tx_task���� */
    create_tx_task();

    /* process����������ص���ʱ����, �˴�ֻ��ʼ������� */
    process_task_pool_init();
}

/***********************************************************************************
��������: ��ʼ�������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void process_task_pool_init(void)
{
    /* ��ʼ��������ź��� */
    if(NULL == (gtask_pool.psem = MDS_SemCreate(NULL, SEMCNT | SEMPRIO, SEMFULL)))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TASK,
                  "\r\ntask_pool sem init failed. (%s, %d)", __FILE__, __LINE__);
    }

    /* ��ʼ����������� */
    slist_init(&gtask_pool.list);

    /* ��ʼ�������ͳ��ֵ */
    gtask_pool.s_num = 0;
    gtask_pool.d_num = 0;
}

/***********************************************************************************
��������: �������������һ������. ����������п���������������������һ������, ����
          �����û�п��������򴴽�һ���µ�����.
��ڲ���: none.
�� �� ֵ: ���뵽������ṹָ��, �����벻�������򷵻�NULL.
***********************************************************************************/
TASK_STR* process_task_malloc(void)
{
    TASK_STR* ptask;

    /* ��ȡ�ź���, ��֤�������������Ĵ����� */
    if(RTN_SUCD != MDS_SemTake(gtask_pool.psem, SEMWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TASK,
                  "\r\nprocess_task wait sem failed. (%d, %s)", __LINE__, __FILE__);

        return NULL;
    }

    /* �������������һ������ڵ� */
    ptask = (TASK_STR*)slist_node_delete(&gtask_pool.list);

    /* �����������뵽���� */
    if(NULL != ptask)
    {
        /* ���Ӷ�̬��������ļ���ͳ�� */
        gtask_pool.d_num++;

        print_msg(PRINT_SUGGEST, PRINT_TASK,
                  "\r\nmalloc 0x%x process_task successfully. (%s, %d)", ptask,
                  __FILE__, __LINE__);
    }
    /* ���������û�п��õ��������´���һ������ */
    else
    {
        /* ��������ṹ */
        ptask = malloc(sizeof(TASK_STR));

        /*  ��ϵͳ�����ڴ�ɹ� */
        if(NULL != ptask)
        {
            print_msg(PRINT_SUGGEST, PRINT_TASK,
                      "\r\nmalloc 0x%x process_task from system successfully."
                      " (%s, %d)", ptask, __FILE__, __LINE__);
        }
        else /* ��ϵͳ�����ڴ�ʧ�� */
        {
            print_msg(PRINT_IMPORTANT, PRINT_TASK,
                      "\r\nmalloc %d bytes from system failed. (%s, %d)",
                      sizeof(TASK_STR), __FILE__, __LINE__);

            goto RTN;
        }

        /* ����process���� */
        if(RTN_SUCD != create_process_task(ptask))
        {
            /* �ͷ����������ṹ���� */
            free(ptask);

            goto RTN;
        }
        else /* ��������ɹ� */
        {
            /* ���Ӿ�̬��������ļ���ͳ�� */
            gtask_pool.s_num++;

            /* ���Ӷ�̬��������ļ���ͳ�� */
            gtask_pool.d_num++;
        }
    }

RTN:

    /* �ͷ��ź��� */
    if(RTN_SUCD != MDS_SemGive(gtask_pool.psem))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TASK,
                  "\r\nprocess_task post sem failed. (%d, %s)", __LINE__, __FILE__);

        return NULL;
    }

    return ptask;
}

/***********************************************************************************
��������: �ͷ�process����. ��process����������������, ���´�����ʹ��.
��ڲ���: ptask: �ͷŵ�����ṹָ��.
�� �� ֵ: none.
***********************************************************************************/
void process_task_free(TASK_STR* ptask)
{
    /* ��ȡ�ź���, ��֤�������������Ĵ����� */
    if(RTN_SUCD != MDS_SemTake(gtask_pool.psem, SEMWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TASK,
                  "\r\nprocess_task wait sem failed. (%d, %s)", __LINE__, __FILE__);

        return;
    }

    /* ���ͷŵ�������뵽���������� */
    slist_node_add(&gtask_pool.list, &ptask->list);

    gtask_pool.d_num--;

    /* �ͷ��ź��� */
    if(RTN_SUCD != MDS_SemGive(gtask_pool.psem))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TASK,
                  "\r\nprocess_task post sem failed. (%d, %s)", __LINE__, __FILE__);

        return;
    }

    print_msg(PRINT_SUGGEST, PRINT_TASK,
              "\r\nfree process_task 0x%x to taskpool successfully. (%s, %d)",
              ptask, __FILE__, __LINE__);
}

