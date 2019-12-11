/*##############################################################


@change		增加了用于确定当前模式的全局变量：查找模式，查找模式中的结果显示等
			增加了用于存储用户输入的数组

################################################################*/


/* EXTERN is defined as extern except in global.c */
#ifdef	GLOBAL_VARIABLES_HERE
#undef	EXTERN
#define	EXTERN
#endif

EXTERN	int		ticks;

EXTERN	int		disp_pos;
EXTERN	u8		gdt_ptr[6];	/* 0~15:Limit  16~47:Base */
EXTERN	DESCRIPTOR	gdt[GDT_SIZE];
EXTERN	u8		idt_ptr[6];	/* 0~15:Limit  16~47:Base */
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

// 增加的全局变量

EXTERN int isfindmode;					// 当前是否为查找模式
EXTERN int showres;				// 当前是否在显示查找结果

EXTERN u8 inputCharArr[1024];			// 非查找模式下 用户输入的每一个可显示字符 组成的数组
EXTERN int inputCharPositionArr[1024];  // 非查找模式下 用户输入的每一个可显示字符对应屏幕上的位置  组成的数组
EXTERN int counterInInputArr;			// 非查找模式下 用户输入的可显示字符的计数器，被作为数组的当前位置指针

EXTERN u8 findCharArr[256];				// 查找模式下 用户要查找的每一个可显示字符 组成的数组
EXTERN int findCharPositionArr[256];	// 查找模式下 用户要查找的每一个可显示字符对应屏幕上的位置  组成的数组
EXTERN int counterInFindArr;			// 查找模式下 用户要查找的可显示字符的计数器，被作为数组的当前位置指针

EXTERN int matchArr[1024];				// inputCharArr中的每一个字符是否对应findCharArr的对应位置