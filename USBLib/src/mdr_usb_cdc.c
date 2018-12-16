#include "mdr_usb_cdc.h"

#define ASSERT_INFO_FILE_ID (FILEID__USER_LO+1)

#ifdef MDR_USB_CDC_CALLBACK
#undef MDR_USB_CDC_CALLBACK
#endif

#define MDR_USB_CDC_CALLBACK __attribute__ ((weak))

#define USB_SEP_TS_SETUP_TRANS 0
#define USB_SEP_TS_IN_TRANS    1
#define USB_SEP_TS_OUT_TRANS   2

#define ENDPOINT_CLEAR_RX(endpoint) (MDR_USB->USB_SEP_FIFO[endpoint].RXFC = 1)
#define ENDPOINT_CLEAR_TX(endpoint) (MDR_USB->USB_SEP_FIFO[endpoint].TXFDC = 1)

#define ENDPOINT_SETUP    0
#define ENDPOINT_SEND     1
#define ENDPOINT_STAT     2
#define ENDPOINT_RECV     3
#define Num_USB_EndPoints 4

typedef struct
{
    uint32_t CTRL;
    uint32_t STS;
    uint32_t TS;
    uint8_t  error;
} mdr_usb_cdc_endpoint_state;

typedef enum
{
    USB_DEV_STATE_UNKNOWN = 0,
    USB_DEV_STATE_ATTACHED,
    USB_DEV_STATE_POWERED,
    USB_DEV_STATE_DEFAULT,
    USB_DEV_STATE_ADDRESS,
    USB_DEV_STATE_CONFIGURED,
} usb_device_state;

typedef enum
{
    USB_TYPE_STANDARD  = 0x0,
    USB_TYPE_CLASS     = 0x20,
    USB_TYPE_VENDOR    = 0x40
} usb_request_type;

typedef enum
{
    USB_HOST_TO_DEVICE  = 0x0,
    USB_DEVICE_TO_HOST  = 0x80
} usb_request_direction;

typedef enum
{
    USB_RECIPIENT_DEVICE    = 0x0,
    USB_RECIPIENT_INTERFACE = 0x1,
    USB_RECIPIENT_ENDPOINT  = 0x2,
    USB_RECIPIENT_OTHER     = 0x3
} usb_request_recipient;

typedef enum
{
    USB_DEVICE = 1,
    USB_CONFIGURATION,
    USB_STRING,
    USB_INTERFACE,
    USB_ENDPOINT,
    USB_DEVICE_QUALIFIER,
    USB_OTHER_SPEED_CONFIGURATION,
    USB_INTERFACE_POWER
} usb_standart_request_descriptor;

typedef enum
{
    USB_GET_STATUS = 0,
    USB_CLEAR_FEATURE,
    USB_Reserved0,
    USB_SET_FEATURE,
    USB_Reserved1,
    USB_SET_ADDRESS,
    USB_GET_DESCRIPTOR,
    USB_SET_DESCRIPTOR,
    USB_GET_CONFIGURATION,
    USB_SET_CONFIGURATION,
    USB_GET_INTERFACE,
    USB_SET_INTERFACE,
    USB_SYNCH_FRAME
} usb_standart_request_num;

typedef enum
{
    USB_CDC_SET_LINE_CODING = 0x20,
    USB_CDC_GET_LINE_CODING
} usb_cdc_class_request;

static void    mdr_usb_cdc_reset(void);
static void    mdr_usb_cdc_dispatch_setup_endpoint(void);
static void    mdr_usb_cdc_dispatch_send_endpoint(void);
static void    mdr_usb_cdc_dispatch_recv_endpoint(void);
static void    mdr_usb_cdc_set_device_state(usb_device_state);
static uint8_t mdr_usb_cdc_get_endpoint_state(uint8_t, mdr_usb_cdc_endpoint_state *);

static void               setup_endpoint_reset(void);
static MDR_USB_CDC_Result setup_endpoint_on_setup_trans(void);
static MDR_USB_CDC_Result (*setup_endpoint_in_trans_handler)(void);
static MDR_USB_CDC_Result (*setup_endpoint_out_trans_handler)(void);
static MDR_USB_CDC_Result setup_endpoint_switch_after_in_trans(void);
static MDR_USB_CDC_Result setup_endpoint_control_trans_end(void);
static uint32_t           setup_endpoint_wait_trans;
static void               (*setup_endpoint_control_trans_end_handler)(void);
static void               setup_endpoint_control_trans_end_def_handler(void);

static void recv_endpoint_init(void);
static void recv_endpoint_deinit(void);
static void send_endpoint_init(void);
static void send_endpoint_deinit(void);

#define SETUP_ENDPOINT_WAIT_IN_TRANS(ptr) \
    setup_endpoint_wait_trans = USB_SEP_TS_IN_TRANS; \
    setup_endpoint_in_trans_handler = ptr;

#define SETUP_ENDPOINT_WAIT_OUT_TRANS(ptr) \
    setup_endpoint_wait_trans = USB_SEP_TS_OUT_TRANS; \
    setup_endpoint_out_trans_handler = ptr;

#define SETUP_ENDPOINT_WAIT_IN_THEN_OUT_TRANS  SETUP_ENDPOINT_WAIT_IN_TRANS(setup_endpoint_switch_after_in_trans)
#define SETUP_ENDPOINT_WAIT_TRANS_DONE         SETUP_ENDPOINT_WAIT_IN_TRANS(setup_endpoint_control_trans_end)
#define SETUP_ENDPOINT_WAIT_TRANS_DONE_FUNC(x) \
    setup_endpoint_control_trans_end_handler = x; \
    SETUP_ENDPOINT_WAIT_IN_TRANS(setup_endpoint_control_trans_end);

typedef union _union_usb_setup_packet {
    struct __attribute__ ((packed)) _struct_usb_setup_packet
    {
        uint8_t  bmRequestType;
        uint8_t  bRequest;
        uint16_t wValue;
        uint16_t wIndex;
        uint16_t wLength;
    } data;
    uint8_t raw_data[sizeof(struct _struct_usb_setup_packet)];
} usb_setup_packet;

static usb_setup_packet setup_packet;
static usb_device_state device_state;

#define USB_REQUEST_DT_Msk        0x80
#define USB_REQUEST_TYPE_Msk      0x60
#define USB_RECIPIENT_TYPE_Msk    0x1F

#define GET_USB_REQUEST_DIR       (setup_packet.data.bmRequestType & USB_REQUEST_DT_Msk)
#define GET_USB_REQUEST_RECIPIENT (setup_packet.data.bmRequestType & USB_RECIPIENT_TYPE_Msk)

static MDR_USB_CDC_Result std_handler_get_status(void);
static MDR_USB_CDC_Result std_handler_clear_feature(void);
static MDR_USB_CDC_Result std_handler_set_feature(void);
static MDR_USB_CDC_Result std_handler_set_address(void);
static void               std_handler_set_address_status_stage(void);
static MDR_USB_CDC_Result std_handler_get_descriptor(void);
static MDR_USB_CDC_Result std_handler_get_descriptor_data_stage(void);
static MDR_USB_CDC_Result std_handler_get_descriptor_send(uint8_t);
static MDR_USB_CDC_Result std_handler_set_descriptor(void);
static MDR_USB_CDC_Result std_handler_get_configuration(void);
static MDR_USB_CDC_Result std_handler_set_configuration(void);
static MDR_USB_CDC_Result std_handler_get_interface(void);
static MDR_USB_CDC_Result std_handler_set_interface(void);
static MDR_USB_CDC_Result std_handler_synch_frame(void);
static MDR_USB_CDC_Result mdr_usb_cdc_error_handler(void);

static MDR_USB_CDC_Result mdr_usb_cdc_resolve_discriptor(const uint8_t **data, uint16_t *data_len);

static MDR_USB_CDC_Result (* const std_request_handlers[])(void) =
{
    [USB_GET_STATUS]        = std_handler_get_status,
    [USB_CLEAR_FEATURE]     = std_handler_clear_feature,
    [USB_Reserved0]         = mdr_usb_cdc_error_handler,
    [USB_SET_FEATURE]       = std_handler_set_feature,
    [USB_Reserved1]         = mdr_usb_cdc_error_handler,
    [USB_SET_ADDRESS]       = std_handler_set_address,
    [USB_GET_DESCRIPTOR]    = std_handler_get_descriptor,
    [USB_SET_DESCRIPTOR]    = std_handler_set_descriptor,
    [USB_GET_CONFIGURATION] = std_handler_get_configuration,
    [USB_SET_CONFIGURATION] = std_handler_set_configuration,
    [USB_GET_INTERFACE]     = std_handler_get_interface,
    [USB_SET_INTERFACE]     = std_handler_set_interface,
    [USB_SYNCH_FRAME]       = std_handler_synch_frame
};

static MDR_USB_CDC_Result mdr_usb_cdc_class_request_handler(void);
static MDR_USB_CDC_Result mdr_usb_cdc_set_line_coding(void);

typedef union _union_usb_cdc_line_coding
{
    struct __attribute__ ((packed)) _struct_usb_cdc_line_coding
    {
      uint32_t dwDTERate;
      uint8_t  bCharFormat;
      uint8_t  bParityType;
      uint8_t  bDataBits;
    } data;
    uint8_t raw_data[sizeof(struct _struct_usb_cdc_line_coding)];
} usb_cdc_line_coding;
static usb_cdc_line_coding current_line_coding;

#define MDR_USB_CDC_DEVICE_DESCR_LENGTH 0x12
#define MDR_USB_CDC_CONFIG_DESCR_LENGTH 0x43

static const uint8_t Usb_CDC_Device_Descriptor[MDR_USB_CDC_DEVICE_DESCR_LENGTH] =
{
    MDR_USB_CDC_DEVICE_DESCR_LENGTH, /* bLength                  */
    0x01,            /* bDescriptorType (Device) */
    0x10, 0x01,      /* bcdUSB                   */
    0x02,            /* bDeviceClass (CDC)       */
    0x00,            /* bDeviceSubClass          */
    0x00,            /* bDeviceProtocol          */
    MDR_USB_CDC_MAX_PACKET_SIZE, /* bMaxPacketSize0          */
    0x83, 0x04,      /* idVendor                 */
    0x25, 0xF1,      /* idProduct                */
    0x00, 0x00,      /* bcdDevice                */
    0x00,            /* iManufacturer            */
    0x00,            /* iProduct                 */
    0x00,            /* iSerialNumber            */
    0x01             /* bNumConfigurations       */
};

static const uint8_t Usb_CDC_Configuration_Descriptor[MDR_USB_CDC_CONFIG_DESCR_LENGTH] =
{
    /* Configuration Descriptor */
    0x09,                /* bLength                         */
    0x02,                /* bDescriptorType (Configuration) */
    MDR_USB_CDC_CONFIG_DESCR_LENGTH, 0x00,  /* wTotalLength                    */
    0x02,                /* bNumInterfaces                  */
    0x01,                /* bConfigurationValue             */
    0x00,                /* iConfiguration                  */
    0x80,                /* bmAttributes                    */
    0x32,                /* bMaxPower (100 mA)              */

    /* Interface Descriptor */
    0x09, /* bLength                                         */
    0x04, /* bDescriptorType (Interface)                     */
    0x00, /* bInterfaceNumber                                */
    0x00, /* bAlternateSetting                               */
    0x01, /* bNumEndpoints                                   */
    0x02, /* bInterfaceClass (Communication Interface Class) */
    0x02, /* bInterfaceSubClass (Abstract Control Model)     */
    0x01, /* bInterfaceProtocol (Common AT commands)         */
    0x00, /* iInterface                                      */
    /* Header Functional Descriptor */
    0x05,       /* bLength (Endpoint Descriptor size)        */
    0x24,       /* bDescriptorType (CS_INTERFACE)            */
    0x00,       /* bDescriptorSubtype (Header Func Desc)     */
    0x10, 0x01, /* bcdCDC (Release Number)                   */
    /* Call Managment Functional Descriptor */
    0x05,       /* bFunctionLength                           */
    0x24,       /* bDescriptorType (CS_INTERFACE)            */
    0x01,       /* bDescriptorSubtype (Call Management)      */
    0x00,       /* bmCapabilities:                           */
    0x01,       /* bDataInterface                            */
    /* ACM Functional Descriptor */
    0x04,       /* bFunctionLength                           */
    0x24,       /* bDescriptorType (CS_INTERFACE)            */
    0x02,       /* bDescriptorSubtype (Abstract Control Management)*/
    0x00,       /* bmCapabilities                            */
    /* Union Functional Descriptor */
    0x05,       /* bFunctionLength                           */
    0x24,       /* bDescriptorType (CS_INTERFACE)            */
    0x06,       /* bDescriptorSubtype (Union func desc)      */
    0x00,       /* bMasterInterface (Communication class interface) */
    0x01,       /* bSlaveInterface0 (Data Class Interface)   */
    /* Endpoint 2 Descriptor */
    0x07,       /* bLength (Endpoint Descriptor size)        */
    0x05,       /* bDescriptorType (Endpoint)                */
    0x80 | ENDPOINT_STAT,       /* bEndpointAddress (IN | 2)                 */
    0x03,       /* bmAttributes (Interrupt)                  */
    MDR_USB_CDC_MAX_PACKET_SIZE, 0x00, /* wMaxPacketSize                 */
    0x02,       /* bInterval
                                */
    /* Data class interface Descriptor */
    0x09,       /* bLength (Endpoint Descriptor size)        */
    0x04,       /* bDescriptorType (Interface)               */
    0x01,       /* bInterfaceNumber                          */
    0x00,       /* bAlternateSetting                         */
    0x02,       /* bNumEndpoints (Two endpoints used)        */
    0x0A,       /* bInterfaceClass (CDC)                     */
    0x00,       /* bInterfaceSubClass                        */
    0x00,       /* bInterfaceProtocol                        */
    0x00,       /* iInterface                                */
    /* Endpoint 1 Descriptor */
    0x07,       /* bLength (Endpoint Descriptor size)        */
    0x05,       /* bDescriptorType (Endpoint)                */
    0x80 | ENDPOINT_SEND,       /* bEndpointAddress (IN | 1)                 */
    0x02,       /* bmAttributes (Bulk)                       */
    MDR_USB_CDC_MAX_PACKET_SIZE, 0x00, /* wMaxPacketSize                 */
    0x00,       /* bInterval                                 */
    /* Endpoint 3 Descriptor */
    0x07,       /* bLength (Endpoint Descriptor size)        */
    0x05,       /* bDescriptorType (Endpoint)                */
    ENDPOINT_RECV,       /* bEndpointAddress (OUT | 3)                */
    0x02,       /* bmAttributes (Bulk)                       */
    MDR_USB_CDC_MAX_PACKET_SIZE, 0x00, /* wMaxPacketSize                 */
    0x00,       /* bInterval                                 */
};

void mdr_usb_cdc_init()
{
    assert_param(MDR_USB_PLL * MDR_USB_HSE_VALUE == 48000000);

    uint32_t pll_control = MDR_RST_CLK->PLL_CONTROL;
    MDR_RST_CLK->PER_CLOCK |= (1 << 2); //тактирование USB
    MDR_RST_CLK->USB_CLOCK = ((1 << 8) /* есть тактовая частота */
                              | (2 << 0) /*источник для USB_C1*/
                              | (1 << 2)/*источник для USB_C2*/
                              | (0 << 4));/*источник для USB_C3 (USB_CLK)*/
    pll_control |= ((1 << 0) | ((MDR_USB_PLL-1) << 4)); //вкл. PLL_USB
    MDR_RST_CLK->PLL_CONTROL = pll_control;
    while ((MDR_RST_CLK->CLOCK_STATUS & 0x01) != 0x01); //ждем когда PLL_USB выйдет в раб. режим
    mdr_usb_cdc_reset();
}

void mdr_usb_cdc_reset(void)
{
    unsigned char i = 0;
    MDR_USB->HSCR = (1 << 1); //программный сброс контроллера USB
    for (i = 0; i < 40; i++)
        __NOP();

    MDR_USB->HSCR &=~ (1 << 1); //рабочий режим контроллера USB
    MDR_USB->HSCR = ((0 << 0) /* режим работы - Device */
                     | (1 << 2) | (1 << 3) //TX - включен | RX - включен
                     | (0x1 << 4)); //Управление подтяжкой - Подтяжка к VCC

    MDR_USB->SA = 0;
    MDR_USB->SC |= ((1 << 5) | (1 << 4) | 1); //cкорость = 12 Мбит/с | полярность = Full Speed | вкл. окон. точек

    device_state = USB_DEV_STATE_DEFAULT;
    setup_endpoint_reset();
    MDR_USB->USB_SEP[ENDPOINT_SETUP].CTRL = // Вкл endpoint 0
            USB_SEP_CTRL_EPEN  |
            USB_SEP_CTRL_EPRDY;

    MDR_USB->SIM = USB_SIM_SCTDONEIE | USB_SIM_SCRESETEVIE | USB_SIS_SCUSBON;
    NVIC_EnableIRQ(USB_IRQn);
}

uint8_t mdr_usb_cdc_get_endpoint_state(uint8_t endpoint, mdr_usb_cdc_endpoint_state *state)
{
    state->CTRL = MDR_USB->USB_SEP[endpoint].CTRL;
    if((state->CTRL & USB_SEP_CTRL_EPEN) && ! (state->CTRL & USB_SEP_CTRL_EPRDY))
    {
        const uint8_t error_mask =
                USB_SEP_STS_SCSTALLSENT |
                USB_SEP_STS_SCCRCERR    |
                USB_SEP_STS_SCBSERR     |
                USB_SEP_STS_SCRXTO      |
                USB_SEP_STS_SCRXOF;

        state->STS   = MDR_USB->USB_SEP[endpoint].STS;
        state->error = (state->STS & error_mask) != 0;
        state->TS    = MDR_USB->USB_SEP[endpoint].TS;
        return 1;
    }
    return 0;
}

void USB_IRQHandler(void)
{
    const uint32_t usb_status = MDR_USB->SIS;
    MDR_USB->SIS = usb_status; // cleared by writing 1

    if(usb_status & USB_SIS_SCTDONE)
    {
        mdr_usb_cdc_dispatch_setup_endpoint();
        mdr_usb_cdc_dispatch_send_endpoint();
        mdr_usb_cdc_dispatch_recv_endpoint();
    }

    if(usb_status & USB_SIS_SCUSBON)
    {
    }
    else if(usb_status & USB_SIS_SCRESETEV)
    {
        mdr_usb_cdc_reset();
    }
}

void mdr_usb_cdc_dispatch_setup_endpoint(void)
{
    mdr_usb_cdc_endpoint_state e_setup_state;

    if(mdr_usb_cdc_get_endpoint_state(ENDPOINT_SETUP, &e_setup_state))
    {
        uint32_t next_ctrl = USB_SEP_CTRL_EPEN  | USB_SEP_CTRL_EPRDY;
        MDR_USB_CDC_Result result = MDR_USB_CDC_Fail;
        if(!e_setup_state.error)
        {
            if(e_setup_state.TS == USB_SEP_TS_SETUP_TRANS)
            {
                result = setup_endpoint_on_setup_trans();
                next_ctrl |= USB_SEP_CTRL_EPDATASEQ;
            }
            else
            {
                if(e_setup_state.TS == setup_endpoint_wait_trans)
                {
                    if(e_setup_state.TS == USB_SEP_TS_OUT_TRANS)
                    {
                        result = (*setup_endpoint_out_trans_handler)();
                        ENDPOINT_CLEAR_RX(ENDPOINT_SETUP);
                    }
                    else
                    {
                        result = MDR_USB_CDC_Ok;
                        ENDPOINT_CLEAR_TX(ENDPOINT_SETUP);
                        if(e_setup_state.STS & USB_SEP_STS_SCACKRXED)
                        {
                            result = (*setup_endpoint_in_trans_handler)();
                            next_ctrl |= (e_setup_state.CTRL ^ USB_SEP_CTRL_EPDATASEQ) & USB_SEP_CTRL_EPDATASEQ;
                        }
                    }
                }
            }

            if(result != MDR_USB_CDC_Ok)
            {
                next_ctrl |= USB_SEP_CTRL_EPSSTALL;
            }
        }

        if(e_setup_state.error || result != MDR_USB_CDC_Ok)
        {
            setup_endpoint_reset();
        }

        MDR_USB->USB_SEP[ENDPOINT_SETUP].STS = 0;
        MDR_USB->USB_SEP[ENDPOINT_SETUP].CTRL = next_ctrl;
    }
}

void setup_endpoint_reset()
{
    setup_endpoint_wait_trans = USB_SEP_TS_SETUP_TRANS;
    setup_endpoint_control_trans_end_handler = setup_endpoint_control_trans_end_def_handler;
    ENDPOINT_CLEAR_RX(ENDPOINT_SETUP);
    ENDPOINT_CLEAR_TX(ENDPOINT_SETUP);
}

MDR_USB_CDC_Result setup_endpoint_on_setup_trans(void)
{
    MDR_USB_CDC_Result result = MDR_USB_CDC_Fail;

    const uint32_t data_len = MDR_USB->USB_SEP_FIFO[ENDPOINT_SETUP].RXFDC_H;
    if(data_len == sizeof(usb_setup_packet))
    {
        for(uint8_t i=0; i<sizeof(usb_setup_packet); i++)
            setup_packet.raw_data[i] = MDR_USB->USB_SEP_FIFO[ENDPOINT_SETUP].RXFD;

        const uint8_t standart_request =
                (setup_packet.data.bmRequestType & USB_REQUEST_TYPE_Msk) == USB_TYPE_STANDARD;

        MDR_USB_CDC_Result (*handler)(void) = mdr_usb_cdc_error_handler;
        if(standart_request && (GET_USB_REQUEST_RECIPIENT <= USB_RECIPIENT_OTHER))
        {
            const uint8_t std_handler_count =
                    sizeof(std_request_handlers)/sizeof(std_request_handlers[0]);

            if(setup_packet.data.bRequest < std_handler_count)
                handler = std_request_handlers[setup_packet.data.bRequest];
        }
        else if((setup_packet.data.bmRequestType & USB_REQUEST_TYPE_Msk) == USB_TYPE_CLASS)
        {
            handler = mdr_usb_cdc_class_request_handler;
        }

        if(setup_packet.data.wLength == 0)
            SETUP_ENDPOINT_WAIT_TRANS_DONE;
        result = (*handler)();
    }
    ENDPOINT_CLEAR_RX(ENDPOINT_SETUP);

    return result;
}

static MDR_USB_CDC_Result std_handler_get_status(void)
{
    MDR_USB_CDC_Result result = MDR_USB_CDC_Fail;
    const uint8_t recipient = GET_USB_REQUEST_RECIPIENT;
    const uint8_t bad_request =
            GET_USB_REQUEST_DIR != USB_DEVICE_TO_HOST ||
            (device_state == USB_DEV_STATE_ADDRESS && (recipient == USB_RECIPIENT_INTERFACE ||
            (recipient == USB_RECIPIENT_ENDPOINT && setup_packet.data.wIndex >= Num_USB_EndPoints)));

    if(!bad_request)
    {
        MDR_USB->USB_SEP_FIFO[ENDPOINT_SETUP].TXFD = 0;
        MDR_USB->USB_SEP_FIFO[ENDPOINT_SETUP].TXFD = 0;
        SETUP_ENDPOINT_WAIT_IN_THEN_OUT_TRANS;

        result = MDR_USB_CDC_Ok;
    }

    return result;
}

MDR_USB_CDC_Result std_handler_clear_feature(void)
{
    SETUP_ENDPOINT_WAIT_TRANS_DONE;
    return MDR_USB_CDC_Ok;
}

MDR_USB_CDC_Result std_handler_set_feature(void)
{
    SETUP_ENDPOINT_WAIT_TRANS_DONE;
    return MDR_USB_CDC_Ok;
}

MDR_USB_CDC_Result std_handler_set_address(void)
{
    MDR_USB_CDC_Result result = MDR_USB_CDC_Fail;

    const uint8_t update_address =
            (setup_packet.data.wValue != 0 && device_state == USB_DEV_STATE_DEFAULT) ||
            (setup_packet.data.wValue == 0 && device_state == USB_DEV_STATE_ADDRESS);

    if(update_address)
    {
        SETUP_ENDPOINT_WAIT_TRANS_DONE_FUNC(std_handler_set_address_status_stage);
        result = MDR_USB_CDC_Ok;
    }

    return result;
}

void std_handler_set_address_status_stage(void)
{
    MDR_USB->SA  = setup_packet.data.wValue;
    mdr_usb_cdc_set_device_state(
        (setup_packet.data.wValue == 0) ? USB_DEV_STATE_DEFAULT : USB_DEV_STATE_ADDRESS
    );
}

MDR_USB_CDC_Result std_handler_get_descriptor(void)
{
    return std_handler_get_descriptor_send(1);
}

MDR_USB_CDC_Result std_handler_get_descriptor_data_stage(void)
{
    return std_handler_get_descriptor_send(0);
}

MDR_USB_CDC_Result std_handler_get_descriptor_send(uint8_t first_chunk)
{
    static uint16_t desc_data_offset;
    desc_data_offset = first_chunk ? 0 : (desc_data_offset+MDR_USB_CDC_MAX_PACKET_SIZE);

    MDR_USB_CDC_Result result;
    const uint8_t *desc_data;
    uint16_t desc_data_len;

    result = mdr_usb_cdc_resolve_discriptor(&desc_data, &desc_data_len);
    if(result == MDR_USB_CDC_Ok)
    {
        assert_param(desc_data_len > desc_data_offset);
        desc_data_len -= desc_data_offset;
        desc_data     += desc_data_offset;

        uint8_t to_send;
        if(desc_data_len > MDR_USB_CDC_MAX_PACKET_SIZE)
        {
            to_send = MDR_USB_CDC_MAX_PACKET_SIZE;
            SETUP_ENDPOINT_WAIT_IN_TRANS(std_handler_get_descriptor_data_stage);
        }
        else
        {
            to_send = desc_data_len;
            SETUP_ENDPOINT_WAIT_IN_THEN_OUT_TRANS;
        }

        do
        {
            MDR_USB->USB_SEP_FIFO[ENDPOINT_SETUP].TXFD = *desc_data++;
        } while(--to_send);
        result = MDR_USB_CDC_Ok;
    }

    return result;
}

MDR_USB_CDC_Result mdr_usb_cdc_resolve_discriptor(const uint8_t **data, uint16_t *data_len)
{
    MDR_USB_CDC_Result result = MDR_USB_CDC_Fail;
    const uint8_t desc_type  = setup_packet.data.wValue >> 8;
    const uint8_t desc_index = setup_packet.data.wValue & 0xFF;

    if(desc_index == 0)
    {
        if(desc_type == USB_DEVICE)
        {
            *data = Usb_CDC_Device_Descriptor;
            *data_len = MDR_USB_CDC_DEVICE_DESCR_LENGTH;
            result = MDR_USB_CDC_Ok;
        }
        else if(desc_type == USB_CONFIGURATION)
        {
            *data = Usb_CDC_Configuration_Descriptor;
            *data_len = MDR_USB_CDC_CONFIG_DESCR_LENGTH;
            result = MDR_USB_CDC_Ok;
        }

        if(*data_len > setup_packet.data.wLength)
            *data_len = setup_packet.data.wLength;
    }
    return result;
}

MDR_USB_CDC_Result std_handler_set_descriptor(void)
{
    MDR_USB_CDC_Result result = MDR_USB_CDC_Fail;
    return result;
}

MDR_USB_CDC_Result std_handler_get_configuration(void)
{
    uint8_t configuration_num = device_state == USB_DEV_STATE_CONFIGURED ? 1 : 0;

    MDR_USB->USB_SEP_FIFO[ENDPOINT_SETUP].TXFD = configuration_num;
    SETUP_ENDPOINT_WAIT_IN_THEN_OUT_TRANS;

    return MDR_USB_CDC_Ok;
}

MDR_USB_CDC_Result std_handler_set_configuration(void)
{
    MDR_USB_CDC_Result result = MDR_USB_CDC_Fail;
    if(setup_packet.data.wValue == 0)
    {
        mdr_usb_cdc_set_device_state(USB_DEV_STATE_ADDRESS);
        result = MDR_USB_CDC_Ok;
    }
    else if(setup_packet.data.wValue == 1)
    {
        mdr_usb_cdc_set_device_state(USB_DEV_STATE_CONFIGURED);
        result = MDR_USB_CDC_Ok;
    }
    if(result == MDR_USB_CDC_Ok)
        SETUP_ENDPOINT_WAIT_TRANS_DONE;

    return result;
}

MDR_USB_CDC_Result std_handler_get_interface(void)
{
    MDR_USB_CDC_Result result = MDR_USB_CDC_Fail;
    if(device_state == USB_DEV_STATE_CONFIGURED)
    {
        result = MDR_USB_CDC_Fail;
        MDR_USB->USB_SEP_FIFO[ENDPOINT_SETUP].TXFD = 0;
        SETUP_ENDPOINT_WAIT_IN_THEN_OUT_TRANS;
    }
    return result;
}

MDR_USB_CDC_Result std_handler_set_interface(void)
{
    MDR_USB_CDC_Result result = MDR_USB_CDC_Fail;
    if( device_state == USB_DEV_STATE_CONFIGURED &&
            setup_packet.data.wValue == 0 &&
            setup_packet.data.wIndex == 0 )
    {
        SETUP_ENDPOINT_WAIT_TRANS_DONE;
        result = MDR_USB_CDC_Ok;
    }

    return result;
}

MDR_USB_CDC_Result std_handler_synch_frame(void)
{
    MDR_USB_CDC_Result result = MDR_USB_CDC_Fail;
    return result;
}

MDR_USB_CDC_Result mdr_usb_cdc_class_request_handler(void)
{
    MDR_USB_CDC_Result result = MDR_USB_CDC_Fail;
    if(setup_packet.data.bRequest == USB_CDC_SET_LINE_CODING)
    {
        if(setup_packet.data.wLength == sizeof(usb_cdc_line_coding))
        {
            result = MDR_USB_CDC_Ok;
            SETUP_ENDPOINT_WAIT_OUT_TRANS(mdr_usb_cdc_set_line_coding);
        }
    }
    else if(setup_packet.data.bRequest == USB_CDC_GET_LINE_CODING)
    {
        if(setup_packet.data.wLength == sizeof(usb_cdc_line_coding))
        {
            result = MDR_USB_CDC_Ok;
            for(uint8_t i=0; i<sizeof(usb_cdc_line_coding); i++)
                MDR_USB->USB_SEP_FIFO[ENDPOINT_SETUP].TXFD = current_line_coding.raw_data[i];

            SETUP_ENDPOINT_WAIT_IN_THEN_OUT_TRANS;
        }
    }
    else
    {

    }
    return result;
}

MDR_USB_CDC_Result mdr_usb_cdc_set_line_coding(void)
{
    MDR_USB_CDC_Result result = MDR_USB_CDC_Fail;

    const uint32_t data_len = MDR_USB->USB_SEP_FIFO[ENDPOINT_SETUP].RXFDC_H;
    if(data_len == sizeof(usb_cdc_line_coding))
    {
        result = MDR_USB_CDC_Ok;
        for(uint8_t i=0; i<sizeof(usb_cdc_line_coding); i++)
            current_line_coding.raw_data[i] = MDR_USB->USB_SEP_FIFO[ENDPOINT_SETUP].RXFD;
        SETUP_ENDPOINT_WAIT_TRANS_DONE;
    }
    return result;
}

MDR_USB_CDC_Result setup_endpoint_switch_after_in_trans()
{
    SETUP_ENDPOINT_WAIT_OUT_TRANS(setup_endpoint_control_trans_end);
    return MDR_USB_CDC_Ok;
}

MDR_USB_CDC_Result setup_endpoint_control_trans_end()
{
    (*setup_endpoint_control_trans_end_handler)();
    setup_endpoint_reset();
    return MDR_USB_CDC_Ok;
}

void setup_endpoint_control_trans_end_def_handler(void)
{
}

MDR_USB_CDC_Result mdr_usb_cdc_error_handler(void)
{
    return MDR_USB_CDC_Fail;
}

void mdr_usb_cdc_set_device_state(usb_device_state new_state)
{
    if(device_state != new_state)
    {
        device_state = new_state;
        if(new_state == USB_DEV_STATE_CONFIGURED)
        {
            recv_endpoint_init();
            send_endpoint_init();
        }
        else
        {
            recv_endpoint_deinit();
            send_endpoint_deinit();
        }
    }
}

static volatile uint8_t *recv_endpoint_buf;
static volatile uint8_t send_endpoint_sending_bytes;

void mdr_usb_cdc_dispatch_send_endpoint(void)
{
    mdr_usb_cdc_endpoint_state e_send_state;
    uint8_t sending = mdr_usb_cdc_is_sending() == MDR_USB_CDC_Ok;

    if(sending && mdr_usb_cdc_get_endpoint_state(ENDPOINT_SEND, &e_send_state))
    {
        uint8_t on_data_sent = 0;
        uint32_t next_ctrl = 0;
        if(e_send_state.TS == USB_SEP_TS_IN_TRANS && !e_send_state.error)
        {
            if(e_send_state.STS & USB_SEP_STS_SCACKRXED)
            {
                ENDPOINT_CLEAR_TX(ENDPOINT_SEND);
                on_data_sent=1;
            }
            else
            {
                next_ctrl = e_send_state.CTRL | USB_SEP_CTRL_EPRDY;
            }
        }
        else
        {
            next_ctrl = (e_send_state.CTRL | USB_SEP_CTRL_EPRDY) & ~USB_SEP_CTRL_EPSSTALL;
            ENDPOINT_CLEAR_RX(ENDPOINT_SEND);
        }

        MDR_USB->USB_SEP[ENDPOINT_SEND].STS = 0;
        if(on_data_sent)
        {
            const uint8_t prev_sent_bytes = send_endpoint_sending_bytes;
            send_endpoint_sending_bytes = 0;

            mdr_usb_cdc_on_data_sent();

            sending = mdr_usb_cdc_is_sending() == MDR_USB_CDC_Ok;
            if((prev_sent_bytes == MDR_USB_CDC_MAX_PACKET_SIZE) && !sending)
            {
                send_endpoint_sending_bytes = UINT8_MAX;
                next_ctrl = (e_send_state.CTRL ^ USB_SEP_CTRL_EPDATASEQ) | USB_SEP_CTRL_EPRDY;
            }
        }
        if(next_ctrl != 0)
        {
            MDR_USB->USB_SEP[ENDPOINT_SEND].CTRL = next_ctrl;
        }
    }
}

void mdr_usb_cdc_dispatch_recv_endpoint(void)
{
    mdr_usb_cdc_endpoint_state e_recv_state;
    const uint8_t receiving = mdr_usb_cdc_is_recv_buf_setted() == MDR_USB_CDC_Ok;
    if(receiving && mdr_usb_cdc_get_endpoint_state(ENDPOINT_RECV, &e_recv_state))
    {
        uint8_t on_data_recv = 0;
        uint8_t data_len = 0;
        if(e_recv_state.TS == USB_SEP_TS_OUT_TRANS && !e_recv_state.error)
        {
            data_len = MDR_USB->USB_SEP_FIFO[ENDPOINT_RECV].RXFDC_H;
            if(data_len > 0 && data_len <= MDR_USB_CDC_MAX_PACKET_SIZE)
            {
                uint8_t *data = (uint8_t *) recv_endpoint_buf;
                uint8_t data_read_rem = data_len;

                do
                {
                    *data++ = MDR_USB->USB_SEP_FIFO[ENDPOINT_RECV].RXFD;
                } while(--data_read_rem);

                on_data_recv = 1;
            }
        }

        ENDPOINT_CLEAR_RX(ENDPOINT_RECV);
        MDR_USB->USB_SEP[ENDPOINT_RECV].STS = 0;
        if(on_data_recv)
        {
            recv_endpoint_buf = 0;
            mdr_usb_cdc_on_data_recv(data_len);
        }
    }
}

void recv_endpoint_init()
{
    uint32_t ctrl = USB_SEP_CTRL_EPEN;
    if(mdr_usb_cdc_is_recv_buf_setted() == MDR_USB_CDC_Ok)
    {
        ctrl |= USB_SEP_CTRL_EPRDY;
    }
    MDR_USB->USB_SEP[ENDPOINT_RECV].CTRL = ctrl;

    ENDPOINT_CLEAR_RX(ENDPOINT_RECV);
    ENDPOINT_CLEAR_TX(ENDPOINT_RECV);
}

void send_endpoint_init()
{
    send_endpoint_sending_bytes = 0;
    MDR_USB->USB_SEP[ENDPOINT_SEND].CTRL = USB_SEP_CTRL_EPEN | USB_SEP_CTRL_EPDATASEQ;

    ENDPOINT_CLEAR_RX(ENDPOINT_SEND);
    ENDPOINT_CLEAR_TX(ENDPOINT_SEND);

    MDR_USB->USB_SEP[ENDPOINT_STAT].CTRL = USB_SEP_CTRL_EPEN;
}

void recv_endpoint_deinit()
{
    MDR_USB->USB_SEP[ENDPOINT_RECV].CTRL = 0;
}

void send_endpoint_deinit()
{
    send_endpoint_sending_bytes = 0;
    MDR_USB->USB_SEP[ENDPOINT_SEND].CTRL = 0;
    MDR_USB->USB_SEP[ENDPOINT_STAT].CTRL = 0;
}

MDR_USB_CDC_Result mdr_usb_cdc_is_sending()
{
    return send_endpoint_sending_bytes != 0 ? MDR_USB_CDC_Ok : MDR_USB_CDC_Fail;
}

MDR_USB_CDC_Result mdr_usb_cdc_send(uint8_t data_len, const uint8_t data[data_len])
{
    assert_param(data_len > 0);
    assert_param(data_len <= MDR_USB_CDC_MAX_PACKET_SIZE);

    MDR_USB_CDC_Result result = MDR_USB_CDC_Fail;
    uint8_t sending = mdr_usb_cdc_is_sending() == MDR_USB_CDC_Ok;

    if(!sending && (MDR_USB->USB_SEP[ENDPOINT_SEND].CTRL & USB_SEP_CTRL_EPEN))
    {
        NVIC_DisableIRQ(USB_IRQn);
        result = mdr_usb_cdc_send_isr(data_len, data);
        NVIC_EnableIRQ(USB_IRQn);
    }

    return result;
}

MDR_USB_CDC_Result mdr_usb_cdc_send_isr(uint8_t data_len, const uint8_t data[data_len])
{
    assert_param(data_len > 0);
    assert_param(data_len <= MDR_USB_CDC_MAX_PACKET_SIZE);

    MDR_USB_CDC_Result result = MDR_USB_CDC_Fail;
    uint32_t ctrl;
    uint8_t sending = mdr_usb_cdc_is_sending() == MDR_USB_CDC_Ok;

    if(!sending && ((ctrl = MDR_USB->USB_SEP[ENDPOINT_SEND].CTRL) & USB_SEP_CTRL_EPEN))
    {
        result = MDR_USB_CDC_Ok;
        send_endpoint_sending_bytes=data_len;

        do
        {
            MDR_USB->USB_SEP_FIFO[ENDPOINT_SEND].TXFD = *data++;
        } while(--data_len);

        MDR_USB->USB_SEP[ENDPOINT_SEND].STS = 0;
        ctrl &= ~USB_SEP_CTRL_EPSSTALL;
        ctrl ^= USB_SEP_CTRL_EPDATASEQ;
        MDR_USB->USB_SEP[ENDPOINT_SEND].CTRL = ctrl | USB_SEP_CTRL_EPRDY;
    }

    return result;
}

MDR_USB_CDC_Result mdr_usb_cdc_is_recv_buf_setted()
{
    return recv_endpoint_buf != 0 ? MDR_USB_CDC_Ok : MDR_USB_CDC_Fail;
}

void mdr_usb_cdc_set_recv_buf(uint8_t buf[MDR_USB_CDC_MAX_PACKET_SIZE])
{
    uint32_t ctrl;

    recv_endpoint_buf = buf;

    ctrl = MDR_USB->USB_SEP[ENDPOINT_RECV].CTRL;
    if(ctrl & USB_SEP_CTRL_EPEN)
    {
        NVIC_DisableIRQ(USB_IRQn);
        mdr_usb_cdc_set_recv_buf_isr(buf);
        NVIC_EnableIRQ(USB_IRQn);
    }
}

void mdr_usb_cdc_set_recv_buf_isr(uint8_t buf[MDR_USB_CDC_MAX_PACKET_SIZE])
{
    uint32_t ctrl;
    recv_endpoint_buf = buf;
    ctrl = MDR_USB->USB_SEP[ENDPOINT_RECV].CTRL;
    if(ctrl & USB_SEP_CTRL_EPEN)
    {
        MDR_USB->USB_SEP[ENDPOINT_RECV].CTRL = ctrl | USB_SEP_CTRL_EPRDY;
    }
}

void MDR_USB_CDC_CALLBACK mdr_usb_cdc_on_data_sent()
{

}

void MDR_USB_CDC_CALLBACK mdr_usb_cdc_on_data_recv(uint8_t data_len)
{
    (void) data_len;
}
