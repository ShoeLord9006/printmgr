# ----- Configuration -----

# Fully Qualified Board Name
# ----- Arduino Mega -----
FQBN        ?= arduino:avr:mega
# ----- Arduino Nano -----
#FQBN        ?= arduino:avr:nano
# ----- Arduino Nano (Old Bootloader) -----
#FQBN        ?= arduino:avr:nano:cpu=atmega328old

# Serial Device (Leave blank for autodetect)
#SERIAL_DEV        ?= /dev/ttyUSB0

# Enable/Disable Verbose Output
V          ?= 0
# ----- ------------- -----




MAKE_DIR   := $(PWD)
BUILD_DIR  := $(MAKE_DIR)/build


VFLAG      =
ifeq "$(V)" "1"
VFLAG      =-v
endif

ifndef SERIAL_DEV
  ifneq (,$(wildcard /dev/ttyUSB0))
    SERIAL_DEV = /dev/ttyUSB0
  else ifneq (,$(wildcard /dev/ttyACM0))
    SERIAL_DEV = /dev/ttyACM0
  else
    SERIAL_DEV = unknown
  endif
endif


SRC        := $(wildcard *.ino)
HDRS       := $(wildcard *.h)
BIN        := $(BUILD_DIR)/$(SRC).bin
ELF        := $(BUILD_DIR)/$(SRC).elf

$(info FQBN       is [${FQBN}])
$(info V          is [${V}])
$(info MAKE_DIR   is [${MAKE_DIR}])
$(info BUILD_DIR  is [${BUILD_DIR}])
$(info SRC        is [${SRC}])
$(info HDRS       is [${HDRS}])
$(info BIN        is [${BIN}])
$(info ELF        is [${ELF}])
$(info SERIAL_DEV is [${SERIAL_DEV}])

all: compile upload
.PHONY: all

compile:
	@echo "---> Compiling Sketch"
	arduino-cli compile -b $(FQBN) $(VFLAG) --build-path $(BUILD_DIR)

upload:
	@if [ ! -c $(SERIAL_DEV) ] ; \
	then echo "---> ERROR: Serial Device not available, please set the SERIAL_DEV environment variable"; \
	else echo "---> Uploading sketch"; \
	sudo -E arduino-cli upload -b $(FQBN) -p $(SERIAL_DEV) $(VFLAG) --input-dir $(BUILD_DIR); \
	fi

clean:
	@echo "---> Cleaning the build directory"
	rm -rf build

requirements:
	@if [ -e requirements.txt ]; \
	then while read -r i ; do echo ; \
	  echo "---> Installing " '"'$$i'"' ; \
	  arduino-cli lib install "$$i" ; \
	done < requirements.txt ; \
	else echo "---> MISSING requirements.txt file"; \
	fi

monitor:
	@sudo minicom -D $(SERIAL_DEV)

nothing:
	$(info Doing Nothing lol)
