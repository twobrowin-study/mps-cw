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
  \attention  После прошивки требуется провести сброс МК по питанию без батереи, потом её установить

  Содержит инициализацию часов реального времени
 */
uint32_t time_init(void) {
  // Включаем тактирование блока часов реального времни и сохранения состояния
  RST_CLK_PCLKcmd(RST_CLK_PCLK_BKP, ENABLE);
  
  /* Настройки порта E, к которому подключен осцилятор */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_BKP, ENABLE);
  MDR_PORTE->OE &=0xFFFF0000;
  MDR_PORTE->OE |=(0x1+0x4+0x200);
  MDR_PORTE->FUNC=0;
  MDR_PORTE->FUNC |=(0x1000+0x4000);

  MDR_PORTE->ANALOG |=0xFFFF;
  MDR_PORTE->ANALOG &=(0xFFFF-0x40-0x80);
  MDR_PORTE->PULL =0xFFFF0000;
  MDR_PORTE->PULL &=(0xFFFFFFFF-0x40-0x80-0x400000-0x800000);
  MDR_PORTE->PD=0;
  MDR_PORTE->PWR=0xAAAAAAAA;
  MDR_PORTE->GFEN |=0xFFFF; 

  // Включаем внешний источник LSE
  RST_CLK_LSEconfig(RST_CLK_LSE_ON);

  // Выбираем LSE в качестве источника тактирования
  BKP_RTCclkSource(BKP_RTC_LSEclk);

  /* Проверка на необходимость сброса часов реального времени */
  if (! (MDR_BKP->REG_0F & BKP_REG_0F_RTC_EN)) {
    // Перезпускаем часы реального времени
    BKP_RTC_Reset(ENABLE);
    BKP_RTC_Reset(DISABLE);

    // Устанавливаем инициализованные значения начала и конца суточного интервала 00:00:00 - 23:59:59
    BKP_RTC_WaitForUpdate();
    time_update(0, 0, 0x1517f);
  }

  time_scale();

  // Запуск часов реального времени
  BKP_RTC_WaitForUpdate();
  BKP_RTC_Enable(ENABLE);

  return END_OK;
}


/*!
  \brief      Установить предделитель таймера

  \return    Разультат выполнения

  \attention Предполагается, что частота генератора LSE - 32,768 кГц

  Устанавливает предделитель часов реального времени (32,768 кГц / 32768 = 1Гц)
 */
uint32_t time_scale(void) {
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
uint32_t time_controll(void) {
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
  uint32_t ctb, isb, ieb;

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
uint32_t time_update(uint32_t time, uint32_t binterval_start, uint32_t binterval_end) {
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
