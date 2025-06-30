#include "uart_access.h"
#include "pico/stdlib.h"

// only one of this must be set to 1
#define PICO_USE_USB_CDC_VIRTUAL_UART 1
#define PICO_USE_HW_UART0             0
#define PICO_USE_HW_UART1             0

#if (1 == PICO_USE_USB_CDC_VIRTUAL_UART)
    #include "pico/stdio_usb.h"
#else
    #include "hardware/uart.h"
    #define BAUD_RATE 115200

    #if (1 == PICO_USE_HW_UART0)
        #define UART_ID uart0
        #define UART_TX_PIN 0
        #define UART_RX_PIN 1
    #elif (1 == PICO_USE_HW_UART1)
        #define UART_ID uart1
        #define UART_TX_PIN 4
        #define UART_RX_PIN 5
    #else
        #error "UART port not selected .."
    #endif
#endif

#include <stdarg.h>
#include <stdint.h>

/* ================================================
            private interfaces declaration
==================================================*/

static void print_int(int value, int width, char pad);
static void print_hex(unsigned int value, int width, char pad);
static void print_int_to_buf(char *buf, int *pos, int maxlen, int value, int width, char pad);
static void print_hex_to_buf(char *buf, int *pos, int maxlen, unsigned int value, int width, char pad);


/* ================================================
            public interfaces ddefinition
==================================================*/

#if (1 == PICO_USE_USB_CDC_VIRTUAL_UART)

    void uart_setup(void) {
        stdio_usb_init();
    }

    void uart_putchar(char c) {
        putchar_raw(c);  // Sends character over USB CDC
    }

    int uart_getchar(void) {
        int c;
        do {
            c = getchar_timeout_us(0);  // Non-blocking read
        } while (c == PICO_ERROR_TIMEOUT);
        return c;
    }

#else // HW UART0, UART1

    void uart_setup(void) {
        uart_init(UART_ID, BAUD_RATE);
        gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
        gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    }

    void uart_putchar(char c) {
        uart_putc_raw(UART_ID, c);
    }

    int uart_getchar(void) {
        while (!uart_is_readable(UART_ID)) {
            tight_loop_contents(); // Optional: low-power wait
        }
        return uart_getc(UART_ID);
    }

#endif


/*--------------------------------------------------*/
int uart_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            char pad = ' ';
            int width = 0;
            if (*fmt == '0') {
                pad = '0';
                fmt++;
            }
            while (*fmt >= '0' && *fmt <= '9') {
                width = width * 10 + (*fmt - '0');
                fmt++;
            }
            switch (*fmt) {
                case 's': {
                    const char *s = va_arg(args, const char *);
                    while (*s) {
                        uart_putchar(*s++);
                    }
                    break;
                }
                case 'd':
                    print_int(va_arg(args, int), width, pad);
                    break;
                case 'x':
                    print_hex(va_arg(args, unsigned int), width, pad);
                    break;
                case 'c':
                    uart_putchar((char)va_arg(args, int));
                    break;
                default:
                    uart_putchar('%');
                    uart_putchar(*fmt);
                    break;
            }
        } else {
            uart_putchar(*fmt);
        }
        fmt++;
    }
    va_end(args);

    return 0;
}



/*--------------------------------------------------*/
int uart_snprintf(char *buf, int maxlen, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int pos = 0;

    while (*fmt && pos < maxlen - 1) {
        if (*fmt == '%') {
            fmt++;
            char pad = ' ';
            int width = 0;
            if (*fmt == '0') {
                pad = '0';
                fmt++;
            }
            while (*fmt >= '0' && *fmt <= '9') {
                width = width * 10 + (*fmt - '0');
                fmt++;
            }
            switch (*fmt) {
                case 's': {
                    const char *s = va_arg(args, const char *);
                    while (*s && pos < maxlen - 1) {
                        buf[pos++] = *s++;
                    }
                    break;
                }
                case 'd':
                    print_int_to_buf(buf, &pos, maxlen, va_arg(args, int), width, pad);
                    break;
                case 'x':
                    print_hex_to_buf(buf, &pos, maxlen, va_arg(args, unsigned int), width, pad);
                    break;
                case 'c':
                    buf[pos++] = (char)va_arg(args, int);
                    break;
                default:
                    buf[pos++] = '%';
                    buf[pos++] = *fmt;
                    break;
            }
        } else {
            buf[pos++] = *fmt;
        }
        fmt++;
    }

    buf[pos] = '\0';
    va_end(args);
    return pos;
}



/* ================================================
            private interfaces ddefinition
==================================================*/


/*--------------------------------------------------*/
static void print_int(int value, int width, char pad)
{
    char buffer[12];
    int i = 0;
    if (value < 0) {
        uart_putchar('-');
        value = -value;
        width--;
    }
    do {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    } while (value);
    while (width-- > i) {
        uart_putchar(pad);
    }
    while (i--) {
        uart_putchar(buffer[i]);
    }
}



/*--------------------------------------------------*/
static void print_hex(unsigned int value, int width, char pad)
{
    const char *hex = "0123456789ABCDEF";
    char buffer[8];
    int i = 0;
    do {
        buffer[i++] = hex[value & 0xF];
        value >>= 4;
    } while (value);
    while (width-- > i + 2) {
        uart_putchar(pad);
    }
    uart_putchar('0');
    uart_putchar('x');
    while (i--) {
        uart_putchar(buffer[i]);
    }
}



/*--------------------------------------------------*/
static void print_int_to_buf(char *buf, int *pos, int maxlen, int value, int width, char pad)
{
    char tmp[12];
    int i = 0;
    if (value < 0) {
        if (*pos < maxlen - 1) buf[(*pos)++] = '-';
        value = -value;
        width--;
    }
    do {
        tmp[i++] = '0' + (value % 10);
        value /= 10;
    } while (value);
    while (width-- > i && *pos < maxlen - 1) {
        buf[(*pos)++] = pad;
    }
    while (i-- && *pos < maxlen - 1) {
        buf[(*pos)++] = tmp[i];
    }
}



/*--------------------------------------------------*/
static void print_hex_to_buf(char *buf, int *pos, int maxlen, unsigned int value, int width, char pad)
{
    const char *hex = "0123456789ABCDEF";
    char tmp[8];
    int i = 0;
    do {
        tmp[i++] = hex[value & 0xF];
        value >>= 4;
    } while (value);
    if (*pos < maxlen - 2) {
        buf[(*pos)++] = '0';
        buf[(*pos)++] = 'x';
    }
    while (width-- > i + 2 && *pos < maxlen - 1) {
        buf[(*pos)++] = pad;
    }
    while (i-- && *pos < maxlen - 1) {
        buf[(*pos)++] = tmp[i];
    }
}


