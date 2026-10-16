//###############################################
//# ###                                     ### #
//# ### PI Mercury C-663 Status Parser      ### #
//# ### (single axis)                       ### #
//# ###                                     ### #
//# ### author: D. Hampai - M. Montis       ### #
//# ###                                     ### #
//# ### Ref 2.0; 2026-10-09                 ### #
//###############################################

/*
 *
 * Interpretazione dei registri di status del controller PI Mercury C-663.
 * Interfaccia IDENTICA a motor_status.c (mm4006) per permettere
 * la generalizzazione dei file SNL.
 *
 * Formato risposta SRG?:
 *   "0 2 1 1=0x900A"
 *   Il valore esadecimale e' dopo il carattere '='
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "motor_status.h"

/* ════════════════════════════════════════════════
 *  Funzione interna: stampa valore a 16 bit in binario
 * ════════════════════════════════════════════════ */
static void print_binary_16(unsigned int value)
{
    int bit;
    for (bit = 15; bit >= 0; bit--) {
        printf("%d", (value >> bit) & 1);
        if (bit == 8) printf(" ");
    }
}

/* ════════════════════════════════════════════════
 *  Funzione interna: estrae il valore hex dalla stringa SRG?
 *
 *  Formato atteso: "0 2 1 1=0x900A"
 *  Cerca il carattere '=' e converte la parte destra in unsigned int.
 *  Supporta sia "0x900A" che "900A" (con o senza prefisso 0x).
 *
 *  Ritorna: valore unsigned int, oppure 0xFFFFFFFF in caso di errore.
 * ════════════════════════════════════════════════ */
static unsigned int extract_srg_hex(const char *srg_str)
{
    const char *eq_ptr;
    const char *hex_ptr;
    char *endptr;
    unsigned int value;

    if (srg_str == NULL || srg_str[0] == '\0') {
        printf("WARN extract_srg_hex: stringa vuota o NULL\n");
        return 0xFFFFFFFF;
    }

    /* Cerca il carattere '=' nella stringa */
    eq_ptr = strchr(srg_str, '=');
    if (eq_ptr == NULL) {
        printf("ERROR extract_srg_hex: "
               "carattere '=' non trovato in '%s'\n", srg_str);
        return 0xFFFFFFFF;
    }

    /* Punta al carattere dopo '=' */
    hex_ptr = eq_ptr + 1;

    /* Salta eventuali spazi */
    while (*hex_ptr == ' ') hex_ptr++;

    /* Converte la stringa hex in unsigned int
     * strtoul con base 16 gestisce sia "0x900A" che "900A" */
    if (strncmp(hex_ptr, "0x", 2) == 0 ||
        strncmp(hex_ptr, "0X", 2) == 0) {
        value = (unsigned int)strtoul(hex_ptr, &endptr, 16);
    } else {
        value = (unsigned int)strtoul(hex_ptr, &endptr, 16);
    }

    if (endptr == hex_ptr) {
        printf("ERROR extract_srg_hex: "
               "valore hex non valido: '%s'\n", hex_ptr);
        return 0xFFFFFFFF;
    }

    printf("DEBUG extract_srg_hex: "
           "'%s' -> 0x%04X (%u)\n", srg_str, value, value);

    return value;
}

/* ════════════════════════════════════════════════
 *  SRG?  —  parse_motor_status()
 *
 *  Accetta il valore raw come int (gia' convertito).
 *  Per parsing dalla stringa usa parse_motor_status_from_string().
 * ════════════════════════════════════════════════ */
int parse_motor_status(int srg_value, MotorStatus *ms)
{
    unsigned int v;

    if (ms == NULL) return -1;
    if (srg_value < 0) {
        printf("WARN parse_motor_status: valore negativo ignorato\n");
        return -1;
    }

    v = (unsigned int)srg_value;
    ms->raw = v;

    /* Bit 13: in movimento */
    ms->in_motion      = (v & SRG_BIT_IN_MOTION)      ? 1 : 0;
    /* Bit 12: motore acceso */
    ms->power_on       = (v & SRG_BIT_MOTOR_ON)        ? 1 : 0;
    /* dir_positive: non disponibile su C663 */
    ms->dir_positive   = 0;
    /* Bit 2: limit positivo (+) */
    ms->limit_right    = (v & SRG_BIT_LIMIT_POS)       ? 1 : 0;
    /* Bit 0: limit negativo (-) */
    ms->limit_left     = (v & SRG_BIT_LIMIT_NEG)       ? 1 : 0;
    /* Bit 3: referenziato (mech_zero per compatibilita') */
    ms->mech_zero      = (v & SRG_BIT_REFERENCED)      ? 1 : 0;
    /* Campi aggiuntivi C663 */
    ms->error          = (v & SRG_BIT_ERROR)           ? 1 : 0;
    ms->on_target      = (v & SRG_BIT_ON_TARGET)       ? 1 : 0;
    ms->is_referencing = (v & SRG_BIT_IS_REFERENCING)  ? 1 : 0;
    ms->ref_switch     = (v & SRG_BIT_REF_SWITCH)      ? 1 : 0;
    ms->dig_in[0]      = (v & SRG_BIT_DIG_IN1)         ? 1 : 0;
    ms->dig_in[1]      = (v & SRG_BIT_DIG_IN2)         ? 1 : 0;
    ms->dig_in[2]      = (v & SRG_BIT_DIG_IN3)         ? 1 : 0;
    ms->dig_in[3]      = (v & SRG_BIT_DIG_IN4)         ? 1 : 0;

    return 0;
}

/* ── parse_motor_status_from_string() ──
 *
 * Questa e' la funzione principale da usare nello SNL.
 * Accetta la stringa completa della risposta SRG?
 * nel formato "0 2 1 1=0x900A" e la decodifica.
 */
int parse_motor_status_from_string(const char *srg_str, MotorStatus *ms)
{
    unsigned int hex_value;

    if (srg_str == NULL || ms == NULL) return -1;

    hex_value = extract_srg_hex(srg_str);
    if (hex_value == 0xFFFFFFFF) {
        printf("ERROR parse_motor_status_from_string: "
               "parsing fallito per '%s'\n", srg_str);
        return -1;
    }

    return parse_motor_status((int)hex_value, ms);
}

/* ── Accesso diretto ai bit SRG ── */

int ms_is_in_motion(int srg_value)
{
    return ((unsigned int)srg_value & SRG_BIT_IN_MOTION)  ? 1 : 0;
}

int ms_is_power_on(int srg_value)
{
    return ((unsigned int)srg_value & SRG_BIT_MOTOR_ON)   ? 1 : 0;
}

int ms_is_limit_right(int srg_value)
{
    return ((unsigned int)srg_value & SRG_BIT_LIMIT_POS)  ? 1 : 0;
}

int ms_is_limit_left(int srg_value)
{
    return ((unsigned int)srg_value & SRG_BIT_LIMIT_NEG)  ? 1 : 0;
}

/* ── Controlli composti ── */

int ms_status_ok(int srg_value)
{
    if (!ms_is_power_on(srg_value))    return 0;
    if (ms_is_limit_right(srg_value))  return 0;
    if (ms_is_limit_left(srg_value))   return 0;
    if ((unsigned int)srg_value & SRG_BIT_ERROR) return 0;
    return 1;
}

int ms_is_ready(int srg_value)
{
    if (!ms_is_power_on(srg_value))    return 0;
    if (ms_is_in_motion(srg_value))    return 0;
    if (ms_is_limit_right(srg_value))  return 0;
    if (ms_is_limit_left(srg_value))   return 0;
    if ((unsigned int)srg_value & SRG_BIT_ERROR) return 0;
    return 1;
}

/* ── ms_to_epics() ── */

void ms_to_epics(int srg_value,
                 int *out_in_motion,
                 int *out_power_on,
                 int *out_limit_right,
                 int *out_limit_left)
{
    if (out_in_motion)   *out_in_motion   = ms_is_in_motion(srg_value);
    if (out_power_on)    *out_power_on    = ms_is_power_on(srg_value);
    if (out_limit_right) *out_limit_right = ms_is_limit_right(srg_value);
    if (out_limit_left)  *out_limit_left  = ms_is_limit_left(srg_value);
}

/* ── print_motor_status() ── */

void print_motor_status(const char *label, int srg_value)
{
    MotorStatus ms;
    unsigned int v = (unsigned int)srg_value;

    parse_motor_status(srg_value, &ms);

    printf("=== SRG Status: %s ===\n", label ? label : "?");
    printf("  Risposta   : SRG? -> 0x%04X (%d)\n", v, srg_value);
    printf("  Binario    : ");
    print_binary_16(v);
    printf("\n");
    printf("               FEDC BA98 7654 3210\n");
    printf("  ---\n");
    printf("  Bit  0 - Limit NEG (-)   : %s\n",
           ms.limit_left      ? "SCATTATO !!!"        : "Non scattato");
    printf("  Bit  1 - Ref switch      : %s\n",
           ms.ref_switch      ? "Non attivo"          : "Attivo");
    printf("  Bit  2 - Limit POS (+)   : %s\n",
           ms.limit_right     ? "SCATTATO !!!"        : "Non scattato");
    printf("  Bit  3 - Referenziato    : %s\n",
           ms.mech_zero       ? "SI"                  : "NO");
    printf("  Bit 4-7 - Digital IN 1-4 : %d %d %d %d\n",
           ms.dig_in[0], ms.dig_in[1],
           ms.dig_in[2], ms.dig_in[3]);
    printf("  Bit  8 - Error flag      : %s\n",
           ms.error           ? "ERRORE PENDENTE !!!" : "Nessun errore");
    printf("  Bit 12 - Motor ON        : %s\n",
           ms.power_on        ? "SI (acceso)"         : "NO (spento)");
    printf("  Bit 13 - In motion       : %s\n",
           ms.in_motion       ? "SI - in movimento"   : "NO - fermo");
    printf("  Bit 14 - Is referencing  : %s\n",
           ms.is_referencing  ? "SI"                  : "NO");
    printf("  Bit 15 - On target       : %s\n",
           ms.on_target       ? "SI - posiz. ragg."   : "NO");
    printf("  ---\n");
    printf("  Status OK                : %s\n",
           ms_status_ok(srg_value) ? "SI" : "NO");
    printf("  Pronto a muoversi        : %s\n",
           ms_is_ready(srg_value)  ? "SI" : "NO");
    printf("========================\n");
}

/* ════════════════════════════════════════════════
 *  SVO?  —  parse_total_status()
 * ════════════════════════════════════════════════ */
int parse_total_status(int svo_value, TotalStatus *ts)
{
    if (ts == NULL) return -1;

    ts->raw      = svo_value;
    ts->power_on = (svo_value != 0) ? 1 : 0;

    /* SVO? e' globale: axis_moving non disponibile,
     * usare SRG? per ogni singolo asse */
    ts->axis1_moving = 0;
    ts->axis2_moving = 0;
    ts->axis3_moving = 0;
    ts->axis4_moving = 0;
    ts->srq          = 0;

    return 0;
}

int ts_is_power_on(int svo_value)
{
    return (svo_value != 0) ? 1 : 0;
}

int ts_any_axis_moving(int svo_value)
{
    (void)svo_value;
    return 0;
}

int ts_is_srq(int svo_value)
{
    (void)svo_value;
    return 0;
}

void ts_to_epics(int svo_value,
                 int *out_axis1_moving,
                 int *out_axis2_moving,
                 int *out_axis3_moving,
                 int *out_axis4_moving,
                 int *out_power_on,
                 int *out_srq)
{
    if (out_axis1_moving) *out_axis1_moving = 0;
    if (out_axis2_moving) *out_axis2_moving = 0;
    if (out_axis3_moving) *out_axis3_moving = 0;
    if (out_axis4_moving) *out_axis4_moving = 0;
    if (out_power_on)     *out_power_on     = ts_is_power_on(svo_value);
    if (out_srq)          *out_srq          = 0;
}

void print_total_status(int svo_value)
{
    TotalStatus ts;
    parse_total_status(svo_value, &ts);

    printf("=== SVO Status (Global) ===\n");
    printf("  Risposta   : SVO? -> %d\n", svo_value);
    printf("  ---\n");
    printf("  Motor power  : %s\n",
           ts.power_on ? "ON (acceso)" : "OFF (spento)");
    printf("  Nota: in_motion non disponibile da SVO?,\n");
    printf("        usare SRG? per ogni asse.\n");
    printf("==========================\n");
}
