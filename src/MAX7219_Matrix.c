/*
 * MAX7219_Matrix.h
 *
 *  Created on: Nov 6, 2024
 *      Author: Sagirov Lenar
 *
 *В качестве примера была взята библиоте Алекса Гайвера.
 *
 *Библиотека для работы с матрицами 8x8 на MAX7219
 *
 */

#include "MAX7219_Matrix.h"
#include "math.h"

uint16_t MAX_width = 0;
uint16_t MAX_height = 0;
uint16_t MAX_maxX = 0;
uint16_t MAX_maxY = 0;
bool MAX_type = GM_ZIGZAG;
uint16_t MAX_amount = 0;
uint8_t MAX_rot = 0;
uint8_t MAX_conn = 0;
uint8_t MAX_bx = 0;
uint8_t MAX_flip = 0;
uint8_t MAX_buffer[100] = { 0, };
uint8_t MAX_IconOut[8] = { 0, };

void MAX_fastShiftOut(uint8_t data) { //Функция записи данных в сдвиговый регистр
#if (MAX_7219_USE_SPI == 1)

	HAL_SPI_Transmit(&MAX7219_SPI_PORT, &data, sizeof(data), HAL_MAX_DELAY);

#else
	for (int i = 0; i < 8; i++) {
		HAL_GPIO_WritePin(MAX_DATA_PORT, MAX_DATA_PIN, data << i & 0x80); //Записываем данные
		HAL_GPIO_WritePin(MAX_CLK_PORT, MAX_CLK_PIN, 1); //Формируем тактовый импульс
		HAL_GPIO_WritePin(MAX_CLK_PORT, MAX_CLK_PIN, 0);
	}
#endif
}

void MAX_send_data(uint8_t address, uint8_t value) {
	MAX_fastShiftOut(address);
	MAX_fastShiftOut(value);
}

void MAX_send_smd(uint8_t address, uint8_t value) {
	HAL_GPIO_WritePin(MAX_CS_PORT, MAX_CS_PIN, 0);
	for (int i = 0; i < MAX_amount; i++) {
		MAX_send_data(address, value);
	}
	HAL_GPIO_WritePin(MAX_CS_PORT, MAX_CS_PIN, 1);
}

/*------------ Функция: MAX_clear ------------*/
//Очистить дисплей (не буфер);
void MAX_clear_display() {
	for (uint8_t k = 0; k < 8; k++) {
		HAL_GPIO_WritePin(MAX_CS_PORT, MAX_CS_PIN, 0);
		for (uint16_t i = 0; i < MAX_amount; i++)
			MAX_send_data(8 - k, 0);
		HAL_GPIO_WritePin(MAX_CS_PORT, MAX_CS_PIN, 1);
	}
}

/*------------ Функция: MAX_init ------------*/
//Инициализация дисплея;
//где width - кодичесво дисплеев в ширину
//height - кодичесво дисплеев в высоту
void MAX_init(uint16_t width, uint16_t height) {
	MAX_width = width;
	MAX_height = height;
	MAX_amount = width * height;
	MAX_maxX = width * 8;
	MAX_maxY = height * 8;

	MAX_send_smd(0x0f, 0x00);  // отключить режим теста
	MAX_send_smd(0x09, 0x00);  // выключить декодирование
	MAX_send_smd(0x0a, 0x03);  // яркость
	MAX_send_smd(0x0b, 0x0f);  // отображаем всё
	MAX_send_smd(0x0c, 0x01);  // включить
	MAX_clear_display();  // очистить
}

/*------------ Функция: MAX_set_bright ------------*/
//Установка яркости;
//где br - яткость от 0 до 15
void MAX_set_bright(uint8_t br) {
	br > 15 ? br = 15 : 0;
	MAX_send_smd(0x0a, br);
}

/*------------ Функция: MAX_set_power ------------*/
//Переключение питания
//где 0 - выкл, 1 - вкл
void MAX_set_power(bool value) {
	MAX_send_smd(0x0c, value);
}

/*------------ Функция: MAX_fill_byte ------------*/
//Запонить матрицу байтом;
void MAX_fill_byte(uint8_t data) {
	for (uint16_t i = 0; i < MAX_width * MAX_height * 8; i++)
		MAX_buffer[i] = data;
}

/*------------ Функция: MAX_clear ------------*/
//Очистить матрицу;
void MAX_clear() {
	MAX_fill_byte(0);
}

/*------------ Функция: MAX_fill ------------*/
//Запонить матрицу;
void MAX_fill() {
	MAX_fill_byte(255);
}

int16_t MAX_get_position(int16_t x, int16_t y) {
	int16_t bb = y;
	switch (MAX_conn) {
	// case GM_LEFT_TOP_RIGHT: break;
	case GM_RIGHT_TOP_LEFT:
		x = MAX_maxX - x - 1;
		y = (y & 0xF8) + 7 - (y & 7);  // (v / 8 + 1) * 8 - 1 - (v % 8)
		break;
	case GM_RIGHT_BOTTOM_LEFT:
		y = MAX_maxY - y - 1;
		x = MAX_maxX - x - 1;
		break;
	case GM_LEFT_BOTTOM_RIGHT:
		y = MAX_maxY - y - 1;
		y = (y & 0xF8) + 7 - (y & 7);  // (v / 8 + 1) * 8 - 1 - (v % 8)
		break;
	case GM_LEFT_TOP_DOWN:
		y = x;
		x = bb;
		y = (y & 0xF8) + 7 - (y & 7);  // (v / 8 + 1) * 8 - 1 - (v % 8)
		break;
	case GM_RIGHT_TOP_DOWN:
		y = x;
		x = bb;
		y = MAX_maxY - y - 1;
		break;
	case GM_RIGHT_BOTTOM_UP:
		y = x;
		x = bb;
		x = MAX_maxX - x - 1;
		y = (y & 0xF8) + 7 - (y & 7);  // (v / 8 + 1) * 8 - 1 - (v % 8)
		y = MAX_maxY - y - 1;
		break;
	case GM_LEFT_BOTTOM_UP:
		y = x;
		x = bb;
		x = MAX_maxX - x - 1;
		break;
	}
	if (x < 0 || x >= MAX_maxX || y < 0 || y >= MAX_maxY)
		return -1;

	int16_t b = y;
	switch (MAX_rot) {
	// case 0: break;
	case 1:
		y = (y & 0xF8) + (x & 7);
		x = (x & 0xF8) + 7 - (b & 7);
		break;
	case 2:
		x = (x & 0xF8) + 7 - (x & 7);  // (v / 8 + 1) * 8 - 1 - (v % 8)
		y = (y & 0xF8) + 7 - (y & 7);  // (v / 8 + 1) * 8 - 1 - (v % 8)
		break;
	case 3:
		y = (y & 0xF8) + 7 - (x & 7);
		x = (x & 0xF8) + (b & 7);
		break;
	}

	switch (MAX_flip) {
	// case 0: break;
	case 1:
		x = (x & 0xF8) + 7 - (x & 7);  // (v / 8 + 1) * 8 - 1 - (v % 8)
		break;
	case 2:
		y = (y & 0xF8) + 7 - (y & 7);  // (v / 8 + 1) * 8 - 1 - (v % 8)
		break;
	case 3:
		x = (x & 0xF8) + 7 - (x & 7);  // (v / 8 + 1) * 8 - 1 - (v % 8)
		y = (y & 0xF8) + 7 - (y & 7);  // (v / 8 + 1) * 8 - 1 - (v % 8)
		break;
	}

	if (MAX_type == GM_ZIGZAG) {
		if ((y >> 3) & 1) {            // если это нечётная матрица: (y / 8) % 2
			x = MAX_maxX - 1 - x;               // отзеркалить x
			y = (y & 0xF8) + (7 - (y & 7)); // отзеркалить y: (y / 8) * 8 + (7 - (y % 8));
		}
	}

	MAX_bx = x & 7;
	return MAX_width * (MAX_height - 1 - (y >> 3)) + (MAX_width - 1 - (x >> 3))
			+ (y & 7) * MAX_width * MAX_height;  // позиция в буфере
}

/*------------ Функция: MAX_set_dot ------------*/
//Поставить точку;
//где x и y - координаты точки
//value - состояние вкл/выкл
void MAX_set_dot(int x, int y, uint8_t value) {
	int16_t pos = MAX_get_position(x, y);
	if (pos >= 0)
		bitWrite(MAX_buffer[pos], MAX_bx, value);
}

/*------------ Функция: MAX_get_dot ------------*/
//Получить точку;
//где x и y - координаты точки
//возвращает - состояние  вкл/выкл
bool MAX_get_dot(int x, int y) {
	int16_t pos = MAX_get_position(x, y);
	if (pos >= 0)
		return bitRead(MAX_buffer[pos], MAX_bx);
	else
		return 0;
}

/*------------ Функция: MAX_update ------------*/
//Обновить матрицу;
void MAX_update() {
	uint16_t count = 0;
	for (int k = 0; k < 8; k++) {
		HAL_GPIO_WritePin(MAX_CS_PORT, MAX_CS_PIN, 0);
		for (uint16_t i = 0; i < MAX_amount; i++) {
			MAX_send_data(8 - k, MAX_buffer[count++]);
		}
		HAL_GPIO_WritePin(MAX_CS_PORT, MAX_CS_PIN, 1);
	}
}

/*------------ Функция: MAX_sendByte ------------*/
//Отправка данных напрямую в матрицу;
//где addres - адрес обращения
//value - информация в байтах
void MAX_sendByte(uint8_t address, uint8_t value) {
	MAX_send_smd(address + 1, value);
}

/*------------ Функция: MAX_setRotation ------------*/
//Поворот матрицы (8x8): 0, 1, 2, 3 на 90 град по часовой стрелке
//где rot - значение поворота
void MAX_setRotation(uint8_t rot) {
	MAX_rot = rot;
}

/*------------ Функция: MAX_setType ------------*/
//Установка типа дисплея: построчный последовательный (GM_SERIES) или зигзаг GM_ZIGZAG
//где type - тип дисплея
void MAX_setType(bool type) {
	MAX_type = type;
}

/*------------ Функция: MAX_setFlip ------------*/
// зеркальное отражение матриц (8x8) по x и y
void MAX_setFlip(uint8_t x, uint8_t y) {
	MAX_flip = x | (y << 1);
}

/*------------ Функция: MAX_setConnection ------------*/
// ориентация (точка подключения дисплея)
//где conn - ориентация
void MAX_setConnection(uint8_t conn) {
	MAX_conn = conn;
}

// ============================================ GFX ==========================================

/*------------ Функция: MAX_lineH ------------*/
// Вертикальная линия;
void MAX_lineH(int y, int x0, int x1, bool fill) {
	if (x0 > x1) {
		int c = x0;
		x0 = x1;
		x1 = c;
	}
	for (int x = x0; x <= x1; x++)
		MAX_set_dot(x, y, fill);
}

/*------------ Функция: MAX_lineV ------------*/
// Горизонтальная линия;
void MAX_lineV(int x, int y0, int y1, bool fill) {
	if (y0 > y1) {
		int c = y0;
		y0 = y1;
		y1 = c;
	}
	for (int y = y0; y <= y1; y++)
		MAX_set_dot(x, y, fill);
}

/*------------ Функция: MAX_lineV ------------*/
// Линия;
void MAX_line(int x0, int y0, int x1, int y1, bool fill) {
	if (x0 == x1)
		MAX_lineV(x0, y0, y1, fill);
	else if (y0 == y1)
		MAX_lineH(y0, x0, x1, fill);
	else {
		int8_t sx = (x0 < x1) ? 1 : -1;
		int8_t sy = (y0 < y1) ? 1 : -1;
		int dx = abs(x1 - x0);
		int dy = abs(y1 - y0);
		int err = dx - dy;
		int e2 = 0;
		for (;;) {
			MAX_set_dot(x0, y0, fill);
			if (x0 == x1 && y0 == y1)
				return;
			e2 = err << 1;
			if (e2 > -dy) {
				err -= dy;
				x0 += sx;
			}
			if (e2 < dx) {
				err += dx;
				y0 += sy;
			}
		}
	}
}

/*------------ Функция: MAX_line ------------*/
//Прямоугольник
void MAX_rect(int x0, int y0, int x1, int y1, uint8_t fill) {
	if (y0 > y1) {
		int c = y0;
		y0 = y1;
		y1 = c;
	}
	if (x0 > x1) {
		int c = x0;
		x0 = x1;
		x1 = c;
	}
	if (fill == GFX_STROKE) {
		MAX_lineH(y0, x0 + 1, x1 - 1, fill);
		MAX_lineH(y1, x0 + 1, x1 - 1, fill);
		MAX_lineV(x0, y0, y1, fill);
		MAX_lineV(x1, y0, y1, fill);
	} else {
		for (int y = y0; y <= y1; y++)
			MAX_lineH(y, x0, x1, fill);
	}
}

/*------------ Функция: MAX_rectWH ------------*/
// прямоугольник с реглировкой высоты и длинны с помощю W и H
void MAX_rectWH(int x0, int y0, int w, int h, uint8_t fill) {
	MAX_rect(x0, y0, x0 + w - 1, y0 + h - 1, fill);
}

/*------------ Функция: MAX_roundRect ------------*/
//Скруглённый прямоугольник
void MAX_roundRect(int x0, int y0, int x1, int y1, uint8_t fill) {
	if (y0 > y1) {
		int c = y0;
		y0 = y1;
		y1 = c;
	}
	if (x0 > x1) {
		int c = x0;
		x0 = x1;
		x1 = c;
	}
	if (fill == GFX_STROKE) {
		MAX_lineV(x0, y0 + 2, y1 - 2, fill);
		MAX_lineV(x1, y0 + 2, y1 - 2, fill);
		MAX_lineH(y0, x0 + 2, x1 - 2, fill);
		MAX_lineH(y1, x0 + 2, x1 - 2, fill);
		MAX_set_dot(x0 + 1, y0 + 1, fill);
		MAX_set_dot(x1 - 1, y0 + 1, fill);
		MAX_set_dot(x1 - 1, y1 - 1, fill);
		MAX_set_dot(x0 + 1, y1 - 1, fill);
	} else {
		MAX_lineV(x0, y0 + 2, y1 - 2, fill);
		MAX_lineV(x0 + 1, y0 + 1, y1 - 1, fill);
		MAX_lineV(x1 - 1, y0 + 1, y1 - 1, fill);
		MAX_lineV(x1, y0 + 2, y1 - 2, fill);
		MAX_rect(x0 + 2, y0, x1 - 2, y1, fill);
	}
}

/*------------ Функция: MAX_roundRectWH ------------*/
//Скруглённый прямоугольник
void MAX_roundRectWH(int x0, int y0, int w, int h, uint8_t fill) {
	MAX_roundRect(x0, y0, x0 + w - 1, y0 + h - 1, fill);
}

/*------------ Функция: MAX_circle ------------*/
//Окружность
void MAX_circle(int x, int y, int radius, uint8_t fill) {
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x1 = 0;
	int y1 = radius;
	uint8_t fillLine = (fill == GFX_CLEAR) ? 0 : 1;
	MAX_set_dot(x, y + radius, fillLine);
	MAX_set_dot(x, y - radius, fillLine);
	MAX_set_dot(x + radius, y, fillLine);
	MAX_set_dot(x - radius, y, fillLine);
	if (fill != GFX_STROKE)
		MAX_lineV(x, y - radius, y + radius - 1, fillLine);
	while (x1 < y1) {
		if (f >= 0) {
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}
		x1++;
		ddF_x += 2;
		f += ddF_x;
		if (fill == GFX_STROKE) {
			MAX_set_dot(x + x1, y + y1, fill);
			MAX_set_dot(x - x1, y + y1, fill);
			MAX_set_dot(x + x1, y - y1, fill);
			MAX_set_dot(x - x1, y - y1, fill);
			MAX_set_dot(x + y1, y + x1, fill);
			MAX_set_dot(x - y1, y + x1, fill);
			MAX_set_dot(x + y1, y - x1, fill);
			MAX_set_dot(x - y1, y - x1, fill);
		} else {
			MAX_lineV(x + x1, y - y1, y + y1, fillLine);
			MAX_lineV(x - x1, y - y1, y + y1, fillLine);
			MAX_lineV(x + y1, y - x1, y + x1, fillLine);
			MAX_lineV(x - y1, y - x1, y + x1, fillLine);
		}
	}
}

// Функция для зеркалирования битов внутри байта (по горизонтали)
uint8_t MAX_reverse_bits(uint8_t byte) {
	uint8_t reversed = 0;
	for (int i = 0; i < 8; i++) {
		reversed |= ((byte >> i) & 1) << (7 - i);  // Переворачиваем биты
	}
	return reversed;
}

void MAX_reverse(uint8_t *in, uint8_t *out, int width, int height, uint16_t rot) {
	int width_bytes = (width + 7) / 8;
	int height_bytes = (height + 7) / 8;
	// init *out. You can skip the next line if you are sure that *out is clear.
	if (width >= height) {
		for (int x = 0; x < width; x++) {
			out[x] = 0;
		}
	} else {
		for (int x = 0; x < height; x++) {
			out[x] = 0;
		}
	}

	switch (rot) {
	case 0:
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				if (in[(y * width_bytes + x / 8)] & (1 << (x % 8))) {
					out[(x * height_bytes + y / 8)] |= (1 << (y % 8));
				}
			}
		}
		break;
	case 90:
		if (width >= height) {
			for (int x = 0; x < width; x++) {
				out[x] = in[x];
			}
		} else {
			for (int x = 0; x < height; x++) {
				out[x] = in[x];
			}
		}
		break;
	case 180:
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				if (in[(y * width_bytes + x / 8)] & (1 << (7 - x % 8))) {
					out[(x * height_bytes + y / 8)] |= (1 << (7 - y % 8));
				}
			}
		}
		break;
	case 270:
		if (width >= height) {
			for (int x = 0; x < width; x++) {
				out[x] = in[x];
			}
		} else {
			for (int x = 0; x < height; x++) {
				out[x] = in[x];
			}
		}
		for (int i = 0; i < width * height; i++) {
			out[i] = MAX_reverse_bits(out[i]);  // Переворачиваем каждый байт
		}
		break;
	}
}

/*------------ Функция: MAX_drawBitmap ------------*/
//Битмап
void MAX_drawBitmap(int x, int y, const uint8_t *frame, int width, int height,
		uint8_t invert, uint8_t mode, uint16_t rot) {

	uint8_t bytes = width >> 3;
	uint8_t left = width & 0b111;
	if (left)
		bytes++;

	MAX_reverse(frame, MAX_IconOut, width, height, rot);

	for (int yy = 0; yy < height; yy++) {
		for (int xx = 0; xx < (width >> 3); xx++) {
			uint8_t thisByte = (MAX_IconOut[xx + yy * bytes]) ^ invert;
			for (uint8_t k = 0; k < 8; k++) {
				uint8_t val = thisByte & 0b10000000;
				if (val || !mode)
					MAX_set_dot((xx << 3) + k + x, yy + y, val);
				thisByte <<= 1;
			}
		}
		if (left) {
			uint8_t thisByte = (MAX_IconOut[(width >> 3) + yy * bytes])
					^ invert;
			for (uint8_t k = 0; k < left; k++) {
				uint8_t val = thisByte & 0b10000000;
				if (val || !mode)
					MAX_set_dot(((width >> 3) << 3) + k + x, yy + y, val);
				thisByte <<= 1;
			}
		}
	}
}

// ======================================== TEXT =======================================

struct MAX_gfx {
	uint16_t x;
	uint16_t y;
	uint8_t scale;
	bool invert;
	bool println;
	bool tmode;
	uint16_t tx0;
	uint16_t tx1;
};

struct MAX_gfx cfg;

uint8_t MAX_lastChar;

/*------------ Функция: MAX_strlen_fix ------------*/
//Определить длину строки с любыми символами (в т.ч. русскими)
uint16_t MAX_strlen_fix(const unsigned char *str) {
	uint16_t i = 0, count = 0;
	while (str[i]) {
		if ((str[i] & 0xc0) != 0x80)
			count++;
		i++;
	}
	return count;
}

/*------------ Функция: MAX_setCursor ------------*/
//Установить курсор
void MAX_setCursor(int x, int y) {
	cfg.x = x;
	cfg.y = y;
}

/*------------ Функция: MAX_getCursorX ------------*/
//Получить курсор x
int MAX_getCursorX() {
	return cfg.x;
}

/*------------ Функция: MAX_getCursorY ------------*/
//Получить курсор y
int MAX_getCursorY() {
	return cfg.y;
}

/*------------ Функция: MAX_setScale ------------*/
//Установить масштаб текста (1-4)
void MAX_setScale(uint8_t scale) {
	if (scale < 1)
		scale = 1;
	if (scale > 4)
		scale = 4;
	scale = scale;
	cfg.scale = scale;
}

/*------------ Функция: MAX_getScale ------------*/
//Получить масштаб текста
uint8_t MAX_getScale() {
	return cfg.scale;
}

/*------------ Функция: MAX_invertText ------------*/
//Установить инверсию текста
void MAX_invertText(bool inv) {
	cfg.invert = inv;
}

/*------------ Функция: MAX_getInvertText ------------*/
//Получить инверсию текста
bool MAX_getInvertText() {
	return cfg.invert;
}

/*------------ Функция: MAX_autoPrintln ------------*/
//Установить автоматический перенос текста
void MAX_autoPrintln(bool mode) {
	cfg.println = mode;
}

/*------------ Функция: MAX_getAutoPrintln ------------*/
//Получить автоматический перенос текста
bool MAX_getAutoPrintln() {
	return cfg.println;
}

/*------------ Функция: MAX_textDisplayMode ------------*/
//Установить режим вывода текста GFX_ADD/GFX_REPLACE
void MAX_textDisplayMode(bool mode) {
	cfg.tmode = mode;
}

/*------------ Функция: MAX_getTextDisplayMode ------------*/
//Получить режим вывода текста
bool MAX_getTextDisplayMode() {
	return cfg.tmode;
}

/*------------ Функция: MAX_setTextBound ------------*/
//Установить границы вывода текста по х
void MAX_setTextBound(int x0, int x1) {
	cfg.tx0 = x0;
	cfg.tx1 = x1;
}

/*------------ Функция: MAX_getTextBoundX0 ------------*/
//Получить границу вывода 0
int MAX_getTextBoundX0() {
	return cfg.tx0;
}

/*------------ Функция: MAX_getTextBoundX1 ------------*/
//Получить границу вывода 1
int MAX_getTextBoundX1() {
	return cfg.tx1;
}

/*------------ Функция: MAX_resetTextBound ------------*/
//Сбросить границы вывода текста до (0, ширина)
void MAX_resetTextBound() {
	cfg.tx0 = 0;
	cfg.tx1 = (MAX_width * 8) - 1;
}

// ================== WRITE ===================

void MAX_drawByte(uint8_t bits) {
	if (cfg.invert)
		bits = ~bits;
	if (cfg.scale == 1) {                         // если масштаб 1
		if (cfg.x >= 0 && cfg.x < (MAX_width * 8)) {  // внутри дисплея
			for (uint8_t y = 0; y < 8; y++) {
				bool bit = bitRead(bits, y);
				if (bit || !cfg.tmode) {
					if (cfg.x >= cfg.tx0 && cfg.x <= cfg.tx1)
						MAX_set_dot(cfg.x, cfg.y + y, bit);
				}

			}
		}
		cfg.x++;
	} else {  // масштаб 2, 3 или 4 - растягиваем шрифт
		uint32_t buf = 0;
		for (uint8_t i = 0, count = 0; i < 8; i++) {
			for (uint8_t j = 0; j < cfg.scale; j++, count++) {
				bitWrite(buf, count, bitRead(bits, i)); // пакуем растянутый шрифт
			}
		}

		for (uint8_t i = 0; i < cfg.scale; i++) {
			for (uint8_t j = 0; j < (cfg.scale << 3); j++) {
				bool bit = bitRead(buf, j);
				if ((bit || !cfg.tmode)
						&& (cfg.x + i >= cfg.tx0 && cfg.x + i <= cfg.tx1))
					MAX_set_dot(cfg.x + i, cfg.y + j, bit);
			}
		}
		cfg.x += cfg.scale;
	}
}

/*------------ Функция: MAX_write_char ------------*/
//Написать букву
void MAX_write_char(unsigned char ch) {
	uint8_t data = ch;

	if (data > 191)
		return;
	if (data < 128)
		data = data - 32;
	if (data >= 144 && data <= 175)
		data = data - 49;
	if (data >= 128 && data <= 143)
		data = data + 15;
	if (data >= 176 && data <= 191)
		data = data - 49;
	for (uint8_t col = 0; col < 6; col++) {  // 6 столбиков буквы
		uint8_t bits = MAX_font5x8[data][col];
		if (col > 4)
			bits = 0;
		MAX_drawByte(col > 5 ? 0 : bits);
	}
}

/*------------ Функция: MAX_write_text ------------*/
//Написать текст
void MAX_write_text(char *str) {
	while (*str) {
		MAX_write_char(*str++);
		if (cfg.println && (cfg.x + 6 * cfg.scale) > MAX_width * 8) {
			cfg.x = 0;  // строка переполненена, перевод и возврат
			cfg.y += (cfg.scale << 3);
			MAX_setCursor(cfg.x, cfg.y);
		}
	}
}

// ================== Running text ===================

const unsigned char *MAX_str_buffer = 0;
uint32_t MAX_tmr = 0, MAX_prd = 50;
uint16_t MAX_len = 0;
int16_t MAX_pos = 0;

/*------------ Функция: MAX_RTsetText ------------*/
//Установить текст const char*
void MAX_RTsetText(const unsigned char *str) {
	MAX_str_buffer = str;
	MAX_len = MAX_strlen_fix(MAX_str_buffer) * 6;
}

/*------------ Функция: MAX_RTsetSpeed ------------*/
//Установить скорость (пикселей в секунду)
void MAX_RTsetSpeed(uint16_t pixPerSec) {
	if (!pixPerSec)
		pixPerSec = 1;
	MAX_prd = 1000 / pixPerSec;
}

/*------------ Функция: MAX_RTresume ------------*/
//Продолжить движение с момента остановки
void MAX_RTresume() {
	MAX_tmr = (uint32_t) HAL_GetTick();
	if (!MAX_tmr)
		MAX_tmr = 1;
}

/*------------ Функция: MAX_RTstart ------------*/
//Запустить бегущую строку с начала
void MAX_RTstart() {
	MAX_pos = cfg.tx1 + 1;
	MAX_RTresume();
}

/*------------ Функция: MAX_RTstop ------------*/
//Остановить бегущую строку
void MAX_RTstop() {
	MAX_tmr = 0;
}

/*------------ Функция: MAX_RTtickManual ------------*/
//Сдвинуть строку на 1 пиксель
void MAX_RTtickManual() {
	MAX_setCursor(MAX_pos, cfg.y);
	MAX_write_text(MAX_str_buffer);
	MAX_update();
	if (--MAX_pos <= cfg.tx0 - (int16_t) (MAX_len * cfg.scale)) {
		MAX_RTstart();
	}
}

/*------------ Функция: MAX_RTtick ------------*/
//Тикер
void MAX_RTtick() {
	if (MAX_tmr && (uint32_t) ((uint32_t) HAL_GetTick() - MAX_tmr) >= MAX_prd) {
		MAX_RTresume();
		MAX_RTtickManual();
	}
}

