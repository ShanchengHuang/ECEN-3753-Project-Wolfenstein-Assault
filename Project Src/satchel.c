#include "satchel.h"
#include "em_assert.h"
#include "stdlib.h"

OS_MUTEX satchel_mutex;
struct SatchelData Satchels;

void generate_satchel() {

	Satchels.x = 30;
	Satchels.y = 28+15;
	Satchels.exploded = 0;
	Satchels.vx = (rand() % (20 - 30 + 1)) + 30;
	Satchels.vy = -((rand() % (20 - 30 + 1)) + 30);
}

void satchel_init() {
	RTOS_ERR mutexErr;
	OSMutexCreate(&satchel_mutex, "satchel_mutex", &mutexErr);

	if (mutexErr.Code)
		EFM_ASSERT(false);

	generate_satchel();

}
