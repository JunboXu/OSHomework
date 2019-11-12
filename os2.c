#include <stdio.h>
#include <string.h>
# include <stdlib.h>
//ȫ�ֶ�����
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
//-------------------

//�ṹ��������ȫ�ֱ���������
struct Node {
	char name[11];
	byte attribute;//����
	char reserve[10];//�����ֶ�
	word time1;
	word time2;
	word startSec;//��ʼ����
	dword fileSize;
};

struct Table {
	char path[64];
	char name[11];
	byte attribute;
	word startSec;
	dword fileSize;
};

struct Table table[100];
int t_size = 0;

void preput(char x1[1024], int color);//��ӡ����
void input(FILE* fat);
void printTxtName(char name[11]);
void ls(FILE* fat12, int begin, char* path, int hasParam);//hasParam =0 -->ls();   1-->ls -l; ;  2-->ls -l ·��
int  startwith(char x1[64], char x2[64]);
int findDir(char str[64]);
void load(char name[64], int begin, FILE* fat12);
void lsl(FILE* fat12, int begin, char path[64], int hasParam);//ls -l
int countdir(int begin, FILE* fat12);
int counttxt(int begin, FILE* fat12);

int main() {
	preput("SYSTEM START!",0);
	preput("\n", 0);
	FILE* fat12 = fopen("ref.img", "rb"); //���ļ�
	load("/",19*512,fat12);
	input(fat12);
}

void input(FILE* fat12) {
	char str1[64];
	char str2[64];
	char str3[64];
	while (1){
		preput("PLEASE INPUT:", 0);
		scanf("%s", str1);//��ָ��
		if (strcmp(str1,"exit")==0)
		{
			break;
		}
		else if (strcmp(str1,"cat")==0)
		{
			scanf("%s", str2);//��·��������str2��

		}
		else if (strcmp(str1, "ls") == 0) {//�����lsָ��
			char c = getchar();
			if (c == '\n') {//���������������Ϊls()ָ��Ӹ�Ŀ¼��ʼ���
				ls(fat12, 19 * 512, "", 0);
			}
			else {//�еڶ���ָ��
				scanf("%s", str2);
				if (startwith(str2, "-l") || startwith(str2, "-ll"))
				{
					char c2 = getchar();
					if (c2 == '\n')//ls -l()
					{
						lsl(fat12, 19 * 512, "", 1);
					}
					else {
						scanf("%s", str3);
						int beginx = findDir(str3);
						lsl(fat12, beginx, str3, 1);
					}
				}
				else {
					if (startwith(str2, "/"))
					{
						char c3 = getchar();
						if (c3 == '\n')
						{//ls + ·��
							int beginx = findDir(str2);//findDir ����str2Ŀ¼��startsec
							if (beginx==-1)
							{
								preput("δ�ҵ�·��\n", 0);
								preput("\n", 0);
							}
							else {

								ls(fat12, beginx, str2, 0);
							}
						}
						else {
							scanf("%s", str3);
							if (startwith(str3, "-l") || startwith(str3, "-ll"))
							{//�е��������룬�ҺϷ�

							}
							else {//�Ƿ�����
								preput("����������Ƿ�", 0);
								preput("\n", 0);
							}
						}
					}
				}
			}
		}
		else {
			preput("ָ������ע���Сд",0);
		}
	}
}

void lsl(FILE* fat12, int begin, char path[64], int hasParam) {
	//-----
	struct Node  node0;
	struct Node* nodeptr = &node0; //��ʼ���ڵ�ָ��
	if (strlen(path)==0||path=="/")
	{
		path = "/";
	}
	preput(path, 0);
	preput(" ", 0);
	int beginx = findDir(path);
	int dirn=countdir(begin, fat12);
	int txtn = counttxt(begin, fat12);
	char str[25];
	preput(_itoa(dirn,str,10), 0);
	preput(" ", 0);
	preput(_itoa(txtn, str, 10), 0);
	preput(":", 0);
	preput("\n", 0);

	for (int i = 0; i < 15; i++)//��һ�������ӡֱ������
	{
		fseek(fat12, begin + i * 32, 0);
		fread(nodeptr, 1, 32, fat12);
		if ((nodeptr->name[0] >= 'A' && nodeptr->name[0] <= 'Z') || (nodeptr->name[0] >= 'a' && nodeptr->name[0] <= 'z') || (nodeptr->name[0] >= '0' && nodeptr->name[0] <= '9'))
		{
		}
		else { continue; }
		if (nodeptr->attribute==0x10)//������ļ���
		{
			int nameLen = 0;
			char temp[11];
			for (int i = 0; i < 11; i++)
			{
					if (nodeptr->name[i] == '\0' || nodeptr->name[i] == ' ') {
						temp[i] = '\0';
						break;
					}
					else {
						temp[i] = nodeptr->name[i];
						nameLen++;
					}
			}
			preput(temp, 0);
			preput(" ", 0);//��ӡ�ļ�������
			int beginx = (nodeptr->startSec+31)*512;
			int dirn = countdir(beginx, fat12);
			int txtn = counttxt(beginx, fat12);
			char str[25];
			preput(_itoa(dirn, str, 10), 0);
			preput(" ", 0);
			preput(_itoa(txtn, str, 10), 0);
			preput("\n", 0);
		}
		else if (nodeptr->attribute==0x00)
		{//������ı��ļ�����ӡ��С
			printTxtName(nodeptr->name);
			preput(" ", 0);
			char str[25];
			preput(_itoa(nodeptr->fileSize,str,10), 0);
			preput("\n", 0);
		}
	}
	for (int i = 0; i < 15; i++)//��һ��ѭ��˳���ж�ֱ���Ƿ�Ϊ�ļ��У����ļ��оͽ��ļ��У���ӡ
	{
		fseek(fat12, begin + i * 32, 0);
		fread(nodeptr, 1, 32, fat12);
		if ((nodeptr->name[0] >= 'A' && nodeptr->name[0] <= 'Z') || (nodeptr->name[0] >= 'a' && nodeptr->name[0] <= 'z') || (nodeptr->name[0] >= '0' && nodeptr->name[0] <= '9'))
		{
		}
		else { continue; }
		char temppath[64];
		char nowName[64];
		int tempbegin;
		if (nodeptr->attribute == 0x10)//������ļ��У����ļ�������д�뵱ǰ·��
		{
			char* gang = "/";
			if (strlen(path) == 0 || path == "/")
			{
				path = "/";
			}
			else {
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
			}//����ǰ·���浽temppath��
			for (int i = 0; i < 64; i++)
			{
				nowName[i] = nodeptr->name[i];
				if (nodeptr->name[i] == '\0' || nodeptr->name[i] == ' ')
				{
					nowName[i] = '\0';
					break;
				}
			}
			strcat(temppath, nowName);//д�뵱ǰ·��

			tempbegin = (nodeptr->startSec + 31) * 512;//��ת�����ļ���ָ��Ĵ�

			lsl(fat12, tempbegin, temppath, 0);//�ݹ�ls
		}
	}
	return;

}

void ls(FILE* fat12, int begin, char path[64], int hasParam) {
	struct Node  node0;
	struct Node* nodeptr = &node0; //��ʼ���ڵ�ָ��
	if (hasParam == 0)
	{
		preput(path, 0);
		preput("/:", 0);
		preput("\n", 0);
		if (strlen(path) != 0)
		{
			preput(". .. ", 0);
		}
		for (int i = 0; i < 15; i++)//��һ��ѭ����ӡ��ֱ���ļ��к��ļ�
		{
			fseek(fat12, begin + i * 32, 0);
			fread(nodeptr, 1, 32, fat12);
			if ((nodeptr->name[0] >= 'A' && nodeptr->name[0] <= 'Z') || (nodeptr->name[0] >= 'a' && nodeptr->name[0] <= 'z') || (nodeptr->name[0] >= '0' && nodeptr->name[0] <= '9'))
			{
			}
			else { continue; }
			if (nodeptr->attribute == 0x10 || nodeptr->attribute == 0x00)
			{
				if (nodeptr->attribute == 0x00)//�ı��ļ�
				{
					printTxtName(nodeptr->name);
					preput(" ", 0);
				}
				if (nodeptr->attribute == 0x10)//�ļ���
				{
					int nameLen = 0;
					char temp[11];
					for (int i = 0; i < 11; i++)
					{
						if (nodeptr->name[i] == '\0' || nodeptr->name[i] == ' ') {
							temp[i] = '\0';
							break;
						}
						else {
							temp[i] = nodeptr->name[i];
							nameLen++;
						}
					}
					preput(temp, 0);
					preput(" ", 0);
				}
			}
		}
		preput("\n", 0);
		for (int i = 0; i < 15; i++)//��һ��ѭ��˳���ж�ֱ���Ƿ�Ϊ�ļ��У����ļ��оͽ��ļ��У���ӡ
		{
			fseek(fat12, begin + i * 32, 0);
			fread(nodeptr, 1, 32, fat12);
			if ((nodeptr->name[0] >= 'A' && nodeptr->name[0] <= 'Z') || (nodeptr->name[0] >= 'a' && nodeptr->name[0] <= 'z') || (nodeptr->name[0] >= '0' && nodeptr->name[0] <= '9'))
			{
			}
			else { continue; }
			char temppath[64];
			char nowName[64];
			int tempbegin;
			if (nodeptr->attribute == 0x10)//������ļ��У����ļ�������д�뵱ǰ·��
			{
				char* gang = "/";
				if (strlen(path) == 0 || path == "/")
				{
					path = "/";
				}
				else {
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
				}//����ǰ·���浽temppath��
				for (int i = 0; i < 64; i++)
				{
					nowName[i] = nodeptr->name[i];
					if (nodeptr->name[i] == '\0' || nodeptr->name[i] == ' ')
					{
						nowName[i] = '\0';
						break;
					}
				}
				strcat(temppath, nowName);//д�뵱ǰ·��

				tempbegin = (nodeptr->startSec + 31) * 512;//��ת�����ļ���ָ��Ĵ�

				ls(fat12, tempbegin, temppath, 0);//�ݹ�ls
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

void load(char path[64], int begin, FILE* fat12) {
	//-----
	struct Node  node0;
	struct Node* nodeptr = &node0; //��ʼ���ڵ�ָ��
	for (int i = 0; i < 15; i++)//һ��ѭ��˳���ж�ֱ���Ƿ�Ϊ�ļ��У����ļ��оͽ��ļ���
	{
		fseek(fat12, begin + i * 32, 0);
		fread(nodeptr, 1, 32, fat12);
		if ((nodeptr->name[0] >= 'A' && nodeptr->name[0] <= 'Z') || (nodeptr->name[0] >= 'a' && nodeptr->name[0] <= 'z') || (nodeptr->name[0] >= '0' && nodeptr->name[0] <= '9'))
		{
		}
		else { continue; }
		char temppath[64];
		char nowName[64];
		int tempbegin;
		if (nodeptr->attribute == 0x10|| nodeptr->attribute == 0x00)//������ļ��У����ļ�������д�뵱ǰ·��
		{
			char* gang = "/";
			if (strlen(path) == 0 || path == "/")
			{
				path = "/";
			}
			else {
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
			}//����ǰ·���浽temppath��
			for (int i = 0; i < 64; i++)
			{
				nowName[i] = nodeptr->name[i];
				if (nodeptr->attribute==0x10)
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
						nowName[i+1] = 'T';
						nowName[i+2] = 'X';
						nowName[i+3] = 'T';
						nowName[i + 4] = '\0';
						break;
					}
				}
			}
			strcpy(table[t_size].name, nowName);
			preput(table[t_size].name, 0);
			preput("\n", 0);
			strcat(temppath, nowName);//д�뵱ǰ·��
			strcpy(table[t_size].path, temppath);
			preput(table[t_size].path,0);
			preput("\n", 0);
			tempbegin = (nodeptr->startSec + 31) * 512;//���ļ���ָ��Ĵ�
			table[t_size].startSec = tempbegin;
			t_size++;
			if (nodeptr->attribute==0x00)
			{
				table[t_size].fileSize = nodeptr->fileSize;
			}
			if (nodeptr->attribute == 0x10)
			{
				load(temppath, tempbegin, fat12);//�ݹ�ls
			}
		}
	}
	return;
}

int countdir(int begin, FILE* fat12) {
	struct Node  node0;
	struct Node* nodeptr = &node0; //��ʼ���ڵ�ָ��
	int dirNum=0;
	for (int  i = 0; i < 15; i++)//ֱ�ӱ���ֱ����
	{
		fseek(fat12, begin + i * 32, 0);
		fread(nodeptr, 1, 32, fat12);
		if ((nodeptr->name[0] >= 'A' && nodeptr->name[0] <= 'Z') || (nodeptr->name[0] >= 'a' && nodeptr->name[0] <= 'z') || (nodeptr->name[0] >= '0' && nodeptr->name[0] <= '9'))
		{
		}
		else { continue; }
		if (nodeptr->attribute==0x10)
		{
			dirNum++;
		}
	}
	return dirNum;
}

int counttxt(int begin, FILE* fat12) {
	struct Node  node0;
	struct Node* nodeptr = &node0; //��ʼ���ڵ�ָ��
	int txtNum = 0;
	for (int i = 0; i < 15; i++)//ֱ�ӱ���ֱ����
	{
		fseek(fat12, begin + i * 32, 0);
		fread(nodeptr, 1, 32, fat12);
		if ((nodeptr->name[0] >= 'A' && nodeptr->name[0] <= 'Z') || (nodeptr->name[0] >= 'a' && nodeptr->name[0] <= 'z') || (nodeptr->name[0] >= '0' && nodeptr->name[0] <= '9'))
		{
		}
		else { continue; }
		if (nodeptr->attribute == 0x00)
		{
			txtNum++;
		}
	}
	return txtNum;
}

int findDir(char str[64]) {
	int res = -1;
	for (int i = 0; i < t_size; i++)
	{
		if (strcmp(str, table[i].path) == 0) {
			return table[i].startSec;
		}
	}
	return res;
}

void printTxtName(char name[11]) {
	char temp[11];
	for (int i = 0; i < 8; i++)
	{
		if (name[i] == '\0'||name[i]==' ') {
			temp[i] = '\0';
			break;
		}
		else {
			temp[i] = name[i];
		}
	}
	preput(temp, 1);
	preput(".txt", 1);
	preput(" ", 0);
}
int  startwith(char x1[64], char x2[64]) {
	int len = 0;
	while (x2[len] != '\0') {
		len++;
	}
	int res = 1;
	for (int i = 0; i < len; i++) {
		if (x1[i] != x2[i]) {
			res = 0;
			break;
		}
	}
	return res;
}

void preput(char x1[1024], int color) {
	int len = 0;
	char put[64];
	while (x1[len] != '\0') { len++; }
	strcpy(put, x1);
	printf("%s", put);
	//output(put, color, len);
}
