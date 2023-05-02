#include "railgun.h"

uint8_t railgun_charges = 5;
bool railgun_fired = 0;

struct BulletData Bullet;

static OS_TCB railgunTCB;
static CPU_STK railgunSTK[STACK_SIZES];

struct ShieldState shield_state;

// extern OS_MUTEX sc_mutex;
extern int score;

void railgun_task_create(void)
{
	RTOS_ERR semErr;
	RTOS_ERR tskErr;

	OSMutexCreate(&railgun_mutex, "railgun Mutex", &err);

	OSTaskCreate(&railgunTCB,		  /* Pointer to the task's TCB.  */
				 "railgun Task.",	  /* Name to help debugging.     */
				 &railgun_task,		  /* Pointer to the task's code. */
				 DEF_NULL,			  /* Pointer to task's argument. */
				 NORMAL_PRIORITY,	  /* Task's priority.            */
				 &railgunSTK[0],	  /* Pointer to base of stack.   */
				 (STACK_SIZES / 10u), /* Stack limit, from base.     */
				 STACK_SIZES,		  /* Stack size, in CPU_STK.     */
				 10u,				  /* Messages in task queue.     */
				 120u,				  /* Round-Robin time quanta.    */
				 DEF_NULL,			  /* External TCB data.          */
				 OS_OPT_TASK_STK_CHK, /* Task options.               */
				 &tskErr);
	if (semErr.Code || tskErr.Code)
		EFM_ASSERT(false);
}

void railgun_task(void)
{
	RTOS_ERR err;
	int charger = 0;
	while (1)
	{

		OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &err);

		if (read_button1())
		{
			charger += 1;
		}
		else
		{
			if (shield_state.recharging >= 20)
			{
				if (charger >= 20)
				{
					charger = 20
				}
				OSMutexPend(&railgun_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);

				Bullet.x = platform_data.x;
				Bullet.y = 0;
				Bullet.vx = (charger * 20);
				Bullet.vy = 10;

				shield_state.recharging -= 20;

				railgun_fired = 1;
				OSMutexPost(&railgun_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);
			}
			else
			{
				OSMutexPend(&railgun_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);
				Bullet.x = 0;
				Bullet.y = 0;
				Bullet.vx = 0;
				Bullet.vy = 0;
				railgun_fired = 0;
				OSMutexPost(&railgun_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);
			}
		}
	}
}

// bool shoot_railgun(int idx)
// {
// 	(void)&idx; // Put this into unuse, if I got time to add this.
// 	if (railgun_charges > 0)
// 	{
// 		railgun_charges--;
// 		score++;
// 		railgun_fired += 1;

// 		// RTOS_ERR mutexErr;

// 		// if (mutexErr.Code)
// 		//     EFM_ASSERT(false);
// 		return true;
// 	}
// 	return false;
// }

void shield_task_create(void)
{

	RTOS_ERR err;

	OSTaskCreate(&shieldTCB,		  /* Pointer to the task's TCB.  */
				 "shield Task.",	  /* Name to help debugging.     */
				 &shield_task,		  /* Pointer to the task's code. */
				 DEF_NULL,			  /* Pointer to task's argument. */
				 NORMAL_PRIORITY,	  /* Task's priority.            */
				 &shieldSTK[0],		  /* Pointer to base of stack.   */
				 (STACK_SIZES / 10u), /* Stack limit, from base.     */
				 STACK_SIZES,		  /* Stack size, in CPU_STK.     */
				 10u,				  /* Messages in task queue.     */
				 120u,				  /* Round-Robin time quanta.    */
				 DEF_NULL,			  /* External TCB data.          */
				 OS_OPT_TASK_STK_CHK, /* Task options.               */
				 &err);

	OSMutexCreate(&shield_mutex, "shield Mutex", &err);

	shield_state.active = false;
	shield_state.recharging = 100;
}

void shield_task(void)
{

	RTOS_ERR err;

	while (1)
	{

		OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &err);

		if (read_button0())
		{
			OSMutexPend(&shield_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);
			shield_state.active = true;
			OSMutexPost(&shield_mutex, OS_OPT_POST_NONE, &mutexErr);
		}
		else
		{
			OSMutexPend(&shield_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);
			shield_state.recharging += 10;
			OSMutexPost(&shield_mutex, OS_OPT_POST_NONE, &mutexErr);
		}
	}
}

bool read_button0(void)
{
	if (!GPIO_PinInGet(BUTTON0_port, BUTTON0_pin))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool read_button1(void)
{
	if (!GPIO_PinInGet(BUTTON1_port, BUTTON1_pin))
	{
		return true;
	}
	else
	{
		return false;
	}
}
// void GPIO_EVEN_IRQHandler(void) {
//	// reset the flag
//	uint32_t int_flag;
//	int_flag = (GPIO->IF) & (GPIO->IEN);
//	GPIO->IFC = int_flag;
//
//	// TODO
//
//
//
// }
//
// void GPIO_ODD_IRQHandler(void) {
//	// reset the flag
//	uint32_t int_flag;
//	int_flag = (GPIO->IF) & (GPIO->IEN);
//	GPIO->IFC = int_flag;
//
//	// TODO
// }
