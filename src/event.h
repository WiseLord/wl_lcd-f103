#ifndef EVENT_H
#define EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    EVENT_NONE = 0,

    EVENT_BTN_SHORT,
    EVENT_BTN_LONG,

    EVENT_PAUSE_TIMER,
    EVENT_CONTINUE_TIMER,

    EVENT_END
} EventType;

typedef struct {
    EventType type;
    int16_t value;
} Event;

#ifdef __cplusplus
}
#endif

#endif // EVENT_H
