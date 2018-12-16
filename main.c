/*!
\file
  \bref Основной файл программы
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь-Декабрь 2018 года

  Файл содержит основной алгоритм программы и подпрограмму инициализации оборудования
 */
#include "main.h"

/// Матрица данных датчиков
static uint32_t sensors_data[SENSORS_BUF_LENGTH][SENSORS_COUNT];

/// Номер активной строки матрицы данных датчиков (== -1 если буфер пуст)
static int sensors_buf_row;

/// Макрос очистки буфера
#define clear_sensors_buf() (sensors_buf_row = -1)

/*!
  \bref Основная программа
  \return Результата выполнения

  Содержит вызов функции инициализации и основной алгоритм программы
 */
int main(void) {
  uint32_t i;
  uint32_t current_time;
  
  // Инициализация с проверкой результата
  if(init() != END_OK) return END_ERROR;

  /* Включение светодиода на 1 секунду для сигнализации об успешной инициализации */
  LED_ON(LED0);
  delay(1000);
  LED_OFF(LED0);

  /* Бесконечный цикл основного алгоритма */
  while (1) {
    // Контрольная точка USB
    time_controll();

    // Задержка на 700мс
    delay(700);

    /* Выполнение проверок суточного интервала */
    current_time = get_time_from_date(current_date);
    if (
        (current_time >= interval_start) && // Проверка наступление нижней границы суточного интервала
        (current_time <= interval_end) && // Проверка наступление верхней границы суточного интервала
        (n_proportion(current_time - interval_start) == 0) // Проверка начала 15-ти минутного интервала
        ) {
            // Включение светодиода в связи с проверкой датчиков
            LED_ON(LED0);

            /* Циклическое увеличение крайнего положения буфера данных датчиков */
            sensors_buf_row++;
            sensors_buf_row = sensors_buf_row == SENSORS_BUF_LENGTH ? 0 : sensors_buf_row;

            /* Проход по всем датчикам в текущей строке буфера и получение данных от них */
            for (i = 1; i <= SENSORS_COUNT; i++)
              get_sensor_data(i, sensors_data[sensors_buf_row] + i - 1);

            // Передача данных серверу
            sensors_data_transmit();
          }
    else {
      // Выключение светодиода
      LED_OFF(LED0);
    }
  }

  // Окончание программы
  return END_OK;
}


/*!
  \bref Функция инициализации
  \return Статус завершения

  Содержит функции инициализации и определение успешности инициализации
 */
uint32_t init(void) {
  // Инициализация строки буфера данных датчиков как пустого
  clear_sensors_buf();

  /* Инициализация светодиодов для индикации процессов */
  RST_CLK_PCLKcmd (RST_CLK_PCLK_PORTC, ENABLE);
  PORT_InitTypeDef GPIOInitStruct;
  PORT_StructInit(&GPIOInitStruct);
  GPIOInitStruct.PORT_Pin        = PORT_Pin_0 | PORT_Pin_1;
  GPIOInitStruct.PORT_OE         = PORT_OE_OUT;
  GPIOInitStruct.PORT_SPEED      = PORT_SPEED_SLOW;
  GPIOInitStruct.PORT_MODE       = PORT_MODE_DIGITAL;
  PORT_Init(MDR_PORTC, &GPIOInitStruct);

  /* Трюк описания проверки успешности запуска каждого модуля */
  uint32_t init_status = END_OK;
  init_status += rst_clk_pll_init();
  init_status += time_init();
  init_status += delay_init();

  /* Проверка наличия диагностического режима в поставке */
  #ifdef DIAGNOSTIC
  init_status += sel_butt_init();
  init_status += diagnostic_init();
  init_status += diagnostic_start_init();
  #endif
  
  init_status += usb_init();
  init_status += can_init();

  return init_status;
}


/*!
  \bref Функция инициализации тактирования
  \return Статус завершения
  \attention После инициализации частота процессорного ядра составляет 72 МГц

  Содержит функции инициализации PLL
 */
uint32_t rst_clk_pll_init(void) {
  MDR_RST_CLK->HS_CONTROL = 0x00000001; // Включение HSE, режим осциллятора (8МГц)
  while((MDR_RST_CLK->CLOCK_STATUS&0x04) == 0x00); // Ожидание выхлда HSE в штатный режим
  MDR_EEPROM->CMD = 5<<3;

  // Инициализация PLL
  /*!
    Выбираем в качестве источника опорного сигнала HSE (внешний) с делением на 1.
    Умножаем входную частоту на 8 (8 МГц * 9 = 72 МГц)
   */
  MDR_RST_CLK->PLL_CONTROL = (8<<8) | (1<<2); // Включение CPU_PLL к-та умножения
  while((MDR_RST_CLK->CLOCK_STATUS&0x02)==0x00); // Ожидание выхода CPU_PLL в штатный режим

  MDR_EEPROM->CMD = 3<<3; // Задержка для обращения к flash-памяти Delay = 3; Обход проблемы ядра

  MDR_RST_CLK->CPU_CLOCK |= 0x00000106; // Задать HCLK = CPU_C3 = CPU_C2 = CPU_PLL; CPU_C1 = HSE
  SystemCoreClockUpdate();

  return END_OK;
}


/*!
  \brief      Передача данных датчиков из буфера

  \return     Статус завершения

  Отправляет данные на сервер если есть подключение
 */
uint32_t sensors_data_transmit() {
  uint32_t k, i;

  /* Проверка наличия записей в буфере и подключения по USB */
  if (sensors_buf_row != -1 && is_usb_connected()) {
    /* Проход по всем строкам в буфере */
    for (k = 0; k < sensors_buf_row + 1; k++) {
      // Переключение светодиода обозначает передачу данных серверу
      LED_TOGGLE(LED1);
      
      /* Проход по всем датчикам в строке и передача данных серверу */
      for (i = 1; i <= SENSORS_COUNT; i++) {
        send_via_usb_cdc(
          sensor_data(i, sensors_data[k][i-1], SENSOR_FORMAT_MODE_USB)
        );

        // Ожадание 100мс
        delay(100);
      }
    }
    LED_OFF(LED1);

    // Сброс буфера данных датчиков
    clear_sensors_buf();
    return END_OK;
  }
  else 
    return END_GOOD;
}
