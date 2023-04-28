
#include "stdbool.h"
#include "os.h"
#include "constant.h"

#define SATCH_DIAMETER  4
// #define SIDE_HIT_KINETIC_REDUCTION 0.75

OS_MUTEX sc_mutex;

struct SatchelData {
  double vx;
  double vy;
  double x;
  double y;
  uint8_t exploded;
};


void generate_satchel(int idx);
void satchel_init(void);

