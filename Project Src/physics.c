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

void Physics_Task(void *p_arg) {
	/* Use argument. */
	(void) &p_arg;
	RTOS_ERR err;

	while (DEF_TRUE) {
		
    // Update platform force based on CapSense input
    updatePlatformForce();

    // Update platform position
    updatePlatformPosition(deltaTime);

    // Update satchel charges
    updateSatchelCharges(deltaTime);

    // Update rail gun shots
    updateRailGunShots(deltaTime);

    // Check for collisions
    checkCollisions();

    // Delay for TauPhysics milliseconds before next update
    OSTimeDly((OS_TICK)ConfigurationData.TauPhysics, OS_OPT_TIME_DLY, &err);
	}

}
void Physics_Task_Create() {
	RTOS_ERR err;

	OSTaskCreate(&Physics_TaskTCB, /* Pointer to the task's TCB.  */
	"Physics_Task.", /* Name to help debugging.     */
	&Physics_Task, /* Pointer to the task's code. */
	DEF_NULL, /* Pointer to task's argument. */
	Physics_Task_PRIO, /* Task's priority.            */
	&Physics_TaskStk[0], /* Pointer to base of stack.   */
	(Physics_Task_STK_SIZE / 10u), /* Stack limit, from base.     */
	Physics_Task_STK_SIZE, /* Stack size, in CPU_STK.     */
	10u, /* Messages in task queue.     */
	0u, /* Round-Robin time quanta.    */
	DEF_NULL, /* External TCB data.          */
	OS_OPT_TASK_STK_CHK, /* Task options.               */
	&err);

	if (err.Code != RTOS_ERR_NONE) {
		/* Handle error on task creation. */
	}
}


#include "capsense.h"

// ... other code ...

void updatePlatformForce() {
  // Obtain the slider position from the CapSense input
  uint8_t sliderPosition = CAPSENSE_getSliderPosition();

  // Calculate the force based on the slider position
  int32_t force = ConfigurationData.Platform.MaxForce * (sliderPosition - 128) / 128;

  // Apply the force to the platform
  applyForceToPlatform(force);
}

// ... other code ...
