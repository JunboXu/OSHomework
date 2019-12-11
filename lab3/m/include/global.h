
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
增加了查找模式和非查找模式的数组                    Vanny, 2019
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
EXTERN u8 InputArr[512];         //非查找模式下用户输入字符
EXTERN int InputLocArr[512];     //非查找模式下用户输入字符位置
EXTERN int InputLocPtr;          //非查找模式下用户输入字符位置指针

EXTERN u8 EscInputArr[512];         //查找模式下用户输入字符
EXTERN int EscInputLocArr[512];     //查找模式下用户输入字符位置
EXTERN int EscInputLocPtr;          //查找模式下用户输入字符位置指针

EXTERN int isFind;               //是否是查找模式
EXTERN int inputFlag;            //屏蔽输入