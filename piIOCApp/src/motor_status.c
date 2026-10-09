//###############################################
//# ###                                       ### #
//# ### PI Mercury C-663 Status Parser        ### #
//# ### (single axis)                         ### #
//# ###                                       ### #
//# ### author: D. Hampai - M. Montis         ### #
//# ###                                       ### #
//# ### Ref 3.0; 2026-10-09                   ### #
//###############################################

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "motor_status.h"

// ---------------------------------------------------------------
// extract_srg_value
//   Cerca "0x" (o "0X") nella stringa e parsa l'esadecimale.
//   Se non lo trova, tenta di parsare l'ultimo token esadecimale.
//   Ritorna il valore a 16 bit, oppure 0 in caso di errore.
// ---------------------------------------------------------------
static unsigned int extract_srg_value(const char *resp)
{
    const char *p;
    unsigned int val = 0;

    if (!resp || *resp == '\0')
        return 0;

    // Caso 1: "0x..." (formato atteso dal C-663)
    p = strstr(resp, "0x");
    if (!p) p = strstr(resp, "0X");

    if (p) {
        val = (unsigned int)strtoul(p, NULL, 16);
        return val;
    }

    // Caso 2: stringa senza prefisso "0x"
    //   Cerca l'ultimo token esadecimale valido
    {
        const char *last_start = NULL;
        const char *q = resp;
        const char *tok_start = NULL;
        size_t tok_len = 0;

        while (*q) {
            if (isxdigit((unsigned char)*q)) {
                if (tok_start == NULL)
                    tok_start = q;
                tok_len = (size_t)(q - tok_start) + 1;
            } else {
                if (tok_start != NULL && tok_len > 0) {
                    last_start = tok_start;
                }
                tok_start = NULL;
                tok_len = 0;
            }
            q++;
        }
        if (tok_start != NULL && tok_len > 0) {
            last_start = tok_start;
        }

        if (last_start) {
            char buf[32];
            size_t n = 0;
            while (last_start[n] && isxdigit((unsigned char)last_start[n])
                   && n < sizeof(buf) - 1) {
                buf[n] = last_start[n];
                n++;
            }
            buf[n] = '\0';
            val = (unsigned int)strtoul(buf, NULL, 16);
        }
    }

    return val;
}

// ---------------------------------------------------------------
// parse_motor_status
//   Popola la struct MotorStatus a partire dalla risposta SRG?.
//   Ritorna 0 se ok, -1 se la stringa è vuota/null.
// ---------------------------------------------------------------
int parse_motor_status(const char *resp, MotorStatus *st)
{
    unsigned int val;

    if (!st)
        return -1;

    memset(st, 0, sizeof(*st));

    if (!resp || *resp == '\0')
        return -1;

    val = extract_srg_value(resp);

    st->limit_left     = (val & 0x0001u) ? 1 : 0;  // bit 0  : negative limit
    st->ref_switch     = (val & 0x0002u) ? 1 : 0;  // bit 1  : ref switch
    st->limit_right    = (val & 0x0004u) ? 1 : 0;  // bit 2  : positive limit
    st->referenced     = (val & 0x0008u) ? 1 : 0;  // bit 3  : referenced
    st->error_flag     = (val & 0x0100u) ? 1 : 0;  // bit 8  : error pending
    st->power_on       = (val & 0x1000u) ? 1 : 0;  // bit 12 : motor on
    st->in_motion      = (val & 0x2000u) ? 1 : 0;  // bit 13 : in motion
    st->is_referencing = (val & 0x4000u) ? 1 : 0;  // bit 14 : is referencing
    st->on_target      = (val & 0x8000u) ? 1 : 0;  // bit 15 : on target

    return 0;
}

// ---------------------------------------------------------------
// print_motor_status
//   Debug helper: stampa lo stato del motore.
// ---------------------------------------------------------------
void print_motor_status(const char *name, MotorStatus *st)
{
    if (!name) name = "Motor";
    if (!st) {
        printf("[%s] (null MotorStatus)\n", name);
        return;
    }

    printf("[%s] power_on=%d lim_left=%d lim_right=%d in_motion=%d "
           "referenced=%d error=%d on_target=%d is_ref=%d ref_sw=%d\n",
           name,
           st->power_on,
           st->limit_left,
           st->limit_right,
           st->in_motion,
           st->referenced,
           st->error_flag,
           st->on_target,
           st->is_referencing,
           st->ref_switch);
}
