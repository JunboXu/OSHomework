/*##############################################################

@author 	Jingqi Wu

@change		修改了进程的定义：删去进程优先级，时间设为全局变量
			将NR_TASKS的值由4改为2
			将STACK_SIZE_TOTAL的计算中，进程B和C删去

################################################################*/

/** 栈的定义
    反正就是一堆寄存器……看看就得了，不详细解释
	（我真是不想吐槽原作者，要用什么软件写代码，才能让画图的排版如此糟糕！！）
*/
typedef struct s_stackframe {	/* proc_ptr points here		↑ Low		*/
	u32	gs;						/* ┓						│			*/
	u32	fs;						/* ┃						│			*/
	u32	es;						/* ┃						│			*/
	u32	ds;						/* ┃						│			*/
	u32	edi;					/* ┃						│			*/
	u32	esi;					/* ┣ pushed by save()		│			*/
	u32	ebp;					/* ┃						│			*/
	u32	kernel_esp;				/* <- 'popad' will ignore it│			*/
	u32	ebx;					/* ┃						↑栈从高地址往低地址增长*/		
	u32	edx;					/* ┃						│			*/
	u32	ecx;					/* ┃						│			*/
	u32	eax;					/* ┛						│			*/
	u32	retaddr;				/* return address for assembly code save()	│*/
	u32	eip;					/* ┓						│			*/
	u32	cs;						/* ┃						│			*/
	u32	eflags;					/* ┣ these are pushed by CPU during interrupt	│*/
	u32	esp;					/* ┃						│			*/
	u32	ss;						/* ┛						┷High		*/
}STACK_FRAME;


/** 进程的定义
    每个进程有属于它的栈、选择子（用于定位GDT中的某个LDT描述符，得到LDT的地址）、LDT（局部描述符表）、进程号、进程名 
*/
typedef struct s_proc {
	STACK_FRAME regs;          /* process registers saved in stack frame */

	u16 ldt_sel;               /* gdt selector giving ldt base and limit */
	DESCRIPTOR ldts[LDT_SIZE]; /* local descriptors for code and data */

	//将原有的int ticks，int priority删除，ticks改成了全局变量
    
	u32 pid;                   /* process id passed in from MM */
	char p_name[16];           /* name of the process */
}PROCESS;

/** 任务的定义
	一个任务可以有多个进程，但一个进程只属于一个任务
    每个任务有属于它的函数指针task_f（task_f在type.h中定义，是指向初始化eip函数的函数指针），栈大小，拥有的进程集合中每一个进程的进程名 
*/
typedef struct s_task {
	task_f	initial_eip;
	int	stacksize;
	char	name[32];
}TASK;


// 将NR_TASKS的值由4改为2
/* Number of tasks */
#define NR_TASKS	2

// 改动：将原有的STACK_SIZE_TESTB、STACK_SIZE_TESTC删除
/* stacks of tasks */
#define STACK_SIZE_TTY		0x8000
#define STACK_SIZE_TESTA	0x8000

// 将计算中的进程B和C删去
#define STACK_SIZE_TOTAL	(STACK_SIZE_TTY + STACK_SIZE_TESTA)

