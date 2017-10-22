#include "CodeRecogntion.h"
#include <ctype.h>

static int byte_class_table[256] =
{
	/*       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  */
	/* 00 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 10 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 20 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 30 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 40 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 50 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 60 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 70 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 80 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 90 */ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	/* A0 */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	/* B0 */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	/* C0 */ 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	/* D0 */ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	/* E0 */ 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 7, 7,
	/* F0 */ 9,10,10,10,11, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
	/*       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  */
};
/* state table */
typedef enum {
	kSTART = 0,
	kA,
	kB,
	kC,
	kD,
	kE,
	kF,
	kG,
	kERROR,
	kNumOfStates
} utf8_state;

static utf8_state state_table[] = {
	/*                            kSTART, kA,     kB,     kC,     kD,     kE,     kF,     kG,     kERROR */
	/* 0x00-0x7F: 0            */ kSTART, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR,
	/* 0x80-0x8F: 1            */ kERROR, kSTART, kA,     kERROR, kA,     kB,     kERROR, kB,     kERROR,
	/* 0x90-0x9f: 2            */ kERROR, kSTART, kA,     kERROR, kA,     kB,     kB,     kERROR, kERROR,
	/* 0xa0-ss0xbf: 3            */ kERROR, kSTART, kA,     kA,     kERROR, kB,     kB,     kERROR, kERROR,
	/* 0xc0-0xc1, 0xf5-0xff: 4 */ kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR,
	/* 0xc2-0xdf: 5            */ kA,     kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR,
	/* 0xe0: 6                 */ kC,     kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR,
	/* 0xe1-0xec, 0xee-0xef: 7 */ kB,     kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR,
	/* 0xed: 8                 */ kD,     kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR,
	/* 0xf0: 9                 */ kF,     kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR,
	/* 0xf1-0xf3: 10           */ kE,     kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR,
	/* 0xf4: 11                */ kG,     kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR
};

#define BYTE_CLASS(b) (byte_class_table[(unsigned char)b])
#define NEXT_STATE( b, cur) (state_table[ (BYTE_CLASS(b) * kNumOfStates) + (cur)])


CCodeRecogtion::CCodeRecogtion()
{
}

CCodeRecogtion::~CCodeRecogtion()
{
}

eCodeType CCodeRecogtion::getTextType(const char* sLine, int nLen)
{
	if (nLen <=0 || !sLine )
	{
		return CODE_TYPE_UNKNOWN;
	}

	eCodeType et=CODE_TYPE_UNKNOWN;
	for (int i=0; i<nLen; i++)
	{
		if (isAscii((int)sLine[i]))
		{
			et= CODE_TYPE_ASCII;
			continue;
		}
		else if ( isUtf8(sLine,nLen))
		{
			return CODE_TYPE_UTF8;
		}
		else  if (isGBK(sLine[i],sLine[i+1]))
		{
			return CODE_TYPE_GB;
		} 
		else if (isBig5(sLine[i],sLine[i+1]))
		{
			return CODE_TYPE_BIG5;
		}
	}
	return et;

}

int CCodeRecogtion::utf8ToUnicode(const char* sLine, int& nUnicode)
{
	int nLen = 1;
	if ((sLine[0] & 0x80) == 0x0)   // ���ֽ��ַ� 0xxxxxxx
	{
		nUnicode =(unsigned char)sLine[0];
	}
	else if ((sLine[0] & 0xE0) == 0xC0) // ˫�ֽ� 110xxxxx 10xxxxxx
	{	/*1 1 0 x10 x9 x8 x7 x6 ������0x1F��(11111)��Ϊx10 x9 x8 x7 x6��������λ
		  x10 x9 x8 x7 x6 0  0  0  0  0  0 
		  ��1 0 x5 x4 x3 x2 x1 x0 ������3F��(111111)��Ϊ
					0	0 x5 x4 x3 x2 x1 x0
		   ��ӵõ�x10 x9 x8 x7 x6 x5 x4 x3 x2 x1 x0
	    3�ֽڵ�ͬ��
		*/
		if ((sLine[1] & 0xC0) == 0x80)
		{//
			nUnicode = ((sLine[0] & 0x1F) << 6) + (sLine[1] & 0x3F);
			nLen = 2;
		}
		else // error
		{
			nUnicode = 0;
		}
	}
	else if ((sLine[0] & 0xF0) == 0xE0)   // ���ֽ� 1110xxxx 10xxxxxx 10xxxxxx
	{
		nUnicode = ((sLine[0] & 0x0F) << 12) + ((sLine[1] & 0x3F) << 6) + (sLine[2] & 0x3F);
		nLen = 3;
	}
	else // �����UTF8����
	{
		nUnicode = 0;
	}
	return nLen;
}

bool CCodeRecogtion::isGB2312(unsigned char ch, unsigned char cl)
{
	return (ch >= 0xA1 && ch <= 0xF7 && cl >= 0xA1 && cl <= 0xFE);
}

bool CCodeRecogtion::isBig5(unsigned char ch, unsigned char cl)
{
	return (ch >= 0xA1 && ch <= 0xFE && ((cl >= 0x40 && cl <= 0x7E) || (cl >= 0xA1 && cl <= 0xFE)));
}

bool CCodeRecogtion::isGBK(unsigned char ch, unsigned char cl)
{
	return ((ch >= 0x81 && ch <= 0xfe) &&((cl >= 0x40 && cl <= 0x7e) || (cl >= 0x80 && cl <= 0xfe)));
}

bool CCodeRecogtion::isAscii(int c)
{
	return isascii(c);
}

bool CCodeRecogtion::isUtf8(const char* line, int len)
{
	utf8_state current = kSTART;
	const char* pt = line;
	for(int i = 0; i < len ; i++, pt++)
	{
		/* printf("%x state=%d cls=%d next=%d \n",
		*  *pt, current, BYTE_CLASS(*pt), NEXT_STATE(*pt, current));
		*/

		current = NEXT_STATE(*pt, current);
		if (kERROR == current)
			return false;
	}
	return (current == kSTART) ? true : false;
}
