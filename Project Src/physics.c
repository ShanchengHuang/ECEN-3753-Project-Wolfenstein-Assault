/*
 * physics.c
 *
 *  Created on: 7 Apr 2023
 *      Author: shancheng
 */

#include "physics.h"

void applyForceToPlatform(int32_t force);
void updatePlatformPosition(float deltaTime);
void updateSatchelCharges(float deltaTime);
void updateRailGunShots(float deltaTime);
void checkCollisions();

void Physics_Task(void *p_arg)
{
	/* Use argument. */
	(void)&p_arg;
	RTOS_ERR err;

	while (DEF_TRUE)
	{

		float deltaTime = (float)default_config.tau_physics / 1000.0f;

		
		// Delay for TauPhysics milliseconds before next update
		OSTimeDly((OS_TICK)ConfigurationData.TauPhysics, OS_OPT_TIME_DLY, &err);

		// Lock the PhysicsMutex before accessing shared data
		OSMutexPend(&PhysicsMutex, 0u, OS_OPT_PEND_BLOCKING, 0u, &err);

		// Read the CapSense force value from the shared data
		int32_t capSenseForce = sharedData.capSenseForce;

		// Update platform force based on CapSense input
		updatePlatformForce(capSenseForce);

		// Update platform position
		updatePlatformPosition(deltaTime);

		// Update satchel charges
		updateSatchelCharges(deltaTime);

		// Update rail gun shots
		updateRailGunShots(deltaTime);

		// Check for collisions
		checkCollisions();

		// Update the shared data with the new platform position and other relevant data
		// ... (update sharedData properties here) ...

		// Unlock the PhysicsMutex
		OSMutexPost(&PhysicsMutex, OS_OPT_POST_NONE, &err);
	}
}

void Physics_Task_Create()
{
	RTOS_ERR err;

	OSTaskCreate(&Physics_TaskTCB,				/* Pointer to the task's TCB.  */
				 "Physics_Task.",				/* Name to help debugging.     */
				 &Physics_Task,					/* Pointer to the task's code. */
				 DEF_NULL,						/* Pointer to task's argument. */
				 Physics_Task_PRIO,				/* Task's priority.            */
				 &Physics_TaskStk[0],			/* Pointer to base of stack.   */
				 (Physics_Task_STK_SIZE / 10u), /* Stack limit, from base.     */
				 Physics_Task_STK_SIZE,			/* Stack size, in CPU_STK.     */
				 10u,							/* Messages in task queue.     */
				 0u,							/* Round-Robin time quanta.    */
				 DEF_NULL,						/* External TCB data.          */
				 OS_OPT_TASK_STK_CHK,			/* Task options.               */
				 &err);

	if (err.Code != RTOS_ERR_NONE)
	{
		/* Handle error on task creation. */
	}
}

// ... other code ...

void updatePlatformForce()
{
	// Obtain the slider position from the CapSense input
	uint8_t sliderPosition = CAPSENSE_getSliderPosition();

	// Calculate the force based on the slider position
	int32_t force = ConfigurationData.Platform.MaxForce * (sliderPosition - 128) / 128;

	// Apply the force to the platform
	applyForceToPlatform(force);
}


void railgun_task_create(void) {
  RTOS_ERR semErr;
  RTOS_ERR tskErr;
  OSSemCreate(
      &railgun_semaphore,
      "railgun semaphore",
      0,
      &semErr
  );
  OSTaskCreate(
         &railgunTCB,                /* Pointer to the task's TCB.  */
        "Railgun Task.",                    /* Name to help debugging.     */
        &railgun_task,                   /* Pointer to the task's code. */
         DEF_NULL,                          /* Pointer to task's argument. */
         NORMAL_PRIORITY,             /* Task's priority.            */
        &railgunSTK[0],             /* Pointer to base of stack.   */
        (STACK_SIZES / 10u),  /* Stack limit, from base.     */
         STACK_SIZES,         /* Stack size, in CPU_STK.     */
         10u,                               /* Messages in task queue.     */
         120u,                                /* Round-Robin time quanta.    */
         DEF_NULL,                          /* External TCB data.          */
         OS_OPT_TASK_STK_CHK,               /* Task options.               */
        &tskErr
      );
  if (semErr.Code || tskErr.Code) EFM_ASSERT(false);
}

bool shoot_railgun(int idx) {
  if (railgun_shots > 0) {
    railgun_shots--;
    score++;
    railgun_fired = 3;
    shotX = Platforms[idx].x;
    shotY = Platforms[idx].y;
    RTOS_ERR mutexErr;
    OSMutexPend(&platform_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);
    if (mutexErr.Code) EFM_ASSERT(false);

    generate_platform(idx);

    OSMutexPost(&platform_mutex, OS_OPT_POST_NONE, &mutexErr);
    if (mutexErr.Code) EFM_ASSERT(false);
    return true;
  }
  return false;
}

void railgun_task(void) {
  RTOS_ERR semErr;
  RTOS_ERR mutexErr;
  while (1) {
      OSSemPend(&railgun_semaphore, 0, OS_OPT_PEND_BLOCKING, NULL, &semErr);
      if (semErr.Code) EFM_ASSERT(false);
      //TODO check railgun shot count, if good, decrement and pend platform mutex and respawn it
      int lowestIdx = 0;
      int lowestHeight = 0; //lowest is highest y
      for (int i = 0; i < PLATFORM_COUNT; i++) {
          if (lowestHeight < Platforms[i].y) {
              lowestHeight = Platforms[i].y;
              lowestIdx = i;
          }
      }
      shoot_railgun(lowestIdx);
  }
}

// ... other code ...
