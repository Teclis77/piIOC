#ifndef MOTOR_STATUS_H
#define MOTOR_STATUS_H

/*
 * motor_status.h
 *
 * Interpretazione dei byte di status motore ricevuti come caratteri ASCII.
 *
 * Due comandi supportati:
 *
 * 1) xxMS  (Motor Status per asse singolo)
 *    Risposta: "xxMSch"  dove ch = carattere di status
 *    Bit 0 - Axis in Motion      0=NO (fermo)       1=YES (in moto)
 *    Bit 1 - Motor power         0=ON               1=OFF
 *    Bit 2 - Motion direction    0=Negative (-)     1=Positive (+)
 *    Bit 3 - Right (+) limit     0=Not tripped      1=Tripped
 *    Bit 4 - Left  (-) limit     0=Not tripped      1=Tripped
 *    Bit 5 - Mechanical zero     0=Low              1=High
 *    Bit 6 - Not used
 *    Bit 7 - Not used
 *
 * 2) TS    (Total Status - tutti gli assi)
 *    Risposta: "TSch"  dove ch = carattere di status globale
 *    Bit 0 - Axis #1 motor state  0=Stationary  1=In motion
 *    Bit 1 - Axis #2 motor state  0=Stationary  1=In motion
 *    Bit 2 - Axis #3 motor state  0=Stationary  1=In motion
 *    Bit 3 - Axis #4 motor state  0=Stationary  1=In motion
 *    Bit 4 - Motor power          0=ON          1=OFF
 *    Bit 5 - Not used
 *    Bit 6 - Not used
 *    Bit 7 - IEEE SRQ             0=NO          1=YES
 */

/* ════════════════════════════════════════════════
 *  Maschere di bit  xxMS
 * ════════════════════════════════════════════════ */
#define MS_BIT_IN_MOTION     (1 << 0)   /* 0x01 */
#define MS_BIT_POWER_OFF     (1 << 1)   /* 0x02 - attenzione: 1=OFF */
#define MS_BIT_DIR_POSITIVE  (1 << 2)   /* 0x04 */
#define MS_BIT_LIMIT_RIGHT   (1 << 3)   /* 0x08 */
#define MS_BIT_LIMIT_LEFT    (1 << 4)   /* 0x10 */
#define MS_BIT_MECH_ZERO     (1 << 5)   /* 0x20 */

/* ════════════════════════════════════════════════
 *  Maschere di bit  TS
 * ════════════════════════════════════════════════ */
#define TS_BIT_AXIS1_MOVING  (1 << 0)   /* 0x01 */
#define TS_BIT_AXIS2_MOVING  (1 << 1)   /* 0x02 */
#define TS_BIT_AXIS3_MOVING  (1 << 2)   /* 0x04 */
#define TS_BIT_AXIS4_MOVING  (1 << 3)   /* 0x08 */
#define TS_BIT_POWER_OFF     (1 << 4)   /* 0x10 - attenzione: 1=OFF */
#define TS_BIT_SRQ           (1 << 7)   /* 0x80 */

/* ════════════════════════════════════════════════
 *  Struttura MotorStatus  (xxMS)
 *
 *  Tutti i campi hanno semantica positiva (1 = condizione vera).
 *  Il bit 1 hardware (power) e' invertito: qui power_on=1 significa ON.
 * ════════════════════════════════════════════════ */
typedef struct {
    int           in_motion;     /* 1 = asse in movimento              */
    int           power_on;      /* 1 = alimentazione ON               */
    int           dir_positive;  /* 1 = direzione positiva (+)         */
    int           limit_right;   /* 1 = fine corsa destro (+) scattato */
    int           limit_left;    /* 1 = fine corsa sinistro (-) scattato*/
    int           mech_zero;     /* 1 = segnale zero meccanico high    */
    unsigned char raw;           /* byte originale                     */
} MotorStatus;

/* ════════════════════════════════════════════════
 *  Struttura TotalStatus  (TS)
 *
 *  power_on=1 significa alimentazione ON (bit invertito dal protocollo).
 * ════════════════════════════════════════════════ */
typedef struct {
    int           axis1_moving;  /* 1 = asse 1 in movimento            */
    int           axis2_moving;  /* 1 = asse 2 in movimento            */
    int           axis3_moving;  /* 1 = asse 3 in movimento            */
    int           axis4_moving;  /* 1 = asse 4 in movimento            */
    int           power_on;      /* 1 = alimentazione ON               */
    int           srq;           /* 1 = IEEE SRQ attivo                */
    unsigned char raw;           /* byte originale                     */
} TotalStatus;

/* ════════════════════════════════════════════════
 *  API  xxMS
 * ════════════════════════════════════════════════ */

/*
 * parse_motor_status()
 * Decodifica il carattere 'ch' della risposta "xxMSch".
 * Ritorna 0 OK, -1 se ms e' NULL.
 */
int parse_motor_status(char c, MotorStatus *ms);

/* Accesso diretto ai singoli bit rilevanti (Bit 0,1,3,4) */
int ms_is_in_motion   (char c);   /* Bit 0 */
int ms_is_power_on    (char c);   /* Bit 1 - logica invertita */
int ms_is_limit_right (char c);   /* Bit 3 */
int ms_is_limit_left  (char c);   /* Bit 4 */

/*
 * ms_status_ok()
 * Ritorna 1 se il motore e' in stato operativo normale:
 *   alimentazione ON  AND  nessun fine corsa scattato.
 */
int ms_status_ok(char c);

/*
 * ms_is_ready()
 * Ritorna 1 se il motore e' pronto a muoversi:
 *   alimentazione ON  AND  fermo  AND  nessun fine corsa scattato.
 */
int ms_is_ready(char c);

/*
 * ms_to_epics()
 * Popola quattro variabili intere con i bit principali,
 * pronte per pvPut() dallo SNL. Puntatori NULL ignorati.
 */
void ms_to_epics(char c,
                 int *out_in_motion,
                 int *out_power_on,
                 int *out_limit_right,
                 int *out_limit_left);

/*
 * print_motor_status()
 * Stampa lo stato completo su stdout (debug IOC).
 *   label  - nome asse (es. "X", "Y", "Motor1")
 *   c      - carattere 'ch' della risposta "xxMSch"
 */
void print_motor_status(const char *label, char c);

/* ════════════════════════════════════════════════
 *  API  TS
 * ════════════════════════════════════════════════ */

/*
 * parse_total_status()
 * Decodifica il carattere 'ch' della risposta "TSch".
 * Ritorna 0 OK, -1 se ts e' NULL.
 */
int parse_total_status(char c, TotalStatus *ts);

/* Accesso diretto ai singoli bit */
int ts_is_axis_moving (char c, int axis);  /* axis: 1..4 */
int ts_is_power_on    (char c);            /* Bit 4 - logica invertita */
int ts_is_srq         (char c);            /* Bit 7 */

/*
 * ts_any_axis_moving()
 * Ritorna 1 se almeno uno degli assi 1..4 e' in movimento.
 */
int ts_any_axis_moving(char c);

/*
 * ts_to_epics()
 * Popola le variabili intere con i bit del TS,
 * pronte per pvPut() dallo SNL. Puntatori NULL ignorati.
 */
void ts_to_epics(char c,
                 int *out_axis1_moving,
                 int *out_axis2_moving,
                 int *out_axis3_moving,
                 int *out_axis4_moving,
                 int *out_power_on,
                 int *out_srq);

/*
 * print_total_status()
 * Stampa lo stato globale su stdout (debug IOC).
 *   c  - carattere 'ch' della risposta "TSch"
 */
void print_total_status(char c);

#endif /* MOTOR_STATUS_H */
