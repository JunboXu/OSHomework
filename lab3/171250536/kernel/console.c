/*##############################################################


@change		init_screen			新增了查找模式，初始化查找模式用到的全局变量，使得光标的初始位置为左上角
			is_current_console	删除，注意tty.c中的tty_do_read方法也随之改变
			clean_screen		新增，每隔20秒时调用的清屏方法
			out_char			改变了每个字符在不同模式下的输出方式，尤其是'\n','\t','\b'输出
			show_char_arr		新增，用来在屏幕上显示字符数组。尤其是查找模式中的显示答案情况下
			select_console		删除，注意tty.c中的task_tty()和in_process方法也随之改变

@intent		该文件的功能主要为屏幕初始化，光标位置显示，字符输出

################################################################*/
// ~~~改变了该文件的内容~~~

/*回车键: 把光标移到第一列
  换行键: 把光标前进到下一行*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

PRIVATE void set_cursor(unsigned int position);
PRIVATE void set_video_start_addr(u32 addr);
PRIVATE void flush(CONSOLE* p_con);


/*======================================================================*
@change 新增了查找模式，初始化查找模式用到的全局变量
		使得光标的初始位置为左上角
 *======================================================================*/
PUBLIC void init_screen(TTY* pintty)
{
	
	// 增加全局变量find_mode、showing_result的初始化
	isfindmode = 0;		//当前不是查找模式
	showres = 0;	//当前没有在显示查找结果
	
	
	int nttyr = pintty - tty_table;
	pintty->p_console = console_table + nttyr;

	int vmsize = V_MEM_SIZE >> 1;	/* 显存总大小 (in WORD) */

	int covmsize                   = vmsize / NR_CONSOLES;
	pintty->p_console->original_addr      = nttyr * covmsize;
	pintty->p_console->v_mem_limit        = covmsize;
	pintty->p_console->current_start_addr = pintty->p_console->original_addr;

	/* 默认光标位置在最开始处 */
	pintty->p_console->cursor = pintty->p_console->original_addr;
	
	//增加currentCharInputPos、currentCharFindPos的初始化，因为它们要作为指针使用，所以初始化为-1
	counterInInputArr = -1;
	counterInFindArr = -1;
	

	// 下面几行代码使得光标从屏幕左上角重新显示
	pintty->p_console->cursor = disp_pos / 2;
	disp_pos = 0;
	
	set_cursor(pintty->p_console->cursor);
}


/*======================================================================*
@change 增加的用于清屏的方法
*======================================================================*/
PUBLIC void clean_screen()
{
	disable_int();
	
	disp_pos=0;
	int i=0;
	for(;i<80*25;i++){
		disp_str(" ");		
	}
	disp_pos=0;
	
	init_screen(tty_table);
	
	enable_int();
}


/*======================================================================*
@change 改变out_char方法的具体实现，如：原有的'\n','\t','\b'及可显示字符的输出
		查找模式下变色
@tip    每一次输入字符，都要考虑在什么模式下的输入，不同模式不同对待
 *======================================================================*/
PUBLIC void out_char(CONSOLE* p_con, char ch)
{
	u8* p_vmem = (u8*)(V_MEM_BASE + p_con->cursor * 2);

	switch(ch) {

	// tab对齐键
    case '\t':
		if (p_con->cursor < p_con->original_addr + p_con->v_mem_limit - 4) {
         
			//需要判断是否为查找模式
			
			if(!isfindmode){									// 不是查找模式
				counterInInputArr++;						// 输入计数器的值加1
				inputCharArr[counterInInputArr] = '\t';		// 输入字符数组中记录一个制表符
				inputCharPositionArr[counterInInputArr] = p_con->cursor; // 输入位置数组中记录下当前位置的值
			}
			else{											// 是查找模式
				counterInFindArr++;							// 查找计数器的值加1
				findCharArr[counterInFindArr] = '\t';		// 查找字符数组中记录一个制表符
				findCharPositionArr[counterInFindArr] = p_con->cursor; // 查找位置数组中记录下当前位置的值
			}
			
			p_con->cursor = ( p_con->cursor/4 + 1 ) * 4;	// 光标移至下一个可以整除4的位置
		}
		break;

	// backspace 退格键
	case '\b':
		if (p_con->cursor > p_con->original_addr) {
            
			//需要判断是否为查找模式
			
			if(!isfindmode){
				p_con->cursor = inputCharPositionArr[counterInInputArr];// 光标移至输入位置数组中最后一个有效元素的值
				counterInInputArr--;	// 输入计数器的值减1
			}
			else{
				if(counterInFindArr==-1){
					break;
				}
				p_con->cursor = findCharPositionArr[counterInFindArr];// 光标移至查找位置数组中最后一个有效元素的值
				counterInFindArr--;		// 查找计数器的值减1
			}
			
            p_vmem = (u8*)(V_MEM_BASE + p_con->cursor * 2);            
			*(p_vmem ) = ' ';
			*(p_vmem + 1) = DEFAULT_CHAR_COLOR;              
		}
		break;
		// enter回车键
	case '\n':
		if (p_con->cursor < p_con->original_addr + p_con->v_mem_limit - SCREEN_WIDTH) {

			// 这里不需要判断是否为查找模式，查找模式下的ENTER情况在in_process中已处理

			counterInInputArr++;						// 输入计数器的值加1
			inputCharArr[counterInInputArr] = '\n';		// 输入字符数组中记录一个回车符
			inputCharPositionArr[counterInInputArr] = p_con->cursor; // 输入位置数组中记录下当前位置的值


			p_con->cursor = p_con->original_addr + SCREEN_WIDTH * ((p_con->cursor - p_con->original_addr) / SCREEN_WIDTH + 1); // 光标移至下一行行首
		}
		break;

	// 如果是其它字符，则为一般处理方式
	default:
		if (p_con->cursor < p_con->original_addr + p_con->v_mem_limit - 1) {
                
				//需要判断是否为查找模式
				if(!isfindmode){
					counterInInputArr++;					// 输入计数器的值加1
					inputCharArr[counterInInputArr] = ch;	// 输入字符数组中记录该字符
					inputCharPositionArr[counterInInputArr] = p_con->cursor;  // 输入位置数组中记录下当前位置的值
				}
				else{
					counterInFindArr++;					// 查找计数器的值加1
					findCharArr[counterInFindArr] = ch;	// 查找字符数组中记录该字符
					findCharPositionArr[counterInFindArr] = p_con->cursor;  // 查找位置数组中记录下当前位置的值
				}
				*p_vmem++ = ch;
				p_con->cursor++;
				
				// 增加变颜色语句
				if(!isfindmode){
					*p_vmem++ = DEFAULT_CHAR_COLOR;
				}
				else{
					*p_vmem++ = LIGHT_BLUE_CHAR_COLOR;
				}
		}
		break;
	}

	
	while (p_con->cursor >= p_con->current_start_addr + SCREEN_SIZE) {
		scroll_screen(p_con, SCR_DN);
	}

	flush(p_con);
}


/*======================================================================*
@change 增加show_char_arr方法，用来在屏幕上显示字符数组
 *======================================================================*/
PUBLIC void show_char_arr(CONSOLE* p_c)
{
	int cursor_pos_save = p_c->cursor;	// 保存当前光标的位置
	
	u8* pvm;
		
	if(showres){						// 如果正在显示查找结果，以特殊颜色标出查找关键字
	
		// 重新显示被查找字符数组中的字符
		int i = 0;
		for(i=0; i<=counterInInputArr; i++){
			p_c->cursor = inputCharPositionArr[i];// 光标移至该字符记录的位置
			pvm = (u8*)(V_MEM_BASE + p_c->cursor * 2);
			
			char ch = inputCharArr[i];
			if(ch == '\n' || ch == '\t'){	// 如果输入字符数组中保存的是回车符或制表符
				*pvm++ = ' ';			// 以默认颜色显示空白符，反正光标已经移动到了该在的位置……
				*pvm++ = DEFAULT_CHAR_COLOR;
			}
			else{							// 如果输入字符数组中保存的是可显示字符
				*pvm++ = ch;
				if(matchArr[i]){
					*pvm++ = LIGHT_BLUE_CHAR_COLOR;	// 如果命中，就以查找字符的颜色显示
				}
				else{
					*pvm++ = DEFAULT_CHAR_COLOR; 	// 没有命中就以默认颜色显示
				}
			}
		}
		
		// 重新显示查找字符数组中的字符（否则可能被重新显示的被查找字符数组的最后一个字符覆盖掉）
		for(i=0; i<=counterInFindArr; i++){
			p_c->cursor = findCharPositionArr[i];// 光标移至该字符记录的位置
			pvm = (u8*)(V_MEM_BASE + p_c->cursor * 2);
			
			char ch = findCharArr[i];
			if(ch == '\t'){				// 如果查找字符数组中保存的是制表符（不需要考虑回车符的情况）
				*pvm++ = ' ';			// 以默认颜色显示空白符，反正光标已经移动到了该在的位置……
				*pvm++ = DEFAULT_CHAR_COLOR;
			}
			else{							// 如果查找字符数组中保存的是可显示字符
				*pvm++ = ch;
				*pvm++ = LIGHT_BLUE_CHAR_COLOR;	// 以查找字符的颜色显示
			}
		}
		
		// 重新显示光标的位置
		p_c->cursor = cursor_pos_save;		// 光标所在位置就是开头保存的值
		*pvm++ = ' ';
		*pvm++ = DEFAULT_CHAR_COLOR; 		// 以默认颜色显示光标
	}
	
	else{											// 如果已经退出查找模式，显示进入查找模式前屏幕上的字符
		
		// 清屏
		disable_int();
		disp_pos=0;
		int i=0;
		for(;i<80*25;i++){
			disp_str(" ");		
		}
		disp_pos=0;
		enable_int();
		
		// 只显示输入字符数组中的字符
		for(i=0; i<=counterInInputArr; i++){
			p_c->cursor = inputCharPositionArr[i];// 光标移至该字符记录的位置
			pvm = (u8*)(V_MEM_BASE + p_c->cursor * 2);
			
			char ch = inputCharArr[i];
			if(ch == '\n' || ch == '\t'){
				ch = ' ';
			}
			*pvm++ = ch;
			*pvm++ = DEFAULT_CHAR_COLOR; // 以默认颜色显示	
		}
		
		// 进入查找模式前输入的字符已经显示完毕，还要显示进入查找模式前光标的最后所在位置
		p_c->cursor = findCharPositionArr[0];		// 光标最后所在位置就是查找字符位置数组的第一个元素的值
		*pvm++ = ' ';
		*pvm++ = DEFAULT_CHAR_COLOR; 			// 以默认颜色显示光标
	}

		
	while (p_c->cursor >= p_c->current_start_addr + SCREEN_SIZE) {
		scroll_screen(p_c, SCR_DN);
	}

	flush(p_c);
}


/*======================================================================*
                           flush
*======================================================================*/
PRIVATE void flush(CONSOLE* p_con)
{
        set_cursor(p_con->cursor);
        set_video_start_addr(p_con->current_start_addr);
}

/*======================================================================*
			    set_cursor
 *======================================================================*/
PRIVATE void set_cursor(unsigned int position)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, CURSOR_H);
	out_byte(CRTC_DATA_REG, (position >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, CURSOR_L);
	out_byte(CRTC_DATA_REG, position & 0xFF);
	enable_int();
}

/*======================================================================*
			  set_video_start_addr
 *======================================================================*/
PRIVATE void set_video_start_addr(u32 addr)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, START_ADDR_H);
	out_byte(CRTC_DATA_REG, (addr >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, START_ADDR_L);
	out_byte(CRTC_DATA_REG, addr & 0xFF);
	enable_int();
}



/*======================================================================*
			   scroll_screen 滚屏
 direction:
	SCR_UP	: 向上滚屏
	SCR_DN	: 向下滚屏
	其它	: 不做处理
 *======================================================================*/
PUBLIC void scroll_screen(CONSOLE* p_con, int direction)
{
	if (direction == SCR_UP) {
		if (p_con->current_start_addr > p_con->original_addr) {
			p_con->current_start_addr -= SCREEN_WIDTH;
		}
	}
	else if (direction == SCR_DN) {
		if (p_con->current_start_addr + SCREEN_SIZE <
		    p_con->original_addr + p_con->v_mem_limit) {
			p_con->current_start_addr += SCREEN_WIDTH;
		}
	}
	else{
	}

	set_video_start_addr(p_con->current_start_addr);
	set_cursor(p_con->cursor);
}



