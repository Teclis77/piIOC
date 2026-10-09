//###############################################
//# ###                                       ### #
//# ### Header for PI Mercury C-663 Status     ### #
//# ###                                       ### #
//# ### author: (adatta)                      ### #
//# ###                                       ### #
//# ### Ref 3.0; 2026-10-09                   ### #
//###############################################
#ifndef MOTOR_STATUS_H
#define MOTOR_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------
// Struttura per lo stato dell'asse C-663 (da SRG?)
//   Tutti i campi sono 0/1.
// ---------------------------------------------------------------
typedef struct {
    int neg_limit;      // bit 0  : 1 = negative limit tripped
    int ref_switch;     // bit 1  : 1 = reference switch NOT active
    int pos_limit;      // bit 2  : 1 = positive limit tripped
    int referenced;     // bit 3  : 1 = axis referenced
    int error_flag;     // bit 8  : 1 = error pending
    int motor_on;       // bit 12 : 1 = motor on
    int in_motion;      // bit 13 : 1 = axis in motion
    int is_referencing; // bit 14 : 1 = reference move running
    int on_target;      // bit 15 : 1 = target reached
} C663AxisStatus;

// ---------------------------------------------------------------
// Struttura per lo stato globale motore (da SVO?)
// ---------------------------------------------------------------
typedef struct {
    int motor_on;       // 0 = spento, 1 = acceso
} C663MotorStatus;

// ---------------------------------------------------------------
// Funzioni esposte al SNL
// ---------------------------------------------------------------

// Parsing della risposta a SRG? (status register a 16 bit)
//   resp: stringa tipo "1 1=0x910A" oppure "0x910A" oppure "910A"
//   st  : puntatore alla struct da popolare
//   Ritorna 0 in caso di successo, -1 in caso di errore.
int parse_srg(const char *resp, C663AxisStatus *st);

// Parsing della risposta a SVO? (stato motore)
//   resp: stringa tipo "1", "1=1", "1=0", "0", ...
//   st  : puntatore alla struct da popolare
//   Ritorna 0 in caso di successo, -1 in caso di errore.
int parse_svo(const char *resp, C663MotorStatus *st);

// Helper: estrae il valore esadecimale da una stringa SRG?
//   Ritorna il valore a 16 bit, oppure 0 se non trovato.
unsigned int extract_srg_value(const char *resp);

#ifdef __cplusplus
}
#endif

#endif // MOTOR_STATUS_H