#!../../bin/linux-x86_64/piIOC

#- SPDX-FileCopyrightText: 2003 Argonne National Laboratory
#-
#- SPDX-License-Identifier: EPICS

#- You may have to change piIOC to something else
#- everywhere it appears in this file

< envPaths

cd "${TOP}"

epicsEnvSet ("STREAM_PROTOCOL_PATH", "${TOP}/db")
epicsEnvSet ("PORT_PI_CTS", "serial_pi_1")

## Register all support components
dbLoadDatabase "dbd/piIOC.dbd"
piIOC_registerRecordDeviceDriver pdbbase

drvAsynIPPortConfigure($(PORT_PI_CTS), "100.100.0.11:4001")
asynOctetSetInputEos($(PORT_PI_CTS),0,"\n")
asynOctetSetOutputEos($(PORT_PI_CTS),0,"\n")

asynSetTraceMask($(PORT_PI_CTS),-1,0x9);
asynSetTraceIOMask($(PORT_PI_CTS),-1,0x2)

## Load record instances
#dbLoadRecords("db/piIOC.db","user=xlabsrv2")

#dbLoadRecords("db/piIOC.db", "ASYNPORT=$(PORT_PI_CTS), MOT=pi_1")
dbLoadTemplate("db/piIOC.val")

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncxxx,"user=xlabsrv2"
