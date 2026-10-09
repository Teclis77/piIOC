//###############################################
//# ###                                       ### #
//# ### Header for PI Mercury C-663 Status     ### #
//# ### (single axis)                         ### #
//# ###                                       ### #
//# ### author: D. Hampai - M. Montis         ### #
//# ###                                       ### #
//# ### Ref 3.0; 2026-10-09                   ### #
//###############################################
#ifndef MOTOR_STATUS_H
#define MOTOR_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------
// MotorStatus : stato dell'asse C-663 (da SRG?)
// ---------------------------------------------------------------
typedef struct {
    int power_on;         // bit 12 : 1 = motor on
    int limit_right;      // bit 2  : 1 = positive limit tripped
    int limit_left;       // bit 0  : 1 = negative limit tripped
    int in_motion;        // bit 13 : 1 = axis in motion
    int referenced;       // bit 3  : 1 = axis referenced
    int error_flag;       // bit 8  : 1 = error pending
    int on_target;        // bit 15 : 1 = target reached
    int is_referencing;   // bit 14 : 1 = reference move running
    int ref_switch;       // bit 1  : 1 = reference switch NOT active
} MotorStatus;

// ---------------------------------------------------------------
// Funzioni esposte al SNL
// ---------------------------------------------------------------

// Parsing della risposta a SRG?
//   resp: stringa tipo "0 2 1 1=0x900A" oppure "1 1=0x910A" oppure "0x900A"
//   st  : puntatore alla struct da popolare
//   Ritorna 0 in caso di successo, -1 in caso di errore.
int parse_motor_status(const char *resp, MotorStatus *st);

// Debug helper: stampa lo stato del motore
void print_motor_status(const char *name, MotorStatus *st);

#ifdef __cplusplus
}
#endif

#endif // MOTOR_STATUS_H
