#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <time.h>

struct n
{
	int address;
	char flag;
	int next;
};

struct n cache[65536];
int head[65536];
int tail[65536];
int fifohead[65536];
int fifotail[65536];

int cachesize;
int datacachesize;
int datac;
int instructioncachesize;
int instc;
int blocksize;

int funcmode;
int writemode;
int isdivide;
int blockc;
int listindex;
int replacemode;
int isprefetch;
int nothitmode;
int loc;

float hitc;
float tolc;
float hitp;
FILE *fp;
char add[10];
int addforstruct;
float h1, h2, h0, t0, t1, t2;
char mode;
char filename[10];

void initformain(void);
void startexecute(void);
int findloc(int strlen);
void printresult(void);

void lru(void);
void fifo(void);
void random(void);

void lrufind(int i);
void lrunotfind(void);

void geditmain(void);
void printscreen(int kind);

//int main()
//{
//	srand((int) time(0));
//	cachesize = 2 * 1024;
//	blocksize = 256;
//	datacachesize = 1024;
//	instructioncachesize = 1024;
//	datac = datacachesize / blocksize;
//	instc = instructioncachesize / blocksize;
//	blockc = cachesize / blocksize;
//	funcmode = 2;
//	replacemode = 1;
//	writemode = 0;
//	isprefetch = 0;
//	isdivide = 1;
//	nothitmode = 0;
//
//	strcpy(filename, "instructions\\cc1.din");
//
//
//	geditmain();
//
//
//	system("pause");
//	return 0;
//}
void geditmain(void)
{
	char cmd[10];
	printscreen(0);
	scanf("%s", cmd);
	while (strcmp(cmd, "q") != 0)
	{
		if (strcmp(cmd, "ccs") == 0) printscreen(1);
		else if (strcmp(cmd, "cbs") == 0) printscreen(2);
		else if (strcmp(cmd, "cfm") == 0) printscreen(3);
		else if (strcmp(cmd, "crm") == 0) printscreen(4);
		else if (strcmp(cmd, "cwm") == 0) printscreen(5);
		else if (strcmp(cmd, "cid") == 0) printscreen(6);
		else if (strcmp(cmd, "cif") == 0) printscreen(7);
		else if (strcmp(cmd, "cnm") == 0) printscreen(8);
		else if (strcmp(cmd, "cfn") == 0) printscreen(9);
		else if (strcmp(cmd, "cdcs") == 0) printscreen(10);
		else if (strcmp(cmd, "cics") == 0) printscreen(11);
		else if (strcmp(cmd, "ok") == 0) startexecute();
		else break;
		printscreen(0);
		scanf("%s", cmd);
	}
	return;
}
void printscreen(int kind)
{
	int i;
	int temp;

	for (i = 0; i<30; i++) printf("*");
	printf("\n");
	switch (kind)
	{
	case 0:
		if (isdivide == 0)
		{
			printf("cachesize=%dK,(to change it, enter \"ccs\")\n", cachesize / 1024);
			//printf("blockcount=%d\n",cachesize/blocksize);
		}
		else if (isdivide == 1)
		{
			printf("datacachesize=%dK,(to change it, enter\"cdcs\")\n", datacachesize / 1024);
			printf("instructionsize=%dK,(to change it, enter\"cics\")\n", instructioncachesize / 1024);
			//printf("datacachecount=%d\n",datacachesize/blocksize);
			//printf("instructioncount=%d\n",datacachesize/blocksize);
		}
		printf("blocksize=%dB,(to change it, enter \"cbs\")\n", blocksize);
		if (funcmode == 0) printf("funcmode: direct(to change it, enter \"cfm\")\n");
		else printf("funcmode:%d-way(to change it, enter \"cfm\")\n", funcmode);
		if (replacemode == 0) printf("replacemode:LRU(to change it, enter \"crm\")\n");
		else if (replacemode == 1) printf("replacemode:FIFO(to change it, enter \"crm\")\n");
		else if (replacemode == 2) printf("replacemode:RANDOM(to change it, enter \"crm\")\n");
		if (writemode == 0) printf("writemode:writeback(to change it, enter \"cwm\")\n");
		else printf("writemode:writethrough(to change it, enter \"cwm\")\n");
		if (isdivide == 0) printf("isdevide:FALSE(to change it, enter \"cid\")\n");
		else printf("isdevide:TRUE(to change it, enter \"cid\")\n");
		if (isprefetch == 0) printf("isprefetch:FALSE(to change it, enter \"cif\")\n");
		else printf("isprefetch:TRUE(to change it, enter \"cif\")\n");
		if (nothitmode == 0) printf("not hit mode is arranged by writing(to change it, press \"cnm\")\n");
		else printf("not hit mode is not arranged by writing(to change it, press \"cnm\")\n");
		printf("the file executed is %s(to change it, enter \"cfn\")\n", filename);
		printf("TO EXIT: press \"q\"\n");
		printf("TO EXECUTE: press\"ok\"\n");
		break;
	case 1:
		printf("please enter the size of cache(K):\n");
		scanf("%d", &cachesize);
		printf("cachesize has been changed to %dK.\n", cachesize);
		cachesize *= 1024;
		break;
	case 2:
		printf("please enter the size of block(B):\n");
		scanf("%d", &blocksize);
		printf("piecesize has been changed to %dB.\n", blocksize);
		break;
	case 3:
		printf("please enter the funcmode, with 1 represent to direct mode and other n represents to n-way mode\n");
		scanf("%d", &funcmode);
		if (funcmode == 1) printf("funcmode has been changed to direct mode\n");
		else printf("funcmode has been changed to %d-way mode.\n", funcmode);
		break;
	case 4:
		printf("please enter the replacemode, with 0 represent to LRU,1 represents to FIFO,and 2 repersents to RANDOM mode\n");
		scanf("%d", &replacemode);
		if (replacemode == 0) printf("replacemode has been changed to LRU.\n");
		else if (replacemode == 1) printf("replacemode has been changed to FIFO.\n");
		else if (replacemode == 2) printf("replacemode has been changed to RANDOM.\n");
		break;
	case 5:
		printf("please enter the writecmode, with 0 represent to writeback and 1 represent to writethrough\n");
		scanf("%d", &writemode);
		if (writemode == 0) printf("writemode has been changed to writeback\n");
		else printf("writemode has been changed to writethrough.\n");
		break;
	case 6:
		printf("please enter the mode for the cache, with 0 represents to not to divide the cache and 1 represents to divide\n");
		scanf("%d", &isdivide);
		if (isdivide == 0) printf("the cache is not divided.\n");
		else printf("the cache is divided.\n");
		break;
	case 7:
		printf("please enter the mode for the prefetch mode, with 0 represents not using prefetch and 1 represents prefetch\n");
		scanf("%d", &isprefetch);
		if (isprefetch == 0) printf("not using prefetch.\n");
		else printf("using prefetch.\n");
		break;
	case 8:
		printf("please enter the method for not hitting, with 0 represents arrange according to writing, 1 represents not to arrange according to writing.\n");
		scanf("%d", &nothitmode);
		if (nothitmode == 0) printf("not hitting method has changed to write according to writing.\n");
		else printf("not hitting method is not to arrange according to writing.\n");
		break;
	case 9:
		printf("please enter the file name:\n");
		scanf("%s", filename);
		printf("filename has been changed to %s\n", filename);
		break;
	case 10:
		printf("please enter the size for the datacache(K):\n");
		scanf("%d", &datacachesize);
		printf("datacache has been set to %dK\n", datacachesize);
		datacachesize *= 1024;
		break;
	case 11:
		printf("please enter the size for the instruction cache(K):\n");
		scanf("$d", &instructioncachesize);
		printf("instruction cache has been set to %dK.\n", instructioncachesize);
		instructioncachesize *= 1024;
		break;
	default:
		break;
	}
	for (i = 0; i<30; i++) printf("*");
	printf("\n");
	return;
}
void initformain(void)
{
	int i;
	int tempblockc;
	if (isdivide == 0) tempblockc = blockc;
	else tempblockc = datac + instc;
	for (i = 0; i<tempblockc; i++)
	{
		cache[i].flag = 'f';
		cache[i].address = -1;
		cache[i].next = -1;
		fifohead[i] = 0;
		fifotail[i] = 0;
		head[i] = -1;
		tail[i] = -1;
	}

}
void startexecute(void)
{
	int addlen;
	char temp;

	hitc = 0;
	tolc = 0;
	h1 = 0, h2 = 0, h0 = 0;
	t0 = 0, t1 = 0, t2 = 0;
	datac = datacachesize / blocksize;
	instc = instructioncachesize / blocksize;
	blockc = cachesize / blocksize;
	initformain();
	if ((fp = fopen(filename, "r")) == NULL)
	{
		printf("can not open this file\n");
		exit(0);
	}
	while (!feof(fp))
	{
		if (!feof(fp)) fscanf(fp, "%c", &mode);
		if (!feof(fp)) fscanf(fp, "%s", add);
		fscanf(fp, "%c", &temp);
		while (!feof(fp) && temp != '\n')
		{
			fscanf(fp, "%c", &temp);
		}
		if (mode == '0' || mode == '1' || mode == '2')
		{
			addlen = strlen(add);
			loc = findloc(addlen);

			tolc += 1;
			switch (mode)
			{
			case '0':
				t0 += 1;
				break;
			case '1':
				t1 += 1;
				break;
			case '2':
				t2 += 1;
				break;
			default:
				break;
			}
			if (replacemode == 0)
			{
				lru();
			}
			else if (replacemode == 1)
			{
				fifo();
			}
			else if (replacemode == 2)
			{
				random();
			}
		}
	}
	printresult();
	fclose(fp);
}
void printresult(void)
{
	hitp = hitc / tolc;
	printf("模拟结果：\n");
	printf("访问总次数：%d，不命中次数：%d，不命中率：%f\n", (int) tolc, (int) (tolc - hitc), 1 - hitp);
	printf("其中:\n");
	printf("读指令次数：%d,\t不命中次数：%d，\t不命中率：%f\n", (int) t2, (int) (t2 - h2), 1 - h2 / t2);
	printf("读数据次数：%d,\t不命中次数：%d，\t不命中率：%f\n", (int) t0, (int) (t0 - h0), 1 - h0 / t0);
	printf("写数据次数：%d,\t不命中次数：%d，\t不命中率：%f\n", (int) t1, (int) (t1 - h1), 1 - h1 / t1);
}
int findloc(int strlen)
{
	int temp;
	int total;
	int i;
	int addition;
	int curblockc;
	total = 0;
	temp = 0;
	for (i = 0; i<strlen; i++)
	{
		if (add[i] >= '0'&&add[i] <= '9')
		{
			temp = add[i] - '0';
		}
		else temp = add[i] - 'a' + 10;
		total *= 16;
		total += temp;
	}

	if (isdivide == 0)
	{
		curblockc = blockc;
		addition = 0;
	}
	else if (mode == '2')
	{
		curblockc = instc;
		addition = 0;
	}
	else
	{
		curblockc = datac;
		addition = instc;
	}

	total = total / blocksize;
	addforstruct = total;
	listindex = total % (curblockc / funcmode);
	total = listindex*funcmode;
	total += addition;
	listindex += addition / funcmode;

	return total;
}
void lru(void)
{
	int i;

	for (i = 0; i<funcmode; i++)
	{
		if (cache[loc + i].flag == 't' && cache[loc + i].address == addforstruct)
		{
			break;
		}
	}
	if (i<funcmode)
	{
		switch (mode)
		{
		case '0':
			h0 += 1;
			break;
		case '1':
			h1 += 1;
			break;
		case '2':
			h2 += 1;
			break;
		default:
			break;
		}
		hitc += 1;
		lrufind(i);
	}
	else
	{
		if (nothitmode == 1 && mode == '1') return;
		lrunotfind();
	}
	return;
}
void fifo(void)
{
	int i;
	int j;
	int k;
	for (i = 0; i<funcmode; i++)
	{
		if (cache[i + loc].flag == 't'&& cache[i + loc].address == addforstruct)
		{
			break;
		}
	}
	if (i<funcmode)
	{
		switch (mode)
		{
		case '0':
			h0 += 1;
			break;
		case '1':
			h1 += 1;
			break;
		case '2':
			h2 += 1;
			break;
		default:
			break;
		}
		hitc += 1;
	}
	else
	{
		if (nothitmode == 1 && mode == '1') return;
		for (j = 0; j<funcmode; j++)
		{
			if (cache[loc + j].flag == 'f') break;
		}
		if (j<funcmode)
		{
			cache[j + loc].next = fifotail[listindex];
			cache[j + loc].address = addforstruct;
			cache[j + loc].flag = 't';
			fifotail[listindex] = (fifotail[listindex] + 1) % funcmode;
		}
		else
		{
			for (k = 0; k<funcmode; k++)
			{
				if (cache[k + loc].next == fifohead[listindex]) break;
			}
			if (k<funcmode)
			{
				cache[k + loc].address = addforstruct;
				cache[k + loc].next = fifotail[listindex];
			}
			else
			{
				printf("error3\n");
				system("pause");
				exit(0);
			}
			fifohead[listindex] = (fifohead[listindex] + 1) % funcmode;
			fifotail[listindex] = (fifotail[listindex] + 1) % funcmode;
		}
	}
	return;
}
void random(void)
{
	int i;
	int j;
	int addi;
	for (i = 0; i<funcmode; i++)
	{
		if (cache[i + loc].flag == 't'&& cache[i + loc].address == addforstruct)
		{
			break;
		}
	}
	if (i<funcmode)
	{
		switch (mode)
		{
		case '0':
			h0 += 1;
			break;
		case '1':
			h1 += 1;
			break;
		case '2':
			h2 += 1;
			break;
		default:
			break;
		}
		hitc += 1;
	}
	else
	{
		if (nothitmode == 1 && mode == '1') return;
		for (j = 0; j<funcmode; j++)
		{
			if (cache[loc + j].flag == 'f') break;
		}
		if (j<funcmode)
		{
			cache[j + loc].address = addforstruct;
			cache[j + loc].flag = 't';
		}
		else
		{
			addi = rand() % funcmode;
			cache[addi + loc].address = addforstruct;
		}
	}
	return;
}
void lrufind(int i)
{
	int temp;

	temp = head[listindex];
	if (temp == loc + i)
	{
		if (cache[temp].next != -1)
		{
			head[listindex] = cache[temp].next;
			cache[tail[listindex]].next = temp;
			tail[listindex] = temp;
			cache[temp].next = -1;
		}
	}
	else
	{
		while (temp != -1 && cache[temp].next != loc + i)
		{
			temp = cache[temp].next;
		}
		if (temp == -1)
		{
			printf("error1\n");
			system("pause");
			exit(0);
		}
		else
		{
			if (cache[loc + i].next != -1)
			{
				cache[temp].next = cache[loc + i].next;
				cache[tail[listindex]].next = loc + i;
				tail[listindex] = loc + i;
				cache[loc + i].next = -1;
			}
		}
	}
	return;
}
void lrunotfind(void)
{
	int i;
	int temp;
	for (i = 0; i<funcmode; i++)
	{
		if (cache[loc + i].flag == 'f')
		{
			break;
		}
	}
	if (i<funcmode)
	{
		cache[loc + i].address = addforstruct;
		cache[loc + i].flag = 't';
		if (head[listindex] == -1)
		{
			head[listindex] = loc + i;
			tail[listindex] = loc + i;
			cache[loc + i].next = -1;
		}
		else
		{
			cache[tail[listindex]].next = loc + i;
			tail[listindex] = loc + i;
			cache[loc + i].next = -1;
		}
	}
	else
	{
		temp = head[listindex];
		cache[temp].address = addforstruct;
		if (cache[temp].next != -1)
		{
			head[listindex] = cache[head[listindex]].next;
			cache[tail[listindex]].next = temp;
			tail[listindex] = temp;
			cache[temp].next = -1;
		}
	}
	return;
}
