/**
 * @file fmx.c
 * @brief FMX main runtime and event orchestration.
 *
 * Manages ThreadX resources, the event queue, and the flow state
 * for the FLOWMEET application.
 * @details
 * Consolidates contingency handling, REQ-FMX-INIT-001 traceability, and coordination with ThreadX timers.
 */

// --- Includes ---
#include "fmx.h"
#include "lptim.h"
#include "fm_debug.h"
#include "fm_fmc.h"
#include "fmx_lp.h"
#include "fm_lcd.h"
#include "fm_user.h"
#include "fm_setup.h"
#include "fm_log.h"
#include "fm_mxc.h"
#include "fm_cmd.h"
#include "fm_usart.h"
#include "tx_api.h"

// --- Defines ---
// Timers and limits filter bounce, enforce REQ-FMX-TIMER-001, and align with ThreadX rates.
#define TRUE                  (1u)
#define FALSE                 (0u)
#define QUEUE_EVENT_SIZE      (4u)
#define TIMER_BACKLIGHT_INIT  (1000u)
#define TIMER_BACKLIGHT_GUI   (500u)
#define TIMER_EXTI_DEBOUNCE   (25u)
#define FMX_DEBUG_LOCAL

// --- Globals ---
// Global counter keeps the UI refresh alive even when events are lost.
ULONG global_menu_refresh = 0;

// --- Static Data ---
// Private state coordinates ISRs, ThreadX timers, and debounce isolation.
static uint8_t key_ext_debouce_flag = 0;
static fmx_rate_status_t rate_status = FMX_RATE_OFF;
static uint8_t pulse_in_active;
static uint16_t lptim3_capture;
static uint16_t lptim4_counter;
static uint16_t rate_tick_old = 0;
static uint16_t rate_tick_new = 0;
static uint16_t rate_tick_delta;
static uint16_t rate_pulse_old = 0;
static uint16_t rate_pulse_new = 0;
static uint16_t rate_pulse_delta = 0;
static uint16_t vol_pulse_old = 0;
static uint16_t vol_pulse_new = 0;
static uint16_t vol_pulse_delta;
static TX_QUEUE event_queue;
// Buffer sized in ULONG keeps conversions straightforward and latency stable.
static uint32_t queue_storage_event[QUEUE_EVENT_SIZE];
// Semaphore handles the hand-off between the ISR and the BT task without busy waiting.
static TX_SEMAPHORE bluetooth_slave_semaphore;
static TX_THREAD main_thread;
static TX_TIMER key_long_timer;
static TX_TIMER backlight_off_timer;
static TX_TIMER debunde_timer;
static TX_THREAD bluetooth_slave_thread;
uint8_t key_up_skip_next = FALSE;
uint8_t key_down_skip_next = FALSE;
uint8_t key_enter_skip_next = FALSE;
uint8_t key_esc_skip_next = FALSE;

// --- Static Prototypes ---
// Normalizes flow counters before updating the GUI.
static void PulseUpdate(void);
// Backlight timer prevents noticeable flicker.
static void TimerEntryBackLightOff(ULONG timer_key);
// Debounce timer satisfies REQ-FMX-DEBOUNCE-003.
static void TimerEntryDebunce(ULONG timer_key);
// Long-press timer prevents blocking other tasks.
static void TimerEntryKeyThreeSeconds(ULONG timer_key);
// Main thread coordinates flow state and ThreadX queues.
static void ThreadEntryMain(ULONG thread_input);

// --- Public API ---

/**
 * @brief Initialize FMX core tasks and queues.
 * @param memory_ptr Byte pool provided by the ThreadX startup.
 * @return TX_SUCCESS or a ThreadX error code.
 * @details
 * Verifies each allocation, establishes deterministic resources, and forces a fail-safe on errors.
 */
UINT FMX_Init(VOID *memory_ptr)
{
    UINT ret_status = TX_SUCCESS;
    CHAR *pointer;
    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*) memory_ptr;

    // Check dynamic memory before creating threads.
    ret_status = tx_byte_allocate(byte_pool, (VOID**)&pointer, FMX_STACK_SIZE, TX_NO_WAIT);
    // On a critical error, force an immediate fail-safe.
    if (ret_status != TX_SUCCESS) {
        __disable_irq();
        FM_DEBUG_LedError(1);
        while (1);
    }

    ret_status = tx_thread_create(&main_thread, "MAIN_THREAD", ThreadEntryMain, 0, pointer,
                                  FMX_STACK_SIZE, FMX_THREAD_PRIORITY_10, FMX_THRESHOLD_10, FMX_SLICE_0, TX_AUTO_START);
    if (ret_status != TX_SUCCESS) {
        __disable_irq();
        FM_DEBUG_LedError(1);
        while (1);
    }

    ret_status = tx_queue_create(&event_queue, "EVENT_QUEUE", 1, queue_storage_event,
                                 sizeof(queue_storage_event));
    if (ret_status != TX_SUCCESS) {
        FM_DEBUG_LedError(1);
        while (1);
    }

    ret_status = tx_timer_create(&key_long_timer, "KEY_LONG_TIMER", TimerEntryKeyThreeSeconds,
                                 0x1234, 300, 100, TX_NO_ACTIVATE);
    if (ret_status != TX_SUCCESS) {
        FM_DEBUG_LedError(1);
        return TX_TIMER_ERROR;
        while (1);
    }

    ret_status = tx_timer_create(&backlight_off_timer, "BACKLIGHT_TIMER", TimerEntryBackLightOff,
                                 0x1234, TIMER_BACKLIGHT_INIT, 0, TX_AUTO_ACTIVATE);
    if (ret_status != TX_SUCCESS) {
        FM_DEBUG_LedError(1);
        return TX_TIMER_ERROR;
        while (1);
    }

    ret_status = tx_timer_create(&debunde_timer, "DEBUNCE_TIMER", TimerEntryDebunce, 0x1234,
                                 TIMER_EXTI_DEBOUNCE, TIMER_EXTI_DEBOUNCE, TX_NO_ACTIVATE);
    if (ret_status != TX_SUCCESS) {
        FM_DEBUG_LedError(1);
        return TX_TIMER_ERROR;
        while (1);
    }

    ret_status = tx_semaphore_create(&bluetooth_slave_semaphore, "BT_SLAVE_SEMAPHORE", 0);
    FM_CMD_RtosInit(memory_ptr);
    FM_USART_RtosInit(memory_ptr);

    ret_status = tx_byte_allocate(byte_pool, (VOID**)&pointer, FMX_STACK_SIZE, TX_NO_WAIT);
    if (ret_status != TX_SUCCESS) {
        __disable_irq();
        FM_DEBUG_LedError(1);
        while (1);
    }

    ret_status = tx_thread_create(&bluetooth_slave_thread, "BT_SLAVE",
                                  FM_USER_ThreadEntryBluetoothSlave, (ULONG)&bluetooth_slave_semaphore, pointer,
                                  FMX_STACK_SIZE, FMX_THREAD_PRIORITY_10, FMX_THRESHOLD_10, FMX_SLICE_0, TX_AUTO_START);
    if (ret_status != TX_SUCCESS) {
        __disable_irq();
        FM_DEBUG_LedError(1);
        while (1);
    }

    return ret_status;
}

/**
 * @brief Forces the LCD backlight on while there is interaction.
 * @details
 * Keeps the backlight active by reprogramming the backlight timer to avoid flicker.
 */
void FMX_LcdBackLightOn(void)
{
    HAL_GPIO_WritePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_RESET);
    tx_timer_deactivate(&backlight_off_timer);
    tx_timer_change(&backlight_off_timer, TIMER_BACKLIGHT_GUI, TIMER_BACKLIGHT_GUI);
    tx_timer_activate(&backlight_off_timer);
}

/**
 * @brief Returns the current state of the flow state machine.
 * @return @ref fmx_rate_status_t value used for reporting and UI.
 * @details
 */
fmx_rate_status_t FMX_GetRateStatus(void)
{
    return rate_status;
}

/**
 * @brief Ensures the presence of a refresh event in the queue.
 * @details
 * Maintains at least one refresh event to meet REQ-FMX-GUI-002 and avoid starvation.
 */
void FMX_RefreshEventTrue(void)
{
    ULONG event_new;
    if (event_queue.tx_queue_enqueued == 0) {
        event_new = FMX_EVENT_MENU_REFRESH;
        tx_queue_send(&event_queue, &event_new, TX_NO_WAIT);
    }
}

/**
 * @brief Wakes the thread that handles the Bluetooth slave connection.
 * @details
 * Uses a semaphore to wake the Bluetooth thread without polling.
 */
void FMX_Trigger_BluetoothSlave(void)
{
    tx_semaphore_put(&bluetooth_slave_semaphore);
}

// --- Private Function Implementations ---

/**
 * @brief Update pulse and flow measurement state, and trigger calculations.
 */
static void PulseUpdate(void)
{
    static uint8_t blink = 1;
    blink ^= 1;

    if (rate_pulse_delta) {
        rate_tick_old = rate_tick_new;
        rate_tick_new = lptim3_capture;
        rate_tick_delta = (rate_tick_new - rate_tick_old);
        FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_POINT, blink);
    } else {
        rate_tick_delta = 32768;
        FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_POINT, 0);
    }

    rate_pulse_old = rate_pulse_new;
    rate_pulse_new = lptim4_counter;
    rate_pulse_delta = (rate_pulse_new - rate_pulse_old);

    vol_pulse_old = vol_pulse_new;
    vol_pulse_new = LPTIM4->CNT;
    vol_pulse_delta = (vol_pulse_new - vol_pulse_old);

    __HAL_LPTIM_ENABLE_IT(&hlptim3, LPTIM_IT_CC1);
    FM_FMC_PulseAdd(vol_pulse_delta);
    FM_FMC_CaptureSet(rate_pulse_delta, rate_tick_delta);
    FM_FMC_TtlCalc();
    FM_FMC_AcmCalc();
    FM_FMC_RateCalc();
}

/**
 * @brief Timer callback to turn off the LCD backlight after inactivity.
 * @param timer_input Unused timer parameter.
 */
static void TimerEntryBackLightOff(ULONG timer_input)
{
    tx_timer_deactivate(&backlight_off_timer);
    HAL_GPIO_WritePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_SET);
}

/**
 * @brief Timer callback for 3-second key press detection (long press).
 * @param timer_input Unused timer parameter.
 */
static void TimerEntryKeyThreeSeconds(ULONG timer_input)
{
    fmx_events_t event_new;

    if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin)) {
        event_new = FMX_EVENT_KEY_DOWN_LONG;
        if (tx_queue_send(&event_queue, &event_new, TX_NO_WAIT) != TX_SUCCESS) {
            FM_DEBUG_LedError(1);
        }
        key_down_skip_next = TRUE;
    }

    if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin)) {
        event_new = FMX_EVENT_KEY_UP_LONG;
        if (tx_queue_send(&event_queue, &event_new, TX_NO_WAIT) != TX_SUCCESS) {
            FM_DEBUG_LedError(1);
        }
        key_up_skip_next = TRUE;
    }

    if (HAL_GPIO_ReadPin(KEY_ESC_GPIO_Port, KEY_ESC_Pin)) {
        event_new = FMX_EVENT_KEY_ESC_LONG;
        if (tx_queue_send(&event_queue, &event_new, TX_NO_WAIT) != TX_SUCCESS) {
            FM_DEBUG_LedError(1);
        }
        key_esc_skip_next = TRUE;
    }

    if (HAL_GPIO_ReadPin(KEY_ENTER_GPIO_Port, KEY_ENTER_Pin)) {
        event_new = FMX_EVENT_KEY_ENTER_LONG;
        if (tx_queue_send(&event_queue, &event_new, TX_NO_WAIT) != TX_SUCCESS) {
            FM_DEBUG_LedError(1);
        }
        key_enter_skip_next = TRUE;
    }
}

/**
 * @brief Timer callback to clear debounce flag for external keys.
 * @param timer_input Unused timer parameter.
 */
static void TimerEntryDebunce(ULONG timer_input)
{
    key_ext_debouce_flag = 0;
    tx_timer_deactivate(&debunde_timer);
}

/**
 * @brief Main thread entry for menu and event processing.
 * @param thread_input Unused thread parameter.
 */
static void ThreadEntryMain(ULONG thread_input)
{
    uint32_t received_event;
    uint8_t menu = 0;
    uint8_t menu_change;
    UINT tx_status;
    ULONG sleep_time = 1000;

    HAL_GPIO_WritePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_RESET);
    tx_timer_activate(&backlight_off_timer);

    do {
        tx_status = tx_queue_receive(&event_queue, &received_event, TX_NO_WAIT);
    } while (tx_status != TX_QUEUE_EMPTY);

    received_event = FMX_EVENT_MENU_REFRESH;

    for (;;) {
        sleep_time = 1000;
        PulseUpdate();

        if ((received_event >= FMX_EVENT_MENU_REFRESH) && (received_event < FMX_EVENT_TIME_OUT)) {
            switch (menu) {
            case 0:
                menu_change = FM_USER_MenuNav(received_event);
                if (menu_change) {
                    menu_change = 0;
                    menu = 1;
                    FMX_RefreshEventTrue();
                }
                break;
            case 1:
                if (received_event == FMX_EVENT_EMPTY) {
                    received_event = FMX_EVENT_MENU_REFRESH;
                }
                menu_change = FM_SETUP_MenuNav(received_event);
                if (menu_change) {
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

        if (global_menu_refresh) {
            sleep_time = global_menu_refresh;
        }

        FM_LCD_LL_Refresh();
        FM_LOG_Monitor(rate_status);
        tx_status = tx_queue_receive(&event_queue, &received_event, sleep_time / 10);

        if (tx_status != TX_SUCCESS) {
            received_event = FMX_EVENT_MENU_REFRESH;
        }

        if ((received_event >= FMX_EVENT_KEY_DOWN) && (received_event <= FMX_EVENT_KEY_EXT_2)) {
            FMX_LcdBackLightOn();
        }

        global_menu_refresh = 0;
    }
}

// --- Interrupts ---

/**
 * @brief EXTI falling edge callback for key/button events.
 * @param GPIO_Pin Pin number that triggered the interrupt.
 */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
    uint32_t event_new;
    UNUSED(GPIO_Pin);
    tx_timer_deactivate(&key_long_timer);

    switch (GPIO_Pin) {
    case KEY_ENTER_Pin:
        if (key_enter_skip_next) {
            key_enter_skip_next = FALSE;
        } else {
            event_new = FMX_EVENT_KEY_ENTER;
            if (tx_queue_send(&event_queue, &event_new, TX_NO_WAIT) != TX_SUCCESS) {
                FM_DEBUG_LedError(1);
            }
        }
        break;
    case KEY_DOWN_Pin:
        if (key_down_skip_next) {
            key_down_skip_next = FALSE;
        } else {
            event_new = FMX_EVENT_KEY_DOWN;
            if (tx_queue_send(&event_queue, &event_new, TX_NO_WAIT) != TX_SUCCESS) {
                FM_DEBUG_LedError(1);
            }
        }
        break;
    case KEY_ESC_Pin:
        if (key_esc_skip_next) {
            key_esc_skip_next = FALSE;
        } else {
            event_new = FMX_EVENT_KEY_ESC;
            if (tx_queue_send(&event_queue, &event_new, TX_NO_WAIT) != TX_SUCCESS) {
                FM_DEBUG_LedError(1);
            }
        }
        break;
    case KEY_UP_Pin:
        if (key_up_skip_next) {
            key_up_skip_next = FALSE;
        } else {
            event_new = FMX_EVENT_KEY_UP;
            if (tx_queue_send(&event_queue, &event_new, TX_NO_WAIT) != TX_SUCCESS) {
                FM_DEBUG_LedError(1);
            }
        }
        break;
    case KEY_EXT_1_Pin:
        if (key_ext_debouce_flag) break;
        key_ext_debouce_flag = 1;
        tx_timer_activate(&debunde_timer);
        event_new = FMX_EVENT_KEY_EXT_1;
        if (tx_queue_send(&event_queue, &event_new, TX_NO_WAIT) != TX_SUCCESS) {
            FM_DEBUG_LedError(1);
        }
        break;
    case KEY_EXT_2_Pin:
        if (key_ext_debouce_flag) break;
        key_ext_debouce_flag = 1;
        tx_timer_activate(&debunde_timer);
        event_new = FMX_EVENT_KEY_EXT_2;
        if (tx_queue_send(&event_queue, &event_new, TX_NO_WAIT) != TX_SUCCESS) {
            FM_DEBUG_LedError(1);
        }
        break;
    default:
        FM_DEBUG_LedError(1);
        break;
    }
}

/**
 * @brief EXTI rising edge callback for key/button events (start long press timer).
 * @param GPIO_Pin Pin number that triggered the interrupt.
 */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
    UNUSED(GPIO_Pin);
    tx_timer_change(&key_long_timer, 250, 150);
    tx_timer_activate(&key_long_timer);
}

/**
 * @brief LPTIM input capture callback for flow measurement.
 * @param hlptim Pointer to the LPTIM handle.
 */
void HAL_LPTIM_IC_CaptureCallback(LPTIM_HandleTypeDef *hlptim)
{
    lptim3_capture = LPTIM3->CCR1;
    lptim4_counter = LPTIM4->CNT;

    if (rate_pulse_delta == 0) {
        FMX_RefreshEventTrue();
        rate_tick_new = lptim3_capture;
    }

    pulse_in_active = TRUE;
    __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_CC1);
}

/*** END OF FILE ***/








