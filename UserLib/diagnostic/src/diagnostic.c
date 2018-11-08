/*!
\file
  \bref Меню диагностического режима
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь 2018 года

  Файл содержит реализации подпрограмм меню диагностического режима
 */
#include "diagnostic.h"


/*!
  \bref Инициализация меню диагностического режима
  \return Статус завершения

  Содержит инизиализацию портов, обеспечивающийх работу меню и очищение экрана
    (вызывать следует при инициализации устройства!)
 */
END_STATUS diagnostic_init(void) {
  // Включение тактирования необходимых для диагностичекого режима портов
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTC
                  | RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF,
                  ENABLE);

  // Инициализация меню диагностического режима
  LCD_INIT();
  InitPortJoystick();
  MenuInit();
  ResetLCD();

  return END_OK;
}


/*!
  \bref Выполение меню диагностического режима
  \return Статус завершения

  Содержит выполнение графа состояний меню диагностического режима
 */
END_STATUS diagnostic_release(void) {
  DisplayMenu();
  ReadKey();
  return END_OK;
}
