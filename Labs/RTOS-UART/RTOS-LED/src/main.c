/*
* Example RTOS Atmel Studio
*/

#include <asf.h>
#include <string.h>
#include "conf_board.h"

#define TASK_MONITOR_STACK_SIZE            (2048/sizeof(portSTACK_TYPE))
#define TASK_MONITOR_STACK_PRIORITY        (tskIDLE_PRIORITY)

#define TASK_LED1_STACK_SIZE                (1024/sizeof(portSTACK_TYPE))
#define TASK_LED1_STACK_PRIORITY            (tskIDLE_PRIORITY)

#define TASK_LED2_STACK_SIZE								(1024/sizeof(portSTACK_TYPE))
#define TASK_LED2_STACK_PRIORITY						(tskIDLE_PRIORITY)

/**
* LEDs OLED
*/
#define LED1_PIO_ID	    ID_PIOA
#define LED1_PIO         PIOA
#define LED1_PIN		      0
#define LED1_IDX_MASK    (1<<LED1_PIN)

#define LED2_PIO_ID	    ID_PIOC
#define LED2_PIO         PIOC
#define LED2_PIN		      30
#define LED2_IDX_MASK    (1<<LED2_PIN)

#define LED3_PIO_ID	    ID_PIOB
#define LED3_PIO         PIOB
#define LED3_PIN		      2
#define LED3_IDX_MASK    (1<<LED3_PIN)

#define BUT1_PIO            PIOD
#define BUT1_PIO_ID         16
#define BUT1_PIO_IDX        28
#define BUT1_PIO_IDX_MASK   (1u << BUT1_PIO_IDX)

#define BUT2_PIO PIOC
#define BUT2_PIO_ID ID_PIOC
#define BUT2_PIO_IDX 31
#define BUT2_PIO_IDX_MASK (1 << BUT2_PIO_IDX)

#define BUT3_PIO PIOA
#define BUT3_PIO_ID ID_PIOA
#define BUT3_PIO_IDX 19
#define BUT3_PIO_IDX_MASK (1 << BUT3_PIO_IDX)


extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

SemaphoreHandle_t xSemaphore1;
SemaphoreHandle_t xSemaphore2;
SemaphoreHandle_t xSemaphore3;

#define TASK_UART_STACK_SIZE (1024 / sizeof(portSTACK_TYPE))
#define TASK_UART_STACK_PRIORITY (tskIDLE_PRIORITY)
QueueHandle_t xQueueUART;
QueueHandle_t xQueueCommand;

#define TASK_EXECUTE_STACK_SIZE (1024 / sizeof(portSTACK_TYPE))
#define TASK_EXECUTE_STACK_PRIORITY (tskIDLE_PRIORITY)

void USART1_Handler(void){
	uint32_t ret = usart_get_status(USART1);

	BaseType_t xHigherPriorityTaskWoken = pdTRUE;
	char c;

	// Verifica por qual motivo entrou na interrupçcao?
	// RXRDY ou TXRDY

	//  Dados disponível para leitura
	if(ret & US_IER_RXRDY){
		usart_serial_getchar(USART1, &c);
	//	printf("%c", c);
		xQueueSendFromISR(xQueueUART, &c, 0);

		// -  Transmissoa finalizada
		} else if(ret & US_IER_TXRDY){

	}
}

uint32_t usart1_puts(uint8_t *pstring){
	uint32_t i ;

	while(*(pstring + i))
	if(uart_is_tx_empty(USART1))
	usart_serial_putchar(USART1, *(pstring+i++));
}


extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
		signed char *pcTaskName)
{
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	/* If the parameters have been corrupted then inspect pxCurrentTCB to
	 * identify which task has overflowed its stack.
	 */
	for (;;) {
	}
}

extern void vApplicationIdleHook(void)
{
}


extern void vApplicationTickHook(void)
{
}

extern void vApplicationMallocFailedHook(void)
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}

/**
 * \brief This task, when activated, send every ten seconds on debug UART
 * the whole report of free heap and total tasks status
 */
static void task_monitor(void *pvParameters)
{
	static portCHAR szList[256];
	UNUSED(pvParameters);

	for (;;) {
		printf("--- task ## %u\n", (unsigned int)uxTaskGetNumberOfTasks());
		vTaskList((signed portCHAR *)szList);
		printf(szList);
		vTaskDelay(3000);
	}
}


void pin_toggle(Pio *pio, uint32_t mask) {
	if (pio_get_output_data_status(pio, mask)) pio_clear(pio, mask);
	else pio_set(pio, mask);
}

void but1_callback() {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	printf("but_callback \n");
	xSemaphoreGiveFromISR(xSemaphore1, &xHigherPriorityTaskWoken);
	printf("semafaro tx \n");
}

void but2_callback() {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	printf("but2_callback \n");
	xSemaphoreGiveFromISR(xSemaphore2, &xHigherPriorityTaskWoken);
	printf("semafaro2 tx \n");
}

void but3_callback() {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	printf("but3_callback \n");
	xSemaphoreGiveFromISR(xSemaphore3, &xHigherPriorityTaskWoken);
	printf("semafaro3 tx \n");
}


void task_uart(void *pvParameters){
	char c;
	char  s[32];
	int i = 0 ;
	
	xQueueUART = xQueueCreate(32, sizeof(char));
	
	while(1){
		if (xQueueReceive( xQueueUART, &(c), ( TickType_t )  100 / portTICK_PERIOD_MS)) {
			if(c != '\n'){
				s[i]=c;
				i++;
			}else {
				s[i]=0;
					xQueueSend(xQueueCommand, &s, 0);
					i = 0 ;
					printf(s);
					}
				}
			}
}

static void task_execute(void *pvParameters) {
	char msgBuffer[64];

	xQueueCommand = xQueueCreate(5, sizeof(char[64]));

	while (1) {
		if (xQueueReceive(xQueueCommand, &msgBuffer, (TickType_t)500)) {
			printf("comando: %s\n", msgBuffer);
			
			if (strcmp(msgBuffer, "led 1 toggle") == 0) pin_toggle(LED1_PIO, LED1_IDX_MASK);
			
			if (strcmp(msgBuffer, "led 3 toggle") == 0) pin_toggle(LED3_PIO, LED3_IDX_MASK);
		}
	}
}

static void task_led1(void *pvParameters)
{
	pmc_enable_periph_clk(LED1_PIO_ID);
	pio_configure(LED1_PIO, PIO_OUTPUT_0, LED1_IDX_MASK, PIO_DEFAULT);
	pio_set(LED1_PIO, LED1_IDX_MASK);
	/* Block for 2000ms. */
	const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;
	const TickType_t xDelayLed = 50 / portTICK_PERIOD_MS;

	for (;;) {
		pin_toggle(LED1_PIO, LED1_IDX_MASK);
		vTaskDelay(xDelayLed);
	}
}

static void task_led2(void *pvParameters)
{
	/* Block for 2000ms. */
	const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;
	int flag = 0;

	for (;;) {
  pmc_enable_periph_clk(BUT2_PIO_ID);
  pio_configure(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK, PIO_PULLUP);
  pio_handler_set(BUT2_PIO, BUT2_PIO_ID, BUT2_PIO_IDX_MASK, PIO_IT_FALL_EDGE, but2_callback);
  pio_enable_interrupt(BUT2_PIO, BUT2_PIO_IDX_MASK);
  NVIC_EnableIRQ(BUT2_PIO_ID);
  NVIC_SetPriority(BUT2_PIO_ID, 4); // Prioridade 4

  if (xSemaphore2 == NULL)
    printf("falha em criar o semaforo \n");

  for (;;) {
    if( xSemaphoreTake(xSemaphore2, ( TickType_t ) 500) == pdTRUE ){
      	if (flag == 1) {
	      	pio_clear(LED2_PIO, LED2_IDX_MASK);
	      	flag = 0;
      	}
      	
      	else {
	      	pio_set(LED2_PIO, LED2_IDX_MASK);
	      	flag = 1;
      	}
    }
  }
		//vTaskDelay(xDelay);
	}
}


static void USART1_init(void){
	/* Configura USART1 Pinos */
	sysclk_enable_peripheral_clock(ID_PIOB);
	sysclk_enable_peripheral_clock(ID_PIOA);
	pio_set_peripheral(PIOB, PIO_PERIPH_D, PIO_PB4); // RX
	pio_set_peripheral(PIOA, PIO_PERIPH_A, PIO_PA21); // TX
	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;

	/* Configura opcoes USART */
	const sam_usart_opt_t usart_settings = {
		.baudrate       = 115200,
		.char_length    = US_MR_CHRL_8_BIT,
		.parity_type    = US_MR_PAR_NO,
		.stop_bits    = US_MR_NBSTOP_1_BIT    ,
		.channel_mode   = US_MR_CHMODE_NORMAL
	};

	/* Ativa Clock periferico USART0 */
	sysclk_enable_peripheral_clock(ID_USART1);

	stdio_serial_init(CONF_UART, &usart_settings);

	/* Enable the receiver and transmitter. */
	usart_enable_tx(USART1);
	usart_enable_rx(USART1);

	/* map printf to usart */
	ptr_put = (int (*)(void volatile*,char))&usart_serial_putchar;
	ptr_get = (void (*)(void volatile*,char*))&usart_serial_getchar;

	/* ativando interrupcao */
	usart_enable_interrupt(USART1, US_IER_RXRDY);
	NVIC_SetPriority(ID_USART1, 4);
	NVIC_EnableIRQ(ID_USART1);
}



/**
 * \brief Configure the console UART.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#if (defined CONF_UART_CHAR_LENGTH)
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#if (defined CONF_UART_STOP_BITS)
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
	stdio_serial_init(CONF_UART, &uart_serial_options);

	/* Specify that stdout should not be buffered. */
#if defined(__GNUC__)
	setbuf(stdout, NULL);
#else
	
#endif
}


int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Initialize the console uart */
	xQueueUART  = xQueueCreate( 32, sizeof( char ) );
	USART1_init();


	
	/* Output demo information. */
	printf("-- Freertos Example --\n\r");
	printf("-- %s\n\r", BOARD_NAME);
	printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);


// 	/* Create task to monitor processor activity */
// 	if (xTaskCreate(task_monitor, "Monitor", TASK_MONITOR_STACK_SIZE, NULL,
// 			TASK_MONITOR_STACK_PRIORITY, NULL) != pdPASS) {
// 		printf("Failed to create Monitor task\r\n");
// 	}

	/* Create task to make led blink */
	if (xTaskCreate(task_led1, "Led1", TASK_LED1_STACK_SIZE, NULL,
			TASK_LED1_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create test led task\r\n");
	}
	
	/* Create task to make led blink */
	if (xTaskCreate(task_led2, "Led2", TASK_LED2_STACK_SIZE, NULL,
	TASK_LED2_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create test led task\r\n");
	}
	
	/* Create task to make led blink */
	if (xTaskCreate(task_uart, "uart", TASK_UART_STACK_SIZE, NULL,
			TASK_UART_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create test led task\r\n");
	}
	if (xTaskCreate(task_execute, "execute", TASK_EXECUTE_STACK_SIZE, NULL,
			TASK_EXECUTE_STACK_PRIORITY, NULL) != pdPASS)
	printf("Failed to create EXECUTE task\r\n");
	
	
	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
