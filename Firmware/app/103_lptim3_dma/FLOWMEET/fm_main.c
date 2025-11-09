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

static volatile uint32_t lptim3_capture_dma;


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

  if (HAL_LPTIM_IC_Start_DMA(&hlptim3, LPTIM_CHANNEL_1, (uint32_t *)&lptim3_capture_dma, 1) != HAL_OK)
  {
    FM_DEBUG_UartUint32(0xDEADBEEF);
  }

  /*__HAL_DMA_DISABLE_IT(hlptim3.hdma[LPTIM_DMA_ID_CC1],
                       DMA_IT_TC | DMA_IT_HT | DMA_IT_DTE |
                       DMA_IT_ULE | DMA_IT_USE | DMA_IT_SUSP | DMA_IT_TO);
  HAL_NVIC_DisableIRQ(LPDMA1_Channel3_IRQn);
  HAL_NVIC_ClearPendingIRQ(LPDMA1_Channel3_IRQn);

  */

  HAL_Delay(100);

  for (;;)
  {
    uint32_t capture_copy = (lptim3_capture_dma & 0xFFFFu);
    FM_DEBUG_UartUint32(capture_copy);

    HAL_GPIO_WritePin(LED_ACTIVE_GPIO_Port, LED_ACTIVE_Pin, GPIO_PIN_RESET);
    HAL_SuspendTick();
    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
    HAL_ResumeTick();
    HAL_GPIO_WritePin(LED_ACTIVE_GPIO_Port, LED_ACTIVE_Pin, GPIO_PIN_SET);
  }
}

// Interrupts




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
}



/*** end of file ***/




