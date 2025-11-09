/*
 * Computador de caudales -> Medición de frecuencia
 *
 * Originalmente se tenia:
 * El LPTIM 3 como fuente de reloj a LSE 32768Hz.
 * Canal 1 de LPTIM 3 modo captura, fuente LPTIM3 CH1->PD14.
 * La señal que ingresa al canal 1es onda cuadrada, desde el sensor primario.
 * El registro CCR1 debería tener siempre la ultima captura, lectura del reloj
 * que correo a 32768Hz.
 *
 * LPTIM 4, como contador de los pulsos del sensor primario.
 *
 * La idea es simple:
 * Esperamos un flanco descendente del sensor primario y
 * 1) Anotamos la captura del LPTIM 3
 * 2) Anotamos el contador del LPTIM 4
 *
 * Para tener buena resolución no medimos entre flancos, esperamos 1 segundo,
 * luego de un segundo esperamos el siguiente flanco.
 *
 * Los datos que tenemos son pulsos del sensor primario como un delta de pulsos
 * medidos en el LPTIM 4, su contador, y la cantidad de pulsos de 32768Hz entre
 * estos dos pulsos.
 *
 * El problema que se presente es que no su puedo hacer la captura funcionara
 * correctamente, capturaba bien para un numero de pulsos impares pero para
 * pulsos pares se congelaba el refresco de CCR1.
 *
 * ES INCREIBLE QUE EL MICROCONTROLADOR FALLLE DE ESTA FORMA, SE REQUIERE
 * INVESTIGACION ADICIONAL!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 * Es importante notar que esto se da solamente si los pulsos que disparan la
 * captura se producen durante stop mode.
 *
 * Se deicidio cambiar el esquema a habilitar una interrupción, la de captura.
 * La interrupción de captura no se la puede dejar todo el tiempo habilitada,
 * despertaría al micro a la frecuencia de la señal del sensor primario. Es
 * suficiente con habilitar est cada un segundo, se obtienen los mismos datos
 * que se pretendían, con la penalidad de una salida de stop mode adicional
 * al refrescar los calor de captura en LPTIM 3 y contador en LPTIM 4.
 *
 * Autor: DHS
 * Fecha: 13/10/2024
 * Version 1:
 * Notas: Version inicial
 *
 *
 */

// Includes.
#include "fm_debug.h"
#include "fm_main.h"
#include "main.h"

// Typedef.

// Const data.

// Defines.

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.
extern LPTIM_HandleTypeDef hlptim3;
extern LPTIM_HandleTypeDef hlptim4;

// Global variables, statics
int refresh_label = 0;

uint16_t lptim3_capture_new;
uint16_t lptim3_capture_old;
uint16_t lptim3_capture_delta;

uint16_t lptim4_counter_new;
uint16_t lptim4_counter_old;
uint16_t lptim4_counter_delta;

uint32_t freq;

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief   Loop infinito flowmeet.
 * @Note    El programa principal se desarrolla en este modulo. No escribir código en archivos.
 *          generados  por los IDEs.
 * @param   Ninguno.
 * @retval  Ninguno.
 *
 */
void FM_MAIN_Main()
{
  __HAL_RCC_LPTIM3_CLKAM_ENABLE();
  __HAL_RCC_LPTIM4_CLKAM_ENABLE();


  HAL_LPTIM_IC_Start_IT(&hlptim3, LPTIM_CHANNEL_1);
  HAL_LPTIM_Counter_Start(&hlptim4);

  HAL_Delay(100);

  for (;;)
  {
    if(refresh_label)
    {
      FM_DEBUG_UartUint32(freq);
      FM_DEBUG_UartUint32(lptim4_counter_delta);
      FM_DEBUG_UartUint32(lptim3_capture_delta);

    }
    refresh_label = 0;

    HAL_GPIO_WritePin(LED_ACTIVE_GPIO_Port, LED_ACTIVE_Pin, GPIO_PIN_RESET);
    HAL_SuspendTick();
    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
    HAL_ResumeTick();
    HAL_GPIO_WritePin(LED_ACTIVE_GPIO_Port, LED_ACTIVE_Pin, GPIO_PIN_SET);
  }
}

// Interrupts

/**
  * @brief  Esta función esta declarada como weak, redefino.
  * @param  hlptim LPTIM handle
  * @retval None
  */
void HAL_LPTIM_IC_CaptureCallback(LPTIM_HandleTypeDef *hlptim)
{
  lptim3_capture_old = lptim3_capture_new;
  lptim3_capture_new = LPTIM3->CCR1;
  lptim3_capture_delta = lptim3_capture_new - lptim3_capture_old;

  lptim4_counter_old = lptim4_counter_new;
  lptim4_counter_new = LPTIM4->CNT;
  lptim4_counter_delta = lptim4_counter_new - lptim4_counter_old;

  freq = lptim4_counter_delta;
  freq *= 32768;
  freq *= 10;
  freq /= lptim3_capture_delta;

  __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_CC1);
}


/**
  * @brief  Wake Up Timer callback.
  * @param  hrtc RTC handle
  * @retval None
  */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);
  __HAL_LPTIM_CLEAR_FLAG(&hlptim3, LPTIM_FLAG_CC1);
  __HAL_LPTIM_ENABLE_IT(&hlptim3, LPTIM_IT_CC1);
  refresh_label = 1;
}



/*** end of file ***/

