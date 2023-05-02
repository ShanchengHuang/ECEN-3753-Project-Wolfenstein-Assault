#include "satchel.h"
#include "em_assert.h"
#include "stdlib.h"

OS_MUTEX satchel_mutex;
struct SatchelData Satchels;

void generate_satchel() {
//	Satchels.x = (rand() % (CANYON_SIZE_PIXELS - (2 * SATCH_DIAMETER)))
//			+ SATCH_DIAMETER + CANYON_START;

	Satchels.x = 30;
	Satchels.y = 28+15;
	Satchels.exploded = 0;
	Satchels.vx = (rand() % 100);
	Satchels.vy = rand() % 30;
}

void satchel_init() {
	RTOS_ERR mutexErr;
	OSMutexCreate(&satchel_mutex, "satchel_mutex", &mutexErr);

	if (mutexErr.Code)
		EFM_ASSERT(false);

	generate_satchel();

}
