#ifndef INPUT_H
#define INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define BTN_NO                  0x0000

#define BTN_D0                  0x0001
#define BTN_ALL                 (BTN_D0)

// Handling long press actions
#define SHORT_PRESS             60
#define LONG_PRESS              600
#define AUTOREPEAT              150

#define BTN_FLAG_NO             0x0000
#define BTN_FLAG_LONG_PRESS     0x0001

// Input pins are shared with display data port
#define INPUT_Port              GPIOB
#define INPUT_Pin               (LL_GPIO_PIN_0)

typedef struct {
    uint16_t btn;
    uint16_t flags;
} CmdBtn;

typedef struct {
    uint16_t btn;
    uint16_t flags;
} Input;

void inputInit(void);
Input *inputGet(void);

CmdBtn inputGetBtnCmd(void);

#ifdef __cplusplus
}
#endif

#endif // INPUT_H
