/*##############################################################


@change		tty_do_read		被删除，注意本文件中的task_tty方法也随之改变
			tty_do_read 	被删除，因为不需要考虑多终端显示，所以直接调用keyboard_read

			in_process		增加对不可显示按键ESC、ENTER的功能判断
			search_keyword	新增，用于在查找模式下寻找制定字符串

@intent		该文件的功能主要为终端的字符显示

################################################################*/

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

#define TTY_FIRST	(tty_table)
#define TTY_END		(tty_table + NR_CONSOLES)

PRIVATE void init_tty(TTY* p_tty);
PRIVATE void tty_do_write(TTY* p_tty);
PRIVATE void put_key(TTY* p_tty, u32 key);

/*======================================================================*
                           task_tty
 *======================================================================*/
PUBLIC void task_tty()
{
	TTY*	pintty;

	init_keyboard();

	for (pintty=TTY_FIRST;pintty<TTY_END;pintty++) {
		init_tty(pintty);
	}
	
	while (1) {
		for (pintty=TTY_FIRST;pintty<TTY_END;pintty++) {
			keyboard_read(pintty);
			tty_do_write(pintty);
		}
	}
}

/*======================================================================*
			   init_tty
 *======================================================================*/
PRIVATE void init_tty(TTY* p_tty)
{
	p_tty->inbuf_count = 0;
	p_tty->p_inbuf_head = p_tty->p_inbuf_tail = p_tty->in_buf;

	init_screen(p_tty);
}

/*======================================================================*
@change 增加对不可显示按键ESC、ENTER的功能判断
 *======================================================================*/
PUBLIC void in_process(TTY* p_tty, u32 key)
{
        char output[2] = {'\0', '\0'};

        if (!(key & FLAG_EXT)) {	// 如果是可显示字符，显示该字符
			put_key(p_tty, key);
        }
        else {						// 如果不是可显示字符，分情况处理
			int raw_code = key & MASK_RAW;
			switch(raw_code) {
			
			// 增加ESC、ENTER键的功能判断！
			case ESC:
				isfindmode = 1;
				break;
			case ENTER:				// 回车键分2种情况
				if(isfindmode){		// 如果当前是查找模式，按下回车表示结束输入，查找结果
					showres = 1;
					search_keyword();
					show_char_arr(p_tty->p_console);
				}
				else{				// 如果当前不是查找模式，按下回车表示换行
					put_key(p_tty, '\n');
				}
				break;
			case TAB:
				put_key(p_tty, '\t');
				break;
			case UP:
				if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
					scroll_screen(p_tty->p_console, SCR_DN);
				}
				break;
			case BACKSPACE:
				put_key(p_tty, '\b');
				break;
			case DOWN:
				if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
					scroll_screen(p_tty->p_console, SCR_UP);
				}
				break;
			default:
				break;
			}
        }
}

/*======================================================================*
			      put_key
*======================================================================*/
PRIVATE void put_key(TTY* p_tty, u32 key)
{
	if (p_tty->inbuf_count < TTY_IN_BYTES) {
		*(p_tty->p_inbuf_head) = key;
		p_tty->p_inbuf_head++;
		if (p_tty->p_inbuf_head == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_inbuf_head = p_tty->in_buf;
		}
		p_tty->inbuf_count++;
	}
}


/*======================================================================*
			      tty_do_write
 *======================================================================*/
PRIVATE void tty_do_write(TTY* p_tty)
{
	if (p_tty->inbuf_count) {
		char ch = *(p_tty->p_inbuf_tail);
		p_tty->p_inbuf_tail++;
		if (p_tty->p_inbuf_tail == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_inbuf_tail = p_tty->in_buf;
		}
		p_tty->inbuf_count--;

		out_char(p_tty->p_console, ch);
	}
}


/*======================================================================*
@change 用于在查找模式下寻找制定字符串
 *======================================================================*/
PUBLIC void search_keyword()
{
	char searchH = findCharArr[0];					// searchH是要查找的字符串的首个字符
	
	for(int i=0; i<=counterInInputArr; ){				// 从头开始，遍历输入字符数组
		if(inputCharArr[i] == searchH){				// 如果某个字符和searchH一样
			int matchhh = 1;	
			int j = 0;				
			for(j=0; j<=counterInFindArr; j++){		// 以它为开始，遍历长度为counterInFindArr+1的数组元素
				if(inputCharArr[i+j] != findCharArr[j]){// 如果中间的某个字符不是一一对应
					matchhh = 0;
					break;								// 结束循环
				}
			}
			
			// 循环结束
			if(matchhh){									// 如果匹配
				for(j=0; j<=counterInFindArr; j++){
					matchArr[i+j] = 1;					// 该字符开始，长度为counterInFindArr+1的数组元素对应的match全部置为1
				}
				i = i + (counterInFindArr+1);			// i往后加counterInFindArr+1位
			}
			else{										// 如果没有匹配，从第j位开始是不同的
				for(int l=0; l<j; l++){
					matchArr[i+l] = 0;					// 该字符开始，长度为j-1的数组元素对应的match全部置为0
				}
				i = i + j;								// i往后加j位
			}
		}
		else{											// 如果某个字符和searchH不同
			matchArr[i] = 0;							// 该字符对应的match置为0
			i++;
		}
	}
}
