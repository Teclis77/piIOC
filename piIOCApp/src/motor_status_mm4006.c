/*
 * motor_status.c
 *
 * Implementazione dell'interpretazione dei byte di status motore.
 *
 * Supporta due comandi:
 *   xxMS  - status asse singolo  (risposta "xxMSch")
 *   TS    - status globale       (risposta "TSch")
 *
 * Compilare e linkare all'IOC EPICS:
 *   myioc_SRCS += motor_status.c
 */

#include <stdio.h>
#include <string.h>
#include "motor_status.h"

/* ════════════════════════════════════════════════
 *  Funzione interna: stampa byte in binario
 * ════════════════════════════════════════════════ */
static void print_binary(unsigned char byte)
{
    int bit;
    for (bit = 7; bit >= 0; bit--) {
        printf("%d", (byte >> bit) & 1);
        if (bit == 4) printf(" ");   /* spazio visivo tra nibble */
    }
}

/* ════════════════════════════════════════════════
 *  xxMS  —  parse_motor_status()
 * ════════════════════════════════════════════════ */
int parse_motor_status(char c, MotorStatus *ms)
{
    unsigned char byte;

    if (ms == NULL) return -1;

    byte = (unsigned char)c;

//	printf("Debug in motor_status.c: Char %c, Byte %c\n", c, byte);

    ms->raw          = byte;

    /* Bit 0: asse in movimento (1=YES) */
    ms->in_motion    = (byte & MS_BIT_IN_MOTION)    ? 1 : 0;

    /* Bit 1: power (0=ON, 1=OFF) -> invertiamo per semantica positiva */
    ms->power_on     = (byte & MS_BIT_POWER_OFF)    ? 0 : 1;

    /* Bit 2: direzione (0=negativa, 1=positiva) */
    ms->dir_positive = (byte & MS_BIT_DIR_POSITIVE) ? 1 : 0;

    /* Bit 3: fine corsa destro (+) */
    ms->limit_right  = (byte & MS_BIT_LIMIT_RIGHT)  ? 1 : 0;

    /* Bit 4: fine corsa sinistro (-) */
    ms->limit_left   = (byte & MS_BIT_LIMIT_LEFT)   ? 1 : 0;

    /* Bit 5: zero meccanico */
    ms->mech_zero    = (byte & MS_BIT_MECH_ZERO)    ? 1 : 0;

    return 0;
}

/* ── Accesso diretto ai bit xxMS ── */

int ms_is_in_motion(char c)
{
    return ((unsigned char)c & MS_BIT_IN_MOTION)  ? 1 : 0;
}

int ms_is_power_on(char c)
{
    /* Bit 1 hardware = 1 -> power OFF; invertiamo */
    return ((unsigned char)c & MS_BIT_POWER_OFF)  ? 0 : 1;
}

int ms_is_limit_right(char c)
{
    return ((unsigned char)c & MS_BIT_LIMIT_RIGHT) ? 1 : 0;
}

int ms_is_limit_left(char c)
{
    return ((unsigned char)c & MS_BIT_LIMIT_LEFT)  ? 1 : 0;
}

/* ── Controlli composti xxMS ── */

int ms_status_ok(char c)
{
    if (!ms_is_power_on(c))    return 0;
    if (ms_is_limit_right(c))  return 0;
    if (ms_is_limit_left(c))   return 0;
    return 1;
}

int ms_is_ready(char c)
{
    if (!ms_is_power_on(c))    return 0;
    if (ms_is_in_motion(c))    return 0;
    if (ms_is_limit_right(c))  return 0;
    if (ms_is_limit_left(c))   return 0;
    return 1;
}

/* ── ms_to_epics() ── */

void ms_to_epics(char c,
                 int *out_in_motion,
                 int *out_power_on,
                 int *out_limit_right,
                 int *out_limit_left)
{
    if (out_in_motion)   *out_in_motion   = ms_is_in_motion(c);
    if (out_power_on)    *out_power_on    = ms_is_power_on(c);
    if (out_limit_right) *out_limit_right = ms_is_limit_right(c);
    if (out_limit_left)  *out_limit_left  = ms_is_limit_left(c);
}

/* ── print_motor_status() ── */

void print_motor_status(const char *label, char c)
{
    MotorStatus ms;
    unsigned char byte = (unsigned char)c;

    parse_motor_status(c, &ms);

    printf("=== xxMS Status: %s ===\n", label ? label : "?");
    printf("  Risposta   : %sMSch  (ch=0x%02X, dec=%d)\n",
           label ? label : "xx", byte, byte);
    printf("  Binario    : ");
    print_binary(byte);
    printf("\n");
    printf("               76543 210\n");
    printf("  ---\n");
    printf("  Bit 0 - In motion    : %s\n",
           ms.in_motion    ? "YES - in movimento"   : "NO  - fermo");
    printf("  Bit 1 - Motor power  : %s\n",
           ms.power_on     ? "ON"                   : "OFF");
    printf("  Bit 2 - Direction    : %s\n",
           ms.dir_positive ? "Positiva (+)"         : "Negativa (-)");
    printf("  Bit 3 - Limit right+ : %s\n",
           ms.limit_right  ? "SCATTATO !!!"         : "Non scattato");
    printf("  Bit 4 - Limit left-  : %s\n",
           ms.limit_left   ? "SCATTATO !!!"         : "Non scattato");
    printf("  Bit 5 - Mech zero    : %s\n",
           ms.mech_zero    ? "High"                 : "Low");
    printf("  ---\n");
    printf("  Status OK            : %s\n",
           ms_status_ok(c) ? "SI"                   : "NO");
    printf("  Pronto a muoversi    : %s\n",
           ms_is_ready(c)  ? "SI"                   : "NO");
    printf("========================\n");
}

/* ════════════════════════════════════════════════
 *  TS  —  parse_total_status()
 * ════════════════════════════════════════════════ */
int parse_total_status(char c, TotalStatus *ts)
{
    unsigned char byte;

    if (ts == NULL) return -1;

    if (c == '\0') {
	printf("WARN parse_total_status: char=0x00 ignorato\n");
	return -1;
    }

    byte = (unsigned char)c;
    ts->raw = byte;

//	printf("Debug in motor_status.c: Char %c, Byte %c\n", c, byte);

    /* Bit 0..3: stato assi 1..4 (0=fermo, 1=in moto) */
    ts->axis1_moving = (byte & TS_BIT_AXIS1_MOVING) ? 1 : 0;
    ts->axis2_moving = (byte & TS_BIT_AXIS2_MOVING) ? 1 : 0;
    ts->axis3_moving = (byte & TS_BIT_AXIS3_MOVING) ? 1 : 0;
    ts->axis4_moving = (byte & TS_BIT_AXIS4_MOVING) ? 1 : 0;

    /* Bit 4: power (0=ON, 1=OFF) -> invertiamo */
    ts->power_on = (byte & TS_BIT_POWER_OFF) ? 0 : 1;

    /* Bit 7: IEEE SRQ */
    ts->srq = (byte & TS_BIT_SRQ) ? 1 : 0;

    return 0;
}

/* ── Accesso diretto ai bit TS ── */

int ts_is_axis_moving(char c, int axis)
{
    unsigned char byte = (unsigned char)c;
    switch (axis) {
        case 1: return (byte & TS_BIT_AXIS1_MOVING) ? 1 : 0;
        case 2: return (byte & TS_BIT_AXIS2_MOVING) ? 1 : 0;
        case 3: return (byte & TS_BIT_AXIS3_MOVING) ? 1 : 0;
        case 4: return (byte & TS_BIT_AXIS4_MOVING) ? 1 : 0;
        default:
            printf("WARN ts_is_axis_moving: asse %d non valido (1..4)\n",
                   axis);
            return 0;
    }
}

int ts_is_power_on(char c)
{
    /* Bit 4 hardware = 1 -> power OFF; invertiamo */
    return ((unsigned char)c & TS_BIT_POWER_OFF) ? 0 : 1;
}

int ts_is_srq(char c)
{
    return ((unsigned char)c & TS_BIT_SRQ) ? 1 : 0;
}

int ts_any_axis_moving(char c)
{
    unsigned char byte = (unsigned char)c;
    /* Bit 0..3 = assi 1..4 */
    return (byte & 0x0F) ? 1 : 0;
}

/* ── ts_to_epics() ── */

void ts_to_epics(char c,
                 int *out_axis1_moving,
                 int *out_axis2_moving,
                 int *out_axis3_moving,
                 int *out_axis4_moving,
                 int *out_power_on,
                 int *out_srq)
{
    if (out_axis1_moving) *out_axis1_moving = ts_is_axis_moving(c, 1);
    if (out_axis2_moving) *out_axis2_moving = ts_is_axis_moving(c, 2);
    if (out_axis3_moving) *out_axis3_moving = ts_is_axis_moving(c, 3);
    if (out_axis4_moving) *out_axis4_moving = ts_is_axis_moving(c, 4);
    if (out_power_on)     *out_power_on     = ts_is_power_on(c);
    if (out_srq)          *out_srq          = ts_is_srq(c);
}

/* ── print_total_status() ── */

void print_total_status(char c)
{
    TotalStatus ts;
    unsigned char byte = (unsigned char)c;

    parse_total_status(c, &ts);

    printf("=== TS Total Status ===\n");
    printf("  Risposta   : TSch  (ch=0x%02X, dec=%d)\n", byte, byte);
    printf("  Binario    : ");
    print_binary(byte);
    printf("\n");
    printf("               76543 210\n");
    printf("  ---\n");
    printf("  Bit 0 - Asse #1      : %s\n",
           ts.axis1_moving ? "In movimento"  : "Fermo");
    printf("  Bit 1 - Asse #2      : %s\n",
           ts.axis2_moving ? "In movimento"  : "Fermo");
    printf("  Bit 2 - Asse #3      : %s\n",
           ts.axis3_moving ? "In movimento"  : "Fermo");
    printf("  Bit 3 - Asse #4      : %s\n",
           ts.axis4_moving ? "In movimento"  : "Fermo");
    printf("  Bit 4 - Motor power  : %s\n",
           ts.power_on     ? "ON"            : "OFF");
    printf("  Bit 5 - Not used     : -\n");
    printf("  Bit 6 - Not used     : -\n");
    printf("  Bit 7 - IEEE SRQ     : %s\n",
           ts.srq          ? "YES (attivo)"  : "NO");
    printf("  ---\n");
    printf("  Qualche asse in moto : %s\n",
           ts_any_axis_moving(c) ? "SI" : "NO");
    printf("  Power ON             : %s\n",
           ts.power_on ? "SI" : "NO");
    printf("=======================\n");
}
