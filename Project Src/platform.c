#include "platform.h"

#define PLATFORM_PERIOD 1

int PLATFORM_BOUNCE_ENABLED;

//int MAX_SPEED = ConfigurationData.platform.max_platform_speed; // Update this line
int MAX_SPEED = 150; // Update this line

struct PlatData platform_data;

void platform_task_create(void) {

	RTOS_ERR err;
	RTOS_ERR mutexErr;

	// Create the platform task with the specified parameters
	OSTaskCreate(&platformTCB, /* Pointer to the task's TCB.  */
	"platform Task.", /* Name to help debugging.     */
	&platform_task, /* Pointer to the task's code. */
	DEF_NULL, /* Pointer to task's argument. */
	(ABOVE_NORMAL_PRIORITY - 1), /* Task's priority.            */
	&platformSTK[0], /* Pointer to base of stack.   */
	(STACK_SIZES / 10u), /* Stack limit, from base.     */
	STACK_SIZES, /* Stack size, in CPU_STK.     */
	10u, /* Messages in task queue.     */
	120u, /* Round-Robin time quanta.    */
	DEF_NULL, /* External TCB data.          */
	OS_OPT_TASK_STK_CHK, /* Task options.               */
	&err);

	// Create a mutex for the platform task
	OSMutexCreate(&platform_mutex, "platform_mutex", &mutexErr);
	// Check for errors in semaphore, timer, task, and mutex creation

	if (err.Code)
		EFM_ASSERT(false);

	// Initialize platform bounce flag and platform data variables

	PLATFORM_BOUNCE_ENABLED = false;
	platform_data.ax = 0;
	platform_data.vx = 0;
	platform_data.x = SCREEN_PIXELS / 2;
}

void platform_task(void) {
	RTOS_ERR mutexErr;
	RTOS_ERR err;
	float delta_t = PLATFORM_PERIOD / 1000.0;

	while (1) {

		// Delate for awakened periodically
		OSTimeDlyHMSM(0, 0, 0, 150, OS_OPT_TIME_HMSM_STRICT, &err);

		CAPSENSE_Sense();
		int pressed = -1;
		for (int i = 0; i < 4; i++) {
			if (CAPSENSE_getPressed(i)) {
				if (pressed == -1) {
					pressed = i;
				} else {
					pressed = -1;
					break;
				}
			}
		}
		OSMutexPend(&platform_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);
		if (mutexErr.Code)
			EFM_ASSERT(false);

		switch (pressed) {
		case 0:
			platform_data.ax = -24;
			break;
		case 1:
			platform_data.ax = -12;
			break;
		case 2:
			platform_data.ax = 12;
			break;
		case 3:
			platform_data.ax = 24;
			break;
		default:
			platform_data.ax = 0;
			break;
		}

		if (railgun_fired > 0) {
			// Apply railgun physics
			float railgun_impact_force = 2000.0;
			float railgun_distance = sqrt(
					pow((platform_data.x - shotX), 2)
							+ pow((128 - 5 - shotY), 2));
			float railgun_force_on_platform = railgun_impact_force
					/ pow(railgun_distance, 2);
			platform_data.ax += railgun_force_on_platform
					* ((platform_data.x - shotX) / railgun_distance);

			railgun_fired--;
		}

		// Update platform velocity and position
		platform_data.vx += platform_data.ax * delta_t;
		platform_data.x += platform_data.vx * delta_t;

		// Limit platform speed
		if (platform_data.vx > MAX_SPEED) {
			platform_data.vx = MAX_SPEED;
		} else if (platform_data.vx < -MAX_SPEED) {
			platform_data.vx = -MAX_SPEED;
		}

		// Constrain platform to screen
		if (platform_data.x < 0) {
			platform_data.x = 0;
			platform_data.vx = 0;
		} else if (platform_data.x > SCREEN_PIXELS - PLATFORM_WIDTH) {
			platform_data.x = SCREEN_PIXELS - PLATFORM_WIDTH;
			platform_data.vx = 0;
		}

		OSMutexPost(&platform_mutex, OS_OPT_POST_NONE, &mutexErr);

		if (mutexErr.Code)
			EFM_ASSERT(false);
	}
}
