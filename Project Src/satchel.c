#include "satchel.h"
#include "em_assert.h"
#include "stdlib.h"

//uint8_t SATCHEL_COUNT = 1;

void generate_satchel() {
	Satchels.x = (rand() % (CANYON_SIZE_PIXELS - (2 * SATCH_DIAMETER)))
			+ SATCH_DIAMETER + CANYON_START;
	Satchels.y = SATCH_DIAMETER;
	Satchels.exploded = 0;
	Satchels.vx = (rand() % 20) - 10;
	Satchels.vy = rand() % 10;
}

void satchel_init() {
	RTOS_ERR mutexErr;
	OSMutexCreate(&satchel_mutex, "satchel_mutex", &mutexErr);

	if (mutexErr.Code)
		EFM_ASSERT(false);

	generate_satchel();

}
