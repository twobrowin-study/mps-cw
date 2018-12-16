#ifndef MDR_USB_CDC_H
#define MDR_USB_CDC_H

#include <stdint.h>

#include "MDR32F9Qx_config.h"

#include "usb.h"

#define MDR_USB_CDC_MAX_PACKET_SIZE 64

typedef enum _enum_MDR_USB_CDC_Result
{
    MDR_USB_CDC_Ok,
    MDR_USB_CDC_Fail
} MDR_USB_CDC_Result;

void               mdr_usb_cdc_init();

MDR_USB_CDC_Result mdr_usb_cdc_is_sending();
MDR_USB_CDC_Result mdr_usb_cdc_send(uint8_t data_len, const uint8_t data[data_len]);
MDR_USB_CDC_Result mdr_usb_cdc_send_isr(uint8_t data_len, const uint8_t data[data_len]);

MDR_USB_CDC_Result mdr_usb_cdc_is_recv_buf_setted();
void               mdr_usb_cdc_set_recv_buf(uint8_t buf[MDR_USB_CDC_MAX_PACKET_SIZE]);
void               mdr_usb_cdc_set_recv_buf_isr(uint8_t buf[MDR_USB_CDC_MAX_PACKET_SIZE]);

#define MDR_USB_CDC_CALLBACK

void MDR_USB_CDC_CALLBACK mdr_usb_cdc_on_data_sent();
void MDR_USB_CDC_CALLBACK mdr_usb_cdc_on_data_recv(uint8_t data_len);

#endif // MDR_USB_CDC_H
