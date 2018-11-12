/*!
\file
  \bref Точная задержка
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь-Ноябрь 2018 года

  Файл содержит реализацию счётчика реального времени и инициализации суточных
    интервалов и соответствующих флагов (см. Переменные.md),
    функцию обновления текущего времени и суточных интервалов
 */
#include "time.h"


/*!
  \bref Инициализация счётчика реального времени
  \return Статус завершения

  Содержит инициализацию часов реального времени
 */
END_STATUS time_init(void) {
  // Включаем тактирование блока часов реального времни и сохранения состояния
  RST_CLK_PCLKcmd(RST_CLK_PCLK_BKP, ENABLE);
  
  // Перезпускаем часы реального времени
  BKP_RTC_Reset(ENABLE);
  BKP_RTC_Reset(DISABLE);
  
  // Устанавливаем инициализованные значения начала и конца суточного интервала
  BKP_RTC_WaitForUpdate();
  time_update(0, 0, day_proportion - 1);
  
  time_scale();

  // Запуск часов реального времени
  BKP_RTC_WaitForUpdate();
  BKP_RTC_Enable(ENABLE);

  return END_OK;
}


/*!
  \brief      Установить предделитель таймера

  \return     { description_of_the_return_value }

  \attention Предполагается, что частота генератора LSE - 32,768 кГц

  Устанавливает предделитель часов реального времени (32,768 кГц / 32768 = 1Гц)
 */
END_STATUS time_scale(void) {
  BKP_RTC_WaitForUpdate();
  BKP_RTC_SetPrescaler(32768);
  return END_OK;
}


/*!
  \brief      Функция контроля времени и суточного интервала по USB CDC

  \return     Статус завершения

  Проверяет нобходимости обновления данных и обновляет - если необходимо
    Размер буфера данных соответсвует протоколу передачи - 26 символов (+ \0)
 */
END_STATUS time_controll(void) {
  // Буфер данных
  char data[27];
  data[0] = '\0';
  data[26] = '\0';

  /* Получение входных данных */
  check_usb_cdc();
  strcpy(data, get_recv_via_usb_cdc());
  clean_usb_cdc_recv_buf();

  /* Продолжить только если есть информация */
  if (data[0] == '\0')
    return END_GOOD;

  /* Получение текущей даты */
  // Префиксы передачи
  char dd[2], is[2], ie[2];
  // Буферные числа для считывания
  uint ctb, isb, ieb;

  // Считывание по формату передачи
  deformat_dai(data, dd, &ctb, is, &isb, ie, &ieb);

  /* Проверка правильного заполнения формата передачи */
  if (!((dd[0] == 'd') && (dd[1] == 'd') &&
        (is[0] == 'i') && (is[1] == 's') &&
        (ie[0] == 'i') && (ie[1] == 'e'))
    )
    return END_NOT_OK;

  /* Применение изменений */
  time_update(ctb, isb, ieb);

  return END_OK;
}


/*!
  \brief      Устанавливает текущее время.

  \param[in]  time             Время
  \param[in]  binterval_start  Начало интерала
  \param[in]  binterval_end    Окончание интервала

  \return     Статус завершения
 */
END_STATUS time_update(uint time, uint binterval_start, uint binterval_end) {
  /* Отключение часов реального времени для обновления */
  // BKP_RTC_Enable(DISABLE);
  BKP_RTC_WaitForUpdate();

  /* Применение изменений */
  interval_start = binterval_start;
  interval_end = binterval_end;
  BKP_RTC_SetCounter(time);

  // Обновить тактирование
  time_scale();
  
  /* Включить часы реального времени */
  BKP_RTC_WaitForUpdate();
  BKP_RTC_Enable(ENABLE);
  
  return END_OK;
}
