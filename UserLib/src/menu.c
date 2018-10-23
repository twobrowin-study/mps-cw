/*!
\file
  \bref Меню диагностического режима
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь 2018 года

  Файл содержит реализации подпрограмм меню диагностического режима
*/
#include "menu.h"

/*!
  \bref Инициализация меню диагностического режима
  \return Статус завершения

  Содержит инизиализацию портов, обеспечивающийх работу меню и очищение экрана
    (вызывать следует при инициализации устройства!)
*/
END_STATUS menu_init(void) {

  // Инициализируем порт C
  RST_CLK_PCLKcmd (RST_CLK_PCLK_PORTC, ENABLE);
  PORT_InitTypeDef GPIOInitStruct;
  PORT_StructInit(&GPIOInitStruct);
  GPIOInitStruct.PORT_Pin        = PORT_Pin_0 | PORT_Pin_1;
  GPIOInitStruct.PORT_OE         = PORT_OE_OUT;
  GPIOInitStruct.PORT_SPEED      = PORT_SPEED_SLOW;
  GPIOInitStruct.PORT_MODE       = PORT_MODE_DIGITAL;
  PORT_Init(MDR_PORTC, &GPIOInitStruct);

  return END_OK;
}

/*!
  \bref Выполение меню диагностического режима
  \return Статус завершения

  Содержит выполнение графа состояний меню жиагностического режима
*/
END_STATUS menu_release(void) {
  while(1) {
    MDR_PORTC->RXTX ^= 0b01;
    delay(1000);
    MDR_PORTC->RXTX ^= 0b10;
  }
  return END_OK;
}
