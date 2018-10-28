/*!
\file
  \bref Точная задержка
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь 2018 года

  Файл содержит реализацию счётчика реального времени и инициализации суточных
    интервалов и соответствующих флагов (см. Переменные.md)
*/
#include "time.h"


/*!
  \bref Инициализация счётчика реального времени
  \return Статус завершения
  \attention Предполагается, что частота генератора LSE - 32,768 кГц

  Содержит инициализацию LSE и подачу частоты в 1 Гц на счётчик реального времени
*/
END_STATUS time_init(void) {
  // Выключить часы реального времени
  BKP_RTC_Enable(DISABLE);
  // Устанавливаем работу LSE
  RST_CLK_LSEconfig(RST_CLK_LSE_ON);
  // Включаем тактирование блока часов реального времни и сохранения состояния
  RST_CLK_PCLKcmd(RST_CLK_PCLK_BKP, ENABLE);
  // Устанавливаем предделитель часов реального времени (32,768 кГц / 32768 = 1Гц)
  BKP_RTC_SetPrescaler(32768);
  // Устанавливаем источник тактирования часов реального времени LSE
  BKP_RTCclkSource(BKP_RTC_LSEclk);
  // Включить часы реального времени
  BKP_RTC_Enable(ENABLE);

  return END_OK;
}


/// Буфер форматизации времени
static char time_format_buf[9] = "";
/*!
  \bref Преобразование числа времени ко строке
  \param time Количество секунд с начала дня
  \return Строка - время в формате HH24:MI:SS

  Преобразует время из числа 3-х битного целого ко строке стандартного формата РФ
*/
char* time_as_string(uint time) {
  // Количество часов (3600 секунд в часу)
  uint hours = time / 3600;
  // Количество минут (1440 минут в сутках и 60 секунд в минуте)
  uint minutes = (time % 1440) / 60;
  // Количество секунд (60 секунд в минуте)
  uint seconds = time % 60;

  /* Заполним шаблон */
  time_format_buf[8] = '\0';
  time_format_buf[2] = time_format_buf[5] = ':';
  /* Часы */
  time_format_buf[0] = (hours / 10) + '0';
  time_format_buf[1] = hours % 10 + '0';
  /* Минуты */
  time_format_buf[3] = (minutes / 10) + '0';
  time_format_buf[4] = minutes % 10 + '0';
  /* Секунды */
  time_format_buf[6] = (seconds / 10) + '0';
  time_format_buf[7] = seconds % 10 + '0';

  return time_format_buf;
}
