/*
 * @brief	Menu de usuario
 * @notes	El ordenamiento de las funciones de este modulo no es alfabetico, respetan el orden aparecion en
 * 			menu.
 *
 * Versión 1
 * Autor: Daniel H Sagarra
 * Fecha: 08/09/2024
 * Modificaciones: version inicial.
 *
 *
 */

// Includes.
#include "fm_debug.h"
#include "fm_factory.h"
#include "fm_fmc.h"
#include "fm_lcd.h"
#include "fm_logger.h"
#include "fm_rtc.h"
#include "fm_setup.h"
#include "fmx.h"
#include "fm_rtc.h"
#include "string.h"

// Typedef.

// Listado de las pantallas del menu de configuración.
typedef enum
{
	MENU_SETUP_INIT = 0,
	MENU_SETUP_PASSWORD,
	MENU_SETUP_FACTOR_C,
	MENU_SETUP_VOL_UNIT,
	MENU_SETUP_TIME_UNIT,
	MENU_SETUP_DATE,
	MENU_SETUP_TIME,
	MENU_SETUP_END,				// Se usa para chequeo de contorno.
} menu_user_t;

// Lista de comandos que se pude ejecutar al editar una dato de configuración.
typedef enum
{
	MENU_MODE_INIT,			// Se ingresa a un nuevo menu de edición.
	MENU_MODE_INC,			// Incrementa variable.
	MENU_MODE_DEC,			// Decrementa variable.
	MENU_MODE_NEXT,			// Se mueve al siguiente campo o variable a editar.
	MENU_MODE_EXIT,			// Finaliza la edición actual.
	MENU_MODE_REFRESH,	// Refresca valor.,
	MENU_MODE_END				// Se usa para chequeo de contorno.
} menu_mode_t;

// Const data.

// Defines.

#define TRUE  1
#define FALSE 0

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.
extern RTC_HandleTypeDef hrtc;
extern TX_EVENT_FLAGS_GROUP event_cb_keypad;

// Global variables, statics.

const char debug_error_case[] = "Case doesnt' exist in Switch";

const uint32_t power_of_10[] =
		{
				1,
				10,
				100,
				1000,
				10000,
				100000,
				1000000,
				10000000
		};

char setup_line_1[20];
char setup_line_2[20];

// Private function prototypes

/*
 * Los siguientes prototipos de funciones se listan en el orden que luego aparacen sus definiciones, que
 * es el mismo orden en que aparecen en pantallas si se las recorre. no están ordenadas alfabéticamente.
 */
void MenuSetupInitEntry();

void MenuSetupPasswordEntry(uint8_t pass_len);
void MenuSetupPasswordEnter(menu_mode_t mode);

void MenuSetupFactorCalEntry();
ufp3_t MenuSetupFactorCalEdit(menu_mode_t mode);

void MenuSetupTimeUnitEntry();
void MenuSetupTimeUnitEdit(menu_mode_t mode);

void MenuSetupVolUnitEntry();
void MenuSetupVolUnitEdit(menu_mode_t mode);

void MenuSetupDateEntry();
void MenuSetupDateEdit(menu_mode_t mode);

// Private function bodies.

// Public function bodies.

/*
 *  Navegación por el menu.
 *
 */
uint8_t FM_SETUP_MenuNav(FMX_Events_t this_event)
{
	static menu_user_t menu_index = 0;
	uint8_t menu_user = FALSE;

	/*
	 *  La variable entry_counter se usa para ejecutar código cada vez que se ingresa a un nuevo menu.
	 *  también se lo usa en algún caso como variable de control de otro aspecto del menu actual.
	 */
	static uint8_t entry_counter = 0;
	uint8_t entries_to_exit;

	// Debe ser TRUE al finalizar el ingreso de la password para poder modificar valores del setup
	static uint8_t password_ok = TRUE;

	// La password correcta es apretar ABAJO ABAJO ARRIBA ENTER.
	const FMX_Events_t password[] =
			{
					FMX_EVENT_KEY_DOWN,
					FMX_EVENT_KEY_DOWN,
					FMX_EVENT_KEY_UP,
					FMX_EVENT_KEY_ENTER
			};

	if ((password_ok == FALSE) && (menu_index > MENU_SETUP_PASSWORD))
	{
		if (this_event & (FMX_EVENT_KEY_DOWN + FMX_EVENT_KEY_UP + FMX_EVENT_KEY_ENTER))
		{
			this_event = FMX_EVENT_REFRESH;
		}
	}

	switch (menu_index)
	{
		case MENU_SETUP_INIT:
			if (!entry_counter)
			{
				FM_LCD_Init(0xff);
				password_ok = TRUE; // se presume password correcta hasta que se ingrese secuencia incorrecta.
			}
			entry_counter++;
			entries_to_exit = 3;
			if (entry_counter >= entries_to_exit)
			{
				entry_counter = 0; // Permite ejecutar función Entry del siguiente menu.
				menu_index++; // en la próxima entada se ejecuta el siguiente menu.
				tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);
			}
			switch (this_event)
			{
				case FMX_EVENT_REFRESH:
					break;
				case FMX_EVENT_KEY_ENTER:
					entry_counter = entries_to_exit; // Fuerzo salida.
					tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);
					break;
				default:
					FM_DEUBUG_LedError();
					break;
			}
			break;
		case MENU_SETUP_PASSWORD:
			if (!entry_counter)
			{
				MenuSetupPasswordEntry(sizeof(password) / sizeof(FMX_Events_t));
				MenuSetupPasswordEnter(MENU_MODE_INIT);
				entry_counter++; // controla
			}
			switch (this_event)
			{
				case FMX_EVENT_REFRESH:
					//MenuSetupPasswordEnter(MENU_MODE_);
					break;
				case FMX_EVENT_KEY_DOWN:
					if (password[entry_counter - 1] != this_event)
					{
						password_ok = FALSE;
					}
					entry_counter++;
					MenuSetupPasswordEnter(MENU_MODE_INC);
					break;
				case FMX_EVENT_KEY_UP:
					if (password[entry_counter - 1] != this_event)
					{
						password_ok = FALSE;
					}
					entry_counter++;
					MenuSetupPasswordEnter(MENU_MODE_INC);
					break;
				case FMX_EVENT_KEY_ESC:
					if (password[entry_counter - 1] != this_event)
					{
						password_ok = FALSE;
					}
					entry_counter++;
					MenuSetupPasswordEnter(MENU_MODE_INC);
					break;
				case FMX_EVENT_KEY_ENTER:
					if (password[entry_counter - 1] != this_event)
					{
						password_ok = FALSE;
					}
					entry_counter++;
					MenuSetupPasswordEnter(MENU_MODE_INC);
					break;
				default:
					FM_DEUBUG_LedError();
					break;
			}

			/*
			 * entry_counter cuenta la cantidad de teclas presionadas, si este valor es igual al tamaño de la
			 * password es hora de salir de esta pantalla.
			 */
			if (entry_counter > (sizeof(password) / sizeof(FMX_Events_t)))
			{
				entry_counter = 0;
				menu_index++;
				tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);
			}
			break;
		case MENU_SETUP_FACTOR_C:
			if (!entry_counter)
			{
				MenuSetupFactorCalEntry();
				entry_counter++;
			}
			switch (this_event)
			{
				case FMX_EVENT_REFRESH:
					MenuSetupFactorCalEdit(MENU_MODE_REFRESH);
					break;
				case FMX_EVENT_KEY_DOWN:
					MenuSetupFactorCalEdit(MENU_MODE_DEC);
					break;
				case FMX_EVENT_KEY_UP:
					MenuSetupFactorCalEdit(MENU_MODE_INC);
					break;
				case FMX_EVENT_KEY_ESC:
					MenuSetupFactorCalEdit(MENU_MODE_EXIT);
					entry_counter = 0;
					menu_index++;
					tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);
					break;
				case FMX_EVENT_KEY_ENTER:
					MenuSetupFactorCalEdit(MENU_MODE_NEXT);
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
					FM_DEUBUG_LedError();
					break;
			}
			break;
		case MENU_SETUP_VOL_UNIT:
			if (!entry_counter)
			{
				entry_counter++;
				MenuSetupVolUnitEntry();
			}
			switch (this_event)
			{
				case FMX_EVENT_REFRESH:
					break;
				case FMX_EVENT_KEY_DOWN:
					MenuSetupVolUnitEdit(MENU_MODE_DEC);
					break;
				case FMX_EVENT_KEY_UP:
					MenuSetupVolUnitEdit(MENU_MODE_INC);
					break;
				case FMX_EVENT_KEY_ESC:
					MenuSetupVolUnitEdit(MENU_MODE_EXIT);
					entry_counter = 0;
					menu_index++;
					tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);
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
					FM_DEUBUG_LedError();
				default:
					break;
			}
			break;
		case MENU_SETUP_TIME_UNIT:
			if (!entry_counter)
			{
				MenuSetupTimeUnitEntry();
				entry_counter++;
			}
			switch (this_event)
			{
				case FMX_EVENT_REFRESH:
					MenuSetupTimeUnitEdit(MENU_MODE_REFRESH);
					break;
				case FMX_EVENT_KEY_DOWN:
					MenuSetupTimeUnitEdit(MENU_MODE_DEC);
					break;
				case FMX_EVENT_KEY_UP:
					MenuSetupTimeUnitEdit(MENU_MODE_INC);
					break;
				case FMX_EVENT_KEY_ESC:
					MenuSetupTimeUnitEdit(MENU_MODE_EXIT);
					tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);
					entry_counter = 0;
					menu_index++;
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
					FM_DEUBUG_LedError();
					break;
			}
			break;
		case MENU_SETUP_DATE:
			if (!entry_counter)
			{
				MenuSetupDateEntry();
				entry_counter++;
			}
			switch (this_event)
			{
				case FMX_EVENT_REFRESH:
					MenuSetupDateEdit(MENU_MODE_REFRESH);
					break;
				case FMX_EVENT_KEY_DOWN:
					MenuSetupDateEdit(MENU_MODE_DEC);
					break;
				case FMX_EVENT_KEY_UP:
					MenuSetupDateEdit(MENU_MODE_INC);
					break;
				case FMX_EVENT_KEY_ESC:
					MenuSetupDateEdit(MENU_MODE_EXIT);
					tx_event_flags_set(&event_cb_keypad, (ULONG) FMX_EVENT_REFRESH, TX_OR);
					entry_counter = 0;
					// Las siguientes instrucciones solo al salir del menu de configuración.
					FM_FMC_Init(FM_FACTORY_RAM_BACKUP); // necesario para calcular factores con nueva configuración.
					menu_index = MENU_SETUP_INIT;
					menu_user = TRUE;
					break;
				case FMX_EVENT_KEY_ENTER:
					MenuSetupDateEdit(MENU_MODE_NEXT);
					break;
				case FMX_EVENT_KEY_ENTER_LONG:
					break;
				default:
					FM_DEUBUG_LedError();
					break;
			}
			break;
	  case MENU_SETUP_END:
			break;
		default:
			FM_DEUBUG_LedError();
			break;
	}
	return menu_user;
}

/*
 *	@brief	Función llamada al ingresar al menu de configuraciones.
 *	@note
 *	@param	line_2, buffer que contiene lo que se imprime en la linea 2.
 *	@param	len, tamaño de la password.
 *	@retval	ninguno.
 */
void MenuSetupInitEntry()
{
	const char debug_msg[] = "MenuSetupInitEntry";

	if (fm_debug_uart_msg)
	{
		FM_DEBUG_UartMsg(debug_msg, sizeof(debug_msg));
	}

	FM_LCD_Init(0xff);
}

/*
 * @brief
 * @note
 * @param	pass_tam, longitud de la password en caracteres.
 * @retval
 */
void MenuSetupPasswordEntry(uint8_t pass_len)
{
	const char debug_msg[] = "MenuSetupPasswordEntry";
	const char msg_pass[] = "PASS";

	int i;

	if (fm_debug_uart_msg)
	{
		FM_DEBUG_UartMsg(debug_msg, sizeof(debug_msg));
	}

	for (i = 0; i < pass_len; i++)
	{
		setup_line_2[i] = '_';
	}

	setup_line_2[i] = 0; // Agrego terminador nulo

	FM_LCD_LL_Clear();
	FM_LCD_PutString(msg_pass, sizeof(msg_pass) - 1, FM_LCD_LL_ROW_1);
	FM_LCD_PutString(setup_line_2, strlen(setup_line_2), FM_LCD_LL_ROW_2);
}

/*
 * @brief		función para registrar el ingreso de la password.
 * @param 	mode 0, indica que se aca de ingresar al menu, se debe hacer una configuración inicial.
 * 					mode 1, registra tecla presionada.
 * 					mode 2, solo refresca la pantalla.
 * @retval	ninguno.
 */
void MenuSetupPasswordEnter(uint8_t cmd)
{
	const char debug_msg[] = "MenuSetupPasswordEnter";

	static uint8_t my_index = 0;

	if (fm_debug_uart_msg)
	{
		FM_DEBUG_UartMsg(debug_msg, sizeof(debug_msg));
	}

	switch (cmd)
	{
		case MENU_MODE_INIT:
			my_index = 0;
			break;
		case MENU_MODE_INC:
			my_index++;
			setup_line_2[my_index - 1] = '8';
			break;
		case MENU_MODE_EXIT: // no esta implementado, omito el break para detectar error de entrada.
		default:
			FM_DEUBUG_LedError();
			break;
	}
	FM_LCD_PutString(setup_line_2, strlen(setup_line_2), FM_LCD_LL_ROW_2);
}

/*
 * @brief	función llamada al ingresar al menu de configuración del factor de calibración.
 * @param	ninguno.
 * retval	ninguno.
 */
void MenuSetupFactorCalEntry()
{
	const char debug_msg[] = "MenuSetupFactorCalEntry";
	char msg_cal[] = "  PULS_";

	if (fm_debug_uart_msg)
	{
		FM_DEBUG_UartMsg(debug_msg, sizeof(debug_msg));
	}

	FM_LCD_LL_Clear();

	// Existe una nueva función que hace estos dos pasos en uno, cambiar a nueva función.
	FM_LCD_LL_PutHex_1('L');
	FM_LCD_LL_PutHex_2('T');

	FM_LCD_PutString(msg_cal, strlen(msg_cal), FM_LCD_LL_ROW_2);

	FM_LCD_LL_BlinkWrite(FM_LCD_LL_ROW_1, FM_LCD_LL_ROW_1_COLS - 1, FM_LCD_LL_BLINK_ON_CHAR_ON); // activa parpadeo del digito menos significativo.

	MenuSetupFactorCalEdit(MENU_MODE_INIT);

}

/*
 * @brief	edita el factor de calibración
 * @note
 * @param	mode 0, al ingresar al menu de edición lee el factor actual y se prepara para editar.
 * 			mode 1, incrementa el digito en edición si es menor a 9
 * 			mode 2, decrementa el digito en edición si es mayor a 0
 * 			mode 99, indica que se esta saliendo de este menu,
 * @retval 	retorna una copia del factor original con los cambios realizados desde el ultimo mode 0.
 *
 */
ufp3_t MenuSetupFactorCalEdit(menu_mode_t mode)
{
	const char debug_msg[] = "MenuSetupFactorCalEdit";
	static ufp3_t factor_cal = 0; // copia del valor actual del factor de calibración
	static uint8_t my_index = 0;

	if (fm_debug_uart_msg)
	{
		FM_DEBUG_UartMsg(debug_msg, sizeof(debug_msg));
	}

	switch (mode)
	{
		case MENU_MODE_INIT:
			factor_cal = FM_FMC_FactorCalGet();
			my_index = 0;
			break;
		case MENU_MODE_INC: // incrementa
			if ((factor_cal / power_of_10[my_index]) % 10 != 9)
			{
				factor_cal += power_of_10[my_index];
			}
			break;
		case MENU_MODE_DEC: // decrementa
			if ((factor_cal / power_of_10[my_index]) % 10 != 0)
			{
				factor_cal -= power_of_10[my_index];
			}
			break;
		case MENU_MODE_NEXT:
			if (my_index < 7)
			{
				FM_LCD_LL_BlinkWrite(FM_LCD_LL_ROW_1, (FM_LCD_LL_ROW_1_COLS - 1) - my_index, FM_LCD_LL_BLINK_OFF);
				my_index++;
				FM_LCD_LL_BlinkWrite(FM_LCD_LL_ROW_1, (FM_LCD_LL_ROW_1_COLS - 1) - my_index,
						FM_LCD_LL_BLINK_ON_CHAR_ON);
			}
			else
			{
				FM_LCD_LL_BlinkWrite(FM_LCD_LL_ROW_1, (FM_LCD_LL_ROW_1_COLS - 1) - my_index, FM_LCD_LL_BLINK_OFF);
				my_index = 0;
				FM_LCD_LL_BlinkWrite(FM_LCD_LL_ROW_1, (FM_LCD_LL_ROW_1_COLS - 1) - my_index,
						FM_LCD_LL_BLINK_ON_CHAR_ON);
			}
			break;
		case MENU_MODE_EXIT: // Guarda el valor editado en en la variable de entorno.
			FM_FMC_FactorCalSet(factor_cal);
			FM_LCD_LL_BlinkClear();
			break;
		case MENU_MODE_REFRESH: // Guarda el valor editado en en la variable de entorno.
			break;
		default:
			FM_DEUBUG_LedError(); // Estado no permitido, enciendo led de error.
			break;
	}
	snprintf(setup_line_1, sizeof(setup_line_1), "%05lu.%03lu", factor_cal / 1000, factor_cal % 1000);
	FM_LCD_PutString(setup_line_1, FM_LCD_LL_ROW_1_COLS + 1, FM_LCD_LL_ROW_1);

	return factor_cal;
}

/*
 * @brief	función llamada al ingresar el menu de configuración de la unidad de volumen.
 * @param	ninguno.
 * @retval	ninguno.
 */
void MenuSetupVolUnitEntry()
{
	const char debug_msg[] = "MenuSetupVolUnitEntry";

	if (fm_debug_uart_msg)
	{
		FM_DEBUG_UartMsg(debug_msg, sizeof(debug_msg));
	}

	FM_LCD_LL_BlinkHex(1);

	FM_LCD_LL_Clear();
	MenuSetupVolUnitEdit(MENU_MODE_INIT);
}

/*
 * @brief 	cambia unidad de volumen
 * @param	mode 0, solo refresca
 * 			mode 1, cambia a siguiente unidad de volumen.
 * 			mode 2, cambia a anterior unidad de volumen.
 *
 */
void MenuSetupVolUnitEdit(uint8_t mode)
{
	const char debug_msg[] = "MenuSetupVolUnitEdit";
	char *ptr;

	static fm_fmc_vol_unit_t vol_unit = VOL_UNIT_LT;

	if (fm_debug_uart_msg)
	{
		FM_DEBUG_UartMsg(debug_msg, sizeof(debug_msg));
	}

	switch (mode)
	{
		case MENU_MODE_INIT:
			vol_unit = FM_FMC_TotalizerVolUnitGet();
			break;
		case MENU_MODE_INC:
			if (vol_unit < (VOL_UNIT_END - 1))
			{
				vol_unit++;
			}
			break;
		case MENU_MODE_DEC:
			if (vol_unit > 0)
			{
				vol_unit--;
			}
			break;
		case MENU_MODE_EXIT:
			FM_LCD_LL_BlinkClear();
			FM_FMC_TotalizerVolUnitSet(vol_unit);
			break;
		case MENU_MODE_REFRESH:
			break;
		default:
			FM_DEUBUG_LedError();
			break;
	}

	FM_FMC_TotalizerStrUnitGet(&ptr, vol_unit);
	FM_LCD_PutHex(ptr);
}

/*
 * @brief	función llamada al ingresar el menu de configuración de la unidad de tiempo.
 * @param	ninguno.
 * @retval	ninguno.
 */
void MenuSetupTimeUnitEntry()
{
	const char debug_msg[] = "MenuSetupTimeUnitEntry";
	char *ptr;
	static fm_fmc_vol_unit_t vol_unit = VOL_UNIT_LT;

	if (fm_debug_uart_msg)
	{
		FM_DEBUG_UartMsg(debug_msg, sizeof(debug_msg));
	}
	FM_LCD_LL_Clear();

	// Escribe en pantalla unidad de volumen
	vol_unit = FM_FMC_TotalizerVolUnitGet();
	FM_FMC_TotalizerStrUnitGet(&ptr, vol_unit);
	FM_LCD_PutHex(ptr);

	FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_BACKSLASH, FM_LCD_LL_SYM_ON);

	// La siguiente función es la que se encarga de imprimir la unidad de tiempo.
	MenuSetupTimeUnitEdit(MENU_MODE_INIT);
}

/*
 * @brief 	cambia unidad de volumen
 * @param	mode 0, solo refresca
 * 			mode 1, cambia a siguiente unidad tiempo.
 * 			mode 2, cambia a anterior unidad de tiempo.
 *
 */
void MenuSetupTimeUnitEdit(uint8_t mode)
{
	const char debug_msg[] = "MenuSetupTimeUnitEdit";

	static fm_fmc_time_unit_t time_unit = TIME_UNIT_SECOND;

	if (fm_debug_uart_msg)
	{
		FM_DEBUG_UartMsg(debug_msg, sizeof(debug_msg));
	}

	switch (mode)
	{
		case MENU_MODE_INIT:
			time_unit = FM_FMC_TotalizerTimeUnitGet();
			break;
		case MENU_MODE_INC:
			if (time_unit < (TIME_UNIT_END - 1))
			{
				time_unit++;
			}
			break;
		case MENU_MODE_DEC:
			if (time_unit > TIME_UNIT_SECOND)
			{
				time_unit--;
			}
			break;
		case MENU_MODE_EXIT:
			if (!FM_FMC_TotalizerTimeUnitSet(time_unit))
			{
				FM_DEUBUG_LedError(); // Si no se acepta nueva unidad de tiempo se enciende led de error.
			}
			break;
		case MENU_MODE_REFRESH:
			break;
		default:
			FM_DEUBUG_LedError();
			break;
	}
	FM_FMC_TimeUnitSymbolWrite(time_unit);
}

/*
 * @brief		ingreso al menu de configuración de hora, minuto y segundo.
 * @param		ninguno.
 * @retval	ninguno.
 */
void MenuSetupDateEntry()
{
	const char msg_debug_data_time_entry[] = "date&time entry";
	if (fm_debug_uart_msg)
	{
		FM_DEBUG_UartMsg(msg_debug_data_time_entry, sizeof(msg_debug_data_time_entry));
	}

	// activa parpadeo del digito menos significativo.
	FM_LCD_LL_BlinkWrite(FM_LCD_LL_ROW_1, FM_LCD_LL_ROW_1_COLS - 1, FM_LCD_LL_BLINK_ON_CHAR_ON);

	FM_LCD_LL_Clear();
	MenuSetupDateEdit(MENU_MODE_INIT);
}

/*
 * @brief 	edición de hora, minuto y segundo.
 * @param		mode, comando a ejecutar.
 * @retval	ninguno.
 */
void MenuSetupDateEdit(menu_mode_t sel)
{
	static fm_rtc_set_t idx = FM_RTC_SET_YEAR_UNIT; //

	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;

	switch (sel)
	{
		case MENU_MODE_INIT:
			break;
		case MENU_MODE_INC:
			FM_RTC_Set(idx, 1);
			break;
		case MENU_MODE_DEC:
			FM_RTC_Set(idx, 0);
			break;
		case MENU_MODE_NEXT:
			FM_LCD_LL_BlinkClear();
			if(idx < FM_RTC_SET_DAY_TEN)
			{
				idx++;
			}
			else
			{
				idx = 0;
			}

			switch(idx)
			{
				case FM_RTC_SET_YEAR_UNIT:
					FM_LCD_LL_BlinkWrite(FM_LCD_LL_ROW_1, FM_LCD_LL_ROW_1_COLS - 1, FM_LCD_LL_BLINK_ON_CHAR_ON);
					break;
				case FM_RTC_SET_YEAR_TEN:
					FM_LCD_LL_BlinkWrite(FM_LCD_LL_ROW_1, FM_LCD_LL_ROW_1_COLS - 2, FM_LCD_LL_BLINK_ON_CHAR_ON);
					break;
				case FM_RTC_SET_MONTH_UNIT:
					FM_LCD_LL_BlinkWrite(FM_LCD_LL_ROW_1, FM_LCD_LL_ROW_1_COLS - 5, FM_LCD_LL_BLINK_ON_CHAR_ON);
					break;
				case FM_RTC_SET_MONTH_TEN:
					FM_LCD_LL_BlinkWrite(FM_LCD_LL_ROW_1, FM_LCD_LL_ROW_1_COLS - 6, FM_LCD_LL_BLINK_ON_CHAR_ON);
					break;
				case FM_RTC_SET_DAY_UNIT:
					FM_LCD_LL_BlinkWrite(FM_LCD_LL_ROW_1, FM_LCD_LL_ROW_1_COLS - 7, FM_LCD_LL_BLINK_ON_CHAR_ON);
					break;
				case FM_RTC_SET_DAY_TEN:
					FM_LCD_LL_BlinkWrite(FM_LCD_LL_ROW_1, FM_LCD_LL_ROW_1_COLS - 8, FM_LCD_LL_BLINK_ON_CHAR_ON);
					break;
				default:
					break;
			}
			break;
		case MENU_MODE_EXIT:
			FM_LCD_LL_BlinkClear();
			break;
		case MENU_MODE_REFRESH:
			break;
		default:
			FM_DEUBUG_LedError();
			break;
	}

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	sprintf(setup_line_2, "%02d.%02d.%02d ", time.Hours, time.Minutes, time.Seconds);
	sprintf(setup_line_1, "%02d.%02d.20%02d ", date.Date, date.Month, date.Year);

	FM_LCD_PutString(setup_line_1, FM_LCD_LL_ROW_1_COLS, FM_LCD_LL_ROW_1);
	FM_LCD_PutString(setup_line_2, FM_LCD_LL_ROW_2_COLS, FM_LCD_LL_ROW_2);
}



// Interrupts

/*** end of file ***/
