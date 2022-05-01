#include <stdio.h>
#include <stdint.h>
#include "font_lite.h"


#if ENABLE_FONT_SIZE_12
#include "ascii12.h"
const uint8_t ErrPic12[24] = {
	0x00,0x66,0x02,0xA8,0x50,0xAA,0x52,0xA8,
	0x50,0x02,0x66,0x00,0x00,0x06,0x04,0x00,
	0x01,0x04,0x05,0x00,0x01,0x04,0x06,0x00,
};

extern const font12_struct font12_table[79];
const uint16_t font12_num = sizeof(font12_table) / sizeof(font12_struct);
#endif

#if ENABLE_FONT_SIZE_16
#include "ascii16.h"
const uint8_t ErrPic16[32] = {
	0x00,0x66,0x02,0xA8,0x50,0xAA,0x52,0xA8,
	0x50,0xAA,0x52,0xA8,0x50,0x02,0x66,0x00,
	0x00,0x66,0x40,0x0A,0x15,0x4A,0x55,0x0A,
	0x15,0x4A,0x55,0x0A,0x15,0x40,0x66,0x00,
};

extern const font12_struct font16_table[];
const uint16_t font16_num = sizeof(font16_table) / sizeof(font16_struct);
#endif

static mgl_color_t fg_color;
static mgl_color_t bg_color;

void set_fg_color(mgl_color_t color)
{
	fg_color = color;
}

void set_bg_color(mgl_color_t color)
{
	bg_color = color;
}

void DrawPoint(int16_t x, int16_t y)
{
	mgl_draw_point(x, y, fg_color);
}

void CleanPoint(int16_t x, int16_t y)
{
	mgl_draw_point(x, y, bg_color);
}

// ����Ƿ����GB2312����
// uint8_t encodingCheck(uint8_t ch, uint8_t cl)
uint8_t encodingCheck(uint8_t *pchar)
{
	uint8_t ch;
	// uint8_t cl;
	ch = *pchar;
	// cl = *pchar+1;

	// ���װ�
	return (ch < 0x81 || ch > 0xFE) ? 1 : 0;

	// �����Ķ���
	// uint8_t ret = 0;
	// ret = (ch < 0x81 || ch > 0xFE) ? 1 : 0;
	// ret += (cl < 0x40 || cl > 0xFE) ? 1 : 0;
	// return ret;

	// �߶ȼ��ɰ�
	// return (ch < 0x81 || ch > 0xFE) ? 1 : 0 + (cl < 0x40 || cl > 0xFE) ? 1 : 0;
}

#if ENABLE_FONT_SIZE_12

uint8_t* font12_search(uint8_t *pchar)
{
	// !!!! ���ֲ���Ҫ�����к��������������� !!!!

	uint16_t low, high, mid, aim, now;
	// ֱ�ӽ�������ַ���Ϊһ��uint16_t�����ִ���ע���С��
	aim = (pchar[0] << 8) + (pchar[1]); // �����ַ������ʱ���� HardFault
	// aim = (pchar[1] << 8) + (pchar[0]); // �����ַ������ʱ���� HardFault
	low = 0;
	high = font12_num;

	while (low <= high)
	{
		mid = low + (high - low) / 2;
		// ע���С��
		now = (font12_table[mid].character[0] << 8) + font12_table[mid].character[1];
		// now = (font12_table[mid].character[1] << 8) + font12_table[mid].character[0];

		if (aim == now)
		{
			return (uint8_t*)&font12_table[mid].fontdata;
		}
		else if (aim < now)
		{
			high = mid - 1;
		}
		else
		{
			low = mid + 1;
		}	
	}
	return (uint8_t *)ErrPic12;
	// return NULL;
}

// 12������
void DrawFontInGram12(uint8_t type, uint8_t *font, int16_t x, int16_t y)
{
	uint8_t i, j;
	uint8_t tmpfont; // �ݴ�һ�ֽڵ��ַ�ͼ������
	
	if(type == 'c')
	{
		for(j=0; j<12; j++)
		{
			tmpfont = font[j];
			for(i=0; i<8; i++)
			{
				if(tmpfont & 0x01)
				{
					DrawPoint(x+j, y+i);
				}
				else
				{
					CleanPoint(x+j, y+i);
				}
				tmpfont >>= 1;
			}
		}
		for(; j<24; j++)
		{
			tmpfont = font[j];
			for(i=0; i<4; i++)
			{
				if(tmpfont & 0x01)
				{
					DrawPoint(x+j-12, y+i+8);
				}
				else
				{
					CleanPoint(x+j-12, y+i+8);
				}
				tmpfont >>= 1;
			}
		}
	}
	
	if(type == 'e')
	{
		for(j=0; j<6; j++)
		{
			tmpfont = font[j];
			for(i=0; i<8; i++)
			{
				if(tmpfont & 0x01)
				{
					DrawPoint(x+j, y+i);
				}
				else
				{
					CleanPoint(x+j, y+i);
				}
				tmpfont >>= 1;
			}
		}
		for(; j<12; j++)
		{
			tmpfont = font[j];
			for(i=0; i<4; i++)
			{
				if(tmpfont & 0x01)
				{
					DrawPoint(x+j-6, y+i+8);
				}
				else
				{
					CleanPoint(x+j-6, y+i+8);
				}
				tmpfont >>= 1;
			}
		}
	}
}

// 12������
void PrintString12(int16_t x, int16_t y, char *string)
{
	// uint8_t i;
	uint8_t *pchar;
	uint8_t *pfont; // �����ҵ����������ݵ�ַ
	
	pchar = (uint8_t *)string;//ע���������ת��

	while(*pchar != 0x00)
	{
		//if(*pchar > 31 && *pchar < 128)
		if(*pchar <= 31 || *pchar == 127)
		{
			// �����ַ�ֱ������
			pchar++;
			continue;
		}
		if(*pchar > 31 && *pchar < 127)//����Ӧ����С��127����127���ַ���del
		{
			// ֱ������ַ����ݵĵ�ַ
			pfont = (uint8_t *)&ASCII12[(*pchar - 32) * 12];

			//���´���ʵ���Զ�����
			if(LCD_LENGTH - x < 6)
			{
				y += 13; // ����ʱ���һ���أ��������һ�к��ֻ���һ�𿴲���
				x = 0;
			}

			//������һ����ʾ���£�ֱ�ӷ���
			if(y >= LCD_WIDTH)
			{
				return;
			}
			
			//��ʾ�ַ�
			DrawFontInGram12('e', pfont, x, y);
			x += 6;
			
			pchar++;
		}
		else
		{
			pfont = font12_search(pchar);

			//���´���ʵ���Զ�����
			if(LCD_LENGTH - x < 12)
			{
				y += 13; // ����ʱ���һ���أ��������һ�к��ֻ���һ�𿴲���
				x = 0;
			}

			//������һ����ʾ���£�ֱ�ӷ���
			if(y >= LCD_WIDTH)
			{
				return;
			}

			//��ʾ�ַ�
			DrawFontInGram12('c', pfont, x, y);
			x += 12;

			//�������ֽڣ�ָ���2
			pchar += 2;
		}
	}
}

#endif // ENABLE_FONT_SIZE_12



#if ENABLE_FONT_SIZE_16

// uint8_t* font_search(uint8_t ch, uint8_t cl)
uint8_t* font16_search(uint8_t *pchar)
{
// !!!! ���ֲ���Ҫ�����к��������������� !!!!

	uint16_t low, high, mid, aim, now;
	// ֱ�ӽ�������ַ���Ϊһ��uint16_t�����ִ���ע���С��
	aim = (pchar[0] << 8) + (pchar[1]); // �����ַ������ʱ���� HardFault
	// aim = (pchar[1] << 8) + (pchar[0]); // �����ַ������ʱ���� HardFault
	low = 0;
	high = font16_num;

	while (low <= high)
	{
		mid = low + (high - low) / 2;
		// ע���С��
		now = (font12_table[mid].character[0] << 8) + font12_table[mid].character[1];
		// now = (font12_table[mid].character[1] << 8) + font12_table[mid].character[0];

		if (aim == now)
		{
			return (uint8_t*)&font16_table[mid].fontdata;
		}
		else if (aim < now)
		{
			high = mid - 1;
		}
		else
		{
			low = mid + 1;
		}	
	}
	return (uint8_t *)ErrPic16;
	// return NULL;
}

void DrawFontInGram16(uint8_t type, uint8_t *font, int16_t x, int16_t y)
{
	uint8_t i, j;
	uint8_t tmpfont; // �ݴ�һ�ֽڵ��ַ�ͼ������
	
	if(type == 'c')
	{
		for(j=0; j<16; j++)
		{
			tmpfont = font[j];
			for(i=0; i<8; i++)
			{
				if(tmpfont & 0x01)
				{
					DrawPoint(x+j, y+i);
				}
				else
				{
					CleanPoint(x+j, y+i);
				}
				tmpfont >>= 1;
			}
		}
		for(; j<32; j++)
		{
			tmpfont = font[j];
			for(i=0; i<8; i++)
			{
				if(tmpfont & 0x01)
				{
					DrawPoint(x+j-16, y+i+8);
				}
				else
				{
					CleanPoint(x+j-16, y+i+8);
				}
				tmpfont >>= 1;
			}
		}
	}
	
	if(type == 'e')
	{
		for(j=0; j<8; j++)
		{
			tmpfont = font[j];
			for(i=0; i<8; i++)
			{
				if(tmpfont & 0x01)
				{
					DrawPoint(x+j, y+i);
				}
				else
				{
					CleanPoint(x+j, y+i);
				}
				tmpfont >>= 1;
			}
		}
		for(; j<16; j++)
		{
			tmpfont = font[j];
			for(i=0; i<8; i++)
			{
				if(tmpfont & 0x01)
				{
					DrawPoint(x+j-8, y+i+8);
				}
				else
				{
					CleanPoint(x+j-8, y+i+8);
				}
				tmpfont >>= 1;
			}
		}
	}
}

void PrintString16(int16_t x, int16_t y, char *string)
{
	uint8_t *pchar;
	uint8_t *pfont; // �����ҵ����������ݵ�ַ
	
	pchar = (uint8_t *)string;//ע���������ת��

	while(*pchar != 0x00)
	{
		//if(*pchar > 31 && *pchar < 128)
		if(*pchar <= 31 || *pchar == 127)
		{
			// �����ַ�ֱ������
			pchar++;
			continue;
		}
		if(*pchar > 31 && *pchar < 127)//����Ӧ����С��127����127���ַ���del
		{
			// ֱ������ַ����ݵĵ�ַ
			pfont = (uint8_t *)&ASCII16[(*pchar - 32) * 16];

			//���´���ʵ���Զ�����
			if(LCD_LENGTH - x < 8)
			{
				// y += 17; // ����ʱ���һ���أ��������һ�к��ֻ���һ�𿴲���
				y += 16;
				x = 0;
			}

			//������һ����ʾ���£�ֱ�ӷ���
			if(y >= LCD_WIDTH)
			{
				return;
			}
			
			//��ʾ�ַ�
			DrawFontInGram16('e', pfont, x, y);
			x += 8;
			
			pchar++;
		}
		else
		{
			pfont = font16_search(pchar);

			//���´���ʵ���Զ�����
			if(LCD_LENGTH - x < 16)
			{
				// y += 17; // ����ʱ���һ���أ��������һ�к��ֻ���һ�𿴲���
				y += 16;
				x = 0;
			}

			//������һ����ʾ���£�ֱ�ӷ���
			if(y >= LCD_WIDTH)
			{
				return;
			}

			//��ʾ�ַ�
			DrawFontInGram16('c', pfont, x, y);
			x += 16;

			//�������ֽڣ�ָ���2
			pchar += 2;
		}
	}
}

#endif // ENABLE_FONT_SIZE_16
