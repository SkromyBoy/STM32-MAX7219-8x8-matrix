/*
 * MAX7219_Matrix.h
 *
 *  Created on: Nov 6, 2024
 *      Author: SkromnyBoy
 *
 * В качестве примера была взята библиоте Алекса Гайвера.
 *
 * Библиотека для работы с матрицами 8x8 на MAX7219
 *
 */

#ifndef INC_MAX7219_MATRIX_H_
#define INC_MAX7219_MATRIX_H_

#include "stm32f7xx_hal.h"
#include "main.h"
#include "stdbool.h"
#include "MAX_Font5x8.h"

#define MAX_7219_USE_SPI 1

#if (MAX_7219_USE_SPI == 1)
//Для работы MAX7219 c помощю spi:
//MAX7219 CS -> MAX_CS
//MAX7219 CLK -> CLK
//MAX7219 DATA -> MOSI

#define MAX_CS_PORT MAX_CS_GPIO_Port
#define MAX_CS_PIN  MAX_CS_Pin

extern SPI_HandleTypeDef hspi1;
#define MAX7219_SPI_PORT hspi1

#else
//Для работы MAX7219 без spi назовите пины такими названиями:
//MAX7219 CS -> MAX_CS
//MAX7219 CLK -> MAX_CLK
//MAX7219 DATA -> MAX_DATA

#define MAX_CS_PORT MAX_CS_GPIO_Port
#define MAX_CS_PIN  MAX_CS_Pin
#define MAX_CLK_PORT MAX_CLK_GPIO_Port
#define MAX_CLK_PIN  MAX_CLK_Pin
#define MAX_DATA_PORT MAX_DATA_GPIO_Port
#define MAX_DATA_PIN  MAX_DATA_Pin
#endif

#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

#define GM_ZIGZAG 0
#define GM_SERIES 1

#define GM_LEFT_TOP_RIGHT 0
#define GM_RIGHT_TOP_LEFT 1
#define GM_RIGHT_BOTTOM_LEFT 2
#define GM_LEFT_BOTTOM_RIGHT 3

#define GM_LEFT_TOP_DOWN 4
#define GM_RIGHT_TOP_DOWN 5
#define GM_RIGHT_BOTTOM_UP 6
#define GM_LEFT_BOTTOM_UP 7

#define GFX_CLEAR 0 //Очистить фигуру
#define GFX_FILL 1 //Залить фигуру
#define GFX_STROKE 2//Обвести фигуру

/*------------ Функция: MAX_init ------------*/
//Инициализация дисплея;
//где width - кодичесво дисплеев в ширину
//height - кодичесво дисплеев в высоту
void MAX_init(uint16_t width, uint16_t height);

/*------------ Функция: MAX_set_bright ------------*/
//Установка яркости;
//где br - яткость от 0 до 15
void MAX_set_bright(uint8_t br);

/*------------ Функция: MAX_set_power ------------*/
//Переключение питания;
//где 0 - выкл, 1 - вкл
void MAX_set_power(bool value);

/*------------ Функция: MAX_clear ------------*/
//Очистить матрицу;
void MAX_clear(void);

/*------------ Функция: MAX_fill ------------*/
//Запонить матрицу;
void MAX_fill(void);

/*------------ Функция: MAX_fill_byte ------------*/
//Запонить матрицу байтом;
void MAX_fill_byte(uint8_t data);

/*------------ Функция: MAX_set_dot ------------*/
//Поставить точку;
//где x и y - координаты точки
//value - состояние вкл/выкл
void MAX_set_dot(int x, int y, uint8_t value);

/*------------ Функция: MAX_get_dot ------------*/
//Получить точку;
//где x и y - координаты точки
//возвращает - состояние точки вкл/выкл
bool MAX_get_dot(int x, int y);

/*------------ Функция: MAX_update ------------*/
//Обновить матрицу;
void MAX_update(void);

/*------------ Функция: MAX_sendByte ------------*/
//Отправка данных напрямую в матрицу;
//где addres - адрес обращения
//value - информация в байтах
void MAX_sendByte(uint8_t address, uint8_t value);

/*------------ Функция: MAX_clear_display ------------*/
//Очистить дисплей (не буфер);
void MAX_clear_display(void);

/*------------ Функция: MAX_setRotation ------------*/
//Поворот матрицы (8x8): 0, 1, 2, 3 на 90 град по часовой стрелке;
//где rot - значение поворота
void MAX_setRotation(uint8_t rot);

/*------------ Функция: MAX_setFlip ------------*/
// зеркальное отражение матриц (8x8) по x и y
void MAX_setFlip(uint8_t x, uint8_t y);

/*------------ Функция: MAX_setType ------------*/
//Установка типа дисплея: построчный последовательный (GM_SERIES) или зигзаг GM_ZIGZAG
//где type - тип дисплея
void MAX_setType(bool type);

/*------------ Функция: MAX_setConnection ------------*/
//Ориентация (точка подключения дисплея)
//где conn - ориентация
void MAX_setConnection(uint8_t conn);

// ======================================== GFX =============================================

/*------------ Функция: MAX_lineH ------------*/
//Вертикальная линия;
void MAX_lineH(int y, int x0, int x1, bool fill);

/*------------ Функция: MAX_lineV ------------*/
//Горизонтальная линия;
void MAX_lineV(int x, int y0, int y1, bool fill);

/*------------ Функция: MAX_line ------------*/
//Линия;
void MAX_line(int x0, int y0, int x1, int y1, bool fill);

/*------------ Функция: MAX_rect ------------*/
//Прямоугольник
void MAX_rect(int x0, int y0, int x1, int y1, uint8_t fill);

/*------------ Функция: MAX_rectWH ------------*/
// прямоугольник с реглировкой высоты и длинны с помощю W и H
void MAX_rectWH(int x0, int y0, int w, int h, uint8_t fill);

/*------------ Функция: MAX_roundRect ------------*/
//Скруглённый прямоугольник
void MAX_roundRect(int x0, int y0, int x1, int y1, uint8_t fill);

/*------------ Функция: MAX_roundRectWH ------------*/
//Скруглённый прямоугольник
void MAX_roundRectWH(int x0, int y0, int w, int h, uint8_t fill);

/*------------ Функция: MAX_circle ------------*/
//Окружность
void MAX_circle(int x, int y, int radius, uint8_t fill);

/*------------ Функция: MAX_drawBitmap ------------*/
//Битмап
void MAX_drawBitmap(int x, int y, const uint8_t *frame, int width, int height,
		uint8_t invert, uint8_t mode, uint16_t rot);

// ======================================== TEXT =============================================

/*------------ Функция: MAX_strlen_fix ------------*/
//Определить длину строки с любыми символами
uint16_t MAX_strlen_fix(const unsigned char *str);

/*------------ Функция: MAX_setCursor ------------*/
//Установить курсор
void MAX_setCursor(int x, int y);

/*------------ Функция: MAX_getCursorX ------------*/
//Получить курсор x
int MAX_getCursorX();

/*------------ Функция: MAX_getCursorY ------------*/
//Получить курсор y
int MAX_getCursorY();

/*------------ Функция: MAX_setScale ------------*/
//Установить масштаб текста (1-4)
void MAX_setScale(uint8_t scale);

/*------------ Функция: MAX_getScale ------------*/
//Получить масштаб текста
uint8_t MAX_getScale();

/*------------ Функция: MAX_invertText ------------*/
//Установить инверсию текста
void MAX_invertText(bool inv);

/*------------ Функция: MAX_getInvertText ------------*/
//Получить инверсию текста
bool MAX_getInvertText();

/*------------ Функция: MAX_autoPrintln ------------*/
//Установить автоматический перенос текста
void MAX_autoPrintln(bool mode);

/*------------ Функция: MAX_getAutoPrintln ------------*/
//Получить автоматический перенос текста
bool MAX_getAutoPrintln();

/*------------ Функция: MAX_textDisplayMode ------------*/
//Установить режим вывода текста GFX_ADD/GFX_REPLACE
void MAX_textDisplayMode(bool mode);

/*------------ Функция: MAX_getTextDisplayMode ------------*/
//Получить режим вывода текста
bool MAX_getTextDisplayMode();

/*------------ Функция: MAX_setTextBound ------------*/
//Установить границы вывода текста по х
void MAX_setTextBound(int x0, int x1);

/*------------ Функция: MAX_getTextBoundX0 ------------*/
//Получить границу вывода 0
int MAX_getTextBoundX0();

/*------------ Функция: MAX_getTextBoundX1 ------------*/
//Получить границу вывода 1
int MAX_getTextBoundX1();

/*------------ Функция: MAX_resetTextBound ------------*/
//Сбросить границы вывода текста до (0, ширина)
void MAX_resetTextBound();

/*------------ Функция: MAX_write_char ------------*/
//Написать букву
void MAX_write_char(unsigned char ch);

/*------------ Функция: MAX_write_text ------------*/
//Написать текст
void MAX_write_text(char *str);

// ================== Running text ===================

/*------------ Функция: MAX_RTsetText ------------*/
//Установить текст const char*
void MAX_RTsetText(const unsigned char *str);

/*------------ Функция: MAX_RTsetSpeed ------------*/
//Установить скорость (пикселей в секунду)
void MAX_RTsetSpeed(uint16_t pixPerSec);

/*------------ Функция: MAX_RTresume ------------*/
//Продолжить движение с момента остановки
void MAX_RTresume(void);

/*------------ Функция: MAX_RTstart ------------*/
//Запустить бегущую строку с начала
void MAX_RTstart(void);

/*------------ Функция: MAX_RTstop ------------*/
//Остановить бегущую строку
void MAX_RTstop(void);

/*------------ Функция: MAX_RTtick ------------*/
//Постоянная работа в цикле, обязательно пишем внутри While;
void MAX_RTtick(void);

#endif /* INC_MAX7219_MATRIX_H_ */
