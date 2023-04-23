
#include "stdbool.h"
#include "os.h"
#include "constant.h"

#define SATCH_DIAMETER  ConfigurationData.satchel_charges.display_diameter
// #define SIDE_HIT_KINETIC_REDUCTION 0.75


struct SatchelData {
  double vx;
  double vy;
  double x;
  double y;
  uint8_t exploded;
};


void generate_satchel(int idx);
void satchel_init(void);

