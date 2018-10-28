/*!
\file
  \bref ������ ���� ���������������� ������
  \authors �������� ���� ��. ��6-72, LDM-SYSTEMS (2013 �.)
  \date ������� 2018 ����
  \attention ���� ���� ������� � ��������� Windows CP1251!

  ���� �������� �������� �������, ���������� ��� ������ �� �� �������
    ������� ���������������� ������
*/
#include "cp1251_diagnostic_menu_items.h"

/*!
  \bref ������� �� ������ ������� ��������� ��

  ��� ������ � ���� ������� ������ ������ "�����"
*/
void DiagnoseSettings(void) {

  /// ������� ��������
  LCD_CLS();
  CurrentMethod = MET_AND;
  DisplayMenuTitle("������ ������");
  WAIT_UNTIL_KEY_RELEASED(SEL);

  // ������
  LCD_PUTS(0, 12, "����� 6X8");

  LCD_PUTS(0, 22, "����� 12X16");

  LCD_PUTS(0, 40, "����� 7X10");

  LCD_PUTS(0, 52, "����� 7X10 ���������");

  // ����������� � ���� �� ������� ������ "�����"
  BackToMenuOnSel();
}
