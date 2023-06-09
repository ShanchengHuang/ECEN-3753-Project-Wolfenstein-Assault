// Configuration Data Structure

#ifndef _CONSTANT_H_
#define _CONSTANT_H_

#include <stdint.h>

#define STACK_SIZES 256u

#define NORMAL_PRIORITY 21
#define BELOW_NORMAL_PRIORITY (NORMAL_PRIORITY + 1)
#define ABOVE_NORMAL_PRIORITY (NORMAL_PRIORITY - 1)

#define SCREEN_PIXELS 128u
#define SCREEN_MM 23.04
#define CANYON_SIZE_PIXELS 118
#define CANYON_SIZE_MM (CANYON_SIZE_PIXELS * SCREEN_MM / SCREEN_PIXELS)
#define CANYON_START ((SCREEN_PIXELS - CANYON_SIZE_PIXELS) / 2)
#define CANYON_END (SCREEN_PIXELS - CANYON_START)

extern int win_time;

// Declaration of the external ConfigurationData variable
// extern GameConfig ConfigurationData;

//typedef struct
//{
//	uint32_t data_structure_version;
//	uint32_t tau_physics;
//	uint32_t tau_display;
//	uint32_t tau_slider;
//	uint32_t canyon_size;
//	struct
//	{
//		uint32_t castle_height;
//		uint32_t foundation_hits_required;
//		uint32_t foundation_depth;
//	} wolfenstein;
//	struct
//	{
//		uint32_t limiting_method;
//		uint32_t display_diameter;
//		union
//		{
//			uint32_t tau_throw;
//			uint32_t max_num_in_flight;
//		};
//	} satchel_charges;
//	struct
//	{
//		uint32_t max_force;
//		uint32_t mass;
//		uint32_t length;
//		uint32_t max_platform_speed;
//	} platform;
//	struct
//	{
//		uint32_t effective_range;
//		uint32_t activation_energy;
//	} shield;
//	struct
//	{
//		uint32_t elevation_angle;
//		uint32_t shot_mass;
//		uint32_t shot_display_diameter;
//	} railgun;
//	struct
//	{
//		uint32_t energy_storage;
//		uint32_t power;
//	} generator;
//} GameConfig;

//GameConfig ConfigurationData = {
//	.data_structure_version = 1,
//	.tau_physics = 50,
//	.tau_display = 150,
//	.tau_slider = 100,
//	.canyon_size = 100000,
//	.wolfenstein = {
//		.castle_height = 5000,
//		.foundation_hits_required = 2,
//		.foundation_depth = 5000,
//	},
//	.satchel_charges = {
//		.limiting_method = 0, .display_diameter = 10, .tau_throw = 0, // Union with MaxNumInFlight
//	},
//	.platform = {
//		.max_force = 20000000,
//		.mass = 100,
//		.length = 10000,
//		.max_platform_speed = 50000,
//	},
//	.shield = {
//		.effective_range = 15000,
//		.activation_energy = 30000,
//	},
//	.railgun = {
//		.elevation_angle = 800,
//		.shot_mass = 50,
//		.shot_display_diameter = 5,
//	},
//	.generator = {
//		.energy_storage = 50000,
//		.power = 20000,
//	},
//};

// typedef struct
//{
//     uint8_t button_id;    // Button ID (0 for Button0, 1 for Button1, etc.)
//     uint8_t button_state; // Button state (0 for released, 1 for pressed)
// } button_event;

// typedef struct
//{
//     button_event buttonEvent; // Button event data
//     int32_t capSenseForce;    // CapSense force value
//     float xPos;               // Platform x position
//     float yPos;               // Platform y position
//     float xVel;               // Platform x velocity
//     float yVel;               // Platform y velocity
// } SharedData;
//
//// Declare a global instance of the shared data structure
// SharedData sharedData;

#endif /* _CONSTANT_H_ */
