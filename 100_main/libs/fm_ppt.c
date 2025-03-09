/*
 * @brief Portable Printer Thermal.
 *
 * Version: 1
 * Autor: Daniel H Sagarra.
 * Fecha: 2/3/2025
 * - Version inicial.
 *
 *
 */

// Includes.
#include "fm_fmc.h"
#include "fm_mxc.h"
#include "string.h"
#include "stdio.h"
#include "main.h"
#include "fm_rtc.h"

// Defines.
#define RIGHT_LEN 18 // Cantidad de columnas de la impresora
#define LEFT_LEN 12 // Cantidad de columnas de la impresora
#define PRINTER_LEN 32      // Cantidad de columnas de la impresora
#define MAX_FIELD_LEN (PRINTER_LEN + 2)  // Columnas de la impresora + \n\0


// Typedef.
typedef struct {
    char number[RIGHT_LEN];
    char ttl[RIGHT_LEN];
    char date[RIGHT_LEN];   // formato esperado: "DD/MM/AAAA"
    char time[RIGHT_LEN];   // formato esperado: "HH:MM" (o "HH:MM:SS")
    char acm[RIGHT_LEN];
}ticket_data_t;

// Const data.

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.

// Global variables, statics.
ticket_data_t ticket;

// Private function prototypes.

// Private function bodies.

// Public function bodies.

void FM_PPT_FormatTicket()
{

  snprintf(ticket.number, MAX_FIELD_LEN, "%u", FM_FMC_TicketNumberGet());
  snprintf(ticket.ttl, MAX_FIELD_LEN, "%lu", FM_FMC_TtlGet());
  FM_RTC_GetPpt(ticket.time, ticket.date);
  snprintf(ticket.acm, MAX_FIELD_LEN, "%lu", FM_FMC_AcmGet());
}

void FM_PPT_PrintTicket()
{
  char line[MAX_FIELD_LEN];

  //Línea separadora superior
  snprintf(line, MAX_FIELD_LEN, "  ============================\n");
  FM_MXC_Print(line);

  // Número de control (etiqueta izquierda, valor derecha)
  snprintf(line, MAX_FIELD_LEN, "%-15s%s\n", "Ticket Nro:", ticket.number);
  FM_MXC_Print(line);

  // TTL
  snprintf(line, MAX_FIELD_LEN, "%-15s%s\n", "TTL:", ticket.ttl);
  FM_MXC_Print(line);

  // Fecha
  snprintf(line, MAX_FIELD_LEN, "%-15s%s\n", "Fecha:", ticket.date);
  FM_MXC_Print(line);

  // Hora
  snprintf(line, MAX_FIELD_LEN, "%-15s%s\n", "Hora:", ticket.time);
  FM_MXC_Print(line);

  // ACM
  snprintf(line, MAX_FIELD_LEN, "%-15s%s\n", "ACM:", ticket.acm);
  FM_MXC_Print(line);


  snprintf(line, MAX_FIELD_LEN, "\n\n\n\n");
  FM_MXC_Print(line);

  // Operario (dejar en blanco para llenar a mano)
  snprintf(line, MAX_FIELD_LEN, "Operario: \n\n\n\n");
  FM_MXC_Print(line);


  // Recibió (dejar en blanco para llenar a mano)
  snprintf(line, MAX_FIELD_LEN, "Recibió: \n\n\n\n");
  FM_MXC_Print(line);

  // (Opcional) Línea separadora inferior
  snprintf(line, MAX_FIELD_LEN, "  ============================  \n");
  FM_MXC_Print(line);

  snprintf(line, MAX_FIELD_LEN, "\n\n\n\n");
  FM_MXC_Print(line);
}

// Interrupts

/*** end of file ***/

