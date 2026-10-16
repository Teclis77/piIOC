# poll_motor_status.py
# Triggera ogni secondo il record SCAN=Passive del motore
# facendo pvPut su .PROC
#
# Configurazione nel Display Builder:
#   Widget: Label (o qualsiasi widget, anche invisibile)
#   Script PVs:
#     pvs[0] = sim://flipflop(1)   -> Trigger = YES
#     pvs[1] = mm4006_1_1_1:ms_status_opt.PROC   -> Trigger = NO
#
# Il flipflop cambia ogni secondo e triggera lo script,
# che a sua volta forza il processo del record Passive.

from org.csstudio.display.builder.runtime.script import PVUtil

# pvs[0] = sim://flipflop(1)  - trigger, non ci serve il valore
# pvs[1] = record.PROC        - lo scriviamo per forzare l'aggiornamento

pvs[1].write(1)