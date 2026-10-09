//###############################################
//# ###                                       ### #
//# ### PI Mercury C-663 Status Parser        ### #
//# ###                                       ### #
//# ### author: (adatta)                      ### #
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
//   Se non lo trova, tenta di parsare l'ultimo token numerico.
//   Ritorna il valore a 16 bit, oppure 0 in caso di errore.
// ---------------------------------------------------------------
unsigned int extract_srg_value(const char *resp)
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
    //   Cerca l'ultimo token composto da soli caratteri esadecimali
    //   (0-9, a-f, A-F) e prova a parsarlo.
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
            // Copia il token in un buffer locale e parsalo
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
// parse_srg
//   Popola la struct C663AxisStatus a partire dalla risposta SRG?.
//   Ritorna 0 se ok, -1 se la stringa è vuota/null.
// ---------------------------------------------------------------
int parse_srg(const char *resp, C663AxisStatus *st)
{
    unsigned int val;

    if (!st)
        return -1;

    memset(st, 0, sizeof(*st));

    if (!resp || *resp == '\0')
        return -1;

    val = extract_srg_value(resp);

    st->neg_limit      = (val & 0x0001u) ? 1 : 0;
    st->ref_switch     = (val & 0x0002u) ? 1 : 0;
    st->pos_limit      = (val & 0x0004u) ? 1 : 0;
    st->referenced     = (val & 0x0008u) ? 1 : 0;
    st->error_flag     = (val & 0x0100u) ? 1 : 0;
    st->motor_on       = (val & 0x1000u) ? 1 : 0;
    st->in_motion      = (val & 0x2000u) ? 1 : 0;
    st->is_referencing = (val & 0x4000u) ? 1 : 0;
    st->on_target      = (val & 0x8000u) ? 1 : 0;

    return 0;
}

// ---------------------------------------------------------------
// parse_svo
//   Popola la struct C663MotorStatus a partire dalla risposta SVO?.
//   Cerca l'ultimo carattere '0' o '1' nella stringa.
//   Ritorna 0 se ok, -1 se la stringa è vuota/null.
// ---------------------------------------------------------------
int parse_svo(const char *resp, C663MotorStatus *st)
{
    int len, i;
    char c = '0';

    if (!st)
        return -1;

    memset(st, 0, sizeof(*st));

    if (!resp || *resp == '\0')
        return -1;

    len = (int)strlen(resp);
    for (i = len - 1; i >= 0; i--) {
        if (resp[i] == '0' || resp[i] == '1') {
            c = resp[i];
            break;
        }
    }

    st->motor_on = (c == '1') ? 1 : 0;
    return 0;
}