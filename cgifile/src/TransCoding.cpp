#include "TransCoding.h"
#include <string.h>
#include <stdio.h>
#include "GbkCodeTable.h"
#include "Big5CodeTable.h"

//Gbk转换成Utf-8
int CTransCoding::GbkToUtf8(char *pSents,int iLen,char *pRetSents,int &iRetLen)
{
	int i=0;
	for(;i<iLen-1;i++)
	{
		if(pSents[i]>=0 && pSents[i]<=127)
		{
			pRetSents[iRetLen] = pSents[i];
			iRetLen++;
			continue;
		}

		int iFirst = 0;
		int iSecond = 0;
		iFirst = pSents[i]&0xFF;
		iSecond = pSents[i+1]&0xFF;

		/*
		GBK 亦采用双字节表示，总体编码范围为 8140-FEFE 之间，
		首字节在 81-FE 之间，尾字节在 40-FE 之间，剔除 XX7F 一条线
		*/
		int iValue = 0;
		if((iFirst>=129 && iFirst<=254) && (iSecond>=64 && iSecond<=254) && (iSecond != 127))
		{
			iValue = iFirst*256+iSecond;
		}
		else
		{
			pRetSents[iRetLen] = pSents[i];
			iRetLen++;
			continue;
		}

		char cResult[4]={0};
		int iResult = 0;
		UnicodeToUtf8(GbkCodeTable[iValue],cResult,iResult);

		memcpy(pRetSents+iRetLen,cResult,iResult);
		iRetLen+=iResult;
		i++;
	}

	if(i==iLen-1)	//不要忘记遗漏最后一个字节
	{
		pRetSents[iRetLen] = pSents[i];
		iRetLen++;
	}

	return 0;
}

//Utf-8转换成Gbk
int CTransCoding::Utf8ToGbk(char *pSents,int iLen,char *pRetSents,int &iRetLen)
{
	for(int i=0;i<iLen;i++)
	{
		if(pSents[i]>=0 && pSents[i]<=127)
		{
			pRetSents[iRetLen] = pSents[i];
			iRetLen++;
			continue;
		}

		
		if((pSents[i]&0xC0) == 0xC0)	//一个字的开始
		{
			//判断字的位数
			int iWordLen = 1;
			int Temp = pSents[i];
			while(((Temp<<iWordLen)&0x80)==0x80)
			{
				iWordLen++;
			}
			char chWord[16]={0};
			chWord[0]=pSents[i];
			int k=1;
			for(int j=i+1;(j < iLen)&&(k < iWordLen)&&((pSents[j]&0xC0) != 0xC0);j++,k++)
			{
				chWord[k] = pSents[j];
				i = j;
			}

			//无法识别的字符直接拷贝
			if(k!=iWordLen)	
			{
				for(int j=i;(j < (i+k));j++)
				{
					pRetSents[iRetLen] = pSents[j];
					iRetLen++;
					i = j;
				}
				continue;
			}

			chWord[k]='\0';
			int iRetValue = 0;
			//
			Utf8ToUnicode(chWord,k,iRetValue);
			int iGbkValue = GbkUCodeTable[iRetValue];
			pRetSents[iRetLen] = iGbkValue/256;
			iRetLen++;
			pRetSents[iRetLen] = iGbkValue%256;
			iRetLen++;
			
		}
		else
		{
			pRetSents[iRetLen] = pSents[i];
			iRetLen++;
		}
	}
	return 0;
}


//Big5转换成Utf-8
int CTransCoding::Big5ToUtf8(char *pSents,int iLen,char *pRetSents,int &iRetLen)
{
	int i=0;
	for(;i<iLen-1;i++)
	{
		if(pSents[i]>=0 && pSents[i]<=127)
		{
			pRetSents[iRetLen] = pSents[i];
			iRetLen++;
			continue;
		}

		int iFirst = 0;
		int iSecond = 0;
		iFirst = pSents[i]&0xFF;
		iSecond = pSents[i+1]&0xFF;

		/*
		每个字由两个字节组成，其第一字节编码范围为0xA1~0xF9，第二字节编码范围为0×40~0×7E与0xA1~0xFE
		*/
		int iValue = 0;
		if((iFirst>=161 && iFirst<=249) && ((iSecond>=64 && iSecond<=126) || (iSecond>=161 && iSecond<=254)))
		{
			iValue = iFirst*256+iSecond;
		}
		else
		{
			pRetSents[iRetLen] = pSents[i];
			iRetLen++;
			continue;
		}

		

		char cResult[4]={0};
		int iResult = 0;
		UnicodeToUtf8(Big5CodeTable[iValue],cResult,iResult);

		memcpy(pRetSents+iRetLen,cResult,iResult);
		iRetLen+=iResult;
		i++;
	}

	if(i==iLen-1)
	{
		pRetSents[iRetLen] = pSents[i];
		iRetLen++;
	}

	return 0;
}

//Utf-8转换成Big5
int CTransCoding::Utf8ToBig5(char *pSents,int iLen,char *pRetSents,int &iRetLen)
{
	for(int i=0;i<iLen;i++)
	{
		if(pSents[i]>=0 && pSents[i]<=127)
		{
			pRetSents[iRetLen] = pSents[i];
			iRetLen++;
			continue;
		}

		if((pSents[i]&0xC0) == 0xC0)	//一个字的开始
		{
			//判断字的位数
			int iWordLen = 1;
			int Temp = pSents[i];
			while(((Temp<<iWordLen)&0x80)==0x80)
			{
				iWordLen++;
			}
			char chWord[16]={0};
			chWord[0]=pSents[i];
			int k=1;
			for(int j=i+1;(j < iLen)&&(k < iWordLen)&&((pSents[j]&0xC0) != 0xC0);j++,k++)
			{
				chWord[k] = pSents[j];
				i = j;
			}

			//无法识别的字符直接拷贝
			if(k!=iWordLen)	
			{
				for(int j=i;(j < (i+k));j++)
				{
					pRetSents[iRetLen] = pSents[j];
					iRetLen++;
					i = j;
				}
				continue;
			}

			chWord[k]='\0';
			int iRetValue = 0;
			//
			Utf8ToUnicode(chWord,k,iRetValue);
			int iGbkValue = Big5UCodeTable[iRetValue];
			pRetSents[iRetLen] = iGbkValue/256;
			iRetLen++;
			pRetSents[iRetLen] = iGbkValue%256;
			iRetLen++;
		}
		else
		{
			pRetSents[iRetLen] = pSents[i];
			iRetLen++;
		}
	}
	return 0;
}

//日文转换成Utf-8
int CTransCoding::JapanToUtf8(char *pSents,int iLen,char *pRetSents,int &iRetLen)
{
	
	return 0;
}

//Utf-8转换成日文
int CTransCoding::Utf8ToJapan(char *pSents,int iLen,char *pRetSents,int &iRetLen)
{
	
	return 0;
}

//韩语转换成Utf-8
int CTransCoding::KoreaToUtf8(char *pSents,int iLen,char *pRetSents,int &iRetLen)
{

	return 0;
}

//Utf-8转换成韩语
int CTransCoding::Utf8ToKorea(char *pSents,int iLen,char *pRetSents,int &iRetLen)
{

	return 0;
}

//Unicode值转换成UTF8编码
int CTransCoding::UnicodeToUtf8(int iWordValue,char *pResult,int &iRetLen)
{
	if(iWordValue>=0 && iWordValue<=127)
	{
		pResult[0] = iWordValue;
		iRetLen = 1;
	}

	if(iWordValue>=128 && iWordValue<=2047)
	{
		pResult[0] = 0xC0|(iWordValue>>6);
        pResult[1] = 0x80|(iWordValue&0x3F);
        pResult[2] = '\0';
		iRetLen = 2;
	}

	if(iWordValue>=2048 && iWordValue<=65535)
	{
        pResult[0] = 0xE0|(iWordValue>>12);
        pResult[1] = 0x80|((iWordValue>>6)&0x3F);
        pResult[2] = 0x80|(iWordValue&0x3F);
        pResult[3] = '\0';
		iRetLen = 3;
	}

	return 0;
}
//UTF8编码转换成Unicode值
int CTransCoding::Utf8ToUnicode(char *pWord,int iLen,int &iWordValue)
{
	switch(iLen)
	{
	case 1:
		{
			iWordValue = pWord[0];
		}
		break;
	case 2:
		{
			iWordValue = (pWord[0] & 0x1F) << 6;
			iWordValue |= (pWord[2] & 0x3F);
		}
		break;
	case 3:
		{
			iWordValue = (pWord[0] & 0x0F) << 12;
			iWordValue |= (pWord[1] & 0x3F) << 6;
			iWordValue |= (pWord[2] & 0x3F);
		}
		break;
	default:
		break;
	}

	return 0;
}
