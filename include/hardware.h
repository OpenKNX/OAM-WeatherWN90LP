#ifdef DEVICE_UP1_GW_RS485

    #include "HardwareConfig/OpenKNX/UP1.h"

#endif

#ifdef TESTDEVICE_PIPICO_GW_RS485

    // Debug PiPico

    #define PROG_LED_PIN (2)
    #define PROG_LED_PIN_ACTIVE_ON HIGH
    #define PROG_BUTTON_PIN (7)
    #define PROG_BUTTON_PIN_INTERRUPT_ON FALLING
    #define KNX_SERIAL Serial1
    #define KNX_UART_RX_PIN (1)
    #define KNX_UART_TX_PIN (0)

    #define RS485_SERIAL Serial2
    #define RS485_UART_RX_PIN (5)
    #define RS485_UART_TX_PIN (4)
    #define RS485_UART_DIR_PIN (3)


#endif

