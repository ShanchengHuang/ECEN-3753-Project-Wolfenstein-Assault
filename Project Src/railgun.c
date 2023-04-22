#include "railgun.h"

uint8_t railgun_charges = 5;
OS_SEM railgun_semaphore;
bool auto_cannon = true;
int railgun_fired = 0;
int shotX;
int shotY;

static OS_TCB railgunTCB;
static CPU_STK railgunSTK[STACK_SIZES];

extern uint8_t HM_COUNT;
extern struct HoltzmanData HMs[];
extern OS_MUTEX hm_mutex;
extern int score;

void railgun_task_create(void)
{
    RTOS_ERR semErr;
    RTOS_ERR tskErr;
    OSSemCreate(
        &railgun_semaphore,
        "railgun semaphore",
        0,
        &semErr);
    OSTaskCreate(
        &railgunTCB,         /* Pointer to the task's TCB.  */
        "railgun Task.",     /* Name to help debugging.     */
        &railgun_task,       /* Pointer to the task's code. */
        DEF_NULL,            /* Pointer to task's argument. */
        NORMAL_PRIORITY,     /* Task's priority.            */
        &railgunSTK[0],      /* Pointer to base of stack.   */
        (STACK_SIZES / 10u), /* Stack limit, from base.     */
        STACK_SIZES,         /* Stack size, in CPU_STK.     */
        10u,                 /* Messages in task queue.     */
        120u,                /* Round-Robin time quanta.    */
        DEF_NULL,            /* External TCB data.          */
        OS_OPT_TASK_STK_CHK, /* Task options.               */
        &tskErr);
    if (semErr.Code || tskErr.Code)
        EFM_ASSERT(false);
}
bool shoot_railgun(int idx)
{
    if (railgun_charges > 0)
    {
        railgun_charges--;
        score++;
        railgun_fired = 3;
        shotX = HMs[idx].x;
        shotY = HMs[idx].y;
        RTOS_ERR mutexErr;
        OSMutexPend(&hm_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);
        if (mutexErr.Code)
            EFM_ASSERT(false);

        generate_hm(idx);

        OSMutexPost(&hm_mutex, OS_OPT_POST_NONE, &mutexErr);
        if (mutexErr.Code)
            EFM_ASSERT(false);
        return true;
    }
    return false;
}

void railgun_task(void)
{
    RTOS_ERR semErr;
    RTOS_ERR mutexErr;
    while (1)
    {
        OSSemPend(&railgun_semaphore, 0, OS_OPT_PEND_BLOCKING, NULL, &semErr);
        if (semErr.Code)
            EFM_ASSERT(false);
        // TODO check railgun count, if good, decrement and pend hm mutex and respawn it
        int lowestIdx = 0;
        int lowestHeight = 0; // lowest is highest y
        for (int i = 0; i < HM_COUNT; i++)
        {
            if (lowestHeight < HMs[i].y)
            {
                lowestHeight = HMs[i].y;
                lowestIdx = i;
            }
        }
        shoot_railgun(lowestIdx);
    }
}
