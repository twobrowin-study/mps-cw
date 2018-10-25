/*!
\file
  \bref Меню диагностического режима
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь 2018 года

  Файл содержит реализации подпрограмм меню диагностического режима
*/
#include "diagnostic.h"
#include "1251_diagnostic_text.h"

/*!
  \bref Инициализация меню диагностического режима
  \return Статус завершения

  Содержит инизиализацию портов, обеспечивающийх работу меню и очищение экрана
    (вызывать следует при инициализации устройства!)
*/
END_STATUS diagnostic_init(void) {
  /// \todo Включать тактирование только для нужныйх здесь устройств
  MDR_RST_CLK->PER_CLOCK = 0xFFFFFFFF;

  // Инициализация из библиотеки LCDLib
  LCD_INIT();
  InitPortLED();
  InitPortJoystick();

  // Инициализация меню диагностического режима
  CustomMenuInit(&DiagnosticMainMenu, DiagnosticMainMenuItems);
  // Menu_Init();

  return END_OK;
}

/*!
  \bref Выполение меню диагностического режима
  \return Статус завершения

  Содержит выполнение графа состояний меню жиагностического режима
*/
END_STATUS diagnostic_start(void) {
  /// \todo Написать возвращение из диагностического режима
  DisplayMenu();
  ReadKey();
  return END_OK;
}
