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
  uint32_t y = CurrentFont->Height + 4;
  DisplayMenuItemStringPrefix(y, "�����: ", time_as_string(get_time_from_date(current_date)));

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
  uint32_t y = CurrentFont->Height + 4;
  if (is_usb_connected()) { // ����������� �����������
    DisplayMenuItemString(y, "����� �����������");

    /* ����� ������ �� �������� ��������� ������� */
    send_via_usb_cdc(current_settings());
    y += CurrentFont->Height + 2;
    DisplayMenuItemString(y, "����� ���������");

    /* ����� ������ � ��������� ������ */
    uint32_t trys = 100; // ���������� ������� �������� �����
    while ((time_controll() != END_OK) && (trys)) {
      trys -= 1;
      delay(10);
    }
    y += CurrentFont->Height + 2;
    if (trys) { // ����� �������
      DisplayMenuItemString(y, "����� ������");

      /* ����� ��������� � ���������� ����������� */
      y += CurrentFont->Height + 2;
      DisplayMenuItemString(y, "����������� ���������");
    }
    else // ����� ��� � �� �������
      DisplayMenuItemString(y, "����� �� ������");
  }
  else // ����������� �� �����������
    DisplayMenuItemString(y, "����� �� �����������");

  /* ��������� ��� ������������ */
  y += CurrentFont->Height + 2;
  DisplayMenuItemString(y, "������� �����");

  /* ����������� � ���� �� ������� ������ "�����" */
  BackToMenuOnSel();
}


void SensorMenu(uint32_t addr) {
  /* ������� �������� */
  LCD_CLS();
  CurrentMethod = MET_AND;
  DisplayMenuTitleInt("���������� ������� ", addr, 10);
  WAIT_UNTIL_KEY_RELEASED(SEL);

  /* ��������� ������ �� ������� */
  uint32_t data = 0;
  get_sensor_data(addr, &data);

  /* ����� ��������� � �������� ������ */
  uint32_t y = CurrentFont->Height + 4;
  DisplayMenuItemString(y, "�������� ������:");

  /* ����� ������ ������� */
  y += CurrentFont->Height + 2;
  DisplayMenuItemString(y, sensor_data(addr, data, SENSOR_FORMAT_MODE_DIAGNOSTIC));

  /* ����� ��������� � ���������� ����������� */
  y += CurrentFont->Height + 2;
  DisplayMenuItemString(y, "����������� ���������");

  /* ��������� ��� ������������ */
  y += CurrentFont->Height + 2;
  DisplayMenuItemString(y, "������� �����");

  /* ����������� � ���� �� ������� ������ "�����" */
  BackToMenuOnSel();
}