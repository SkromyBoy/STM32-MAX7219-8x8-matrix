# MAX7219_Matrix

Библиотека для матриц 8x8 MAX7219

В качестве основы была взята библиотека Алекса Гайвера, [GyverMAX7219](https://github.com/GyverLibs/GyverMAX7219)
данная библиотека была переписана и адаптирована под stm32.

- Подключение матриц зигзагом
- Аппаратный и программный SPI
- Работает с дисплеями любой конструкции

### Совместимость

Совместима со всеми STM32 платформами (используются HAL функции)

## Содержание

- [Инициализация](#init)
- [Использование](#usage)
- [Пример](#example)
- [Версии](#versions)

<a id="init"></a>

## Инициализация

### MAX7219_Matrix.h

```c
#define MAX_7219_USE_SPI 1 //Выбор подключания (аппаратный / программный spi)

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
```

### main.c

```c
/* USER CODE BEGIN 2 */
MAX_init(3, 1);                  //Инициализация дисплея;
MAX_set_bright(5);               //Установка яркости;
MAX_setRotation(3);              //Поворот матрицы (8x8): 0, 1, 2, 3 на 90 град по часовой стрелке;
MAX_setType(0);                  //Установка типа дисплея: построчный последовательный (GM_SERIES) или зигзаг GM_ZIGZAG
MAX_setConnection(0);            //Ориентация (точка подключения дисплея)
/* USER CODE END 2 */
```

## Дисплей

![setType](/doc/setType.png)
![setConnection](/doc/setConnection.png)

<a id="usage"></a>

## Использование

```c
// настройка
void begin();                       // запустить
void MAX_set_bright(uint8_t br);    // установить яркость [0-15]
void MAX_set_power(bool value);     // переключить питание

// ориентация
void MAX_setRotation(uint8_t rot);        // поворот МАТРИЦ (8x8): 0, 1, 2, 3 на 90 град по часовой стрелке
void MAX_setFlip(uint8_t x, uint8_t y);   // зеркальное отражение МАТРИЦ (8x8) по x и y
void MAX_setType(bool type);              // конструкция дисплея (тип строчности)
void MAX_setConnection(uint8_t conn);     // точка подключения дисплея

// рисование
void MAX_clear(void);                           // очистить
void MAX_fill(void);                            // залить
void MAX_fill_byte(uint8_t data);               // залить байтом
void MAX_set_dot(int x, int y, uint8_t value);  // установить точку
bool MAX_get_dot(int x, int y);                 // получить точку
void MAX_update(void);                          // обновить

// графика
void MAX_lineH(int y, int x0, int x1, bool fill);                   // вертикальная линия, fill - GFX_CLEAR/GFX_FILL/GFX_STROKE
void MAX_lineV(int x, int y0, int y1, bool fill);                   // горизонтальная линия, fill - GFX_CLEAR/GFX_FILL/GFX_STROKE
void MAX_line(int x0, int y0, int x1, int y1, bool fill);           // линия, fill - GFX_CLEAR/GFX_FILL/GFX_STROKE
void MAX_rect(int x0, int y0, int x1, int y1, uint8_t fill);        // прямоугольник, fill - GFX_CLEAR/GFX_FILL/GFX_STROKE
void MAX_roundRect(int x0, int y0, int x1, int y1, uint8_t fill);   // скруглённый прямоугольник, fill - GFX_CLEAR/GFX_FILL/GFX_STROKE
void MAX_circle(int x, int y, int radius, uint8_t fill);            // окружность, fill - GFX_CLEAR/GFX_FILL/GFX_STROKE
void MAX_drawBitmap(int x, int y, const uint8_t *frame, int width, int height, uint8_t invert, uint8_t mode, uint16_t rot);  // битмап

// текст
void MAX_setCursor(int x, int y);             // установить курсор
void MAX_setScale(uint8_t scale);             // масштаб текста
void MAX_invertText(bool inv);                // инвертировать текст
void MAX_autoPrintln(bool mode);              // автоматический перенос строки
void MAX_textDisplayMode(bool mode);          // режим вывода текста GFX_ADD/GFX_REPLACE
void MAX_setTextBound(int x0, int x1);        // граница ввода текста
void MAX_resetTextBound();                    // сброс границы ввода текста (0, ширина)
void MAX_write_text(char *str);               // написать текст (в том числе русский)

// бегущая строка
void MAX_RTsetText(const unsigned char *str); // написать текст (в том числе русский)
void MAX_RTsetSpeed(uint16_t pixPerSec);      // установить скорость (пикселей в секунду)
void MAX_RTresume(void);                      // продолжить движение с момента остановки
void MAX_RTstart(void);                       // запустить бегущую строку с начала
void MAX_RTstop(void);                        // остановить бегущую строку
void MAX_RTtick(void);                        // постоянная работа в цикле, обязательно пишем внутри While;
```

<a id="example"></a>

## Пример

Остальные примеры смотри в **examples**!

```c
 /* USER CODE BEGIN 2 */
	MAX_init(3, 1);
	MAX_set_bright(5);
	MAX_setRotation(3);
	MAX_setType(0);

	MAX_setCursor(0, 0);
	MAX_setScale(1);
	MAX_resetTextBound();
	MAX_autoPrintln(0);

	MAX_write_text("Test");
	MAX_update();
	HAL_Delay(1000);
	MAX_setCursor(0, 0);
	MAX_clear();
	MAX_write_text("Тест");
	MAX_update();
	HAL_Delay(1000);
	MAX_setCursor(0, 0);
	MAX_clear();
	MAX_lineH(0, 0, 4, 1);
	MAX_lineV(2, 2, 8, 1);
	MAX_line(7, 0, 14, 7, 1);
	MAX_rect(4, 4, 7, 7, 1);
	MAX_rectWH(16, 2, 5, 5, 2);
	MAX_update();
	HAL_Delay(1000);
	MAX_clear();
	MAX_roundRect(0, 0, 6, 6, 2);
	MAX_roundRectWH(9, 0, 6, 6, 1);
	MAX_circle(19, 3, 2, 2);
	MAX_update();
	HAL_Delay(1000);
	MAX_clear();

	MAX_drawBitmap(15, 0, smile1, 8, 8, 0, 1, 0);
	MAX_update();

	MAX_setTextBound(0, 13);
	MAX_RTsetText("Никогда не сдавайся и у тебя все получится!");
	MAX_RTsetSpeed(15);
	MAX_RTstart();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		MAX_RTtick();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
```

<a id="versions"></a>

## Версии

- v1.0 - релиз
