//###############################################
//# ###                                     ### #
//# ### Header for PI Mercury C-663 Status  ### #
//# ### (single axis)                       ### #
//# ###                                     ### #
//# ### author: D. Hampai - M. Montis       ### #
//# ###                                     ### #
//# ### Ref 2.0; 2026-10-09                 ### #
//###############################################
#ifndef MOTOR_STATUS_H
#define MOTOR_STATUS_H

/*
 *
 * Interpretazione dei registri di status del controller PI Mercury C-663.
 * Interfaccia IDENTICA a motor_status.h (mm4006) per permettere
 * la generalizzazione dei file SNL.
 *
 * Mapping comandi:
 *   SRG? -> equivalente xxMS (status asse singolo) -> parse_motor_status()
 *   SVO? -> equivalente TS  (status globale)       -> parse_total_status()
 *
 * SRG? risposta: valore esadecimale a 16 bit
 *   Bit  0  0x0001  Negative limit sw    1=attivo (toccato)
 *   Bit  1  0x0002  Reference switch     1=non attivo
 *   Bit  2  0x0004  Positive limit sw    1=attivo (toccato)
 *   Bit  3  0x0008  Asse referenziato    1=referenziato
 *   Bit 4-7 0x00F0  Digital input 1-4   logica dipende dal cablaggio
 *   Bit  8  0x0100  Error flag           1=errore pendente
 *   Bit 12  0x1000  Motor on             1=attivo
 *   Bit 13  0x2000  In motion            1=in movimento
 *   Bit 14  0x4000  Is referencing       1=in reference move
 *   Bit 15  0x8000  On-target            1=posizione raggiunta
 *
 * SVO? risposta: intero  0=motore spento  1=motore acceso
 */

/* ════════════════════════════════════════════════
 *  Maschere di bit SRG?
 * ════════════════════════════════════════════════ */
#define SRG_BIT_LIMIT_NEG       (1 << 0)   /* 0x0001 */
#define SRG_BIT_REF_SWITCH      (1 << 1)   /* 0x0002 */
#define SRG_BIT_LIMIT_POS       (1 << 2)   /* 0x0004 */
#define SRG_BIT_REFERENCED      (1 << 3)   /* 0x0008 */
#define SRG_BIT_DIG_IN1         (1 << 4)   /* 0x0010 */
#define SRG_BIT_DIG_IN2         (1 << 5)   /* 0x0020 */
#define SRG_BIT_DIG_IN3         (1 << 6)   /* 0x0040 */
#define SRG_BIT_DIG_IN4         (1 << 7)   /* 0x0080 */
#define SRG_BIT_ERROR           (1 << 8)   /* 0x0100 */
#define SRG_BIT_MOTOR_ON        (1 << 12)  /* 0x1000 */
#define SRG_BIT_IN_MOTION       (1 << 13)  /* 0x2000 */
#define SRG_BIT_IS_REFERENCING  (1 << 14)  /* 0x4000 */
#define SRG_BIT_ON_TARGET       (1 << 15)  /* 0x8000 */

/* ════════════════════════════════════════════════
 *  MotorStatus  (SRG? -> equivalente xxMS)
 *  Stessa struttura di motor_status.h per compatibilita' SNL
 * ════════════════════════════════════════════════ */
typedef struct {
    int          in_motion;      /* 1 = asse in movimento  (Bit 13) */
    int          power_on;       /* 1 = motore acceso      (Bit 12) */
    int          dir_positive;   /* non disponibile su C663, sempre 0 */
    int          limit_right;    /* 1 = limit POS scattato (Bit 2)  */
    int          limit_left;     /* 1 = limit NEG scattato (Bit 0)  */
    int          mech_zero;      /* 1 = referenziato       (Bit 3)  */
    /* Campi aggiuntivi specifici C663 */
    int          error;          /* 1 = errore pendente    (Bit 8)  */
    int          on_target;      /* 1 = posiz. raggiunta   (Bit 15) */
    int          is_referencing; /* 1 = in ref. move       (Bit 14) */
    int          ref_switch;     /* 1 = ref switch non att.(Bit 1)  */
    int          dig_in[4];      /* digital input 1-4    (Bit 4-7)  */
    unsigned int raw;            /* valore raw 16 bit               */
} MotorStatus;

/* ════════════════════════════════════════════════
 *  TotalStatus  (SVO? -> equivalente TS)
 *  Stessa struttura di motor_status.h per compatibilita' SNL
 *  Nota: SVO? e' globale, non per singolo asse.
 *  axis1_moving..axis4_moving vengono popolati tutti
 *  con lo stesso valore (in_motion da SRG?) se disponibile,
 *  oppure lasciati a 0 se non rilevante.
 * ════════════════════════════════════════════════ */
typedef struct {
    int          axis1_moving;   /* non disponibile da SVO?, usa SRG? */
    int          axis2_moving;   /* non disponibile da SVO?, usa SRG? */
    int          axis3_moving;   /* non disponibile da SVO?, usa SRG? */
    int          axis4_moving;   /* non disponibile da SVO?, usa SRG? */
    int          power_on;       /* 1 = motore acceso  (SVO?=1)      */
    int          srq;            /* non disponibile su C663, sempre 0 */
    int          raw;            /* valore raw 0 o 1                  */
} TotalStatus;

/* ════════════════════════════════════════════════
 *  API SRG? — stessi nomi di motor_status.h
 * ════════════════════════════════════════════════ */

/*
 * parse_motor_status()
 * Decodifica il valore intero a 16 bit ricevuto da SRG?
 * Parametro 'c' usato come int (valore raw SRG).
 * Stessa firma di motor_status.h per compatibilita' SNL.
 * Ritorna 0 OK, -1 se ms e' NULL.
 */
int parse_motor_status(int srg_value, MotorStatus *ms);

/*
 * parse_motor_status_from_string()
 * Variante che accetta la risposta SRG? come stringa hex (es. "2004").
 */
int parse_motor_status_from_string(const char *hex_str, MotorStatus *ms);

/* Accesso diretto ai bit principali - stessi nomi di motor_status.h */
int ms_is_in_motion   (int srg_value);   /* Bit 13 */
int ms_is_power_on    (int srg_value);   /* Bit 12 */
int ms_is_limit_right (int srg_value);   /* Bit 2  */
int ms_is_limit_left  (int srg_value);   /* Bit 0  */

/* Controlli composti - stessi nomi di motor_status.h */
int ms_status_ok (int srg_value);
int ms_is_ready  (int srg_value);

/*
 * ms_to_epics() - stessa firma di motor_status.h
 */
void ms_to_epics(int srg_value,
                 int *out_in_motion,
                 int *out_power_on,
                 int *out_limit_right,
                 int *out_limit_left);

/*
 * print_motor_status() - stessa firma di motor_status.h
 */
void print_motor_status(const char *label, int srg_value);

/* ════════════════════════════════════════════════
 *  API SVO? — stessi nomi di motor_status.h
 * ════════════════════════════════════════════════ */

/*
 * parse_total_status()
 * Decodifica il valore intero ricevuto da SVO?
 * Stessa firma di motor_status.h per compatibilita' SNL.
 * Ritorna 0 OK, -1 se ts e' NULL.
 */
int parse_total_status(int svo_value, TotalStatus *ts);

/* Accesso diretto - stessi nomi di motor_status.h */
int ts_is_power_on    (int svo_value);
int ts_any_axis_moving(int svo_value);   /* sempre 0 per SVO?, usa SRG? */
int ts_is_srq         (int svo_value);   /* sempre 0 per C663           */

/*
 * ts_to_epics() - stessa firma di motor_status.h
 */
void ts_to_epics(int svo_value,
                 int *out_axis1_moving,
                 int *out_axis2_moving,
                 int *out_axis3_moving,
                 int *out_axis4_moving,
                 int *out_power_on,
                 int *out_srq);

/*
 * print_total_status() - stessa firma di motor_status.h
 */
void print_total_status(int svo_value);

#endif // MOTOR_STATUS_H
