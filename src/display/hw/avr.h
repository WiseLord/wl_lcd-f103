#ifndef AVR_H
#define AVR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>

#define CONCAT(x,y)             x ## y

#define DDR(x)                  CONCAT(DDR,x)
#define PORT(x)                 CONCAT(PORT,x)
#define PIN(x)                  CONCAT(PIN,x)

#define SET(x)                  (PORT(x) |= x ## _LINE)
#define CLR(x)                  (PORT(x) &= ~x ## _LINE)
#define READ(x)                 (PIN(x) & x ## _LINE)
#define OUT(x)                  (DDR(x) |= x ## _LINE)

#define READ_BYTE(p)            (PIN(p))
#define WRITE_BYTE(p, data)     (PORT(p) = data)
#define IN_BYTE(p)              (DDR(p) = 0x00)
#define OUT_BYTE(p)             (DDR(p) = 0xFF)



#ifdef __cplusplus
}
#endif

#endif // AVR_H
