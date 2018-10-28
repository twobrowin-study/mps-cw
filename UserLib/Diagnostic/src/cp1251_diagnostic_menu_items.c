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
  /* ������� �������� */
  LCD_CLS();
  CurrentMethod = MET_AND;
  DisplayMenuTitle("������ ������");
  WAIT_UNTIL_KEY_RELEASED(SEL);

  /* ����� �� ����� ���� �������� ��������� */
  // ������ ������� �����
  uint y = CurrentFont->Height + 4;
  DisplayMenuItemStringPrefix(y, "����� : ", time_as_string(current_time % day_proportion));

  //  ����� ������ ���������
  y += CurrentFont->Height + 2;
  DisplayMenuItemIntPrefix(y, "����� : ", current_time % day_proportion, 10);

  /* ����������� � ���� �� ������� ������ "�����" */
  BackToMenuOnSel();
}
