#include "satchel.h"
#include "em_assert.h"
#include "stdlib.h"

uint8_t SATCHEL_COUNT = 1;
struct SatchelData Satchels[2];
OS_MUTEX sc_mutex;

void generate_satchel(int idx)
{
    Satchels[idx].x = (rand() % (CANYON_SIZE_PIXELS - (2 * SATCH_PIXEL_RADIUS))) + SATCH_PIXEL_RADIUS + CANYON_START;
    Satchels[idx].y = SATCH_PIXEL_RADIUS;
    Satchels[idx].exploded = 0;
    Satchels[idx].vx = (rand() % 20) - 10;
    Satchels[idx].vy = rand() % 10;
}

void satchel_init()
{
    RTOS_ERR mutexErr;
    OSMutexCreate(&sc_mutex, "sc_mutex", &mutexErr);
    if (mutexErr.Code)
        EFM_ASSERT(false);
    for (int i = 0; i < SATCHEL_COUNT; i++)
    {
        generate_satchel(i);
    }
}