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

 #include "usb.h"

/*!
  \bref ������� �� ������ ������� ��������� ��

  ��� ������ � ���� ������� ������ ������ "�����"
 */
void DiagnoseSettings(void) {
  /* ������� �������� */
  LCD_CLS();
  CurrentMethod = MET_AND;
  DisplayMenuTitle("������� ��������� ��");
  WAIT_UNTIL_KEY_RELEASED(SEL);

  /* ����� �� ����� ���� �������� ��������� */
  // ������ ������� �����
  uint y = CurrentFont->Height + 4;
  DisplayMenuItemStringPrefix(y, "�����: ", time_as_string(current_time % day_proportion));

  // ����� ������ ���������
  y += CurrentFont->Height + 2;
  DisplayMenuItemStringPrefix(y, "������ �: ", time_as_string(interval_start));

  // ����� ����� ���������
  y += CurrentFont->Height + 2;
  DisplayMenuItemStringPrefix(y, "�����  �: ", time_as_string(interval_end));

  // ��������� ��� ������������
  y += CurrentFont->Height + 2;
  DisplayMenuItemString(y, "������� �����");

  /* ����������� � ���� �� ������� ������ "�����" */
  BackToMenuOnSel();
}


/*!
  \brief      ����������� �������

  ��� ������ � ���� ������� ������ ������ "�����"
 */
void DiagnoseServer(void) {
  /* ������� �������� */
  LCD_CLS();
  CurrentMethod = MET_AND;
  DisplayMenuTitle("���������� �������");
  WAIT_UNTIL_KEY_RELEASED(SEL);

  /* ����� ������ � ������� ����������� */
  while (!is_usb_connected());
  uint y = CurrentFont->Height + 4;
  DisplayMenuItemString(y, "����� �����������");

  /* ����� ������ �� �������� ��������� ������� */
  send_via_usb_cdc(current_settings());
  y += CurrentFont->Height + 2;
  DisplayMenuItemString(y, "����� ���������");

  /* ����� ������ � ��������� ������ */
  while (time_controll() != END_OK)
    delay(100);
  y += CurrentFont->Height + 2;
  DisplayMenuItemString(y, "����� ������");

  /* ����� ��������� � ���������� ����������� */
  y += CurrentFont->Height + 2;
  DisplayMenuItemString(y, "����������� ���������");

  // ��������� ��� ������������
  y += CurrentFont->Height + 2;
  DisplayMenuItemString(y, "������� �����");

  /* ����������� � ���� �� ������� ������ "�����" */
  BackToMenuOnSel();
}
