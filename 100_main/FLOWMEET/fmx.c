/*
 * Brief Este modulo es equivalente al main.c, como se usa RTOS, no hay loop infinito, hay apps de ThreadX.
 *
 * Version 1
 * Autor: Daniel H Sagarra
 * Fecha: 10/11/2024
 * Modificaciones:
 * - Se crea la version de firmware 01.01.03, version inicial.
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
extern LPTIM_HandleTypeDef hlptim3; // Pulsos de sensor primario, para calculo de caudal.
extern LPTIM_HandleTypeDef hlptim4; // Pulsos de sensor primario, para calculo de caudal.

// Global variables, statics.
uint16_t lptim3_capture;
uint16_t lptim4_counter;

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
  if (tx_byte_allocate(byte_pool, (VOID**) &pointer, STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    __disable_irq();
    FM_DEBUG_LedError(1);

    for (;;)
    {
    }
  }

  if (tx_thread_create(&thread_cb_pulse_update, thread_name_pulse_update, ThreadEntryPulseUpdate,
      0, pointer, STACK_SIZE, THREAD_PRIORITY_10, THRESHOLD_10, SLICE_0,
      TX_AUTO_START) != TX_SUCCESS)
  {
    __disable_irq();
    FM_DEBUG_LedError(1);
    for (;;)
    {
    }
  }

  // Reserva memoria para un thread, luego lo crea.
  if (tx_byte_allocate(byte_pool, (VOID**) &pointer, STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    __disable_irq();
    FM_DEBUG_LedError(1);
    for (;;)
    {
    }
  }

  if (tx_thread_create(&thread_cb_main, thread_name_main, ThreadEntryMain,
      0, pointer, STACK_SIZE, THREAD_PRIORITY_10, THRESHOLD_10, SLICE_0,
      TX_AUTO_START) != TX_SUCCESS)
  {
    __disable_irq();
    FM_DEBUG_LedError(1);
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
      500, 500, TX_AUTO_ACTIVATE) != TX_SUCCESS)
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
  uint8_t blink_point = 0;
  uint16_t capture_delta;
  uint16_t capture_new = 0;
  uint16_t capture_old = 0;
  uint16_t counter_delta;
  uint16_t counter_new = 0;
  uint16_t counter_old = 0;

  for (;;)
  {

    capture_old = capture_new;
    capture_new = lptim3_capture;
    capture_delta = (capture_new - capture_old);

    counter_old = counter_new;
    counter_new = lptim4_counter;
    counter_delta = (counter_new - counter_old);

    FM_FMC_PulseAdd(counter_delta);

    FM_FMC_CaptureSet(counter_delta, capture_delta);

    FM_FMC_TtlCalc();
    FM_FMC_AcmCalc();
    FM_FMC_RateCalc();

    // Parpadeo del segmento testigo si se reciben pulsos del sensor primario
    if (capture_delta && blink_point)
    {
      FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_POINT, 1);
      blink_point = 0;
    }
    else
    {
      FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_POINT, 0);
      blink_point = 1;
    }

    // Evento para refrescar la pantalla.
    tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);

    // Este thread es el unico que se ejecuta regularmente, si no hay interrupción ira a idle 1 segundo.
    tx_thread_sleep(102); // Se ajusto a 102 para tener 1 segundo aproximadamente.

    /*
     * Los pulsos del sensor primario se usan para capturar el valor del contador del LPTIM 3.
     * LPTIM 3 alimentado por el LSE corre a 32768Hz. Como el modulo de captura no esta funcionando
     * en stop mode, habilito la interrupción, en el proximo flanco se tomara nota del contador
     * del LPTIM 3 y de los pulsos acumulados del sensor primario en el LPTIM 4. Tendremos cantidad
     * de pulsos del sensor primario y la cantidad de pulsos de 32768Hz dentro de estos. Con los
     * datos anteriores se puede calcular la frecuencia con resoluciond de 0.1Hz.
     */
    __HAL_LPTIM_ENABLE_IT(&hlptim3, LPTIM_IT_CC1);
  }
}

/*
 * @brief   Al presionar un botón se inicia este timer, si el botón se suelta el timer se des-habilita
 *          antes de ser ejecutado, pero si se mantiene presiona el timer sigue corriendo y a los tres
 *          segundos se ejecuta esta TimerEntry.
 * @param   Ver Documentación de ThreadX.
 * @retval  Ninguno.
 */
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

/*
 * @brief   Timer por software del ThreadX, apaga el backlight luego de estar encendido 3 segundos.
 * @param   Ver documentación del ThreadX.
 * @retval  Ver documentación del ThreadX.
 */
void TimerEntryBackLightOff(ULONG timer_input)
{
  tx_timer_deactivate(&timer_cb_backlight_off);
  HAL_GPIO_WritePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_SET);
}

// Public function bodies.

// Interrupts

/*
 * @brief	Interrupción generada porque se soltó una tecla.
 * @note	La acción de presionar una tecla no es la que ejecuta su función, al momento de soltar la tecla
 * 			es cuando se ejecuta su función.
 * @param	Pin que generó la interrupción.
 * @retval	Ninguno.
 *
 */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{

  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);

  // Se soltó la tecla, detengo el timer que detecta si fue presionada por 3 segundos
  tx_timer_deactivate(&timer_cb_key_three_seconds);

  /*
   * El backlight del LCD deme permanecer encendido durante 10 segundos luego de soltar la tecla.
   * Luego de liberar por primera vez la tecla, se puede volver a presionar y solar la misma u otra tecla.
   * El siguiente algoritmo frenan el timer, lo pone a cero, y lo vuelve a iniciar con frescos
   * 10 segundos para el apagado.
   *
   */
  tx_timer_deactivate(&timer_cb_backlight_off);
  tx_timer_change(&timer_cb_backlight_off, 500, 500);
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
    FM_DEBUG_LedError(1);
    break;
  }
}

/*
 * @brief	Interrupción generada porque se presiono una tecla.
 * @note	La acción de presionar una tecla no es la que ejecuta su función, al momento de soltar la tecla
 * 			es cuando se ejecuta su función.
 * @param	Pin que generó la interrupción.
 * @retval	Ninguno.
 *
 */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);

  /*
   * Antes de activar un timer para que tenga 3 segundos frescos se debe usar tx_timer_change.
   * Para la doble función de la tecla primero se espera 3 segundos, pero esta funcion luego repite cada
   * un segundo si se mantiene presionada la tecla.
   */
  tx_timer_change(&timer_cb_key_three_seconds, 300, 100);
  tx_timer_activate(&timer_cb_key_three_seconds);

  /*
   * Se enciende el backlight del LCD al presionar una tecla.
   */
  HAL_GPIO_WritePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_RESET);

}

/*
 * @brief
 * @note
 * @param
 * @retval
 *
 */
void HAL_LPTIM_IC_CaptureCallback(LPTIM_HandleTypeDef *hlptim)
{

  lptim3_capture = LPTIM3->CCR1;
  lptim4_counter = LPTIM4->CNT;

  __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_CC1);
}

/*** end of file ***/

