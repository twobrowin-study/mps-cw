/*!
\file
  \bref Форматизация времени
  \author Дубровин Егор гр. ИУ6-72
  \date Ноябрь 2018 года

  Файл содержит реализацию функций форматизации времени и данных от сервера
 */
#include "format.h"

/// Буфер форматизации времени
static char format_buf[9] = "";

/// Буфер форматизации текущих настроек
static char current_settings_buf[28] = "";

/// Буфер форматизации данных датчика
static char sensor_format_string_buf [27] = "";


/*!
  \bref Преобразование числа времени ко строке
  \param time Количество секунд с начала дня
  \return Строка - время в формате HH24:MI:SS

  Преобразует время из числа 3-х битного целого ко строке стандартного формата РФ
 */
char* time_as_string(uint time) {
  // Количество часов (3600 секунд в часу)
  uint hours = time / 3600;
  // Количество минут (60 секунд в минуте (первым делением уберём секунды) и 60 минут в часу)
  uint minutes = (time / 60) % 60;
  // Количество секунд (60 секунд в минуте)
  uint seconds = time % 60;

  /* Заполним шаблон */
  format_buf[8] = '\0';
  format_buf[2] = format_buf[5] = ':';
  /* Часы */
  format_buf[0] = (hours / 10) + '0';
  format_buf[1] = hours % 10 + '0';
  /* Минуты */
  format_buf[3] = (minutes / 10) + '0';
  format_buf[4] = minutes % 10 + '0';
  /* Секунды */
  format_buf[6] = (seconds / 10) + '0';
  format_buf[7] = seconds % 10 + '0';

  return format_buf;
}


/*!
  \brief      Де-форматизация данных от сервера

  \param[in]  data  Входные данные
  \param[in]  dd    Префикс времени
  \param[in]  ctb   Буфер времени
  \param[in]  is    Префикс начала интервала
  \param[in]  isb   Буфер начала интервала
  \param[in]  ie    Префикс окончания интервала
  \param[in]  ieb   Буфер окончания интервала

  \return     Статус завершения

  Функция преобразует данные по формату:
    dd%010 - текушее время по UTC+0
    is%05 - начало суточного интервала по UTC+0
    ie%05 - окончание суточного интервала по UTC+0
 */
uint deformat_dai(
  const char* data,
  char*  dd,
  uint* ctb,
  char*  is,
  uint* isb,
  char*  ie,
  uint* ieb) {
  /* Начало функции */
  uint i;
  
  /* Инициализация строковых буферов */
  char ctbs[11], isbs[6], iebs[6];
  ctbs[10] = isbs[5] = iebs[5] = '\0';

  /* Заполнение префиксов */
  for (i = 0; i < 2; i++) {
    dd[i] = data[i];
    is[i] = data[12 + i];
    ie[i] = data[19 + i];
  }

  /* Заполнение строковых буферов */
  for (i = 0; i < 10; i++) {
    ctbs[i] = data[2 + i];
    if (i < 5) {
      isbs[i] = data[14 + i];
      iebs[i] = data[21 + i];
    }
  }

  /* Преобразование к числам */
  *ctb = atoi(ctbs);
  *isb = atoi(isbs);
  *ieb = atoi(iebs);

  return END_OK;
}


/*!
  \brief      Увеличить длину строки

  \param      str   Строка
  \param[in]  len   Длина
  \param[in]  ch    Символ, заполняющий сдвиг

  \return     Статус завершения

  Сдвигает строку направо до длины len и заполняет пустоту ch
  \attention  Длина строки должна быть len+1
 */
uint upscalestrlen(char *str, uint len, char ch) {
  uint i;
  uint slen = strlen(str);
  
  /* Обработка исключений */
  if (slen == len)
    return END_OK;
  if ((len <= 0) || (slen > len))
    return END_NOT_OK;

  /* Установка длины */
  str[len] = '\0';

  /* Вычисление смещения */
  uint shift = len - slen;

  /* Смещение справа - налево */
  for (i = slen; i > 0; i--)
    str[i + shift - 1] = str[i - 1];

  /* Заполнение пустоты */
  for (i = 0; i < shift; i++)
    str[i] = ch;

  return END_OK;
}


/*!
  \brief      Текущие настройки в виде строки

  \return     Строку текущих параметров по формату USB

  Возвращает строку по протоколу передачи USB
    dd%010 - текушее время по UTC+0
    is%05 - начало суточного интервала по UTC+0
    ie%05 - окончание суточного интервала по UTC+0
 */
char* current_settings() {
  uint i;
  char ctbs[11], isbs[6], iebs[6];

  /* Заполнение окончания строки */
  current_settings_buf[27] = '\0';
  current_settings_buf[26] = '\n';

  /* Заполнение префиксов */
  current_settings_buf[0] = 
  current_settings_buf[1] = 'd';
  current_settings_buf[12] = 
  current_settings_buf[19] = 'i';
  current_settings_buf[13] = 's';
  current_settings_buf[20] = 'e';

  /* Заполнение строковых буферов */
  itoa(current_time, ctbs, 10);
  itoa(interval_start, isbs, 10);
  itoa(interval_end, iebs, 10);

  /* Подстановка под формат */
  upscalestrlen(ctbs, 10, '0');
  upscalestrlen(isbs, 5, '0');
  upscalestrlen(iebs, 5, '0');

  /* Заполнение данных */
  for (i = 0; i < 10; i++) {
    current_settings_buf[2 + i] = ctbs[i];
    if (i < 5) {
      current_settings_buf[14 + i] = isbs[i];
      current_settings_buf[21 + i] = iebs[i];
    }
  }

  return current_settings_buf;
}


/*!
  \brief      Данные датчика в формате передачи

  \param[in]  adr   Адрес
  \param[in]  data  Данные

  \return     Возвращает строку в формате <Адрес датчика> <Время> <Данные>

  Возвращает строку по протоколу передачи данных датчиков
    3 символа - адрес датчика
    10 символов - время передачи
    10 символов - данные датчика
 */
char* sensor_data(uint adr, uint data) {
  uint i;
  char adrs[4], times[11], datas[11];

  /* Заполнение окончания строки */
  sensor_format_string_buf[26] = '\0';
  sensor_format_string_buf[25] = '\n';

  /* Заполнение пробелов */
  sensor_format_string_buf[4] = 
  sensor_format_string_buf[15] = ' ';

 /* Заполнение строковых буферов */
  itoa(adr, adrs, 16);
  itoa(current_time, times, 10);
  itoa(data, datas, 10);

  /* Подстановка под формат */
  upscalestrlen(adrs, 3, '0');
  upscalestrlen(times, 10, '0');
  upscalestrlen(datas, 10, '0');

  /* Заполнение данных */
  for (i = 0; i < 10; i++) {
    sensor_format_string_buf[5 + i] = times[i];
    sensor_format_string_buf[16 + i] = datas[i];
    if (i < 3) {
      sensor_format_string_buf[i] = adrs[i];
    }
  }

  return sensor_format_string_buf;
}
