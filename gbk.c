// gbk.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h>
#include <locale.h>
#include <windows.h>
#include "data.h"

void listgbks(unsigned short dbs[], unsigned short dbtws[], unsigned int count);
void listPartGbks(unsigned short dbs[], unsigned short dbtws[], unsigned int count, unsigned int numPerZone);
void writegbks(unsigned short dbs[], unsigned short dbtws[], unsigned int count);
void writegbksByAsc(unsigned short dbs[], unsigned short dbtws[], unsigned int count);
void readgbks(void);
void samplegbks(void);
void convertbin(void);
void reduceListByZone();
void reduceListByBit();
void gbk2unicode();

#define		MaxChars	(1024*1024)
unsigned short *gOut[2] = {NULL};
unsigned int gOutCount	= 0;
const char codeFileName[32] = {"data.dat"};
const char codeFileNameAsc[32] = {"dataAsc.dat"};
const char gbkFileName[32] = {"data-gbk.dat"};
const char utf8FileName[32] = {"data-utf8.dat"};

const char gbk_han_head[128] = {"const unsigned short gbk_han[] = \r\n{  /* double bytes that map to words */"};
const char gbk_han_tail[32] = {"};\r\n\r\n\r\n"};

const char gbk_unicode_head[128] = {"const unsigned short gbk_han_unicode[] = \r\n{  /* double bytes that map to words */"};
const char gbk_unicode_tail[32] = {"};\r\n\r\n\r\n"};

int main(int argc, char* argv[])
{
	int opt = 0;
	//global init
	gOut[0] = (unsigned short *)malloc(MaxChars);
	memset(gOut[0],0x00,MaxChars);
	gOut[1] = (unsigned short *)malloc(MaxChars);
	memset(gOut[1],0x00,MaxChars);
	if(!gOut)
	{
		printf("malloc memory failed\n");
		return 0;
	}
	gbk2unicode();
	//
	while(1)
	{
		printf("\n********************main menu*****************\n");
		printf("01 list all gbk characters\n");
		printf("02 reduce gbk characters by zone\n");
		printf("03 reduce gbk characters by bit\n");
		printf("04 list reduced gbk characters\n");
		printf("05 write reduced gbk characters to code file\n");
		printf("06 read reduced gbk characters from code file\n");
		printf("07 convert code file to binary file\n");
		printf("08 sample gbk characters from code file\n");
		printf("09 write reduced gbk characters to code file by ascend\n");
		printf("00 exit\n");
		printf("*********************************************\n");
		printf("please input choose: ");
		scanf("%i",&opt);
		getchar();
		printf("\n");
		switch(opt)
		{
		case 1:
			{
				listgbks(_gbks,_gbktws,sizeof(_gbks)/sizeof(_gbks[0]));
			}
			break;
		case 2:
			{
				reduceListByZone();
			}
			break;
		case 3:
			{
				reduceListByBit();
			}
			break;
		case 4:
			{
				listgbks(gOut[0],gOut[1],gOutCount);
			}
			break;
		case 5:
			{
				writegbks(gOut[0],gOut[1],gOutCount);
			}
			break;
		case 6:
			{
				readgbks();
			}
			break;
		case 7:
			{
				convertbin();
			}
			break;
		case 8:
			{
				samplegbks();
			}
			break;
		case 9:
			{
				writegbksByAsc(gOut[0],gOut[1],gOutCount);
			}
			break;
		case 0:
			exit(0);
		default:
			printf("invalid choice, please choose again\n");
			break;
		}
	}
	
}

void gbk2UTF8(unsigned short unicode,unsigned char * utf)
{
	int len = 0,low = 0, mid = 0, high = 0;
		if(unicode <= 0x007F)
		{ 
			utf[len++] = unicode;
		}else if(unicode <= 0x07FF)
		{
			low = unicode & 0x3F;
			high = unicode >> 6;
			utf[len++] = (3 << 6) + high;
			utf[len++] = (2 << 6) + low;
			
		}else if(unicode <= 0xFFFF)
		{
			low = unicode & 0x3F;
			mid = (unicode >> 6) & 0x3F;
			high = unicode >> 12;
			utf[len++] = (7 << 5) + high;
			utf[len++] = (2 << 6) + mid;
			utf[len++] = (2 << 6) + low;

		}
}

void listgbks(unsigned short dbs[], unsigned short dbtws[], unsigned int count)
{
	int i = 0, k = 0, ret = 0;
	int lastzone = 0, curzone = 0, lastbit = 0, curbit = 0, line = 0, order = 0;
	unsigned char buff[6] = {0};

	setlocale(LC_CTYPE, "zh-CN");
	printf("console output %i characters of gbk\n",count);
	for(i = 0; i < count; i++)
	{
		curzone = buff[0] =  (dbs[i] & 0x0000FF00) >> 8;//区码
		curbit = buff[1] =  (dbs[i] & 0x000000FF);//位码
		line = (curbit & 0xF0) >> 4;
		order = (curbit & 0x0F);
		buff[2] = 0x00;

		if(curzone != lastzone)//换区
		{
			lastzone = curzone;
			printf("\n%02x 0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F",curzone);
		}
		if((curbit & 0xF0) != (lastbit & 0xF0))//换行
		{
			printf("\n%x\x20",line);
			for(k = 0; k < order; k++)//行内填充空格
				printf("\x20\x20\x20\x20\x20\x20");
		}else
		{
			order = (curbit & 0x0F) - (lastbit & 0x0F) - 1;
			for(k = 0; k < order; k++)//行内填充空格
				printf("\x20\x20\x20\x20\x20\x20");
		}
		//设置代码页，输出gbk
		lastbit = curbit;
		SetConsoleCP(936);
		SetConsoleOutputCP(936);
		printf("%s",buff);

		//设置代码页，输出unicode
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);
		
		if(dbtws)
		{
			memset(buff,0x00,sizeof(buff));
			gbk2UTF8(dbtws[i],buff);
			wprintf(L"%LS  ",buff);
		}else
		{
			printf("\x20\x20\x20\x20");
		}
	}
}


void listPartGbks(unsigned short dbs[], unsigned short dbtws[], unsigned int count, unsigned int numPerZone)
{
	int i = 0, k = 0, ret = 0;
	int lastzone = 0, curzone = 0, lastbit = 0, curbit = 0, line = 0, order = 0;
	unsigned char buff[6] = {0};
	unsigned int *pIndex = NULL, indexLen = 0;

	pIndex = (unsigned int *)malloc(MaxChars);
	if(!pIndex)
	{
		printf("malloc memory failed\n");
		goto ret;
	}
	setlocale(LC_CTYPE, "zh-CN");
	for(i = 0; i < count; i++)
	{
		curzone = buff[0] =  (dbs[i] & 0x0000FF00) >> 8;//区码
		if(curzone != lastzone)//换区
		{
			lastzone = curzone;
			k = 1;
			pIndex[indexLen++] = i;
		}else if(k < numPerZone)
		{
			k++;
			pIndex[indexLen++] = i;
		}
	}
	printf("\n##########################part gbk##################\n");
	for(i = 0; i < indexLen; i++)
	{
		curzone = buff[0] =  (dbs[pIndex[i]] & 0x0000FF00) >> 8;//区码
		curbit = buff[1] =  (dbs[pIndex[i]] & 0x000000FF);//位码
		buff[2] = 0x00;
		//设置代码页，输出gbk
		SetConsoleCP(936);
		SetConsoleOutputCP(936);
		printf("%s",buff);
	}
	printf("\n##################################################\n");
	printf("\n##########################part gbk unicode##################\n");
	for(i = 0; i < indexLen; i++)
	{
		printf("%02X ",dbtws[pIndex[i]]);
	}
	printf("\n############################################################\n");
ret:
	if(pIndex)
		free(pIndex);
}


void listzone(unsigned short dbs[], unsigned short dbtws[], unsigned int count)
{
	int i = 0, k = 0;
	int zone = 0, curbit = 0, lastbit = 0, line = 0, order = 0;
	unsigned char buff[6] = {0};

	zone = (dbs[0] & 0x0000FF00) >> 8;
	printf("\n%02x 0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F",zone);
	for(i = 0; i < count; i++)
	{
		zone = buff[0] =  (dbs[i] & 0x0000FF00) >> 8;//区码
		curbit = buff[1] =  (dbs[i] & 0x000000FF);//位码
		line = (curbit & 0xF0) >> 4;
		order = (curbit & 0x0F);
		buff[2] = 0x00;

		if((curbit & 0xF0) != (lastbit & 0xF0))//换行
		{
			printf("\n%x\x20",line);
			for(k = 0; k < order; k++)//行内填充空格
				printf("\x20\x20\x20\x20\x20\x20");
		}
		lastbit = curbit;
		//设置代码页，输出gbk
		SetConsoleCP(936);
		SetConsoleOutputCP(936);
		printf("%s",buff);

		//设置代码页，输出unicode
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);
		
		memset(buff,0x00,sizeof(buff));
		gbk2UTF8(dbtws[i],buff);
		wprintf(L"%LS  ",buff);
	}
}
void reduceListByZone()
{
	int i = 0, k = 0, ret = 0, count = 0;
	int lastzone = 0, curzone = 0;
	char opt = 0;
	unsigned char buff[6] = {0};

	setlocale(LC_CTYPE, "zh-CN");
	count = sizeof(_gbks)/sizeof(_gbks[0]);
	gOutCount = 0;
	for(i = 0; i < count;)
	{
		lastzone = (_gbks[i] & 0x0000FF00) >> 8;
		for(k = i; k < count; k++)
		{
			curzone = (_gbks[k] & 0x0000FF00) >> 8;
			if(curzone != lastzone)
			{
				break;
			}
		}
		listzone(_gbks + i, _gbktws + i, k - i);
		printf("\nkeep zone(%02x): y(yes),n(no),r(return),default(yes):",lastzone);
		scanf("%c",&opt);
		printf("\n");
		getchar();
		if('r' == opt)
			return;
		if('n' != opt)
		{
			memcpy(gOut[0]+gOutCount, _gbks + i, (k - i)*sizeof(_gbks[0]));
			memcpy(gOut[1]+gOutCount, _gbktws + i, (k - i)*sizeof(_gbktws[0]));
			gOutCount += k - i;
		}
		i = k;
	}
}

void parseZb(unsigned short pole[][1024], unsigned int * count, char * opt)
{
	int i = 0, len = 0, index = 0;
	char *p1 = NULL, *p2 = NULL;
	len = strlen(opt);
	for(i = 0; i < len; i++)
	{
		p1 = strstr(opt + i,"-");
		if(!p1)
			break;
		pole[0][index] = strtol(p1 - 4,NULL,16);
		pole[1][index] = strtol(p1 + 1,NULL,16);
		index++;
		i += p1 - opt - i;
	}
	*count = index;
}
void reduceListByBit()
{
	int i = 0, k = 0,count = 0, poleCount = 0, keep = 1;
	unsigned short pole[2][1024] = {0};
	char opt[4096] = {0};
	printf("please input zone-bit code in exclude:");
	gets(opt);
	printf("\n");
	parseZb(pole,&poleCount,opt);
	count = sizeof(_gbks)/sizeof(_gbks[0]);
	gOutCount = 0;
	for(i = 0; i < count; i++)
	{
		keep = 1;
		for(k = 0; k < poleCount; k++)
		{
			if(_gbks[i] >= pole[0][k] && _gbks[i] <= pole[1][k])
			{
				keep = 0;
				break;
			}
		}
		if(keep)
		{
			gOut[0][gOutCount] = _gbks[i];
			gOut[1][gOutCount] = _gbktws[i];
			gOutCount++;
		}

	}
	printf("keep %i/%i\n",gOutCount, count);
}

void writegbks(unsigned short dbs[], unsigned short dbtws[], unsigned int count)
{
	FILE *fp = NULL;
	char buff[1024] = {0};
	int i = 0, ret = 0, iErr = 0;

	{
		fp =fopen(codeFileName,"wb");
		if(!fp)
		{
			printf("fopen fileName:%s failed\n",codeFileName);
			iErr = 1;
			goto ret;
		}
		sprintf(buff,gbk_han_head);
		ret = strlen(buff);
		if(ret != fwrite(buff,1,ret,fp))
		{
			printf("fwrite fileName:%s failed\n",codeFileName);
			iErr = 1;
			goto ret;
		}
		for( i = 0; i < count; i++)
		{
			if(0 == i % 8)
			{
				sprintf(buff,"\n");
				ret = strlen(buff);
				if(ret != fwrite(buff,1,ret,fp))
				{
					printf("fwrite fileName:%s failed\n",codeFileName);
					iErr = 1;
					goto ret;
				}
			}
			if(i != count - 1)
				sprintf(buff,"0x%04x, ",dbs[i]);
			else
				sprintf(buff,"0x%04x",dbs[i]);
			ret = strlen(buff);
			if(ret != fwrite(buff,1,ret,fp))
			{
				printf("fwrite fileName:%s failed\n",codeFileName);
				iErr = 1;
				goto ret;
			}
		}
		sprintf(buff,gbk_han_tail);
		ret = strlen(buff);
		if(ret != fwrite(buff,1,ret,fp))
		{
			printf("fwrite fileName:%s failed\n",codeFileName);
			iErr = 1;
			goto ret;
		}
	}
	fclose(fp);
	fp = NULL;
	{
		fp =fopen(codeFileName,"ab+");
		if(!fp)
		{
			printf("fopen fileName:%s failed\n",codeFileName);
			iErr = 1;
			goto ret;
		}
		sprintf(buff,gbk_unicode_head);
		ret = strlen(buff);
		if(ret != fwrite(buff,1,ret,fp))
		{
			printf("fwrite fileName:%s failed\n",codeFileName);
			iErr = 1;
			goto ret;
		}
		for( i = 0; i < count; i++)
		{
			if(0 == i % 8)
			{
				sprintf(buff,"\n");
				ret = strlen(buff);
				if(ret != fwrite(buff,1,ret,fp))
				{
					printf("fwrite fileName:%s failed\n",codeFileName);
					iErr = 1;
					goto ret;
				}
			}
			if(i != count - 1)
				sprintf(buff,"0x%04x, ",dbtws[i]);
			else
				sprintf(buff,"0x%04x",dbtws[i]);
			ret = strlen(buff);
			if(ret != fwrite(buff,1,ret,fp))
			{
				printf("fwrite fileName:%s failed\n",codeFileName);
				iErr = 1;
				goto ret;
			}
		}
		
		sprintf(buff,gbk_unicode_tail);
		ret = strlen(buff);
		if(ret != fwrite(buff,1,ret,fp))
		{
			printf("fwrite fileName:%s failed\n",codeFileName);
			iErr = 1;
			goto ret;
		}
	}
	if(iErr)
		printf("%s failed\n",__FUNCTION__);
	else
		printf("%s sucess\n",__FUNCTION__);
ret:
	if(fp)
		fclose(fp);
}


void sortByAsc(unsigned short codes[], unsigned int count)
{
	int i = 0, j = 0, temp = 0;
	for(i = 0; i < count; i++)
	{
		for(j = i; j < count; j++)
		{
			if(codes[i] > codes[j])
			{
				temp = codes[j];
				codes[j] = codes[i];
				codes[i] = temp;
			}
		}
	}
}
void writegbksByAsc(unsigned short dbs[], unsigned short dbtws[], unsigned int count)
{
	FILE *fp = NULL;
	char buff[1024] = {0};
	int i = 0, ret = 0, iErr = 0, dataLen = 0;
	unsigned short *data = NULL;
	dataLen = count*sizeof(unsigned short);
	data = (unsigned short *)malloc(dataLen);
	if(!data)
	{
		printf("malloc memory failed\n");
		goto ret;
	}
	memcpy(data,dbs,dataLen);
	{
		sortByAsc(data,count);
		fp =fopen(codeFileNameAsc,"wb");
		if(!fp)
		{
			printf("fopen fileName:%s failed\n",codeFileNameAsc);
			iErr = 1;
			goto ret;
		}
		sprintf(buff,gbk_han_head);
		ret = strlen(buff);
		if(ret != fwrite(buff,1,ret,fp))
		{
			printf("fwrite fileName:%s failed\n",codeFileNameAsc);
			iErr = 1;
			goto ret;
		}
		for( i = 0; i < count; i++)
		{
			if(0 == i % 8)
			{
				sprintf(buff,"\n");
				ret = strlen(buff);
				if(ret != fwrite(buff,1,ret,fp))
				{
					printf("fwrite fileName:%s failed\n",codeFileNameAsc);
					iErr = 1;
					goto ret;
				}
			}
			if(i != count - 1)
				sprintf(buff,"0x%04x, ",data[i]);
			else
				sprintf(buff,"0x%04x",data[i]);
			ret = strlen(buff);
			if(ret != fwrite(buff,1,ret,fp))
			{
				printf("fwrite fileName:%s failed\n",codeFileNameAsc);
				iErr = 1;
				goto ret;
			}
		}
		sprintf(buff,gbk_han_tail);
		ret = strlen(buff);
		if(ret != fwrite(buff,1,ret,fp))
		{
			printf("fwrite fileName:%s failed\n",codeFileNameAsc);
			iErr = 1;
			goto ret;
		}
	}
	fclose(fp);
	fp = NULL;
	memcpy(data,dbtws,dataLen);
	{
		sortByAsc(data,count);
		fp =fopen(codeFileNameAsc,"ab+");
		if(!fp)
		{
			printf("fopen fileName:%s failed\n",codeFileNameAsc);
			iErr = 1;
			goto ret;
		}
		sprintf(buff,gbk_unicode_head);
		ret = strlen(buff);
		if(ret != fwrite(buff,1,ret,fp))
		{
			printf("fwrite fileName:%s failed\n",codeFileNameAsc);
			iErr = 1;
			goto ret;
		}
		for( i = 0; i < count; i++)
		{
			if(0 == i % 8)
			{
				sprintf(buff,"\n");
				ret = strlen(buff);
				if(ret != fwrite(buff,1,ret,fp))
				{
					printf("fwrite fileName:%s failed\n",codeFileNameAsc);
					iErr = 1;
					goto ret;
				}
			}
			if(i != count - 1)
				sprintf(buff,"0x%04x, ",data[i]);
			else
				sprintf(buff,"0x%04x",data[i]);
			ret = strlen(buff);
			if(ret != fwrite(buff,1,ret,fp))
			{
				printf("fwrite fileName:%s failed\n",codeFileNameAsc);
				iErr = 1;
				goto ret;
			}
		}
		
		sprintf(buff,gbk_unicode_tail);
		ret = strlen(buff);
		if(ret != fwrite(buff,1,ret,fp))
		{
			printf("fwrite fileName:%s failed\n",codeFileNameAsc);
			iErr = 1;
			goto ret;
		}
	}
	if(iErr)
		printf("%s failed\n",__FUNCTION__);
	else
		printf("%s sucess\n",__FUNCTION__);
ret:
	if(fp)
		fclose(fp);
}

void readHexData(unsigned short value[], unsigned int *count, unsigned char *data, unsigned int dataLen)
{
	int i = 0, index = 0;
	char *str = NULL;
	for(i = 0; i < dataLen; i++)
	{
		str = strstr((char*)data + i,"0x");
		if(str)
		{
			value[index++] = strtol(str,NULL,16);
			i += (str -data - i);
		}else
			break;
	}
	*count = index;
}
void readgbks(void)
{
	FILE * fp = NULL;
	unsigned char *data = NULL, *p1 = NULL, *p2 = NULL;
	unsigned short *codes[2] = {NULL};
	int i = 0, ret = 0, iLen = 0, count = 0;
	data = (unsigned char*)malloc(MaxChars*2);
	codes[0] = (unsigned short*)malloc(MaxChars*2);
	codes[1] = (unsigned short*)malloc(MaxChars*2);
	if(!data || !codes[0] || !codes[1])
	{
		printf("malloc memory failed\n");
		goto ret;
	}
	memset(data,0x00,MaxChars*2);
	memset(codes[0],0x00,MaxChars*2);
	memset(codes[1],0x00,MaxChars*2);
	fp = fopen(codeFileName,"rb");
	if(!fp)
	{
		printf("fopen fileName:%s failed\n",codeFileName);
		goto ret;
	}
	fseek(fp,0x00,SEEK_END);
	iLen = ftell(fp);
	fseek(fp,0x00,SEEK_SET);
	ret = fread(data,1,iLen,fp);
	p2 = p1 = data;
	if(!(p1 = (unsigned char *)strstr((char*)p1,gbk_han_head))
		|| !(p2 = (unsigned char *)strstr((char*)p1,gbk_han_tail)))
	{
		printf("data file han format error\n");
		goto ret;
	}
	readHexData(codes[0],&count,p1,p2-p1);

	if(!(p1 = (unsigned char *)strstr((char*)p1,gbk_unicode_head))
		|| !(p2 = (unsigned char *)strstr((char*)p1,gbk_unicode_tail)))
	{
		printf("data file unicode format error\n");
		goto ret;
	}
	readHexData(codes[1],&count,p1,p2-p1);
	listgbks(codes[0],codes[1],count);
	//listgbks(codes[0],NULL,count);
	
ret:
	if(data)
		free(data);
	if(codes[0])
		free(codes[0]);
	if(codes[1])
		free(codes[1]);
	if(fp)
		fclose(fp);
}

void samplegbks(void)
{
	FILE * fp = NULL;
	unsigned char *data = NULL, *p1 = NULL, *p2 = NULL;
	unsigned short *codes[2] = {NULL};
	int i = 0, ret = 0, iLen = 0, count = 0, opt = 0;
	data = (unsigned char*)malloc(MaxChars*2);
	codes[0] = (unsigned short*)malloc(MaxChars*2);
	codes[1] = (unsigned short*)malloc(MaxChars*2);
	if(!data || !codes[0] || !codes[1])
	{
		printf("malloc memory failed\n");
		goto ret;
	}
	memset(data,0x00,MaxChars*2);
	memset(codes[0],0x00,MaxChars*2);
	memset(codes[1],0x00,MaxChars*2);
	fp = fopen(codeFileName,"rb");
	if(!fp)
	{
		printf("fopen fileName:%s failed\n",codeFileName);
		goto ret;
	}
	fseek(fp,0x00,SEEK_END);
	iLen = ftell(fp);
	fseek(fp,0x00,SEEK_SET);
	ret = fread(data,1,iLen,fp);
	p2 = p1 = data;
	if(!(p1 = (unsigned char *)strstr((char*)p1,gbk_han_head))
		|| !(p2 = (unsigned char *)strstr((char*)p1,gbk_han_tail)))
	{
		printf("data file han format error\n");
		goto ret;
	}
	readHexData(codes[0],&count,p1,p2-p1);

	if(!(p1 = (unsigned char *)strstr((char*)p1,gbk_unicode_head))
		|| !(p2 = (unsigned char *)strstr((char*)p1,gbk_unicode_tail)))
	{
		printf("data file unicode format error\n");
		goto ret;
	}
	readHexData(codes[1],&count,p1,p2-p1);
	printf("please input number of each zone chosed from file table:");
	scanf("%i",&opt);
	opt %= 256;
	printf("\n");
	listPartGbks(codes[0],codes[1],count, opt);
	
ret:
	if(data)
		free(data);
	if(codes[0])
		free(codes[0]);
	if(codes[1])
		free(codes[1]);
	if(fp)
		fclose(fp);
}

void convertbin(void)
{
	FILE * fp = NULL;
	unsigned char *data = NULL, *p1 = NULL, *p2 = NULL, *pCur = NULL, buff[6] = {0};
	unsigned short *codes[2] = {NULL}, value = 0;
	int i = 0, ret = 0, iLen = 0, count = 0;
	data = (unsigned char*)malloc(MaxChars*2);
	codes[0] = (unsigned short*)malloc(MaxChars*2);
	codes[1] = (unsigned short*)malloc(MaxChars*2);
	if(!data || !codes[0] || !codes[1])
	{
		printf("malloc memory failed\n");
		goto ret;
	}
	memset(data,0x00,MaxChars*2);
	memset(codes[0],0x00,MaxChars*2);
	memset(codes[1],0x00,MaxChars*2);
	fp = fopen(codeFileName,"rb");
	if(!fp)
	{
		printf("fopen fileName:%s failed\n",codeFileName);
		goto ret;
	}
	fseek(fp,0x00,SEEK_END);
	iLen = ftell(fp);
	fseek(fp,0x00,SEEK_SET);
	ret = fread(data,1,iLen,fp);
	p2 = p1 = data;
	if(!(p1 = (unsigned char *)strstr((char*)p1,gbk_han_head))
		|| !(p2 = (unsigned char *)strstr((char*)p1,gbk_han_tail)))
	{
		printf("data file han format error\n");
		goto ret;
	}
	readHexData(codes[0],&count,p1,p2-p1);

	if(!(p1 = (unsigned char *)strstr((char*)p1,gbk_unicode_head))
		|| !(p2 = (unsigned char *)strstr((char*)p1,gbk_unicode_tail)))
	{
		printf("data file unicode format error\n");
		goto ret;
	}
	readHexData(codes[1],&count,p1,p2-p1);
	
	fclose(fp);
	fp = NULL;
	fp = fopen(gbkFileName,"wb");
	if(!fp)
	{
		printf("fopen failed\n");
		goto ret;
	}
	for(i = 0; i < count; i++)
	{
		buff[0] = (codes[0][i] & 0x0000FF00) >> 8;
		buff[1] = (codes[0][i] & 0x000000FF);
		fwrite(buff,1,2,fp);
	}
	fclose(fp);
	fp = NULL;
	fp = fopen(utf8FileName,"wb");
	if(!fp)
	{
		printf("fopen failed\n");
		goto ret;
	}
	for(i = 0; i < count; i++)
	{
		gbk2UTF8(codes[1][i],buff);
		fwrite(buff,1,3,fp);
	}
	fclose(fp);
	fp = NULL;
ret:
	if(data)
		free(data);
	if(codes[0])
		free(codes[0]);
	if(codes[1])
		free(codes[1]);
	if(fp)
		fclose(fp);
}

void gbk2unicode(void)
{
	int i = 0, ret = 0, count = 0, nMatch = 0;
	unsigned short value = 0;
	unsigned char in[6] = {0}, out[6] = {0};
	count = sizeof(_gbks)/sizeof(_gbks[0]);
	for(i = 0; i < count; i++)
	{
		memset(in,0x00,sizeof(in));
		in[0] = (_gbks[i] & 0x0000FF00) >> 8;
		in[1] = (_gbks[i] & 0x000000FF);
		memset(out,0x00,sizeof(out));
		ret = MultiByteToWideChar(936,0,in,2,out,6);
		if(ret <= 0)
		{
			printf("MultiByteToWideChar failed\n");
		}else if (_gbktws[i] != *(unsigned *)out)
		{
			printf("MultiByteToWideChar not match\n");
			//_gbktws[i] = *(unsigned *)out;
		}else
			nMatch++;
	}
	if(nMatch == count)
		printf("MultiByteToWideChar all match\n");
	else
		printf("MultiByteToWideChar match partly (%i/%i)\n",nMatch,count);
}

