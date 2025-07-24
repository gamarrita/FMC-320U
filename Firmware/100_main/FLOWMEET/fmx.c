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
#include "lptim.h"
#include "fm_debug.h"
#include "fmx.h"
#include "fm_fmc.h"
#include "fmx_lp.h"
#include "fm_lcd.h"
#include "fm_user.h"
#include "fm_setup.h"
#include "tx_api.h"
#include "fm_log.h"
#include "fm_mxc.h"
#include "fm_cmd.h"

// Sección define sin dependencia.

#define TRUE  1
#define FALSE 0
#define QUEUE_EVENT_SIZE (4 * 1) // Mantener múltiplo de 4, solo porque lo via asi, no se si es necesario.
#define STACK_SIZE (1024 * 16)
#define THREAD_PRIORITY_10	10
#define THRESHOLD_10		10
#define SLICE_0				0
#define TIMER_BACKLIGHT_INIT   1000 // ms * 10 de backlight encendido al iniciar, el computador.
#define TIMER_BACKLIGHT_GUI     500  // ms * 10 de backlight encendido al interactuar con el teclado.
#define TIMER_EXTI_DEBUNCE      100   // ms * 10 interrupciones des-habilitada para evitar rebote.
#define FMX_DEBUG_LOCAL // Y esto para que sirve???

// Sección enum y typedef sin dependencia.

// Sección define, enum y typedef con dependencia

// Debug.

// Variables tipo extern, evitar colocar aquí, siempre mejor en el .h del modulo donde se define.

// Variables non-static, primero las tipo const.

// Global variables, no statics.
ULONG global_menu_refresh = 0; // Alguna entidad solicita mantener refrescos cada 1000 mili segundo.

// Variables static, primero las tipo const.

/*
 * En necesario determinar el estado del caudal
 * - El caudal cero en la ultima medición y  cero en la anterior.
 * - El caudal no es cero en la ultima medición, tampoco era cero en la medida anterior a esta.
 * - El caudal es cero en la ultima medición, pero no era cero en la anterior, parada.
 * - El caudal no es cero en la ultima medición, pero era cero en la medición anterior, arranque.
 */
static fmx_rate_status_t rate_status = FMX_RATE_OFF;

static uint8_t pulse_in_active;
static uint16_t lptim3_capture;
static uint16_t lptim4_counter;

static uint16_t rate_tick_old = 0;
static uint16_t rate_tick_new = 0;  // Ultima captura del LPTIM3.
static uint16_t rate_tick_delta; // Período medido en n pulsos, el LPTIM3 mide ticks de clock 32769hz

static uint16_t rate_pulse_old = 0;
static uint16_t rate_pulse_new = 0;
static uint16_t rate_pulse_delta = 0; // Cantidad de pulsos entre captura, los n pulsos medidos en rate_tick_delta.

static uint16_t vol_pulse_old = 0;
static uint16_t vol_pulse_new = 0;
static uint16_t vol_pulse_delta;  // Pulsos que se traducirían a volumen.

// Cola de eventos, teclado y otros eventos.
static TX_QUEUE event_queue;
static uint32_t queue_storage_event[QUEUE_EVENT_SIZE];

// Semáforo para habilita/bloquea el hilo usado en la conexión del modulo bluetooth como esclavo.
static TX_SEMAPHORE bluetooth_slave_semaphore;

// Navegar eL menu de usuario y el menu de configuración.
static TX_THREAD main_thread;

// Timer del RTOS, detecta teclas presionada por 3 segundos.
static TX_TIMER key_long_timer; // control block del timer, manejo de tecla presionada 3 segundos.

// Timer del RTOS, apaga backlight del LCD luego de un tiempo de no actividad.
static TX_TIMER backlight_off_timer;  //

// Timer del RTOS, quita rebotes de KEY_EXT_1 y KEY_EXT_2.
static TX_TIMER debunde_timer;  //

// Elementos del RTOS usados por el modulo fm_mxc.c
static TX_THREAD bluetooth_slave_thread; // Hilo dedicado a establecer comunicación como esclavo.

// Elementos del RTOS usados por el modulo fm_cmd.c
static TX_THREAD cmd_thread; // Hilo dedicado al procesamiento de comandos
static TX_QUEUE cmd_queue; // Cola para comandos FM+ entrante procesados desde UART.
uint8_t cmd_queue_buffer[3 * FM_CMD_BYTE_SIZE];

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

// Ultimas sección defines (solos los que necesitan de una definición declaración anterior).

// Private function prototypes.
void PulseUpdate();
void TimerEntryBackLightOff(ULONG timer_key);
void TimerEntryDebunce(ULONG timer_key);
void TimerEntryKeyThreeSeconds(ULONG timer_key);
void ThreadEntryMain(ULONG thread_input);
void ThreadEntryMenuLog(ULONG timer_input);

// Private function bodies.

/*
 * @brief   Lee los pulsos acumulados del timer 4, actualiza el conteo de pulsos
 * @note
 * @param
 * @retval  Ninguno.
 *
 */
void PulseUpdate()
{
    static uint8_t blink = 1;
    static uint8_t pulse_in_active_mem = TRUE;

    blink ^= 1;

    // Si ingresan pulsos se necesita refrescos cada 1 segundo.
    if ((pulse_in_active == TRUE) || pulse_in_active_mem)
    {
        global_menu_refresh = 1000;
    }

    if (pulse_in_active)
    {
        if (pulse_in_active_mem)
        {
            rate_status = FMX_RATE_ON;
        }
        else
        {
            rate_status = FMX_RATE_TO_ON;
        }
    }
    else
    {
        if (pulse_in_active_mem)
        {
            rate_status = FMX_RATE_TO_OFF;
        }
        else
        {
            rate_status = FMX_RATE_OFF;
        }
    }

    pulse_in_active_mem = pulse_in_active;
    pulse_in_active = FALSE;

    if (rate_pulse_delta)
    {
        //Si  ingresan pulsus el período  se calcula  normalmente.
        rate_tick_old = rate_tick_new;
        rate_tick_new = lptim3_capture;
        rate_tick_delta = (rate_tick_new - rate_tick_old);

        // Si  ingresan pulsos el segmento testigo parpadea.
        FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_POINT, blink);
    }
    else
    {
        // No ingresaron pulsos.
        rate_tick_delta = 32768;

        // Si estoy en este bloque el caudal es cero, segmento testigo de caudal apagado.
        FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_POINT, 0);
    }

    // Pulsos del sensor primario capturados en el ultimo intervalo.
    // El caudal se calculara en función de cuanto pulsos ingresan en aproximadamente 1 segundo.
    rate_pulse_old = rate_pulse_new;
    rate_pulse_new = lptim4_counter;
    rate_pulse_delta = (rate_pulse_new - rate_pulse_old);

    vol_pulse_old = vol_pulse_new;
    vol_pulse_new = LPTIM4->CNT;
    vol_pulse_delta = (vol_pulse_new - vol_pulse_old);

    /*
     * Este bloque explica la técnica utilizada para medir el caudal, un posible bug en el silicio del
     * STM32U575 y la solución aplicada.
     *
     * La forma típica de medir el caudal consiste en determinar el período de una señal, calcular la
     * frecuencia, con la frecuencia y otros datos se obtiene el caudal. Para lograr alta precisión, se requiere
     * una alta frecuencia de reloj. Este esquema típico no es aplicable debido a que alta frecuencia no es
     * compatible con bajo consumo.
     *
     * La técnica empleada en este caso es la siguiente:
     * - Se utiliza un timer (LPTIM3), alimentado por un reloj de 32.768 Hz, configurado en modo captura continua.
     *   Este timer no genera interrupciones por sí mismo.
     * - Periódicamente, el microcontrolador se activa para actualizar los valores de caudal y volumen, y en ese momento:
     *     - Se lee el último valor capturado por LPTIM3 (que representa el tiempo del último flanco de la señal).
     *     - Simultáneamente, otro timer (LPTIM4) cuenta la cantidad de pulsos ingresados durante el período inactivo.
     * - Con ambos datos, se puede calcular el período promedio de los pulsos registrados mientras el sistema estaba en modo de bajo consumo.
     *
     * Este método brinda una resolución adecuada para frecuencias menores a 10 kHz, con una precisión mejor al 0.01%.
     * Ha sido probado con éxito en múltiples modelos de caudalímetros. Sin embargo, se detectó un problema:
     *
     * El módulo de captura (LPTIM3) no actualiza correctamente el valor de la última captura cuando el microcontrolador
     * está en modo de bajo consumo y se han producido una cantidad par de capturas.
     *
     * La solución consiste en habilitar temporalmente la interrupción por captura del LPTIM3. El proceso es:
     * 1) Mientras el microcontrolador está en modo activo (run), se habilita la interrupción de captura en LPTIM3.
     * 2) Se produce una captura, ya sea durante el modo activo o una vez que el sistema ha ingresado en modo stop.
     * 3) En la rutina de interrupción, se registran los valores de LPTIM3 y LPTIM4, y se desactiva la interrupción.
     * 4) El sistema vuelve al modo stop hasta el siguiente ciclo de actividad.
     *
     * Aunque este método funciona, tiene dos desventajas en comparación con el método original:
     * - Si la nueva captura ocurre fuera del período activo, el microcontrolador se despierta antes de tiempo
     *   para atender la interrupción. Esto impacta el consumo, especialmente en señales de baja frecuencia.
     * - Existe un retardo en la actualización de la lectura de caudal: el valor mostrado en pantalla (por ejemplo,
     *   cada segundo) puede corresponder a una captura del segundo anterior. Aunque este desfase no suele ser
     *   crítico, se vuelve perceptible al detener el flujo.
     */
    __HAL_LPTIM_ENABLE_IT(&hlptim3, LPTIM_IT_CC1);

    FM_FMC_PulseAdd(vol_pulse_delta);
    FM_FMC_CaptureSet(rate_pulse_delta, rate_tick_delta);
    FM_FMC_TtlCalc();
    FM_FMC_AcmCalc();
    FM_FMC_RateCalc();
}

/*
 * @brief   Timer por software del ThreadX, apaga el backlight luego de estar encendido 3 segundos.
 * @param   Ver documentación del ThreadX.
 * @retval  Ver documentación del ThreadX.
 */
void TimerEntryBackLightOff(ULONG timer_input)
{
    tx_timer_deactivate(&backlight_off_timer);
    HAL_GPIO_WritePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_SET);
}

/*
 * @brief función principal.
 */
void ThreadEntryMain(ULONG thread_input)
{
    uint32_t received_event;
    uint8_t menu = 0;
    uint8_t menu_change;
    UINT tx_status;
    ULONG sleep_time = 100;

    /*
     * Enciendo el backlight LCD.
     * Este es el único punto donde se puede encender el backlight de manera manual.
     * En cualquier otro punto del firmware usar FMX_LcdBackLightOn(); esto asegura que se apague.
     */
    HAL_GPIO_WritePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_RESET);
    tx_timer_activate(&backlight_off_timer);

    // Limpio eventos.
    do
    {
        tx_status = tx_queue_receive(&event_queue, &received_event,
        TX_NO_WAIT);
    } while (tx_status != TX_QUEUE_EMPTY);

    received_event = FMX_EVENT_MENU_REFRESH;

    for (;;)
    {

        PulseUpdate();
        /*
         * Idealmente micro pudiese permanecer en stop mode hasta que algo pase, tiempo indefinido.
         * Esto es posible, el el RTOS necesita un tiempo de expiración.
         * Para al menos una tarea del RTOS se necesita un idle time finito.
         * Si todos los procesos pasan a idle con TX_WAIT_FOREVER, el calculo que hace el RTOS "falla",
         * se saldrá del stop mode justo después de ingresar.
         * El LPTIM 1 programado con un prescaler de 16 y alimentado con una frecuencia de 32768Hz, tiene
         * una capacidad máxima de 32 segundo. En conclusion no se puede ir a dormir por mas de 32 segundos.
         * Ir a dormir por mas de 5 segundos no aporta mucho a la duración de la batería.
         * Se decide dejar el time out en 5 segundos.
         * Si el computador nunca saliese de stop mode 2 consumiría 33uA.
         * Si el computador sale una vez cada 5 segundos consume 42uA
         * Si el computador sale una vez cada 3 segundos consume 45uA
         * Si el computador sale una vez cada 1 segundos consume 63uA
         *
         * Los consumos anteriores son validos VDD de 3V, con valores de 3.6V estos valores se duplican.
         *
         * Nota, las mediciones en este caso se hicieron con el ST-LKINK desconectado, si se conecta el ST-LINK
         * hay un drenaje de corriente desde ST-LINK hacia el computador, y el consumo disminuye 5uA
         * aproximadamente. Aunque es algo considerable, desconectar y conectar el ST-LINK para cada medición no
         * justifica la operatoria. En otros puntos, donde se menciona consumo, puede que se dejara el ST-LINK
         * conectado, y que el valor corresponda a la lectura directa a esta menos estos 5uA, es decir si no
         * menciona que se desconecto el ST-LINK no sirve para calculo fino, en muchos casos no se necesita.
         */

        if ((received_event >= FMX_EVENT_MENU_REFRESH) && (received_event < FMX_EVENT_TIME_OUT))
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
                if (received_event == FMX_EVENT_EMPTY)
                {
                    received_event = FMX_EVENT_MENU_REFRESH;
                }
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
            received_event = FMX_EVENT_EMPTY;
        }

        // Tiempo a stop mode por defecto 5 segundos
        sleep_time = 500;

        if (global_menu_refresh)
        {
            // Menu user o setup solicitaron un refresco en blocl_menu_refresh mili-segundos
            sleep_time = global_menu_refresh;
        }

        // Refrescar lo que se ve en pantalla.
        FM_LCD_LL_Refresh();
        FM_LOG_Monitor(rate_status);
        tx_status = tx_queue_receive(&event_queue, &received_event, sleep_time);

        // Verifico si expiro el tiempo sin que llegue un mensaje
        if (tx_status != TX_SUCCESS)
        {
            // Si se programo un refresco y el tiempo expiro se debe refrescar el menu.
            if (global_menu_refresh)
            {
                received_event = FMX_EVENT_MENU_REFRESH;
            }
        }

        // Evaluó si el evento producido tiene que prender el backlight, ejemplo tecla presionada.
        if ((received_event >= FMX_EVENT_KEY_DOWN) && (received_event <= FMX_EVENT_KEY_EXT_2))
        {
            FMX_LcdBackLightOn();
        }

        // Los refresco de menu, user y setup, no se conservan, se deben solicitar nuevamente.
        global_menu_refresh = 0;
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
        if (tx_queue_send(&event_queue, &event_new, TX_NO_WAIT) != TX_SUCCESS)
        {
            FM_DEBUG_LedError(1);
        }
        key_down_skip_next = TRUE;
    }

    if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin))
    {
        event_new = FMX_EVENT_KEY_UP_LONG;
        if (tx_queue_send(&event_queue, &event_new, TX_NO_WAIT) != TX_SUCCESS)
        {
            FM_DEBUG_LedError(1);
        }
        key_up_skip_next = TRUE;
    }

    if (HAL_GPIO_ReadPin(KEY_ESC_GPIO_Port, KEY_ESC_Pin))
    {
        event_new = FMX_EVENT_KEY_ESC_LONG;
        if (tx_queue_send(&event_queue, &event_new, TX_NO_WAIT) != TX_SUCCESS)
        {
            FM_DEBUG_LedError(1);
        }
        key_esc_skip_next = TRUE;
    }

    if (HAL_GPIO_ReadPin(KEY_ENTER_GPIO_Port, KEY_ENTER_Pin))
    {
        event_new = FMX_EVENT_KEY_ENTER_LONG;
        if (tx_queue_send(&event_queue, &event_new, TX_NO_WAIT) != TX_SUCCESS)
        {
            FM_DEBUG_LedError(1);
        }
        key_enter_skip_next = TRUE;
    }
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

/*
 * @brief   Timer para refrescar datos del computador y pantalla.
 * @note
 * @param   Ver documentación del ThreadX.
 * @retval  Ninguno.
 */
void TimerEntryEventRefresh(ULONG timer_input)
{
    // Si la cora de eventos esta vaciá agrega un evento de refresco.
    FMX_RefreshEventTrue();

    //
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
    UINT ret_status = TX_SUCCESS;
    CHAR *pointer;

    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*) memory_ptr;

    // Reserva memoria para un crear hilo.
    ret_status = tx_byte_allocate(byte_pool, (VOID**) &pointer, STACK_SIZE, TX_NO_WAIT);
    if (ret_status != TX_SUCCESS)
    {
        __disable_irq();
        FM_DEBUG_LedError(1);
        while (1);
    }
    // Creo hilo principal.
    ret_status = tx_thread_create(&main_thread, "MAIN_THREAD", ThreadEntryMain, 0, pointer,
            STACK_SIZE, THREAD_PRIORITY_10, THRESHOLD_10, SLICE_0, TX_AUTO_START);
    if (ret_status != TX_SUCCESS)
    {
        __disable_irq();
        FM_DEBUG_LedError(1);
        while (1);
    }

    // Crea cola para eventos.
    ret_status = tx_queue_create(&event_queue, "EVENT_QUEUE", 1, queue_storage_event,
            sizeof(queue_storage_event));
    if (ret_status != TX_SUCCESS)
    {
        FM_DEBUG_LedError(1);
        while (1);
    }

    // Crea timer para controlar segunda función de las teclas, cuando se mantienen presionadas.
    ret_status = tx_timer_create(&key_long_timer, "KEY_LONG_TIMER", TimerEntryKeyThreeSeconds,
            0x1234, 300, 100, TX_NO_ACTIVATE);
    if (ret_status != TX_SUCCESS)
    {
        FM_DEBUG_LedError(1);
        return TX_TIMER_ERROR;
        while (1);
    }

    // Crea timer para el apagado automático del backlight del LCD.
    ret_status = tx_timer_create(&backlight_off_timer, "BACKLIGHT_TIMER", TimerEntryBackLightOff,
            0x1234, TIMER_BACKLIGHT_INIT, 0, TX_AUTO_ACTIVATE);
    if (ret_status != TX_SUCCESS)
    {
        FM_DEBUG_LedError(1);
        return TX_TIMER_ERROR;
        while (1);
    }

    // Crea timer para quitar los rebores de los botones externos.
    ret_status = tx_timer_create(&debunde_timer, "DEBUNCE_TIMER", TimerEntryDebunce, 0x1234,
            TIMER_EXTI_DEBUNCE, 0, TX_AUTO_ACTIVATE);
    if (ret_status != TX_SUCCESS)
    {
        FM_DEBUG_LedError(1);
        return TX_TIMER_ERROR;
        while (1);
    }

// Sección para inicializar recursos del ThreadX usados en modulo fm_mxc.c

    // Semáforo, desbloquea al hilo de conexión por un tiempo determinado.
    ret_status = tx_semaphore_create(&bluetooth_slave_semaphore, "BT_SLAVE_SEMAPHORE", 0);
    if (ret_status != TX_SUCCESS)
    {
        FM_DEBUG_LedError(1);
        while (1);
    }

    // Reservo memoria para crear hilo.
    ret_status = tx_byte_allocate(byte_pool, (VOID**) &pointer, STACK_SIZE, TX_NO_WAIT);
    if (ret_status != TX_SUCCESS)
    {
        __disable_irq();
        FM_DEBUG_LedError(1);
        while (1);
    }
    // Crea hilo de conexión, entry en fm_mxc.c, se pasa puntero a control block de un semáforo.
    ret_status = tx_thread_create(&bluetooth_slave_thread, "BT_SLAVE",
            FM_USER_ThreadEntryBluetoothSlave, (ULONG ) &bluetooth_slave_semaphore, pointer,
            STACK_SIZE, THREAD_PRIORITY_10, THRESHOLD_10, SLICE_0, TX_AUTO_START);
    if (ret_status != TX_SUCCESS)
    {
        __disable_irq();
        FM_DEBUG_LedError(1);
        while (1);
    }

// Sección para inicializar recursos del ThreadX usados en modulo fm_mxc.c y fm_cmd.c

    // Crea cola para envió de comando FM.
    ret_status = tx_queue_create(&cmd_queue, "CMD_QUEUE", FM_CMD_ULONG_SIZE, cmd_queue_buffer,
            sizeof(cmd_queue_buffer));
    if (ret_status != TX_SUCCESS)
    {
        __disable_irq();
        FM_DEBUG_LedError(1);
        while (1);
    }

    FM_CMD_InitRtos(&cmd_queue);
    FM_CMD_InitRtos(&cmd_queue);

    // Reserva memoria para un crear hilo.
    ret_status = tx_byte_allocate(byte_pool, (VOID**) &pointer, STACK_SIZE, TX_NO_WAIT);
    if (ret_status != TX_SUCCESS)
    {
        __disable_irq();
        FM_DEBUG_LedError(1);
        while (1);
    }
    // Crea hilo de conexión, entry en fm_mxc.c, se pasa puntero a control block de un semáforo.
    ret_status = tx_thread_create(&cmd_thread, "CMD_THREAD", FM_CMD_ThreadEntry, (ULONG )&cmd_queue,
            pointer, STACK_SIZE, THREAD_PRIORITY_10, THRESHOLD_10, SLICE_0, TX_AUTO_START);
    if (ret_status != TX_SUCCESS)
    {
        __disable_irq();
        FM_DEBUG_LedError(1);
        while (1);
    }

    return ret_status;
}

/*
 * @brief   Enciende el backlight
 * @Note    Esta es la única función que se debería usar para encender el backlight.
 * @retval
 */
void FMX_LcdBackLightOn()
{
    HAL_GPIO_WritePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_RESET);
    tx_timer_deactivate(&backlight_off_timer);
    tx_timer_change(&backlight_off_timer, TIMER_BACKLIGHT_GUI,
    TIMER_BACKLIGHT_GUI);
    tx_timer_activate(&backlight_off_timer);
}

/*
 * @brief
 * @Note
 * @retval
 */
fmx_rate_status_t FMX_GetRateStatus()
{
    return rate_status;
}

/*
 * @brief   Si la cola esta vacía agrega un evento de refresco,
 * @Note
 * @retval
 */
void FMX_RefreshEventTrue()
{
    ULONG event_new;

    if (event_queue.tx_queue_enqueued == 0)
    {
        event_new = FMX_EVENT_MENU_REFRESH;
        tx_queue_send(&event_queue, &event_new, TX_NO_WAIT);
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
    tx_timer_deactivate(&key_long_timer);

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
            if (tx_queue_send(&event_queue, &event_new,
            TX_NO_WAIT) != TX_SUCCESS)
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
            if (tx_queue_send(&event_queue, &event_new,
            TX_NO_WAIT) != TX_SUCCESS)
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
            if (tx_queue_send(&event_queue, &event_new,
            TX_NO_WAIT) != TX_SUCCESS)
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
            if (tx_queue_send(&event_queue, &event_new,
            TX_NO_WAIT) != TX_SUCCESS)
            {
                FM_DEBUG_LedError(1);
            }
        }
        break;
    case KEY_EXT_1_Pin:
        HAL_NVIC_DisableIRQ(EXTI3_IRQn);
        event_new = FMX_EVENT_KEY_EXT_1;
        if (tx_queue_send(&event_queue, &event_new, TX_NO_WAIT) != TX_SUCCESS)
        {
            FM_DEBUG_LedError(1);
        }
        tx_timer_change(&debunde_timer, TIMER_EXTI_DEBUNCE, 0);
        tx_timer_activate(&debunde_timer);
        break;
    case KEY_EXT_2_Pin:
        HAL_NVIC_DisableIRQ(EXTI4_IRQn);
        event_new = FMX_EVENT_KEY_EXT_2;
        if (tx_queue_send(&event_queue, &event_new, TX_NO_WAIT) != TX_SUCCESS)
        {
            FM_DEBUG_LedError(1);
        }
        tx_timer_change(&debunde_timer, TIMER_EXTI_DEBUNCE, 0);
        tx_timer_activate(&debunde_timer);
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
    tx_timer_change(&key_long_timer, 250, 150);
    tx_timer_activate(&key_long_timer);
}

/*
 * @brief   Interrupción de captura en LPTIM 3. Pulsos del sensor primario, turbina u otro, provocan la
 *          captura del contador del LPTIM 3.
 * @note    La salida del front-end del pick-up se conecta a un pin de captura del microcontrolador. Esta
 *          interrupción se habilita habilita cada un segundo, aproximadamente, esperando el flanco.
 *          Entre dos interrupciones se tiene:
 *          - Un numero entero de pulsos del sensor primario
 *          - La cantidad de pulsos de frecuencia 32.768Hz que ingresaron en esa venta de tiempo.
 *           Aunque la frecuencia con la que se mide el período no es muy alta, si la comparamos con por
 *           ejemplo una señal de 200Hz, típica de una turbina chica, al medir sobre mucho periodos, la
 *           resolución en la medición de la frecuencia, y por lo tanto en el caudal, es suficiente.
 * @param
 * @retval
 *
 */
void HAL_LPTIM_IC_CaptureCallback(LPTIM_HandleTypeDef *hlptim)
{
    lptim3_capture = LPTIM3->CCR1;
    lptim4_counter = LPTIM4->CNT;

    if (rate_pulse_delta == 0)
    {
        /*
         * Si estoy dentro de este bloque, el caudal paso de cero a no cero, quiero refrescar el valor en pantalla
         * para tener un rapido feedback en pantalla del computador de esta situación.
         */
        FMX_RefreshEventTrue();

        // Al estar el caudal detenido, rate_tick_new no se calcula desde PulseUpdate();
        rate_tick_new = lptim3_capture;
    }

    pulse_in_active = TRUE;

    // Des-habilito esta interrupción.
    __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_CC1);
}

/*** end of file ***/

