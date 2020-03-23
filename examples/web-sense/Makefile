CONTIKI_PROJECT = web-sense
all: $(CONTIKI_PROJECT)
CONTIKI = ../..

#CI = true

# Include webserver module
MODULES_REL += webserver

# Include optional target-specific module
include $(CONTIKI)/Makefile.identify-target
MODULES_REL += $(TARGET)

include $(CONTIKI)/Makefile.include
