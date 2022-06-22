#ifndef VAC_H
#define VAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "event.h"

typedef enum {
    VAC_OFF = 0,
    VAC_ON,

    VAC_END
} VacState;

typedef struct {
    VacState state;
    int32_t timer;
} VacContext;

void vacInit(void);

void vacSetTimer(int32_t value);

void vacSetState(VacState value);

void vacUpdateTimers();

int32_t vacGetTimer();

Event vacGetButtons();

void vacEventGet();
void vacEventHandle();

#ifdef __cplusplus
}
#endif

#endif // VAC_H
