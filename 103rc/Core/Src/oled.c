/*
 * oled.c
 *
 *  Created on: Nov 17, 2021
 *      Author: wvv
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <main.h>
extern I2C_HandleTypeDef hi2c1;
extern const uint8_t f5x8[5];
void oled_write_cmd(uint8_t cmd)
{
	HAL_I2C_Mem_Write(&hi2c1, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0x100);
}
#define WIDTH 128
#define HEIGHT 4
#define PIXEL_MODE_PAINT 	0
#define PIXEL_MODE_CLEAR	1
uint8_t _buf[HEIGHT * WIDTH] = { 0 };
void oled_clear(void)
{
	memset(_buf, 0x00, sizeof(_buf));
}
void oled_pixel(uint8_t x, uint8_t y, uint8_t mode)
{
	if (x >= WIDTH || y >= HEIGHT * 8)
	{
		return;
	}
	if (mode == PIXEL_MODE_PAINT)
	{
		_buf[(y >> 3) * WIDTH + x] |= 1 << (y & 0x07);
	}
	else
	{
		_buf[(y >> 3) * WIDTH + x] &= ~(1 << (y & 0x07));
	}

}
void oled_putchar(char c, uint8_t x, uint8_t y)
{
	if (x >= WIDTH || y >= HEIGHT)
	{
		return;
	}
	const uint8_t *p = &f5x8[(c - ' ') * 5];
	for (int i = x; (i < x + 5) && i < WIDTH; i++)
	{
		_buf[y * WIDTH + i] = *p;
		p++;
	}
}
void oled_str(char* str, uint8_t x, uint8_t y)
{
	int offset = 0;
	while (*str)
	{
		oled_putchar(*str, x + offset, y);
		offset += 6;
		str++;
	}
}
void oled_refresh(void)
{
	for (uint8_t i = 0; i < HEIGHT; i++)
	{
		oled_write_cmd(0xb0 + i);
		oled_write_cmd(0x00);
		oled_write_cmd(0x10);
		HAL_StatusTypeDef ret = HAL_I2C_Mem_Write(&hi2c1, 0x78, 0x40, 1,
				(uint8_t*) &_buf[i * WIDTH], WIDTH, 100);
		if (ret != HAL_OK)
		{
			HAL_I2C_DeInit(&hi2c1);
			HAL_I2C_Init(&hi2c1);
		}
	}
}
void oled_init(void)
{
	oled_write_cmd(0xAE);
	oled_write_cmd(0x20);
	oled_write_cmd(0x10);

	oled_write_cmd(0xB0);
	oled_write_cmd(0xC8);
	oled_write_cmd(0x00);
	oled_write_cmd(0x10);
	oled_write_cmd(0x40);

	oled_write_cmd(0x81);
	oled_write_cmd(0xff);

	oled_write_cmd(0xa1);
	oled_write_cmd(0xa6);

	oled_write_cmd(0xa8);
	oled_write_cmd(0x1f);

	oled_write_cmd(0xd3);
	oled_write_cmd(0x00);

	oled_write_cmd(0xd5);
	oled_write_cmd(0xf0);

	oled_write_cmd(0xd9);
	oled_write_cmd(0x22);

	oled_write_cmd(0xda);
	oled_write_cmd(0x02);

	oled_write_cmd(0xdb);
	oled_write_cmd(0x20);

	oled_write_cmd(0x8d);
	oled_write_cmd(0x14);

	oled_write_cmd(0xaf);
	HAL_Delay(100);
	oled_clear();
	oled_refresh();
}

void oled_demo_run()
{
	oled_init();
	oled_str("OLED DEMO FPS:",0,0);
	uint32_t last = 0, show = 0;
	char buf[10];
	while(1)
	{
		if(HAL_GetTick()-show >500) //fps
		{
			int det = HAL_GetTick()-last;
			if(det > 0)
			{
				sprintf(buf,"%d",1000/det);
			}
			oled_str(buf,14*6,0);
			show = HAL_GetTick();
		}
		last = HAL_GetTick();
		for(int i=0;i<10;i++)
		{
			oled_putchar(rand()%96+' ', rand()%21*6, rand()%3+1);
		}
		/* USER CODE END WHILE */
		oled_refresh();
	}
}

