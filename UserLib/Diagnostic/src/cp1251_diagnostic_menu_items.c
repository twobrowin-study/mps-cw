/*!
\file
  \bref Пункты меню диагностического режима
  \authors Дубровин Егор гр. ИУ6-72, LDM-SYSTEMS (2013 г.)
  \date Октябрь 2018 года
  \attention Этот файл написан в кодировке Windows CP1251!

  Файл содержит описания функций, вызываемых для вывода на ЖК дисплей
    функций диагностического режима
*/
#include "cp1251_diagnostic_menu_items.h"

/*!
  \bref Вывести на дсплей текущие настройки МК

  Для выхода в меню следует нажать кнопку "Выбор"
*/
void DiagnoseSettings(void) {

  /// Выводим заглавие
  LCD_CLS();
  CurrentMethod = MET_AND;
  DisplayMenuTitle("Шрифты дофига");
  WAIT_UNTIL_KEY_RELEASED(SEL);

  // Шрифты
  LCD_PUTS(0, 12, "Шрифт 6X8");

  LCD_PUTS(0, 22, "Шрифт 12X16");

  LCD_PUTS(0, 40, "Шрифт 7X10");

  LCD_PUTS(0, 52, "Шрифт 7X10 утолщёный");

  // Возвражение в меню по нажатию кнопки "Выбор"
  BackToMenuOnSel();
}
