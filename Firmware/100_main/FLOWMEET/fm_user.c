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
#include <fm_log.h>
#include "fm_user.h"
#include "fm_fmc.h"
#include "fm_lcd.h"
#include "string.h"
#include "fm_debug.h"
#include "fmx.h"
#include "fm_factory.h"
#include "fm_rtc.h"
#include "fm_mxc.h"
#include "fm_ppt.h"

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
  MENU_USER_PRINT_ACM,
  MENU_USER_DATE_TIME,
  MENU_USER_END,
} menu_user_t;

// Código de errores de impresión.
typedef enum
{
  PRINT_OK = 0,         // No se detecta error en la ultima impresión, listo para imprimir.
  PRINT_POWER_ON,    // Se encendiendo la zona de alimentacion para el modulo blueooth
  PRINT_CONNECTING, // Enviando comando para conectar el modulo bluetooth con la impresora
  PRINT_PRINTING,   // Imprimiendo, se le esta enviado el ticket a la impreso
  PRINT_ERROR_1,     // Se detecto error en la ultima impresión, listo para imprimir.
} print_status_t;

// Const data.
char print_status_msg[][3] =
{
    "--",
    "03",
    "02",
    "01",
    "E1",
    "E2", };

// Defines.
#define TRUE 	1
#define FALSE 	0

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.
extern uint8_t gobal_refresh_1000; // Alguna entidad solicita mantener refrescos cada 1 segundo.
extern TX_QUEUE queue_cb_event;

// Global variables, statics.
char user_line_1[20];
char user_line_2[20];
char userline_3[3];

// Private function prototypes.

/*
 * Los siguientes prototipos se ordenan de la forma que se recorren en el menu, no alfabético. El mismo orden
 * se usa para los cuerpos de las funciones.
 */
void MenuUserPowerOnEntry();
void MenuUserVersionEntry();
void MenuUserTtlRateEntry();
void MenuUserTtlRateRefresh();
void MenuUserAcmRateEntry();
void MenuUserAcmRateRefresh();
void MenuUserRateRefresh();
void MenuUserPrintAcmEntry();
void MenuUserPrintAcmRefresh();
void MenuUserClockEntry();
void MenuUserClockRefresh();
void MenuUserPrintAcmStatus(print_status_t sel);

// Private function bodies.

// Public function bodies.

/*
 *  @brief navegación por el menu.
 *  @note
 *  @param
 *  @retval 0, Al salir no se necesita refrescar o cambiar de pantalla.
 *          1, Al salir se debe volver a ingresar para refrescar la pantalla.
 *          2, Al Salir se debe volver  a ingresar por que cambio del menu de usuario al de configuración.
 *          3, Al Salir se debe volver  a ingresar por que cambio del menu de configuración al de usuario.
 */
uint8_t FM_USER_MenuNav(fmx_events_t this_event)
{
  static menu_user_t menu_index = 0;
  static uint32_t entry_counter = 0;
  fmx_status_t exit_status = FMX_STATUS_OK;
  uint8_t menu_setup = FALSE; // pasa a valer TRUE si hay que ingresar a menu setup.
  ULONG received_event;
  //UINT tx_status;

  if (this_event == FMX_EVENT_EMPTY)
  {
    // Idealmente si el evento es el EMPTY, debería salir, y ahorrar el tiempo  de procesamiento, actualmente
    // se hace este cambio de valor porque algun problema hace que no se refresque correctamente la pantalla.
    this_event = FMX_EVENT_REFRESH;
  }

  switch (menu_index)
  {
  case MENU_USER_POWER_RESET:
    MenuUserPowerOnEntry();
    tx_queue_receive(&queue_cb_event, &received_event, 250);
    FMX_RefreshEventTrue();
    menu_index++;
    break;
  case MENU_USER_VERSION:
    MenuUserVersionEntry();
    FM_LCD_LL_Refresh();
    tx_queue_receive(&queue_cb_event, &received_event, 250);
    FMX_RefreshEventTrue();
    menu_index++;
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
    case FMX_EVENT_KEY_EXT_1:
      entry_counter = 0;
      menu_index++;
      FMX_RefreshEventTrue();
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
      FMX_RefreshEventTrue();
      break;
    case FMX_EVENT_KEY_EXT_2:
      FMX_RefreshEventTrue();
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
    case FMX_EVENT_KEY_EXT_1:
      entry_counter = 0;
      menu_index++;
      FMX_RefreshEventTrue();
      break;
    case FMX_EVENT_KEY_UP:
      entry_counter = 0;
      menu_index--;
      FMX_RefreshEventTrue();
      break;
    case FMX_EVENT_KEY_ESC:
      break;
    case FMX_EVENT_KEY_ENTER:
      break;
    case FMX_EVENT_KEY_DOWN_LONG:
      FMX_RefreshEventTrue();
      break;
    case FMX_EVENT_KEY_UP_LONG:
      FMX_RefreshEventTrue();
      break;
    case FMX_EVENT_KEY_ESC_LONG:
      FMX_RefreshEventTrue();
      break;
    case FMX_EVENT_KEY_ENTER_LONG:
    case FMX_EVENT_KEY_EXT_2:
      FM_FMC_AcmReset();
      MenuUserAcmRateRefresh();
      break;
    default:
      FM_DEBUG_LedError(1);
      break;
    }
    break;
  case MENU_USER_PRINT_ACM:
    if (!entry_counter)
    {
      entry_counter++;
      MenuUserPrintAcmEntry();
    }
    switch (this_event)
    {
    case FMX_EVENT_REFRESH:
      MenuUserPrintAcmRefresh();
      break;
    case FMX_EVENT_KEY_DOWN:
    case FMX_EVENT_KEY_EXT_1:
      entry_counter = 0;
      menu_index++;
      FMX_RefreshEventTrue();
      break;
    case FMX_EVENT_KEY_UP:
      entry_counter = 0;
      menu_index--;
      FMX_RefreshEventTrue();
      break;
    case FMX_EVENT_KEY_ESC:
    case FMX_EVENT_KEY_EXT_2:
      FM_PPT_FormatTicket();

      MenuUserPrintAcmStatus(PRINT_POWER_ON);
      FM_MXC_PowerOn();

      MenuUserPrintAcmStatus(PRINT_CONNECTING);
      exit_status = FM_MXC_BTConnect();

      if (exit_status == FMX_STATUS_OK)
      {
        MenuUserPrintAcmStatus(PRINT_PRINTING);
        FM_PPT_PrintTicket();
        tx_thread_sleep(200);
        MenuUserPrintAcmStatus(PRINT_OK);
      }
      else
      {
        MenuUserPrintAcmStatus(PRINT_ERROR_1);
      }
      FM_MXC_PowerOff();
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

    // Este menu solicita refrescos cada 1000 mili segundos.
    gobal_refresh_1000 = TRUE;
    if (!entry_counter)
    {
      entry_counter++;
      MenuUserClockEntry();
      FMX_RefreshEventTrue();
    }
    switch (this_event)
    {
    case FMX_EVENT_REFRESH:
      MenuUserClockRefresh();
      entry_counter++;
      if (entry_counter == 60)
      {
        entry_counter = 0;
        menu_index = MENU_USER_TTL_RATE;
        FMX_RefreshEventTrue();
      }
      break;
    case FMX_EVENT_KEY_DOWN:
      break;
    case FMX_EVENT_KEY_UP:
      entry_counter = 0;
      menu_index--;
      FMX_RefreshEventTrue();
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
    case FMX_EVENT_KEY_EXT_1:
      entry_counter = 0;
      menu_index = MENU_USER_TTL_RATE;
      FMX_RefreshEventTrue();
      break;
    case FMX_EVENT_KEY_EXT_2:
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
  // Enciendo todos los segmentos de LCD para verifica que funcionen
  FM_LCD_Init(0xff);
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
  const char *msg_release;

  msg_version = FM_FACTORY_FirmwareVersionGet();
  FM_LCD_LL_Clear();
  FM_LCD_PutString(msg_version, strlen(msg_version), FM_LCD_LL_ROW_2);
  msg_release = FM_FACTORY_ReleaseGet();

  FM_LCD_LL_PutChar_1(*msg_release);
  FM_LCD_LL_PutChar_2(*(msg_release + 1));
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
 * @brief   Configuración inicial de esta pantalla de menu de usuario.
 * @note
 * @param   Ninguno.
 * @retval  Ninguno.
 */
void MenuUserPrintAcmEntry()
{
  FM_LCD_LL_Clear();
  FM_LCD_LL_BlinkClear();

  // Al ingresar al menu de impression no puede haber error de impression, aun no se imprimió.
  MenuUserPrintAcmStatus(PRINT_OK);

  FM_LCD_LL_PutChar_1('P');
  FM_LCD_LL_PutChar_2('R');

  // Escribo en memoria shadow de pantalla los símbolos de este menu.
  FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_ACM_2, 1);
}

/*
 * @brief
 * @note
 * @param
 * @retval
 */
void MenuUserPrintAcmStatus(print_status_t sel)
{
  FM_LCD_PutString(print_status_msg[sel], strlen(print_status_msg[sel]), FM_LCD_LL_ROW_2);
  FM_LCD_LL_Refresh();
}

/*
 * @brief   Pantalla para impresión de tickets.
 * @note
 * @param   Ninguno.
 * @retval  Ninguno.
 */
void MenuUserPrintAcmRefresh()
{
  ufp3_t acm;
  uint32_t p_integer;       // parte entera
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

