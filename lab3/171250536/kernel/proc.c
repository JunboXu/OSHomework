
/*##############################################################

@author 	Jingqi Wu

@change		schedule	被删除，注意clock.c中的clock_handler方法也随之改变，因为只有一个额外进程

@intent		管理各种进程

################################################################*/

// ~~~改变了该文件的内容~~~

#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"


/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}

