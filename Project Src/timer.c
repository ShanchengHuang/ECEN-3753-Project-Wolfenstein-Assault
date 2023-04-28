///*
// * timer.c
// *
// *  Created on: 7 Apr 2023
// *      Author: shancheng
// */
//
//#include "timer.h"
//
//// need to make a timer task here
//
//void Timer_Task_Create() {
//	// TODO
//}
//
//static void AppTaskStart(void *p_arg) {
//	OS_ERR os_err;
//
//	(void) p_arg; // Unused argument
//
//	// Initialize the system tick
//	BSP_Tick_Init();
//
//	// Create the LED timer
//	OSTmrCreate((OS_TMR*) &LEDTimer, (CPU_CHAR*) "LED Timer",
//			(OS_TICK) OSCfg_TickRate_Hz, // 1-second period
//			(OS_TICK) OSCfg_TickRate_Hz, // 1-second period
//			(OS_OPT) OS_OPT_TMR_PERIODIC, (OS_TMR_CALLBACK_PTR) TimerCallback,
//			(void*) 0, (OS_ERR*) &os_err);
//
//	// Start the LED timer
//	OSTmrStart(&LEDTimer, &os_err);
//
//	// Main loop (does nothing in this example)
//	while (DEF_TRUE) {
//		OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &os_err);
//	}
//}
//
//static void TimerCallback(void *p_arg) {
//	(void) p_arg; // Unused argument
//
//	//TODO
//	// Unit test Here
//}
//
