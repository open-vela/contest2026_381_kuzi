#############################################################################
# AI Butler — Board Sentinel
#############################################################################

include $(APPDIR)/Make.defs

ifeq ($(CONFIG_AI_BUTLER),y)
PROGNAME  = ai_butler
PRIORITY  = 100
STACKSIZE = 4096
MODULE    = $(CONFIG_AI_BUTLER)

MAINSRC = src/ai_butler_main.c
CSRCS   = src/board_monitor.c
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/packages/demos/contest2026_381_ai_butler/src
endif

include $(APPDIR)/Application.mk
