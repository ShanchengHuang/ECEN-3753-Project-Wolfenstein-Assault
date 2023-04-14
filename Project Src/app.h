/*******************************************************************************
 * @file
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef APP_H
#define APP_H

/******************************************************************************
 * Initialize application.
 ******************************************************************************/
void app_init(void);

// For OS
#include <os.h>
#include "em_emu.h"
#include "gpio.h"
#include "capsense.h"
// #include "efm32gg11b.h" //For EFM_ASSERT

// For memcpy()
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

//***********************************************************************************

// global variables

//***********************************************************************************

static volatile bool slider0 = 0;
static volatile bool slider1 = 0;
static volatile bool btn0 = 0;
static volatile bool btn1 = 0;

#define Dly_tick 2

#define SPEEDEDFLAG 0b0001
#define RSET_SPEEDEDFLAG 0b0010
#define DIRECTIONFLAG 0b0100
#define RSET_DIRECTIONFLAG 0b1000

#define UPDATEFLAG1 1
#define UPDATEFLAG2 2

#define BUTTON_FIFO_SIZE 10

// Semaphore, Flag, Mutex
static OS_SEM BTNsemaphore;
static OS_SEM BTNsemaphore;

// Define the event flag group object
static OS_FLAG_GRP UpdateFlag;
static OS_FLAG_GRP AlertFlag;

// Define the Mutex
static OS_MUTEX BTNMutex;
static OS_MUTEX PhysicsMutex;

//***********************************************************************************
// function prototypes
//***********************************************************************************

// OS_TMR CapSenseTimer;

typedef struct
{
	int button_id;	  // Button 0 or Button 1
	int button_state; // 0 for released, 1 for pressed
} ButtonEvent;

typedef struct
{
	ButtonEvent buffer[BUTTON_FIFO_SIZE];
	int head;
	int tail;
	int count;
} ButtonEventFifo;

typedef struct
{
	int speed;
	int increments;
	int decrements;
} SpeedSetpointData;

typedef struct
{
	int current_direction;
	int time_constant;
	int left_turns;
	int right_turns;
} VehicleDirectionData;

void app_init(void);

void LCD_init();

void button_event_fifo_write(ButtonEventFifo *fifo, int button_id,
							 int button_state);

ButtonEvent button_event_fifo_read(ButtonEventFifo *fifo);

void OSFlag_Init(void);

void GPIO_EVEN_IRQHandler(void);
void GPIO_ODD_IRQHandler(void);

void Speed_Setpoint_Task(void *p_arg);
void Speed_Setpoint_Create();

void Vehicle_Direction_Task(void *p_arg);
void Vehicle_Direction_Create();

void Vehicle_Monitor_Task(void *p_arg);
void Vehicle_Monitor_Create();

void LedOutput(void *p_arg);
void LedOutput_Create();

void LCDOutput(void *p_arg);
void LCDOutput_Create();

void read_button0(void);
void read_button1(void);
void read_capsense(void);
int read_capsense2(void);
void write_led(void);

// Added Idle task();
void IdleTask_Create();
void IdleTask(void *p_arg);

void LED_Output_Task_Init(void);

bool TwoButtonsPressed(ButtonEventFifo *fifo);

// Function for the Prject task

// void TimerCallback(OS_TMR *p_tmr, void *p_arg);
// void TimerCallback(void *p_tmr, void *p_arg);
// void EventFlagsCallback(uint8_t buttons);

#endif // APP_H
