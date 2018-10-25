/*!
\file
  \bref ����� ���� ��������������� ������
  \author �������� ���� ��. ��6-72
  \date ������� 2018 ����
  \attention ���� ���� �������� � ��������� Windows 1251!

  ���� �������� ���������� ����������� ���� ���������������� ������
*/
#ifndef DIAGNOSTIC_TEXT_H
#define DIAGNOSTIC_TEXT_H

// ����������� ���������� ��������� LCD
#include <stdio.h>
#include "types.h"
#include "lcd.h"
#include "gl.h"
#include "text.h"
#include "joystick.h"
#include "leds.h"
#include "menu.h"
#include "image.h"

/// ������ �������
static void IdleFunc(void) {}

/// ������� ���� ���������������� ������
struct sMenuItem DiagnosticMainMenuItems[] = {
  {"����", IdleFunc, NULL},
  {"���", IdleFunc, NULL},
  {"3", IdleFunc, NULL},
  {"������������", IdleFunc, NULL},
  {"���������", IdleFunc, NULL}};
struct sMenu DiagnosticMainMenu = {"��������������� �����", DiagnosticMainMenuItems, countof(DiagnosticMainMenuItems)};

#endif // DIAGNOSTIC_TEXT_H
