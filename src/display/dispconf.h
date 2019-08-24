#ifndef DISPCONF_H
#define DISPCONF_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _DISP_SPI
#include "spi.h"
#define SPI_DISPLAY             SPI2
#define DISP_WAIT_BUSY()        spiWaitBusy(SPI_DISPLAY)
#define DISP_SPI_INIT()         spiInit(SPI_DISPLAY, false)
#define DISP_SPI_SEND_BYTE(x)   spiSendByte(SPI_DISPLAY, x)
#else
#define DISP_WAIT_BUSY()        (void)0
#endif

#include "utils.h"
#define DISP_MDELAY(x)          utilmDelay(x)

// TFT LCD pins
#ifdef _DISP_16BIT
#define DISP_DATA_HI_Port       GPIOB
#define DISP_DATA_HI_Pin        0xFF00
#define DISP_DATA_LO_Port       GPIOC
#define DISP_DATA_LO_Pin        0x00FF
#else
#define DISP_DATA_Port          GPIOB

#define DISP_DATA_Pin           0x00FF
#endif

#define DISP_CS_Port            GPIOC
#define DISP_CS_Pin             (1<<9)
#define DISP_RS_Port            GPIOC
#define DISP_RS_Pin             (1<<8)
#define DISP_WR_Port            GPIOC
#define DISP_WR_Pin             (1<<10)

#ifdef _DISP_READ_ENABLED
#define DISP_RD_Port            GPIOC
#define DISP_RD_Pin             (1<<11)
#endif

#ifdef _DISP_RST_ENABLED
#define DISP_RST_Port           GPIOB
#define DISP_RST_Pin            (1<<5)
#endif

#ifdef _DISP_BCKL_ENABLED
#define DISP_BCKL_Port          GPIOC
#define DISP_BCKL_Pin           (1<<12)
#endif

#ifdef __cplusplus
}
#endif

#endif // DISPCONF_H
