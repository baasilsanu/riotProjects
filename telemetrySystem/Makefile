# name of your application
APPLICATION = telemetrySystem

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= /home/RIOT/RIOT


DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

FEATURES_REQUIRED += cpp
USEMODULE += can
USEMODULE += ztimer
USEMODULE += ztimer_sec
USEMODULE += periph_gpio_irq
USEMODULE += shell
USEMODULE += ps
# FEATURES_REQUIRED += periph_can


include $(RIOTBASE)/Makefile.include
