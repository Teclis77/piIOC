# SPDX-FileCopyrightText: 2003 Argonne National Laboratory
#
# SPDX-License-Identifier: EPICS

TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = piIOC
# piIOC.dbd will be created and installed
DBD += piIOC.dbd

# piIOC.dbd will be made up from these files:
piIOC_DBD += base.dbd

# Include dbd files from all support applications:
#piIOC_DBD += xxx.dbd
piIOC_DBD += stream-base.dbd
piIOC_DBD += drvAsynIPPort.dbd
piIOC_DBD += asyn.dbd

# Add all the support libraries needed by this IOC
#piIOC_LIBS += xxx
piIOC_LIBS += stream
piIOC_LIBS += asyn

# piIOC_registerRecordDeviceDriver.cpp derives from piIOC.dbd
piIOC_SRCS += piIOC_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
piIOC_SRCS_DEFAULT += piIOCMain.cpp
piIOC_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#piIOC_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

ifdef EPICS_QSRV_MAJOR_VERSION
    piIOC_LIBS += qsrv
    piIOC_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)
    piIOC_DBD += PVAServerRegister.dbd
    piIOC_DBD += qsrv.dbd
endif

# Finally link to the EPICS Base libraries
piIOC_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

