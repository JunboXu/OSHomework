
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
�����˲���ģʽ�ͷǲ���ģʽ������                    Vanny, 2019
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* EXTERN is defined as extern except in global.c */
#ifdef	GLOBAL_VARIABLES_HERE
#undef	EXTERN
#define	EXTERN
#endif

EXTERN	int		ticks;

EXTERN	int		disp_pos;
EXTERN	u8		gdt_ptr[6];	// 0~15:Limit  16~47:Base
EXTERN	DESCRIPTOR	gdt[GDT_SIZE];
EXTERN	u8		idt_ptr[6];	// 0~15:Limit  16~47:Base
EXTERN	GATE		idt[IDT_SIZE];

EXTERN	u32		k_reenter;

EXTERN	TSS		tss;
EXTERN	PROCESS*	p_proc_ready;

EXTERN	int		nr_current_console;

extern	PROCESS		proc_table[];
extern	char		task_stack[];
extern  TASK            task_table[];
extern	irq_handler	irq_table[];
extern	TTY		tty_table[];
extern  CONSOLE         console_table[];

//add
EXTERN u8 InputArr[512];         //�ǲ���ģʽ���û������ַ�
EXTERN int InputLocArr[512];     //�ǲ���ģʽ���û������ַ�λ��
EXTERN int InputLocPtr;          //�ǲ���ģʽ���û������ַ�λ��ָ��

EXTERN u8 EscInputArr[512];         //����ģʽ���û������ַ�
EXTERN int EscInputLocArr[512];     //����ģʽ���û������ַ�λ��
EXTERN int EscInputLocPtr;          //����ģʽ���û������ַ�λ��ָ��

EXTERN int isFind;               //�Ƿ��ǲ���ģʽ
EXTERN int inputFlag;            //��������