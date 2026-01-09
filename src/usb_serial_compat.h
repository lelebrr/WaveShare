#ifndef USB_SERIAL_COMPAT_H
#define USB_SERIAL_COMPAT_H

#include <Arduino.h>

// ESP32-S3 com ARDUINO_USB_CDC_ON_BOOT=1 usa Serial para USB CDC
// Este header cria um alias USBSerial -> Serial para compatibilidade
#if defined(ARDUINO_USB_CDC_ON_BOOT) && ARDUINO_USB_CDC_ON_BOOT == 1
#define USBSerial Serial
#else
// Fallback: se não tiver USB CDC, USBSerial também aponta para Serial
#define USBSerial Serial
#endif

#endif // USB_SERIAL_COMPAT_H
