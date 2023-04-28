#include "railgun.h"

uint8_t railgun_charges = 5;
OS_SEM railgun_semaphore;
bool auto_cannon = true;
int railgun_fired = 0;
int shotX;
int shotY;

static OS_TCB railgunTCB;
static CPU_STK railgunSTK[STACK_SIZES];

// extern OS_MUTEX sc_mutex;
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

        if (shoot_railgun(lowestIdx))
        {
            RTOS_ERR mutex_err;
            OSMutexPend(&platform_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutex_err);

            // TODO adding the target xy
            draw_laser(shotX, shotY);

            OSMutexPost(&platform_mutex, OS_OPT_POST_NONE, &mutex_err);
        }
    }
}

bool shoot_railgun(int idx)
{
    (void)&idx; // Put this into unuse, if I got time to add this.
    if (railgun_charges > 0)
    {
        railgun_charges--;
        score++;
        railgun_fired += 1;

        // RTOS_ERR mutexErr;
        // OSMutexPend(&sc_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);
        // if (mutexErr.Code)
        //     EFM_ASSERT(false);

        // // TODO

        // OSMutexPost(&sc_mutex, OS_OPT_POST_NONE, &mutexErr);
        // if (mutexErr.Code)
        //     EFM_ASSERT(false);
        return true;
    }
    return false;
}

void shield_active_cb(void)
{
    RTOS_ERR tmrErr;
    RTOS_ERR mutexErr;
    OSMutexPend(&shield_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);
    if (mutexErr.Code)
        EFM_ASSERT(false);

    if (shield_state.active)
    {
        shield_state.active = false;
        shield_state.recharging = true;
        OSTmrStart(&shieldRechargeTimer, &tmrErr);
        if (tmrErr.Code)
            EFM_ASSERT(false);
    }

    OSMutexPost(&shield_mutex, OS_OPT_POST_NONE, &mutexErr);
    if (mutexErr.Code)
        EFM_ASSERT(false);
}

void shield_recharge_cb(void)
{
    RTOS_ERR mutexErr;
    OSMutexPend(&shield_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);
    if (mutexErr.Code)
        EFM_ASSERT(false);

    if (shield_state.recharging)
    {
        shield_state.recharging = false;
    }

    OSMutexPost(&shield_mutex, OS_OPT_POST_NONE, &mutexErr);
    if (mutexErr.Code)
        EFM_ASSERT(false);
}

void shield_task_create(void)
{
    RTOS_ERR mutexErr;
    RTOS_ERR tskErr;
    RTOS_ERR qErr;
    RTOS_ERR semErr;
    RTOS_ERR tmrErr;
    OSTaskCreate(
        &shieldTCB,          /* Pointer to the task's TCB.  */
        "shield Task.",      /* Name to help debugging.     */
        &shield_task,        /* Pointer to the task's code. */
        DEF_NULL,            /* Pointer to task's argument. */
        NORMAL_PRIORITY,     /* Task's priority.            */
        &shieldSTK[0],       /* Pointer to base of stack.   */
        (STACK_SIZES / 10u), /* Stack limit, from base.     */
        STACK_SIZES,         /* Stack size, in CPU_STK.     */
        10u,                 /* Messages in task queue.     */
        120u,                /* Round-Robin time quanta.    */
        DEF_NULL,            /* External TCB data.          */
        OS_OPT_TASK_STK_CHK, /* Task options.               */
        &tskErr);
    OSQCreate(
        &shield_msg,
        "shield Message queue",
        4,
        &qErr);
    OSTmrCreate(
        &shieldActiveTimer,
        "active timer",
        SHIELD_ACTIVE_100MS,
        0,
        OS_OPT_TMR_ONE_SHOT,
        &shield_active_cb,
        NULL,
        &tmrErr);
    OSTmrCreate(
        &shieldRechargeTimer,
        "recharge timer",
        SHIELD_RECHARGE_100MS,
        0,
        OS_OPT_TMR_ONE_SHOT,
        &shield_recharge_cb,
        NULL,
        &tmrErr);
    OSMutexCreate(
        &shield_mutex,
        "shield Mutex",
        &mutexErr);
    if (qErr.Code || mutexErr.Code || tskErr.Code || tmrErr.Code)
        EFM_ASSERT(false);

    shield_state.active = false;
    shield_state.recharging = false;
}

void shield_task(void)
{
    RTOS_ERR qErr;
    RTOS_ERR tmrErr;
    RTOS_ERR mutexErr;

    uint8_t *msg;
    uint8_t *size;
    while (1)
    {
        msg = OSQPend(&shield_msg, 0, OS_OPT_PEND_BLOCKING, &size, NULL, &qErr);
        if (qErr.Code)
            EFM_ASSERT(false);
        // TODO shield task
        OSMutexPend(&shield_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);
        if (mutexErr.Code)
            EFM_ASSERT(false);

        if (*msg == 1 && !shield_state.recharging)
        {
            shield_state.active = true;
            OSTmrStart(&shieldActiveTimer, &tmrErr);
            if (tmrErr.Code)
                EFM_ASSERT(false);
        }
        else if (*msg == 0 && shield_state.active)
        {
            shield_state.active = false;
            shield_state.recharging = true;
            OSTmrStart(&shieldRechargeTimer, &tmrErr);
            if (tmrErr.Code)
                EFM_ASSERT(false);
        }

        OSMutexPost(&shield_mutex, OS_OPT_POST_NONE, &mutexErr);
        if (mutexErr.Code)
            EFM_ASSERT(false);

        free(msg);
    }
}
