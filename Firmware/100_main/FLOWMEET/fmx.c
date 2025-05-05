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

#define QUEUE_EVENT_SIZE (4 * 1) // Mantener múltiplo de 4, solo porque lo via asi, no se si es necesario.
#define STACK_SIZE (1024 * 16)
#define THREAD_PRIORITY_10	10
#define THRESHOLD_10		10
#define SLICE_0				0

#define TIMER_BACKLIGHT_INITI   1000 // ms * 10 de backlight encendido al iniciar, el computador.
#define TIMER_BACKLIGHT_GUI     500  // ms * 10 de backlight encendido al interactuar con el teclado.
#define TIMER_EXTI_DEBUNCE      100   // ms * 10 interrupciones des-habilitada para evitar rebote.

#define FMX_DEBUG_LOCAL // Y esto para que sirve???

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

uint16_t rate_tick_old = 0;
uint16_t rate_tick_new = 0;     // Ultima captura del LPTIM3.
uint16_t rate_tick_delta;   // Período medido en n pulsos, el LPTIM3 mide ticks de clock 32769hz

uint16_t rate_pulse_old = 0;
uint16_t rate_pulse_new = 0;
uint16_t rate_pulse_delta = 0; // Cantidad de pulsos entre captura, los n pulsos medidos en rate_tick_delta.

uint16_t vol_pulse_old = 0;
uint16_t vol_pulse_new = 0;
uint16_t vol_pulse_delta;     // Pulsos que se traducirían a volumen.

// Thread del RTOS, responsable de contabilizar los pulsos acumulados.

// Grupo de eventos del RTOS, responsable de atender al teclado y otros eventos.
TX_QUEUE queue_cb_event;	// control block de eventos, teclado y otros eventos.
char queue_name_event[] = "QUEUE EVENT";
uint32_t queue_storage_event[QUEUE_EVENT_SIZE];

// Semáforo, detiene el caulculo de caudal y volumen si no ingresan pulos.
TX_SEMAPHORE sem_cb_pulse_wakeup;
char sem_name_pulse_wakeup[] = "SEM PULSE WAKEUP";

// Thread del RTOS responsable de actualizar los valores de caudal y volumen
TX_THREAD thread_cb_pulse_update;
char thread_name_pulse_update[] = "THREAD PULSE UPDATE";

// Thread del RTOS, responsable de navegar EL menu de usuario y el menu de configuración.
TX_THREAD thread_cb_main;
char thread_name_main[] = "THREAD MAIN";

// Timer del RTOS, detecta teclas presionada por 3 segundos.
TX_TIMER timer_cb_key_three_seconds; // control block del timer, manejo de tecla presionada 3 segundos.
char timer_name_key_thee_seconds[] = "TIMER KEY THREE SECONDS";

// Timer del RTOS, apaga backlight del LCD luego de un tiempo de no actividad.
TX_TIMER timer_cb_backlight_off;  //
char timer_name_backlight_off[] = "BACK LIGHT ON";

// Timer del RTOS, quita rebotes de KEY_EXT_1 y KEY_EXT_2.
TX_TIMER timer_cb_debunce;    //
char timer_name_debunce[] = "DEBOUNCE EXTI";

// Timer del RTOS, quita rebotes de KEY_EXT_1 y KEY_EXT_2.
TX_TIMER fmx_event_refresh;    //
char fmx_name_event_refresh[] = "TIMER EVENT REFRESH";

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
void TimerEntryDebunce(ULONG timer_key);

// Private function bodies.

/*
 * @brief función principal.
 */
void ThreadEntryMain(ULONG thread_input)
{
  uint32_t received_event;
  uint8_t menu = 0;
  uint8_t menu_change;
  UINT tx_status;

  /*
   * Enciendo el backlight LCD.
   * Este es el único punto donde se puede encender el backlight de manera manual.
   * En cualquier otro punto del firmware usar FMX_LcdBackLightOn(); esto asegura que se apague.
   */
  HAL_GPIO_WritePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_RESET);
  tx_timer_activate(&timer_cb_backlight_off);

  // Limpio eventos.
  do
  {
    tx_status = tx_queue_receive(&queue_cb_event, &received_event, TX_NO_WAIT);
  } while (tx_status != TX_QUEUE_EMPTY);

  for (;;)
  {
    switch (menu)
    {
    case 0:
      menu_change = FM_USER_MenuNav(received_event);
      if (menu_change)
      {
        menu_change = 0;
        menu = 1;
        FMX_RefreshEventTrue();
      }
      break;
    case 1:
      menu_change = FM_SETUP_MenuNav(received_event);
      if (menu_change)
      {
        menu_change = 0;
        menu = 0;
        FMX_RefreshEventTrue();
      }
      break;
    default:
      break;
    }

    // Refrescar lo que se ve en pantalla.
    FM_LCD_LL_Refresh();

    // Se detiene hasta recibir un evento.
    // Se deberían evaluar todos los retornos posibles de tx_status.
    do
    {
      /*
       * En teoría aquí debería esperar, para siempre, hasta que algo pase. Si bien esto es posible,
       * no se puede con el esquema actual. El RTOS necesita un tiempo de expiración, al menos uno,
       * para poder traducir el idle time a un tiempo valido para ingresar a stop mode.
       * Si todos los procesos pasan a idle con TX_WAIT_FOREVER, el calculo que hace el RTOS "falla",
       * se saldrá del stop mode justo después de ingresar.
       * El timer 1 programado con un prescaler de 16 y alimentado con una frecuencia de 32768Hz, tiene
       * una capacidad máxima de 32 segundo. En conclusion no se puede ir a dormir por mas de 32 segundos.
       * Ir a dormir por mas de 5 segundos, o quizás 3 segundos no aporta mucho a la duración de la batería
       * Salir de stop mode cada 3 o 5 segundos ayuda a detectar alguna inconsistencias durante el desarrollo.
       * Se decide dejar el time out en 5 segundos para lo que resta de la vida util del computador de
       * caudales.
       * Si el computador nunca saliese de stop mode 2 consumiría 33uA.
       * Si el computador sale una vez cada 5 segundos consume 42uA
       * Si el computador sale una vez cada 3 segundos consume 45uA
       * Si el computador sale una vez cada 1 segundos consume 63uA
       *
       *
       * Nota, las mediciones en este caso se hicieron con el ST-LKINK desconectado, si se conecta el ST-LINK
       * hay un drenaje de corriente desde ST-LINK hacia el computador, y el consumo disminuye 5uA
       * aproximadamente. Aunque es algo considerable, desconectar y conectar el ST-LINK para cada medición no
       * justifica la operatoria. En otros puntos, donde se menciona consumo, puede que se dejara el ST-LINK
       * conectado, y que el valor corresponda a la lectura directa a esta menos estos 5uA, es decir si no
       * menciona que se desconecto el ST-LINK no sirve para calculo fino, en muchos casos no se necesita.
       */
      tx_status = tx_queue_receive(&queue_cb_event, &received_event, 500);
    } while (tx_status == TX_QUEUE_EMPTY);

    if (tx_status != TX_SUCCESS)
    {
      received_event = FMX_EVENT_EMPTY;
    }

    // Si es evento de interfaz de usuario iluminar el LCD
    if ((received_event > FMX_EVENT_REFRESH) && (received_event <= FMX_EVENT_KEY_EXT_2))
    {
      FMX_LcdBackLightOn();
    }
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
  static uint8_t blink = 1;

  for (;;)
  {

    // Pulsos del sensor primario capturados en el ultimo intervalo.
    // El caudal se calculara en función de cuanto pulsos ingresan en aproximadamente 1 segundo.
    rate_pulse_old = rate_pulse_new;
    rate_pulse_new = lptim4_counter;
    rate_pulse_delta = (rate_pulse_new - rate_pulse_old);

    rate_tick_old = rate_tick_new;
    rate_tick_new = lptim3_capture;
    rate_tick_delta = (rate_tick_new - rate_tick_old);

    vol_pulse_old = vol_pulse_new;
    vol_pulse_new = LPTIM4->CNT;
    vol_pulse_delta = (vol_pulse_new - vol_pulse_old);

    /*
     * En teoría, aquí debería esperarse indefinidamente hasta que ocurra algún evento.
     * Aunque esto es técnicamente posible, no se puede implementar con el esquema actual.
     * El RTOS requiere al menos un tiempo de expiración para poder traducir el tiempo en idle
     * a un valor válido que permita entrar en stop mode.
     *
     * Si todos los procesos entran en idle usando TX_WAIT_FOREVER, el cálculo interno del RTOS falla,
     * provocando que el sistema salga del stop mode inmediatamente después de haber ingresado.
     *
     * El Timer 1, configurado con un prescaler de 16 y una frecuencia base de 32.768 Hz, tiene
     * una capacidad máxima de conteo de 32 segundos. Por lo tanto, no es posible dormir más de 32 segundos.
     *
     * Además, existe una limitación práctica: dormir por más de 3 o 5 segundos no aporta beneficios
     * significativos al ahorro energético, y salir periódicamente del stop mode (cada 3 a 5 segundos)
     * permite detectar posibles inconsistencias en la interfaz de usuario.
     *
     * El valor de timeout para el evento esperado se ajustará dinámicamente. A medida que se adquiera
     * mayor confianza en la estabilidad del sistema, este valor podrá incrementarse hasta unos 30 segundos.
     *
     * Referencias de consumo:
     * - En stop mode 2 sin salir: 33 µA
     * - Saliendo cada 3 segundos: 44 µA
     * - Saliendo cada 5 segundos: 40 µA
     *
     * Nota: Estas mediciones se realizaron con el ST-LINK desconectado. Si se conecta el ST-LINK,
     * hay un drenaje de corriente hacia el computador, lo que reduce el consumo en aproximadamente 5 µA.
     * Aunque este efecto es significativo, no se consideró práctico conectar y desconectar el ST-LINK
     * para cada medición. Este puede ser el único lugar donde se mencione dicho efecto. En otros puntos
     * donde se hable de consumo, es probable que los valores estén subestimados por unos 5 µA, ya que
     * la mayoría de las mediciones se realizaron con el ST-LINK conectado.
     */
    __HAL_LPTIM_ENABLE_IT(&hlptim3, LPTIM_IT_CC1);

    FM_FMC_PulseAdd(vol_pulse_delta);
    FM_FMC_CaptureSet(rate_pulse_delta, rate_tick_delta);
    FM_FMC_TtlCalc();
    FM_FMC_AcmCalc();
    FM_FMC_RateCalc();
    blink ^= 1;
    FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_POINT, blink);
    // Evento para refrescar la pantalla.
    FMX_RefreshEventTrue();

    if (rate_pulse_delta)
    {
      tx_thread_sleep(100);
    }
    else
    {
      tx_semaphore_get(&sem_cb_pulse_wakeup, TX_WAIT_FOREVER);
      rate_tick_new = LPTIM3->CNT;
      tx_thread_sleep(25);
    }
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
  fmx_events_t event_new;

  if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin))
  {
    event_new = FMX_EVENT_KEY_DOWN_LONG;
    tx_queue_send(&queue_cb_event, &event_new, TX_NO_WAIT);
    key_down_skip_next = TRUE;
  }

  if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin))
  {
    event_new = FMX_EVENT_KEY_UP_LONG;
    tx_queue_send(&queue_cb_event, &event_new, TX_NO_WAIT);
    key_up_skip_next = TRUE;
  }

  if (HAL_GPIO_ReadPin(KEY_ESC_GPIO_Port, KEY_ESC_Pin))
  {
    event_new = FMX_EVENT_KEY_ESC_LONG;
    tx_queue_send(&queue_cb_event, &event_new, TX_NO_WAIT);
    key_esc_skip_next = TRUE;
  }

  if (HAL_GPIO_ReadPin(KEY_ENTER_GPIO_Port, KEY_ENTER_Pin))
  {
    event_new = FMX_EVENT_KEY_ENTER_LONG;
    tx_queue_send(&queue_cb_event, &event_new, TX_NO_WAIT);
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

/*
 * @brief   Activa EXTI de los pulsadores externos luego de un tiempo de debunce
 * @param   Ver documentación del ThreadX.
 * @retval  Ver documentación del ThreadX.
 */
void TimerEntryDebunce(ULONG timer_input)
{
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  // Es timer one-shot no se necesita ejecutar tx_timer_deactivate().
}

void TimerEntryEventRefresh(ULONG timer_input)
{
  FMX_RefreshEventTrue();
}

// Public function bodies.

/*
 * @breif   Inicia servicio del RTOS
 * @note    Inicia componente del RTOS. Es llamada por App_ThreadX_Init(). Es mi plantilla, la función creada
 *          por CubeMX se la deja sin modificar y se llama a esta. Similar al concepto de dejar main()
 *          intacto y llamar fm_main(). Al crear los componentes del ThreadX si uno da error se detiene la
 *          ejecución del programa. Esta misma técnica no se puede aplicar en otras partes del firmware,
 *          aca si porque facilita encontrar problemas de configuración del ThreadX rapidamente durante el
 *          desarrollo.
 * @param   puntero al pool memory del RTOS
 * @reval   Retorna éxito o si hay problema.
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
  }
  if (tx_thread_create(&thread_cb_pulse_update, thread_name_pulse_update, ThreadEntryPulseUpdate,
      0, pointer, STACK_SIZE, THREAD_PRIORITY_10, THRESHOLD_10, SLICE_0,
      TX_AUTO_START) != TX_SUCCESS)
  {
    __disable_irq();
    FM_DEBUG_LedError(1);
    while (1);
  }

// Reserva memoria para un thread, luego lo crea.
  if (tx_byte_allocate(byte_pool, (VOID**) &pointer, STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    __disable_irq();
    FM_DEBUG_LedError(1);
    while (1);
  }

  if (tx_thread_create(&thread_cb_main, thread_name_main, ThreadEntryMain,
      0, pointer, STACK_SIZE, THREAD_PRIORITY_10, THRESHOLD_10, SLICE_0,
      TX_AUTO_START) != TX_SUCCESS)
  {
    __disable_irq();
    FM_DEBUG_LedError(1);
    while (1);
  }

// Crea cola para eventos.
  if (tx_queue_create(&queue_cb_event, queue_name_event, 1, queue_storage_event,
      sizeof(queue_storage_event)) != TX_SUCCESS)
  {
    FM_DEBUG_LedError(1);
    while (1);
  }

  // Crea cola la pulse wakeup.
  if (tx_semaphore_create(&sem_cb_pulse_wakeup, sem_name_pulse_wakeup, 0) != TX_SUCCESS)
  {
    FM_DEBUG_LedError(1);
    while (1);
  }

// Crea timer para controlar segunda función de las teclas, cuando se mantienen apretadas.
  if (tx_timer_create(&timer_cb_key_three_seconds, timer_name_key_thee_seconds, TimerEntryKeyThreeSeconds,
      0x1234, 300, 100, TX_NO_ACTIVATE) != TX_SUCCESS)
  {
    FM_DEBUG_LedError(1);
    return TX_TIMER_ERROR;
    while (1);
  }

// Crea timer para el apagado automático del backlight del LCD.
  if (tx_timer_create(&timer_cb_backlight_off, timer_name_backlight_off, TimerEntryBackLightOff, 0x1234,
      TIMER_BACKLIGHT_INITI, 0, TX_AUTO_ACTIVATE) != TX_SUCCESS)
  {
    FM_DEBUG_LedError(1);
    return TX_TIMER_ERROR;
    while (1);
  }

// Crea timer para quitar los rebores de los botones externos.
  if (tx_timer_create(&timer_cb_debunce, timer_name_debunce, TimerEntryDebunce, 0x1234,
      TIMER_EXTI_DEBUNCE, 0, TX_AUTO_ACTIVATE) != TX_SUCCESS)
  {
    FM_DEBUG_LedError(1);
    return TX_TIMER_ERROR;
    while (1);
  }

  // Crea timer para quitar los rebores de los botones externos.
  if (tx_timer_create(&fmx_event_refresh, fmx_name_event_refresh, TimerEntryEventRefresh, 0x1234,
      100, 100, TX_NO_ACTIVATE) != TX_SUCCESS)
  {
    FM_DEBUG_LedError(1);
    return TX_TIMER_ERROR;
    while (1);
  }

  return ret;
}

/*
 * @brief   Enciende el backlight
 * @Note    Esta es la única función que se debería usar para encender el backlight.
 * @retval
 */
void FMX_LcdBackLightOn()
{
  HAL_GPIO_WritePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_RESET);
  tx_timer_deactivate(&timer_cb_backlight_off);
  tx_timer_change(&timer_cb_backlight_off, TIMER_BACKLIGHT_GUI, TIMER_BACKLIGHT_GUI);
  tx_timer_activate(&timer_cb_backlight_off);
}

/*
 * @brief   Si la cola esta vacía agrega un evento de refresco,
 * @Note    Esta es la única función que se debería usar para encender el backlight.
 * @retval
 */
void FMX_RefreshEventTrue()
{
  ULONG event_new;

  if (queue_cb_event.tx_queue_enqueued == 0)
  {
    event_new = FMX_EVENT_REFRESH;
    tx_queue_send(&queue_cb_event, &event_new, TX_NO_WAIT);
  }
}

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
  uint32_t event_new;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);

// Se soltó la tecla, detengo el timer que detecta si fue presionada por 3 segundos
  tx_timer_deactivate(&timer_cb_key_three_seconds);

  switch (GPIO_Pin)
  {
  case KEY_ENTER_Pin:
    if (key_enter_skip_next)
    {
      key_enter_skip_next = FALSE;
    }
    else
    {
      event_new = FMX_EVENT_KEY_ENTER;
      if (tx_queue_send(&queue_cb_event, &event_new, TX_NO_WAIT) != TX_SUCCESS)
      {
        FM_DEBUG_LedError(1);
      }
    }
    break;
  case KEY_DOWN_Pin:
    if (key_down_skip_next)
    {
      key_down_skip_next = FALSE;
    }
    else
    {
      event_new = FMX_EVENT_KEY_DOWN;
      if (tx_queue_send(&queue_cb_event, &event_new, TX_NO_WAIT) != TX_SUCCESS)
      {
        FM_DEBUG_LedError(1);
      }
    }
    break;
  case KEY_ESC_Pin:
    if (key_esc_skip_next)
    {
      key_esc_skip_next = FALSE;
    }
    else
    {
      event_new = FMX_EVENT_KEY_ESC;
      if (tx_queue_send(&queue_cb_event, &event_new, TX_NO_WAIT) != TX_SUCCESS)
      {
        FM_DEBUG_LedError(1);
      }
    }
    break;
  case KEY_UP_Pin:
    if (key_up_skip_next)
    {
      key_up_skip_next = FALSE;
    }
    else
    {
      event_new = FMX_EVENT_KEY_UP;
      if (tx_queue_send(&queue_cb_event, &event_new, TX_NO_WAIT) != TX_SUCCESS)
      {
        FM_DEBUG_LedError(1);
      }
    }
    break;
  case KEY_EXT_1_Pin:
    HAL_NVIC_DisableIRQ(EXTI3_IRQn);
    event_new = FMX_EVENT_KEY_EXT_1;
    if (tx_queue_send(&queue_cb_event, &event_new, TX_NO_WAIT) != TX_SUCCESS)
    {
      FM_DEBUG_LedError(1);
    }
    tx_timer_change(&timer_cb_debunce, TIMER_EXTI_DEBUNCE, 0);
    tx_timer_activate(&timer_cb_debunce);
    break;
  case KEY_EXT_2_Pin:
    HAL_NVIC_DisableIRQ(EXTI4_IRQn);
    event_new = FMX_EVENT_KEY_EXT_2;
    if (tx_queue_send(&queue_cb_event, &event_new, TX_NO_WAIT) != TX_SUCCESS)
    {
      FM_DEBUG_LedError(1);
    }
    tx_timer_change(&timer_cb_debunce, TIMER_EXTI_DEBUNCE, 0);
    tx_timer_activate(&timer_cb_debunce);
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
   * Antes de activar un timer para que tenga x mili-seg frescos se debe usar tx_timer_change.
   * Para la doble función de la tecla primero se espera 3 segundos, pero esta funcion luego repite cada
   * un segundo si se mantiene presionada la tecla.
   */
  tx_timer_change(&timer_cb_key_three_seconds, 250, 80);
  tx_timer_activate(&timer_cb_key_three_seconds);
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

  if (sem_cb_pulse_wakeup.tx_semaphore_count == 0)
  {
    tx_semaphore_put(&sem_cb_pulse_wakeup);
  }

  __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_CC1);
}

/*** end of file ***/

