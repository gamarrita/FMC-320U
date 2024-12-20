/*
 * @brief	Pantallas del menu de usuario
 *
 * Versión: 1
 * Autor: Daniel H Sagarra
 * Fecha: 10/11/2024
 * Modificaciones: version inicial
 *
 */

// Includes.
#include "fm_user.h"
#include "fm_fmc.h"
#include "fm_lcd.h"
#include "string.h"
#include "fm_debug.h"
#include "fmx.h"
#include "fm_factory.h"
#include "fm_logger.h"
#include "fm_rtc.h"

// Typedef.

/*
 * Pantallas del menu de usuario, se pueden agregar en cualquier orden, pero la primer pantallas siempre = 0
 */
typedef enum
{
  MENU_USER_POWER_RESET = 0,
  MENU_USER_VERSION,
  MENU_USER_TTL_RATE,
  MENU_USER_ACM_RATE,
  MENU_USER_DATE_TIME,
  MENU_USER_END,
} enum_menu_user;

// Const data.

// Defines.
#define TRUE 	1
#define FALSE 	0

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.
extern TX_EVENT_FLAGS_GROUP event_cb_keypad;

// Global variables, statics.
char user_line_1[20];
char user_line_2[20];
char userline_3[3];

// Private function prototypes.

void MenuUserAcmRateEntry();
void MenuUserAcmRateRefresh();
void MenuUserRateRefresh();
void MenuUserClockEntry();
void MenuUserClockRefresh();
void MenuUserPowerOnEntry();
void MenuUserTtlRateEntry();
void MenuUserTtlRateRefresh();
void MenuUserVersionEntry();

// Private function bodies.

// Public function bodies.

/*
 *  @brief navegación por el menu.
 *  @note
 *  @param
 *  @retval
 *
 */
uint8_t FM_USER_MenuNav(fmx_events_t this_event)
{
  static enum_menu_user menu_index = 0;
  static uint32_t entry_counter = 0;

  uint32_t entries_to_exti;
  uint8_t menu_setup = FALSE; // pasa a valer TRUE si hay que ingresar a menu setup.

  switch (menu_index)
  {
  case MENU_USER_POWER_RESET:
    if (!entry_counter)
    {
      MenuUserPowerOnEntry();
    }

    entry_counter++;
    entries_to_exti = 3;
    if (entry_counter >= entries_to_exti)
    {
      entry_counter = 0;
      menu_index++;
      tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);
    }

    switch (this_event)
    {
    case FMX_EVENT_REFRESH:
      break;
    case FMX_EVENT_KEY_DOWN:
      break;
    case FMX_EVENT_KEY_UP:
      break;
    case FMX_EVENT_KEY_ESC:
      entry_counter = entries_to_exti; // fuerzo salida
      tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR); // evita retardo en salida
      break;
    case FMX_EVENT_KEY_ENTER:
      break;
    case FMX_EVENT_KEY_DOWN_LONG:
      break;
    case FMX_EVENT_KEY_UP_LONG:
      break;
    case FMX_EVENT_KEY_ESC_LONG:
      break;
    case FMX_EVENT_KEY_ENTER_LONG:
      break;
    default:
      FM_DEBUG_LedError(1);
      break;
    }
    break;
  case MENU_USER_VERSION:
    if (!entry_counter)
    {
      FM_LCD_LL_Clear();
      MenuUserVersionEntry();
    }

    entry_counter++;
    entries_to_exti = 3;
    if (entry_counter >= entries_to_exti)
    {
      entry_counter = 0;
      menu_index++;
      tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);
    }
    switch (this_event)
    {
    case FMX_EVENT_REFRESH:
      break;
    case FMX_EVENT_KEY_DOWN:
      break;
    case FMX_EVENT_KEY_UP:
      break;
    case FMX_EVENT_KEY_ESC:
      break;
    case FMX_EVENT_KEY_ENTER:
      entry_counter = entries_to_exti; // Fuerzo salida
      tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);
      break;
    case FMX_EVENT_KEY_DOWN_LONG:
      break;
    case FMX_EVENT_KEY_UP_LONG:
      break;
    case FMX_EVENT_KEY_ESC_LONG:
      break;
    case FMX_EVENT_KEY_ENTER_LONG:
      break;
    default:
      FM_DEBUG_LedError(1);
      break;
    }
    break;
  case MENU_USER_TTL_RATE:
    if (!entry_counter)
    {
      entry_counter++;
      MenuUserTtlRateEntry();
    }
    switch (this_event)
    {
    case FMX_EVENT_REFRESH:
      MenuUserTtlRateRefresh();
      break;
    case FMX_EVENT_KEY_DOWN:
      entry_counter = 0;
      menu_index++;
      tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);
      break;
    case FMX_EVENT_KEY_UP:
      break;
    case FMX_EVENT_KEY_ESC:
      break;
    case FMX_EVENT_KEY_ENTER:
      break;
    case FMX_EVENT_KEY_DOWN_LONG:
      FM_FMC_TotalizerFpInc();
      MenuUserTtlRateEntry();
      MenuUserTtlRateRefresh();
      break;
    case FMX_EVENT_KEY_UP_LONG:
      FM_FMC_RateFpInc();
      MenuUserTtlRateEntry();
      MenuUserTtlRateRefresh();
      break;
    case FMX_EVENT_KEY_ESC_LONG:
      entry_counter = 0;
      menu_index = MENU_USER_POWER_RESET;
      menu_setup = TRUE;
      break;
    case FMX_EVENT_KEY_ENTER_LONG:
      break;
    default:
      FM_DEBUG_LedError(1);
      break;
    }
    break;
  case MENU_USER_ACM_RATE:
    if (!entry_counter)
    {
      entry_counter++;
      MenuUserAcmRateEntry();
    }
    switch (this_event)
    {
    case FMX_EVENT_REFRESH:
      MenuUserAcmRateRefresh();
      break;
    case FMX_EVENT_KEY_DOWN:
      entry_counter = 0;
      menu_index++;
      tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);
      break;
    case FMX_EVENT_KEY_UP:
      entry_counter = 0;
      menu_index--;
      tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);
      break;
    case FMX_EVENT_KEY_ESC:
      break;
    case FMX_EVENT_KEY_ENTER:
      break;
    case FMX_EVENT_KEY_DOWN_LONG:
      break;
    case FMX_EVENT_KEY_UP_LONG:
      break;
    case FMX_EVENT_KEY_ESC_LONG:
      break;
    case FMX_EVENT_KEY_ENTER_LONG:
      FM_FMC_AcmReset();
      break;
    default:
      FM_DEBUG_LedError(1);
      break;
    }
    break;
  case MENU_USER_DATE_TIME:
    if (!entry_counter)
    {
      entry_counter++;
      MenuUserClockEntry();
    }
    switch (this_event)
    {
    case FMX_EVENT_REFRESH:
      MenuUserClockRefresh();
      break;
    case FMX_EVENT_KEY_DOWN:
      break;
    case FMX_EVENT_KEY_UP:
      entry_counter = 0;
      menu_index--;
      tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);
      break;
    case FMX_EVENT_KEY_ESC:
      break;
    case FMX_EVENT_KEY_ENTER:
      break;
    case FMX_EVENT_KEY_DOWN_LONG:
      break;
    case FMX_EVENT_KEY_UP_LONG:
      break;
    case FMX_EVENT_KEY_ESC_LONG:
      break;
    case FMX_EVENT_KEY_ENTER_LONG:
      break;
    default:
      FM_DEBUG_LedError(1);
      break;
    }
    break;
  case MENU_USER_END:
    menu_index = MENU_USER_POWER_RESET;
    FM_DEBUG_LedError(1);
    break;
  default:
    FM_DEBUG_LedError(1);
    break;
  }
  return menu_setup;
}

// El ordenamiento de las siguiente funciones no es alfabético. Se ordenaron como aparecen de cada menu.

/*
 * @brief		primer pantalla presentada al iniciar el computador de caudales.
 * @note		se encienden todos los segmentos.
 * @param		ninguno.
 * @retval 	ninguno.
 */
void MenuUserPowerOnEntry()
{
  const char msg_debug_power_on[] = "power on entry";

  FM_DEBUG_UartMsg(msg_debug_power_on, sizeof(msg_debug_power_on));


  // Enciendo todos los segmentos de LCD para verifica que funcionen
  FM_LCD_Init(0xff);

  // Se enciende backlight al inicio, para verificar que funciones
  HAL_GPIO_WritePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_RESET);
}

/*
 * @brief		muestra version de firmware.
 * @note
 * @param		ninguno.
 * @retval 	ninguno.
 */
void MenuUserVersionEntry()
{
  const char *msg_version;
  msg_version = FM_FACTORY_FirmwareVersionGet();

  FM_DEBUG_UartMsg(msg_version, strlen(msg_version));

  FM_LCD_LL_Clear();
  FM_LCD_PutString(msg_version, strlen(msg_version), FM_LCD_LL_ROW_2);

  // Apago el backlight antes de salir de este menu.
  HAL_GPIO_WritePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_SET);
}

/*
 * @brief
 * @note
 * @param
 * @retval
 */
void MenuUserTtlRateEntry()
{
  char *unit_str;
  fm_fmc_vol_unit_t vol_unit;

  // Borro completamente la pantalla al ingresar a esta función.
  FM_LCD_LL_Clear();
  FM_LCD_LL_BlinkClear();

  // Enciendo símbolos en la memoria shadow de pantalla los símbolos a mostrar en este menu.
  FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_TTL, 1);
  FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_RATE, 1);
  FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_BACKSLASH, 1);

  // Escribo en la memoria shadow de pantalla.
  vol_unit = FM_FMC_TotalizerVolUnitGet();
  FM_FMC_TotalizerStrUnitGet(&unit_str, vol_unit);
  FM_LCD_LL_PutChar_1(unit_str[0]);
  FM_LCD_LL_PutChar_2(unit_str[1]);

  // Muestro unidad de tiempo
  FM_FMC_TotalizerTimeUnitSel(FM_FMC_TotalizerTimeUnitGet());

}

/*
 * @brief
 * @note
 * @param
 * @retval
 */
void MenuUserTtlRateRefresh()
{
  uint32_t p_integer;
  uint32_t p_frac;
  int sel;

  p_integer = FM_FMC_TtlGet();
  p_integer /= 1000;

  p_frac = FM_FMC_TtlGet();
  p_frac %= 1000;

  sel = FM_FMC_TotalizerFpSelGet();

  switch (sel)
  {
  case FM_FMC_FP_SEL_0:
    snprintf(user_line_1, sizeof(user_line_1), "%8lu", p_integer);
    break;
  case FM_FMC_FP_SEL_1:
    p_frac /= 100;
    snprintf(user_line_1, sizeof(user_line_1), "%7lu.%1lu", p_integer, p_frac);
    break;
  case FM_FMC_FP_SEL_2:
    p_frac /= 10;
    snprintf(user_line_1, sizeof(user_line_1), "%6lu.%02lu", p_integer, p_frac);
    break;
  case FM_FMC_FP_SEL_3:
    snprintf(user_line_1, sizeof(user_line_1), "%5lu.%03lu", p_integer, p_frac);
    break;
  default:
    FM_DEBUG_LedError(1);
    break;
  }

  FM_LCD_PutString(user_line_1, FM_LCD_LL_ROW_1_COLS, FM_LCD_LL_ROW_1);

  MenuUserRateRefresh();
}

/*
 * @brief
 * @note
 * @param
 * @retval
 */
void MenuUserAcmRateEntry()
{
  char *unit_str;
  fm_fmc_vol_unit_t vol_unit;

  FM_LCD_LL_Clear();
  FM_LCD_LL_BlinkClear();

  vol_unit = FM_FMC_TotalizerVolUnitGet();
  FM_FMC_TotalizerStrUnitGet(&unit_str, vol_unit);
  FM_LCD_LL_PutChar_1(unit_str[0]);
  FM_LCD_LL_PutChar_2(unit_str[1]);

  // Escribo en memoria shadow de pantalla los símbolos de este menu.
  FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_BACKSLASH, 1);
  FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_ACM_2, 1);
  FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_RATE, 1);

  // Muestro unidad de tiempo
  FM_FMC_TotalizerTimeUnitSel(FM_FMC_TotalizerTimeUnitGet());

}

/*
 * @brief
 * @note
 * @param
 * @retval
 */
void MenuUserAcmRateRefresh()
{
  ufp3_t acm;
  uint32_t p_integer;		// parte entera
  uint32_t p_frac;  // parte fracional
  int sel;

  //
  acm = FM_FMC_AcmGet();
  sel = FM_FMC_TotalizerFpSelGet();

  p_integer = acm / 1000;
  p_frac = acm % 1000;

  switch (sel)
  {
  case FM_FMC_FP_SEL_0:
    snprintf(user_line_1, sizeof(user_line_1), "%8lu", p_integer);
    break;
  case FM_FMC_FP_SEL_1:
    p_frac /= 100;
    snprintf(user_line_1, sizeof(user_line_1), "%7lu.%1lu", p_integer, p_frac);
    break;
  case FM_FMC_FP_SEL_2:
    p_frac /= 10;
    snprintf(user_line_1, sizeof(user_line_1), "%6lu.%02lu", p_integer, p_frac);
    break;
  case FM_FMC_FP_SEL_3:
    snprintf(user_line_1, sizeof(user_line_1), "%5lu.%03lu", p_integer, p_frac);
    break;
  default:
    FM_DEBUG_LedError(1);
    break;
  }

  FM_LCD_PutString(user_line_1, FM_LCD_LL_ROW_1_COLS, FM_LCD_LL_ROW_1);

  MenuUserRateRefresh();
}

/*
 * @brief
 * @note
 * @param
 * @retval
 */
void MenuUserRateRefresh()
{
  ufp3_t rate;
  uint32_t p_integer;		// parte entera
  uint32_t p_frac;  // parte fracional
  int sel;

  //
  rate = FM_FMC_RateCalc();
  sel = FM_FMC_RateFpSelGet();

  p_integer = rate / 1000;

  switch (sel)
  {
  case FM_FMC_FP_SEL_0:
    snprintf(user_line_2, sizeof(user_line_2), "%07lu", p_integer);
    break;
  case FM_FMC_FP_SEL_1:
    p_frac = rate % 1000;
    p_frac /= 100;
    snprintf(user_line_2, sizeof(user_line_2), "%06lu.%01lu", p_integer, p_frac);
    break;
  case FM_FMC_FP_SEL_2:
    p_frac = rate % 1000;
    p_frac /= 10;
    snprintf(user_line_2, sizeof(user_line_2), "%05lu.%02lu", p_integer, p_frac);
    break;
  case FM_FMC_FP_SEL_3:
    p_frac = rate % 1000;
    snprintf(user_line_2, sizeof(user_line_2), "%04lu.%03lu", p_integer, p_frac);
    break;
  default:
    FM_DEBUG_LedError(1);
    break;
  }

  FM_LCD_PutString(user_line_2, FM_LCD_LL_ROW_2_COLS, FM_LCD_LL_ROW_2);

  FM_DEBUG_UartMsg(user_line_2, strlen(user_line_2));
}

/*
 * @brief
 * @note
 * @param
 * @retval
 */
void MenuUserClockEntry()
{
  FM_LCD_LL_Clear();
}

/*
 * @brief
 * @note
 * @param
 * @retval
 */
void MenuUserClockRefresh()
{
  FM_RTC_Gets(user_line_2, user_line_1);

  FM_LCD_PutString(user_line_1, FM_LCD_LL_ROW_1_COLS, FM_LCD_LL_ROW_1);
  FM_LCD_PutString(user_line_2, FM_LCD_LL_ROW_2_COLS, FM_LCD_LL_ROW_2);

  FM_DEBUG_UartMsg(user_line_1, strlen(user_line_1));
  FM_DEBUG_UartMsg(user_line_2, strlen(user_line_2));

}

// Interrupts

/*** end of file ***/

