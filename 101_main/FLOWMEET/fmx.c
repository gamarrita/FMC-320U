/*
 * Modulo con los principales componentes el RTOS.
 *
 *
 * Version 1
 * Autor: Daniel H Sagarra
 * Fecha 08/09/2024
 * Modificaciones: version inicial.
 *
 */

// Includes.
#include "fm_debug.h"
#include "fmx.h"
#include "fm_fmc.h"
#include "fmx_lp.h"
#include "fm_lcd.h"
#include "fm_user.h"
#include "fm_setup.h"

// Defines.

#define TRUE  1
#define FALSE 0

#define STACK_SIZE (1024 * 16)
#define THREAD_PRIORITY_10	10
#define THRESHOLD_10		10
#define SLICE_0				0

#define FMX_DEBUG_LOCAL

// Typedef.

// Const data.

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.
extern LPTIM_HandleTypeDef hlptim1;
extern LPTIM_HandleTypeDef hlptim3;	// Ticks del LSE, cristal externo 32768 Hz, para calculo de caudal.
extern LPTIM_HandleTypeDef hlptim4; // Pulsos de sensor primario, para calculo de caudal.

// Global variables, statics.

// Thread del RTOS, responsable de contabilizar los pulsos acumulados.

// Grupo de eventos del RTOS, responsable de atender al teclado y otros eventos.
TX_EVENT_FLAGS_GROUP event_cb_keypad;	// control block de eventos, teclado y otros eventos.
char event_name_keypad[] = "EVENT KEYPAD";

TX_THREAD thread_cb_pulse_update;
char thread_name_pulse_update[] = "THREAD PULSE UPDATE";

// Thread del RTOS, responsable de navegar los menues de usuario y configuración.
TX_THREAD thread_cb_main;
char thread_name_main[] = "THREAD MAIN";

// Timer del RTOS, detecta teclas presionada por 3 segundos.
TX_TIMER timer_cb_key_three_seconds;	// control block del timer, manejo de tecla presionada 3 segundos.
char timer_name_key_thee_seconds[] = "TIMER KEY THREE SECONDS";

// Timer del RTOS, apaga backlight del LCD luego de un tiempo de no actividad.
TX_TIMER timer_cb_backlight_off;			// control block time
char timer_name_backlight_off[] = "BACK LIGHT ON";

/*
 * El teclado reacciona cuando se libera una tecla, no al presionar, es su función principal.
 * Una acción secundaria se ejecuta al mantener la tecla presionada por 3 segundos.
 * Si tuvo lugar una acción hay que salter al función primaria al liberar la tecla, las siguientes variables
 * controlan este comportamiento.
 */
uint8_t key_up_skip_next = FALSE;
uint8_t key_down_skip_next = FALSE;
uint8_t key_enter_skip_next = FALSE;
uint8_t key_esc_skip_next = FALSE;

// Private function prototypes.
void ThreadEntryPulseUpdate(ULONG thread_input);
void ThreadEntryMain(ULONG thread_input);
void TimerEntryKeyThreeSeconds(ULONG timer_key);
void TimerEntryBackLightOff(ULONG timer_key);

// Private function bodies.

/*
 * @breif 	Inicia servicio del RTOS
 * @note 	Inicia componente del RTOS. Es llamada por App_ThreadX_Init(), una funcion creada or
 * 			CubeMX que crea los servicios de RTOS seteados en el mismo CubeMX. App_Thread_Init()
 * 			tiene una sección de usuario justamente para el motivo que se la usa aqui.
 * @param 	puntero al pool memory del RTOS
 * @reval	Retorna éxito o si hay problema.
 *
 */
UINT FMX_Init(VOID *memory_ptr)
{
	UINT ret = TX_SUCCESS;
	CHAR *pointer;

	TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*) memory_ptr;

	// Reserva memoria para el thread luego lo crea.
	if (tx_byte_allocate(byte_pool, (VOID**) &pointer, STACK_SIZE,
	TX_NO_WAIT) != TX_SUCCESS)
	{
		__disable_irq();
		HAL_GPIO_WritePin(LED_1_ERROR_GPIO_Port, LED_1_ERROR_Pin, GPIO_PIN_SET);
		for (;;)
		{
		}
	}

	if (tx_thread_create(&thread_cb_pulse_update, thread_name_pulse_update, ThreadEntryPulseUpdate,
			0, pointer, STACK_SIZE, THREAD_PRIORITY_10, THRESHOLD_10, SLICE_0,
			TX_AUTO_START) != TX_SUCCESS)
	{
		__disable_irq();
		HAL_GPIO_WritePin(LED_1_ERROR_GPIO_Port, LED_1_ERROR_Pin, GPIO_PIN_SET);
		for (;;)
		{
		}
	}

	// Reserva memoria para un thread, luego lo crea.
	if (tx_byte_allocate(byte_pool, (VOID**) &pointer, STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
	{
		__disable_irq();
		HAL_GPIO_WritePin(LED_1_ERROR_GPIO_Port, LED_1_ERROR_Pin, GPIO_PIN_SET);
		for (;;)
		{
		}
	}

	if (tx_thread_create(&thread_cb_main, thread_name_main, ThreadEntryMain,
			0, pointer, STACK_SIZE, THREAD_PRIORITY_10, THRESHOLD_10, SLICE_0,
			TX_AUTO_START) != TX_SUCCESS)
	{
		__disable_irq();
		HAL_GPIO_WritePin(LED_1_ERROR_GPIO_Port, LED_1_ERROR_Pin, GPIO_PIN_SET);
		for (;;)
		{
		}
	}

	// Crea grupo de eventos
	if (tx_event_flags_create(&event_cb_keypad, event_name_keypad) != TX_SUCCESS)
	{
		return TX_GROUP_ERROR;
	}

	// Crea timer
	if (tx_timer_create(&timer_cb_key_three_seconds, timer_name_key_thee_seconds, TimerEntryKeyThreeSeconds,
			0x1234, 300, 100, TX_NO_ACTIVATE) != TX_SUCCESS)
	{
		return TX_TIMER_ERROR;
	}

	// Crea timer
	if (tx_timer_create(&timer_cb_backlight_off, timer_name_backlight_off, TimerEntryBackLightOff, 0x1234,
			1000, 1000, TX_AUTO_ACTIVATE) != TX_SUCCESS)
	{
		return TX_TIMER_ERROR;
	}

	return ret;
}

/*
 * @brief función principal.
 */
void ThreadEntryMain(ULONG thread_input)
{
	ULONG new_event;
	uint8_t menu = 0;
	uint8_t menu_change;

	HAL_GPIO_WritePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_SET);

	tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);

	for (;;)
	{
		tx_event_flags_get(&event_cb_keypad, FMX_EVENT_GROUP, TX_OR_CLEAR, &new_event, TX_WAIT_FOREVER);

		switch (menu)
		{
			case 0:
				menu_change = FM_USER_MenuNav(new_event);
				if (menu_change)
				{
					menu_change = 0;
					menu = 1;
					tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);
				}
			break;
			case 1:
				menu_change = FM_SETUP_MenuNav(new_event);
				if (menu_change)
				{
					menu_change = 0;
					menu = 0;
					tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);
				}
			break;
			default:
				break;
		}
		FM_LCD_LL_Refresh();
	}
}

/*
 * @brief	Lee los pulsos acumulados del timer 4, actualiza el conteo de pulsos
 * @note
 * @param	  Parametro pasado al crear el servicio
 * @retval			Ninguno.
 *
 */
void ThreadEntryPulseUpdate(ULONG thread_input)
{
	uint16_t pulse_new;
	uint16_t pulse_old;
	uint16_t pulse_delta;

	uint16_t capture_new;
	uint16_t capture_old;
	uint16_t capture_delta;

	HAL_LPTIM_Counter_Start(&hlptim4);
	HAL_LPTIM_IC_Start(&hlptim3, LPTIM_CHANNEL_1);

	pulse_new = LPTIM4->CNT;
	capture_new = LPTIM3->CCR1;

	for (;;)
	{
		pulse_old = pulse_new;
		capture_old = capture_new;

		capture_new = LPTIM3->CCR1;
		pulse_new = LPTIM4->CNT;

		capture_delta = capture_new;
		capture_delta -= capture_old;

		pulse_delta = pulse_new;
		pulse_delta -= pulse_old;

		FM_FMC_PulseAdd(pulse_delta);
		FM_FMC_CaptureSet(pulse_delta, capture_delta);

		FM_FMC_TtlCalc();
		FM_FMC_AcmCalc();
		FM_FMC_RateCalc();

		// El siguiente evento es responsable del refresco de la pantalla.
		tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);

		//
		HAL_GPIO_WritePin(LED_2_ACTIVE_GPIO_Port, LED_2_ACTIVE_Pin, GPIO_PIN_RESET);
		tx_thread_sleep(100);
		HAL_GPIO_WritePin(LED_2_ACTIVE_GPIO_Port, LED_2_ACTIVE_Pin, GPIO_PIN_SET);
	}
}

// Public function bodies.

// Interrupts

/*
 * @brief	Interrupción generada porque se soltó una tecla.
 *
 * @note	La acción de presionar una tecla no es la que ejecuta su función, al momento de soltar la tecla
 * 			es cuando se ejecuta su función.
 *
 * @param	Pin que generó la interrupción.
 *
 * @retval	Ninguno.
 *
 */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{

	/* Prevent unused argument(s) compilation warning */
	UNUSED(GPIO_Pin);

	// Se solto la tecla, detengo el timer que detecta si fue presionada por 3 segundos
	tx_timer_deactivate(&timer_cb_key_three_seconds);

	/*
	 * El backlight del LCD deme permanecer encendido durante 10 segundos luego de soltar la tecla.
	 * Luego de liberar por primera vez la tecla, se puede volver a presionar y solar la misma u otra tecla.
	 * El siguiente algoritmo frenan el timer, lo pone a cero, y lo vuelve a iniciar con frescos
	 * 10 segundos para el apagado.
	 *
	 */
	tx_timer_deactivate(&timer_cb_backlight_off);
	tx_timer_change(&timer_cb_backlight_off, 1000, 1000);
	tx_timer_activate(&timer_cb_backlight_off);

	switch (GPIO_Pin)
	{
		case KEY_ENTER_Pin:
			if (key_enter_skip_next)
			{
				key_enter_skip_next = FALSE;
			}
			else
			{
				tx_event_flags_set(&event_cb_keypad, (uint32_t) FMX_EVENT_KEY_ENTER, TX_OR);
			}
		break;
		case KEY_DOWN_Pin:
			if (key_down_skip_next)
			{
				key_down_skip_next = FALSE;
			}
			else
			{
				tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_KEY_DOWN, TX_OR);
			}
		break;
		case KEY_ESC_Pin:
			if (key_esc_skip_next)
			{
				key_esc_skip_next = FALSE;
			}
			else
			{
				tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_KEY_ESC, TX_OR);
			}
		break;
		case KEY_UP_Pin:
			if (key_up_skip_next)
			{
				key_up_skip_next = FALSE;
			}
			else
			{
				tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_KEY_UP, TX_OR);
			}
		break;
		default:
			HAL_GPIO_WritePin(LED_1_ERROR_GPIO_Port, LED_1_ERROR_Pin, GPIO_PIN_SET);
		break;
	}
}

/*
 * @brief	Interrupción generada porque se presiono una tecla.
 *
 * @note	La acción de presionar una tecla no es la que ejecuta su función, al momento de soltar la tecla
 * 			es cuando se ejecuta su función.
 *
 * @param	Pin que generó la interrupción.
 *
 * @retval	Ninguno.
 *
 */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(GPIO_Pin);

	/*
	 * Antes de activar un timer para que tenga 3 segundos frescos se debe usar tx_timer_change.
	 * Para la doble funcion de la tecla primero se espera 3 segundos, pero esta funcion luego repite cada
	 * un segundo si se mantiene presionada la tecla.
	 */
	tx_timer_change(&timer_cb_key_three_seconds, 300, 100);
	tx_timer_activate(&timer_cb_key_three_seconds);

	/*
	 * Se enciende el backlight del LCD al presionar una tecla.
	 */
	HAL_GPIO_WritePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_RESET);

}

void TimerEntryKeyThreeSeconds(ULONG timer_input)
{
	if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin))
	{
		tx_event_flags_set(&event_cb_keypad, (uint32_t) FMX_EVENT_KEY_DOWN_LONG, TX_OR);
		key_down_skip_next = TRUE;
	}

	if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin))
	{
		tx_event_flags_set(&event_cb_keypad, (uint32_t) FMX_EVENT_KEY_UP_LONG, TX_OR);
		key_up_skip_next = TRUE;
	}

	if (HAL_GPIO_ReadPin(KEY_ESC_GPIO_Port, KEY_ESC_Pin))
	{
		tx_event_flags_set(&event_cb_keypad, (uint32_t) FMX_EVENT_KEY_ESC_LONG, TX_OR);
		key_esc_skip_next = TRUE;
	}

	if (HAL_GPIO_ReadPin(KEY_ENTER_GPIO_Port, KEY_ENTER_Pin))
	{
		tx_event_flags_set(&event_cb_keypad, (uint32_t) FMX_EVENT_KEY_ENTER_LONG, TX_OR);
		key_enter_skip_next = TRUE;
	}

}

void TimerEntryBackLightOff(ULONG timer_input)
{
	tx_timer_deactivate(&timer_cb_backlight_off);
	HAL_GPIO_WritePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_SET);
}

/*** end of file ***/

