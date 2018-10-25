/*!
\file
  \bref Основной файл программы
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь 2018 года

  Файл содержит основной алгоритм программы и подпрограмму инициализации оборудования
*/
#include "main.h"

/*!
  \bref Основная программа
  \return 0

  Содержит вызов функции инициализации и основной алгоритм программы
*/
int main(void) {
  // Инициализация с проверкой результата
  if(init() != END_OK) return END_ERROR;

  diagnostic_start();

  return 0;
}

/*!
  \bref Функция инициализации
  \return Статус завершения

  Содержит функции инициализации и определение успешности инициализации
*/
END_STATUS init(void) {
  /// \todo Написать определение успешности инициализации
  delay_init();
  rst_clk_pll_init();
  diagnostic_init();

  return END_OK;
}

/*!
  \bref Функция инициализации тактирования
  \return Статус завершения

  Содержит функции инициализации PLL
*/
END_STATUS rst_clk_pll_init(void) {
  MDR_RST_CLK->HS_CONTROL = 0x00000001; // Включение HSE, режим осциллятора (8МГц)
  while((MDR_RST_CLK->CLOCK_STATUS&0x04) == 0x00); // Ожидание выхлда HSE в штатный режим
  MDR_EEPROM->CMD = 5<<3;

  // Инициализация PLL
  /*!
  * Выбираем в качестве источника опорного сигнала HSE (внешний) с делением на 1.
  * Умножаем входную частоту на 9 (8 МГц * 9 = 72 МГц)
  */
  MDR_RST_CLK->PLL_CONTROL = (8<<8) | (1<<2); // Включение CPU_PLL к-та умножения
  while((MDR_RST_CLK->CLOCK_STATUS&0x02)==0x00); // Ожидание выхода CPU_PLL в штатный режим

  MDR_EEPROM->CMD = 3<<3; // Задержка для обращения к flash-памяти Delay = 3; Обход проблемы ядра

  MDR_RST_CLK->CPU_CLOCK |= 0x00000106; // Задать HCLK = CPU_C3 = CPU_C2 = CPU_PLL; CPU_C1 = HSE
  SystemCoreClockUpdate();

  return END_OK;
}
