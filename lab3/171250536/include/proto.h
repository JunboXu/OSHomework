/*##############################################################

@author 	Jingqi Wu

@change		将原有的TestB、TestC的方法声明删除

################################################################*/

/*
    proto是prototype的缩写，中文意思是“原型、蓝本”
*/


/* klib.asm */
PUBLIC void	out_byte(u16 port, u8 value);
PUBLIC u8	in_byte(u16 port);
PUBLIC void	disp_str(char * info);
PUBLIC void	disp_color_str(char * info, int color);

/* protect.c */
PUBLIC void	init_prot();
PUBLIC u32	seg2phys(u16 seg);

/* klib.c */
PUBLIC void	delay(int time);

/* kernel.asm */
void restart();

// 将原有的TestB、TestC的方法声明删除
/* main.c */
void TestA();

/* i8259.c */
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* keyboard.c */
PUBLIC void init_keyboard();

/* clock.c */
PUBLIC void clock_handler(int irq);	
PUBLIC void init_clock();

/* console.c */
PUBLIC void out_char(CONSOLE* p_con, char ch);
PUBLIC void scroll_screen(CONSOLE* p_con, int direction);

/* tty.c */
PUBLIC void task_tty();	
PUBLIC void in_process(TTY* p_tty, u32 key);

/* proc.c */
PUBLIC  int sys_get_ticks();        /* sys_call */

/* syscall.asm */
PUBLIC  void    sys_call();             /* int_handler */
PUBLIC  int     get_ticks();

