#include "Timer.h"

struct Timer {
};


struct Timer *AddTimerImpl(void *data, int ms, TimerCallback action)
{
    return NULL;
}

void RemoveTimerImpl(struct Timer *t)
{

}

void (*RemoveTimer)(struct Timer *t) = RemoveTimerImpl;
struct Timer *(*AddTimer)(void *data, int ms, TimerCallback action) = AddTimerImpl;

