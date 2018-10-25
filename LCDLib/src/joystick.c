/*============================================================================================
 *
 *  (C) 2013, LDM-SYSTEMS
 *
 *  ���������������� ������ ��� ���������� ����� LDM-K1986BE92QI
 *
 *  ������ �� ��������������� "��� ����", �.�. ������������� ��� ������, ���������� ���������
 *  ������������� ���������� �� ���������� ��� ����������� Milandr K1986BE92QI. �������� LDM-SYSTEMS
 *  �� ����� ������� ��������������� �� ��������� ����������� ������������� �������, ���
 *  �������������� ������������� �� ��� ������, ��.
 *
 *  ������������ ��� Keil 5 DeamonClub.RU
 *
 *--------------------------------------------------------------------------------------------
 *
 *  ���� joystick.c: ������ � ������� ������ ("����������")
 *
 *============================================================================================*/

#include "MDR32Fx.h"                    // Device header
#include "joystick.h"

/* ����� � ������������ "����� ������" */

#define No_Key_Pressed      0x7C00

/*static uc32 KeyMasks[NUM_KEY_CODES] = {
        No_Key_Pressed, // NOKEY    
        0x7800,         // SEL      
        0x3C00,         // RIGHT    
        0x5C00,         // LEFT     
        0x7400,         // UP       
        0x6C00          // DOWN     
}; */

void InitPortJoystick(void) {
    MDR_PORTB->FUNC  &= ~((0xF << (5 << 1)));   // Main Function ��� UP - PB5, RIGNHT - PB6
    MDR_PORTB->ANALOG |= 0x3 << 5;     					// Digital */

    MDR_PORTC->FUNC &= ~((0x3 << (2 << 1)));   	// Main Function ��� SEL - PC2
    MDR_PORTC->ANALOG |= 0x1 << 2;     					// Digital

    MDR_PORTE->FUNC &= ~((0x33 << (1 << 1)));   // Main Function ��� DOWN - PE1, LEFT - PE3 (0b110011)
    MDR_PORTE->ANALOG |= 0x5 << 1;     					// Digital
}

// ����������� "����" �� ������� ������� 
KeyCode GetKey(void) 
{
    u32 i, sKey;
    u32 data[5];

    for(i=0;i<5;i++)
    {
      data[i] = 1;
    }

    // �������� ������ � ������ � ������ � ���������  �������� (1 - ������, 0 - �� ������)
    if(MDR_PORTC->RXTX & (1<<2)) data[0] = 0;   // SEL      PC2
    if(MDR_PORTB->RXTX & (1<<6)) data[1] = 0;   // RIGHT    PB6
    if(MDR_PORTE->RXTX & (1<<3)) data[2] = 0;   // LEFT     PE3
    if(MDR_PORTB->RXTX & (1<<5)) data[3] = 0;   // UP       PB5
    if(MDR_PORTE->RXTX & (1<<1)) data[4] = 0;   // DOWN     PE1

    // ��������� ��������� ������
    sKey=0;
    for(i=0;i<5;i++)
    {
      sKey = sKey + data[i];
    }

    if(sKey) 				// ���� �������
    {
			if(sKey > 1)  // MULTIPLE 
			{
				return MULTIPLE;
			}

			// ���� ������ ������ ���� ������, �� ���������� �
			for(i=0;i<5;i++)
			{
				if(data[i] == 1)
				{
					return ((KeyCode)(i+1));
				}
			}
		}	
		return NOKEY;
}

/*============================================================================================
 * ����� ����� joystick.c
 *============================================================================================*/

