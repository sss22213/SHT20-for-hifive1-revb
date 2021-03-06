/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */



/******************************************************************************
 *
 * main() creates one queue, and two tasks.  It then starts the
 * scheduler.
 *
 * The Queue Send Task:
 * The queue send task is implemented by the prvQueueSendTask() function in
 * this file.  prvQueueSendTask() sits in a loop that causes it to repeatedly
 * block for 1000 milliseconds, before sending the value 100 to the queue that
 * was created within main().  Once the value is sent, the task loops
 * back around to block for another 1000 milliseconds...and so on.
 *
 * The Queue Receive Task:
 * The queue receive task is implemented by the prvQueueReceiveTask() function
 * in this file.  prvQueueReceiveTask() sits in a loop where it repeatedly
 * blocks on attempts to read data from the queue that was created within
 * blinky().  When data is received, the task checks the value of the
 * data, and if the value equals the expected 100, writes 'Blink' to the UART
 * (the UART is used in place of the LED to allow easy execution in QEMU).  The
 * 'block time' parameter passed to the queue receive function specifies that
 * the task should be held in the Blocked state indefinitely to wait for data to
 * be available on the queue.  The queue receive task will only leave the
 * Blocked state when the queue send task writes to the queue.  As the queue
 * send task writes to the queue every 1000 milliseconds, the queue receive
 * task leaves the Blocked state every 1000 milliseconds, and therefore toggles
 * the LED every 1 second.
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Include semaphore and mutex function */
#include "semphr.h"

/* Freedom metal includes. */
#include <metal/machine.h>
#include <metal/machine/platform.h>

#include <metal/lock.h>
#include <metal/uart.h>
#include <metal/interrupt.h>
#include <metal/clock.h>
#include <metal/led.h>

extern struct metal_led *led0_red, *led0_green, *led0_blue;

/* Priorities used by the tasks. */
#if( portUSING_MPU_WRAPPERS == 1 )
/* Adding portPRIVILEGE_BIT on task priority permit to this task to be executed into privilleged mode
 * if not, the task will not have access to the global variable. if you need to have access to global 
 * variable, you need to define region and change the function to call to create a task. in order to
 * avoid confusion, it is made in a different example (see example-freertos-blinky-pmp)
 */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( ( tskIDLE_PRIORITY + 2 ) | portPRIVILEGE_BIT )
#define	mainQUEUE_SEND_TASK_PRIORITY		( ( tskIDLE_PRIORITY + 1 ) | portPRIVILEGE_BIT )
#else
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )
#endif

/* The 1s value is converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainQUEUE_TICK_COUNT_FOR_1S			pdMS_TO_TICKS( WAIT_MS )

/* The maximum number items the queue can hold.  The priority of the receiving
task is above the priority of the sending task, so the receiving task will
preempt the sending task and remove the queue items each time the sending task
writes to the queue.  Therefore the queue will never have more than one item in
it at any time, and even with a queue length of 1, the sending task will never
find the queue full. */
#define mainQUEUE_LENGTH					( 1 )
/*-----------------------------------------------------------*/

/*
 * Functions:
 * 		- prvSetupHardware: Setup Hardware according CPU and Board.
 */
static void prvSetupHardware( void );

/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvReadSensor( void *pvParameters );
// static void prvQueueSendTask( void *pvParameters );
/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/* The mutex used by task */
SemaphoreHandle_t xSemaphore_sht20;

struct metal_cpu *cpu0;
struct metal_interrupt *cpu_intr, *tmr_intr;
struct metal_led *led0_red, *led0_green, *led0_blue;

#define LED_ERROR ((led0_red == NULL) || (led0_green == NULL) || (led0_blue == NULL))
/*-----------------------------------------------------------*/

int main( void )
{
	/* Configure hardware */
	prvSetupHardware();

	/* Configure mutex flag */
	xSemaphore_sht20 = xSemaphoreCreateBinary();

	/* Create task for reading sensor */
	xTaskCreate( prvReadSensor,			
				"read_sht20",
				configMINIMAL_STACK_SIZE,
				NULL,
				mainQUEUE_RECEIVE_TASK_PRIORITY,
				NULL );
	
	/* Create task for printing temperature and humidity
	xTaskCreate( prvQueueReceiveTask,			
				"Rx",
				configMINIMAL_STACK_SIZE,
				NULL,
				mainQUEUE_RECEIVE_TASK_PRIORITY,
				NULL );
	*/

	/* Task start*/
	vTaskStartScheduler();
}
/*-----------------------------------------------------------*/

/**
 * Read temperature and humidity from sensor.
 * Tempature is float type, and range from 0 to 40.
 * Humidity is float type, and range from 0 to 100.
 */ 
static void prvReadSensor( void *pvParameters )
{
	while(1)
	{
		/* Avoided use repeatly by create mutex */
        if( xSemaphoreTake( xSemaphore_sht20, ( TickType_t ) 10 ) == pdTRUE )
        {
			
			/* Release mutex */
            xSemaphoreGive( xSemaphore_sht20 );
        }
	}
}

static void prvSetupHardware( void )
{
	const char * const pcWarningMsg = "At least one of LEDs is null.\n";

	/* This demo will toggle LEDs colors so we define them here */
	led0_red = metal_led_get_rgb("LD0", "red");
	led0_green = metal_led_get_rgb("LD0", "green");
	led0_blue = metal_led_get_rgb("LD0", "blue");
	if ((led0_red == NULL) || (led0_green == NULL) || (led0_blue == NULL))
	{
		write( STDOUT_FILENO, pcWarningMsg, strlen( pcWarningMsg ) );
	}
	else
	{
		// Enable each LED
		metal_led_enable(led0_red);
		metal_led_enable(led0_green);
		metal_led_enable(led0_blue);

		// All Off
		metal_led_on(led0_red);
		metal_led_on(led0_green);
		metal_led_on(led0_blue);
	}
}
/*-----------------------------------------------------------*/


void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();

	if ( led0_red != NULL )
	{
		// Red light on
		metal_led_off(led0_red);
	}

	_exit(1);
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();

	write( STDOUT_FILENO, "ERROR Stack overflow on func: ", 30 );
	write( STDOUT_FILENO, pcTaskName, strlen( pcTaskName ) );


	if ( led0_red != NULL )
	{
		// Red light on
		metal_led_off(led0_red);
	}

	_exit(1);
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* The tests in the full demo expect some interaction with interrupts. */
}
/*-----------------------------------------------------------*/

void vAssertCalled( void )
{
	taskDISABLE_INTERRUPTS();

	if ( led0_red != NULL )
	{
		// Red light on
		metal_led_off(led0_red);
	}

	_exit(1);
}
