
    INCLUDE wlx_core_a.h


    ;ջ8�ֽڶ���
    PRESERVE8

    AREA    WLX_CORE_A, CODE, READONLY
    THUMB


    ;��������: ʵ�������л�����, ���Ĵ������ݵ���ǰ����ջ��, �ӽ�Ҫ��������ջ��ȡ��
    ;          �Ĵ�������ֵ����ת����Ҫ��������ִ��.
    ;��ڲ���: none.
    ;�� �� ֵ: none.
WLX_ContextSwitch

    ;���浱ǰ�����ջ��Ϣ
    MOV    R3, R0               ;���Ĵ������ַ����R3�Ĵ���
    STMIA  R3!, {R0, R4 - R12}  ;����R0,R4-R12�Ĵ���
    STMIA  R3!, {R13}           ;����SP�Ĵ���
    STMIA  R3!, {R14}           ;����LR�Ĵ���
    MRS    R2, XPSR             ;��ȡXPSR�Ĵ�����ֵ
    STMIA  R3, {R2}             ;���浽�Ĵ������е�XPSR

    ;�ָ���Ҫ���������ջ��Ϣ������������
    LDMIA  R1!, {R0, R4 - R12}  ;�ָ�R0,R4-R12�Ĵ���
    LDMIA  R1, {R13}            ;�ָ�SP�Ĵ���
    ADD    R1, #8               ;R1ָ��Ĵ������е�XPSR
    LDMIA  R1, {R2}             ;��ȡ�Ĵ������е�XPSR��ֵ
    MSR    XPSR, R2             ;�ָ�XPSR�Ĵ���
    SUB    R1, #4               ;R1ָ��Ĵ������е�LR
    LDMIA  R1, {PC}             ;�л�����


    ;��������: ʵ�ִӷǲ���ϵͳ״̬�л�������ϵͳ״̬, �ӵ�һ������ջ��ȡ���Ĵ�����
    ;          ʼֵ����ת��������ִ��.
    ;��ڲ���: none.
    ;�� �� ֵ: none.
WLX_SwitchToTask

    ;�ָ���Ҫ���������ջ��Ϣ������������
    MOV    R2, R0               ;���Ĵ������ַ����R2�Ĵ���
    LDMIA  R2!, {R0, R4 - R12}  ;�ָ�R0,R4-R12�Ĵ���
    LDMIA  R2, {R13}            ;�ָ�SP�Ĵ���
    ADD    R2, #8               ;R2ָ��Ĵ�������XPSR
    LDMIA  R2, {R1}             ;��ȡ�Ĵ������е�XPSR��ֵ
    MSR    XPSR, R1             ;�ָ�XPSR�Ĵ���
    SUB    R2, #4               ;R2ָ��Ĵ������е�LR
    LDMIA  R2, {PC}             ;�����׸�����


    ALIGN

    END

