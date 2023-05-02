#ifndef _SATCHEL_H_
#define _SATCHEL_H_


#include "stdbool.h"
#include "os.h"
#include "constant.h"

#define SATCH_DIAMETER  4
// #define SIDE_HIT_KINETIC_REDUCTION 0.75

extern OS_MUTEX satchel_mutex;
extern struct SatchelData Satchels;


struct SatchelData {
  double vx;
  double vy;
  double x;
  double y;
  uint8_t exploded;
};


void generate_satchel();
void satchel_init(void);

#endif /* _SATCHEL_H_ */
