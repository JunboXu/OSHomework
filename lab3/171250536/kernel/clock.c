/*##############################################################

@author 	Jingqi Wu

@change		clock_handler	代码回到第6章的版本，使得主任务和用户进程可以同时运行

@intent		用于不同级别的进程调度

################################################################*/

// ~~~改变了该文件的内容~~~

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

/*======================================================================*
 @change 代码回到第6章的版本，使得主任务和用户进程可以同时运行
 *======================================================================*/
PUBLIC void clock_handler(int irq)
{
	// 下面这几行代码，使得在一个进程的ticks没有变成0之前，其他进程不会有机会获得执行
	/*ticks++;
	p_proc_ready->ticks--;
	if (k_reenter != 0) {
		return;
	}
	if (p_proc_ready->ticks > 0) {
		return;							
	}
	schedule();*/
	
	// 下面这几行代码，使得程序可以运行在不同特权级之上，普通的用户进程和系统任务区分开来，可以同时运行。
	ticks++;
	
	if (k_reenter != 0) {
		return;
	}
	
	p_proc_ready++;
	if(p_proc_ready>=proc_table+NR_TASKS){
		p_proc_ready=proc_table;   
	}

}

/*======================================================================*
                              milli_delay
 *======================================================================*/
PUBLIC void milli_delay(int milli_sec)
{
        int t = get_ticks();
        while(((get_ticks() - t) * 1000 / HZ) < milli_sec) {}
}

/*======================================================================*
                           init_clock
 *======================================================================*/
PUBLIC void init_clock()
{
        /* 初始化 8253 PIT */
        out_byte(TIMER_MODE, RATE_GENERATOR);
        out_byte(TIMER0, (u8) (TIMER_FREQ/HZ) );
        out_byte(TIMER0, (u8) ((TIMER_FREQ/HZ) >> 8));

        put_irq_handler(CLOCK_IRQ, clock_handler);    /* 设定时钟中断处理程序 */
        enable_irq(CLOCK_IRQ);                        /* 让8259A可以接收时钟中断 */
}


