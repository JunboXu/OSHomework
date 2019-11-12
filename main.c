#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//ȫ�ֶ�����
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
//-------------------

//�ṹ��������ȫ�ֱ���������
struct Node
{
	char name[11];
	byte attribute;   //����
	char reserve[10]; //�����ֶ�
	word time1;
	word time2;
	word startSec; //��ʼ����
	dword fileSize;
};

struct Table
{
	char path[64];
	char name[11];
	byte attribute;
	word startSec;
	dword fileSize;
};

struct Table table[100];
int t_size = 0;

void preput(char x1[1024], int color); //��ӡ����
void input(FILE* fat);
void printTxtName(char name[11]);
void ls(FILE* fat12, int begin, char* path, int hasParam); //hasParam =0 -->ls();   1-->ls -l; ;  2-->ls -l ·��
int startwith(char x1[64], char x2[64]);
int findDir(char str[64]);
void load(char name[64], int begin, FILE* fat12);
void lsl(FILE* fat12, int begin, char path[64], int hasParam); //ls -l
int countdir(int begin, FILE* fat12);
int counttxt(int begin, FILE* fat12);
//int cat(int begin, FILE* fat12, int fileSize);
void cat(FILE* fat12, char str1[64]);
extern void output(char x1[1024], int color, int len); // ��ɫ������0 �ļ��У�1 �ļ���2 ����
char* itoa(int value, char* result, int base);
int findtxtSize(char str[64]);
int main()
{
	preput("SYSTEM START!", 0);
	preput("\n", 2);
	FILE* fat12 = fopen("ref.img", "rb"); //���ļ�
	load("/", 19 * 512, fat12);
	input(fat12);
}

void input(FILE* fat12)
{
	char str1[64];
	char str2[64];
	char str3[64];
	while (1)
	{
		preput("PLEASE INPUT:", 0);
		scanf("%s", str1); //��ָ��
		if (strcmp(str1, "exit") == 0)
		{
			break;
		}
		else if (strcmp(str1, "cat") == 0)
		{
			scanf("%s", str2); //��·��������str2��
			int find = findDir(str2);
			if (find == -1)
			{
				preput("δ�ҵ�·��", 2);
				preput("\n", 2);
			}
			else
			{
				int index = findDir(str2);
				//preput(str2, 1);
				if (index == -1) //cat dir??
				{
					preput("δ�ҵ�·��", 2);
				}
				else
				{
					//preput("cat begin", 2);
					//preput("\n", 2);
					cat(fat12, str2);

					//char* nodeptr; //��ʼ���ڵ�ָ��
					//int size = findtxtSize(str2);
					//fseek(fat12, index , 0);
					//fread(nodeptr, 1, size, fat12);
					//preput(nodeptr,1);
					//���
				}
			}
		}
		else if (strcmp(str1, "ls") == 0)
		{ //�����lsָ��
			char c = getchar();
			if (c == '\n')
			{ //���������������Ϊls()ָ��Ӹ�Ŀ¼��ʼ���
				ls(fat12, 19 * 512, "", 0);
			}
			else
			{ //�еڶ���ָ��
				scanf("%s", str2);
				if (startwith(str2, "-l") || startwith(str2, "-ll"))
				{
					char c2 = getchar();
					if (c2 == '\n') //ls -l()
					{
						lsl(fat12, 19 * 512, "", 1);
					}
					else
					{
						scanf("%s", str3);
						int beginx = findDir(str3);
						if (beginx == -1)
						{
							preput("δ�ҵ�·��\n", 2);
							preput("\n", 2);
						}
						else
						{
							lsl(fat12, beginx, str3, 1);
						}
					}
				}
				else
				{
					if (startwith(str2, "/"))
					{
						char c3 = getchar();
						if (c3 == '\n')
						{								//ls + ·��
							int beginx = findDir(str2); //findDir ����str2Ŀ¼��startsec
							if (beginx == -1)
							{
								preput("δ�ҵ�·��\n", 2);
								preput("\n", 2);
							}
							else
							{
								ls(fat12, beginx, str2, 0);
							}
						}
						else
						{
							scanf("%s", str3);
							if (startwith(str3, "-l") || startwith(str3, "-ll"))
							{ //�е��������룬�ҺϷ�
								int beginx = findDir(str2);
								lsl(fat12, beginx, str2, 1);
							}
							else
							{ //�Ƿ�����
								preput("����������Ƿ�", 2);
								preput("\n", 2);
							}
						}
					}
				}
			}
		}
		else
		{
			preput("ָ������ע���Сд", 2);
			preput("\n", 2);
		}
	}
}

void lsl(FILE* fat12, int begin, char path[64], int hasParam)
{
	//-----
	struct Node node0;
	struct Node* nodeptr = &node0; //��ʼ���ڵ�ָ��
	if (strlen(path) == 0 || path == "/")
	{
		path = "/";
	}
	preput(path, 2);
	preput(" ", 2);
	int beginx = findDir(path);
	int dirn = countdir(begin, fat12);
	int txtn = counttxt(begin, fat12);
	char str[25];
	preput(_itoa(dirn, str, 10), 2);
	preput(" ", 2);
	preput(_itoa(txtn, str, 10), 2);
	preput(":", 2);
	preput("\n", 2);
	if ((strlen(path) != 0) && (strlen(path) != 1))
	{
		preput(".", 0);
		preput("\n", 2);
		preput("..", 0);
		preput("\n", 2);
	}
	for (int i = 0; i < 15; i++) //��һ�������ӡֱ������
	{
		fseek(fat12, begin + i * 32, 0);
		fread(nodeptr, 1, 32, fat12);
		if ((nodeptr->name[0] >= 'A' && nodeptr->name[0] <= 'Z') || (nodeptr->name[0] >= 'a' && nodeptr->name[0] <= 'z') || (nodeptr->name[0] >= '0' && nodeptr->name[0] <= '9'))
		{
		}
		else
		{
			continue;
		}
		if (nodeptr->attribute == 0x10) //������ļ���
		{
			int nameLen = 0;
			char temp[11];
			for (int i = 0; i < 11; i++)
			{
				if (nodeptr->name[i] == '\0' || nodeptr->name[i] == ' ')
				{
					temp[i] = '\0';
					break;
				}
				else
				{
					temp[i] = nodeptr->name[i];
					nameLen++;
				}
			}
			preput(temp, 0);
			preput(" ", 2); //��ӡ�ļ�������
			int beginx = (nodeptr->startSec + 31) * 512;
			int dirn = countdir(beginx, fat12);
			int txtn = counttxt(beginx, fat12);
			char str[25];
			preput(_itoa(dirn, str, 10), 2);
			preput(" ", 2);
			preput(_itoa(txtn, str, 10), 2);
			preput("\n", 2);
		}
		else if (nodeptr->attribute == 0x00)
		{ //������ı��ļ�����ӡ��С
			printTxtName(nodeptr->name);
			preput(" ", 2);
			char str[25];
			preput(_itoa(nodeptr->fileSize, str, 10), 2);
			preput("\n", 2);
		}
	}
	for (int i = 0; i < 15; i++) //��һ��ѭ��˳���ж�ֱ���Ƿ�Ϊ�ļ��У����ļ��оͽ��ļ��У���ӡ
	{
		fseek(fat12, begin + i * 32, 0);
		fread(nodeptr, 1, 32, fat12);
		if ((nodeptr->name[0] >= 'A' && nodeptr->name[0] <= 'Z') || (nodeptr->name[0] >= 'a' && nodeptr->name[0] <= 'z') || (nodeptr->name[0] >= '0' && nodeptr->name[0] <= '9'))
		{
		}
		else
		{
			continue;
		}
		char temppath[64];
		char nowName[64];
		int tempbegin;
		if (nodeptr->attribute == 0x10) //������ļ��У����ļ�������д�뵱ǰ·��
		{
			char* gang = "/";
			if (strlen(path) == 0 || path == "/")
			{
				path = "/";
			}
			else
			{
				strcat(path, gang);
			}
			for (int i = 0; i < 64; i++)
			{
				temppath[i] = path[i];
				if (path[i] == '\0' || path[i] == ' ')
				{
					temppath[i] = '\0';
					break;
				}
			} //����ǰ·���浽temppath��
			for (int i = 0; i < 64; i++)
			{
				nowName[i] = nodeptr->name[i];
				if (nodeptr->name[i] == '\0' || nodeptr->name[i] == ' ')
				{
					nowName[i] = '\0';
					break;
				}
			}
			strcat(temppath, nowName); //д�뵱ǰ·��

			tempbegin = (nodeptr->startSec + 31) * 512; //��ת�����ļ���ָ��Ĵ�

			lsl(fat12, tempbegin, temppath, 0); //�ݹ�ls
		}
	}
	return;
}

void ls(FILE* fat12, int begin, char path[64], int hasParam)
{
	struct Node node0;
	struct Node* nodeptr = &node0; //��ʼ���ڵ�ָ��
	if (hasParam == 0)
	{
		preput(path, 2);
		preput("/:", 2);
		preput("\n", 2);
		if (strlen(path) != 0)
		{
			preput(". .. ", 0);
		}
		for (int i = 0; i < 15; i++) //��һ��ѭ����ӡ��ֱ���ļ��к��ļ�
		{
			fseek(fat12, begin + i * 32, 0);
			fread(nodeptr, 1, 32, fat12);
			if ((nodeptr->name[0] >= 'A' && nodeptr->name[0] <= 'Z') || (nodeptr->name[0] >= 'a' && nodeptr->name[0] <= 'z') || (nodeptr->name[0] >= '0' && nodeptr->name[0] <= '9'))
			{
			}
			else
			{
				continue;
			}
			if (nodeptr->attribute == 0x10 || nodeptr->attribute == 0x00)
			{
				if (nodeptr->attribute == 0x00) //�ı��ļ�
				{
					printTxtName(nodeptr->name);
					preput(" ", 2);
				}
				if (nodeptr->attribute == 0x10) //�ļ���
				{
					int nameLen = 0;
					char temp[11];
					for (int i = 0; i < 11; i++)
					{
						if (nodeptr->name[i] == '\0' || nodeptr->name[i] == ' ')
						{
							temp[i] = '\0';
							break;
						}
						else
						{
							temp[i] = nodeptr->name[i];
							nameLen++;
						}
					}
					preput(temp, 0);
					preput(" ", 2);
				}
			}
		}
		preput("\n", 2);
		for (int i = 0; i < 15; i++) //��һ��ѭ��˳���ж�ֱ���Ƿ�Ϊ�ļ��У����ļ��оͽ��ļ��У���ӡ
		{
			fseek(fat12, begin + i * 32, 0);
			fread(nodeptr, 1, 32, fat12);
			if ((nodeptr->name[0] >= 'A' && nodeptr->name[0] <= 'Z') || (nodeptr->name[0] >= 'a' && nodeptr->name[0] <= 'z') || (nodeptr->name[0] >= '0' && nodeptr->name[0] <= '9'))
			{
			}
			else
			{
				continue;
			}
			char temppath[64];
			char nowName[64];
			int tempbegin;
			if (nodeptr->attribute == 0x10) //������ļ��У����ļ�������д�뵱ǰ·��
			{
				char* gang = "/";
				if (strlen(path) == 0 || path == "/")
				{
					path = "/";
				}
				else
				{
					strcat(path, gang);
				}
				for (int i = 0; i < 64; i++)
				{
					temppath[i] = path[i];
					if (path[i] == '\0' || path[i] == ' ')
					{
						temppath[i] = '\0';
						break;
					}
				} //����ǰ·���浽temppath��
				for (int i = 0; i < 64; i++)
				{
					nowName[i] = nodeptr->name[i];
					if (nodeptr->name[i] == '\0' || nodeptr->name[i] == ' ')
					{
						nowName[i] = '\0';
						break;
					}
				}
				strcat(temppath, nowName); //д�뵱ǰ·��

				tempbegin = (nodeptr->startSec + 31) * 512; //��ת�����ļ���ָ��Ĵ�

				ls(fat12, tempbegin, temppath, 0); //�ݹ�ls
			}
		}
	}
	return;
}

/*
 * load ������
 * �ݹ��ɨ����ļ����µ��ļ�����Ϊtxt �ļ�������������Ϊ�ļ��У����ȴ棬�ٵݹ�ص��ô˷�����
 * ����һ���ļ����´�Ų�����15���ļ�
 *
 */

void load(char path[64], int begin, FILE* fat12)
{
	//-----
	struct Node node0;
	struct Node* nodeptr = &node0; //��ʼ���ڵ�ָ��
	for (int i = 0; i < 15; i++)   //һ��ѭ��˳���ж�ֱ���Ƿ�Ϊ�ļ��У����ļ��оͽ��ļ���
	{
		fseek(fat12, begin + i * 32, 0);
		fread(nodeptr, 1, 32, fat12);
		if ((nodeptr->name[0] >= 'A' && nodeptr->name[0] <= 'Z') || (nodeptr->name[0] >= 'a' && nodeptr->name[0] <= 'z') || (nodeptr->name[0] >= '0' && nodeptr->name[0] <= '9'))
		{
		}
		else
		{
			continue;
		}
		char temppath[64];
		char nowName[64];
		int tempbegin;
		if (nodeptr->attribute == 0x10 || nodeptr->attribute == 0x00) //������ļ��У����ļ�������д�뵱ǰ·��
		{
			char* gang = "/";
			if (strlen(path) == 0 || path == "/")
			{
				path = "/";
			}
			else
			{
				strcat(path, gang);
			}
			for (int i = 0; i < 64; i++)
			{
				temppath[i] = path[i];
				if (path[i] == '\0' || path[i] == ' ')
				{
					temppath[i] = '\0';
					break;
				}
			} //����ǰ·���浽temppath��
			for (int i = 0; i < 64; i++)
			{
				nowName[i] = nodeptr->name[i];
				if (nodeptr->attribute == 0x10)
				{
					if (nodeptr->name[i] == '\0' || nodeptr->name[i] == ' ')
					{
						nowName[i] = '\0';
						break;
					}
				}
				if (nodeptr->attribute == 0x00)
				{
					if (nodeptr->name[i] == '\0' || nodeptr->name[i] == ' ')
					{
						nowName[i] = '.';
						nowName[i + 1] = 'T';
						nowName[i + 2] = 'X';
						nowName[i + 3] = 'T';
						nowName[i + 4] = '\0';
						break;
					}
				}
			}
			strcpy(table[t_size].name, nowName);
			preput(table[t_size].name, 2);
			preput("\n", 2);
			strcat(temppath, nowName); //д�뵱ǰ·��
			strcpy(table[t_size].path, temppath);
			preput(table[t_size].path, 2);
			preput("\n", 2);
			tempbegin = (nodeptr->startSec + 31) * 512; //���ļ���ָ��Ĵ�
			table[t_size].startSec = tempbegin;
			t_size++;
			if (nodeptr->attribute == 0x00)
			{
				table[t_size].fileSize = nodeptr->fileSize;
			}
			if (nodeptr->attribute == 0x10)
			{
				load(temppath, tempbegin, fat12); //�ݹ�ls
			}
		}
	}
	return;
}

int countdir(int begin, FILE* fat12)
{
	struct Node node0;
	struct Node* nodeptr = &node0; //��ʼ���ڵ�ָ��
	int dirNum = 0;
	for (int i = 0; i < 15; i++) //ֱ�ӱ���ֱ����
	{
		fseek(fat12, begin + i * 32, 0);
		fread(nodeptr, 1, 32, fat12);
		if ((nodeptr->name[0] >= 'A' && nodeptr->name[0] <= 'Z') || (nodeptr->name[0] >= 'a' && nodeptr->name[0] <= 'z') || (nodeptr->name[0] >= '0' && nodeptr->name[0] <= '9'))
		{
		}
		else
		{
			continue;
		}
		if (nodeptr->attribute == 0x10)
		{
			dirNum++;
		}
	}
	return dirNum;
}

int counttxt(int begin, FILE* fat12)
{
	struct Node node0;
	struct Node* nodeptr = &node0; //��ʼ���ڵ�ָ��
	int txtNum = 0;
	for (int i = 0; i < 15; i++) //ֱ�ӱ���ֱ����
	{
		fseek(fat12, begin + i * 32, 0);
		fread(nodeptr, 1, 32, fat12);
		if ((nodeptr->name[0] >= 'A' && nodeptr->name[0] <= 'Z') || (nodeptr->name[0] >= 'a' && nodeptr->name[0] <= 'z') || (nodeptr->name[0] >= '0' && nodeptr->name[0] <= '9'))
		{
		}
		else
		{
			continue;
		}
		if (nodeptr->attribute == 0x00)
		{
			txtNum++;
		}
	}
	return txtNum;
}

int findDir(char str[64])
{
	int res = -1;
	for (int i = 0; i < t_size; i++)
	{
		if (strcmp(str, table[i].path) == 0)
		{
			return table[i].startSec;
		}
	}
	return res;
}

int findtxtSize(char str[64])
{
	int res = -1;
	for (int i = 0; i < t_size; i++)
	{
		if (strcmp(str, table[i].path) == 0)
		{
			return table[i].fileSize;
		}
	}
	return res;
}

void printTxtName(char name[11])
{
	char temp[11];
	for (int i = 0; i < 8; i++)
	{
		if (name[i] == '\0' || name[i] == ' ')
		{
			temp[i] = '\0';
			break;
		}
		else
		{
			temp[i] = name[i];
		}
	}
	preput(temp, 1);
	preput(".TXT", 1);
	preput(" ", 2);
}
int startwith(char x1[64], char x2[64])
{
	int len = 0;
	while (x2[len] != '\0')
	{
		len++;
	}
	int res = 1;
	for (int i = 0; i < len; i++)
	{
		if (x1[i] != x2[i])
		{
			res = 0;
			break;
		}
	}
	return res;
}

void preput(char x1[1024], int color)
{
	int len = 0;
	char put[512];
	while (x1[len] != '\0')
	{
		len++;
	}
	strcpy(put, x1);
	printf("%s", put);
	//output(put, color, len);
}

void cat(FILE* fat12, char* str1)
{
	char fileContent[2048];
	int filepos = -1;
	for (int i = 0; i < t_size; i++)
	{
		if (strcmp(table[i].path, str1) == 0)
		{
			filepos = i;
			break;
		}
	}
	if ((filepos >= 0) && ((table[filepos].startSec / 512 - 31) >= 2))
	{
		fseek(fat12, table[filepos].startSec, 0);
		fread(fileContent, 1, 1024, fat12);
		preput(fileContent, 2);
		preput("\n", 2);
	}
}

// ʮ��������ת�ִ�
char* itoa(int value, char* result, int base)
{
	// ����Ƿ�Ϊ�Ϸ�������
	if (base < 2 || base > 36)
	{
		*result = '\0';
		return result;
	}

	char* ptr = result, * ptr1 = result, tmp_char;
	int tmp_value;

	do
	{
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
	} while (value);

	// ��Ӧ����
	if (tmp_value < 0)
	{
		*ptr++ = '-';
	}
	*ptr-- = '\0';
	while (ptr1 < ptr)
	{
		tmp_char = *ptr;
		*ptr-- = *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}