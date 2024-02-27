CC = avr-gcc 
OBJCPY = avr-objcopy
SIZE = avr-size
MCU = atmega328p
F_CPU = 16000000
CFLAGS = \
	-mmcu=$(MCU) \
	-DF_CPU=$(F_CPU)UL \
	-Os \
	-std=gnu99 \
	-Werror \
	-ffunction-sections \
	-fdata-sections \
	-I./ \
	-I../
LDFLAGS = \
	-Wl,--gc-sections \
	-mmcu=$(MCU)
AVRDUDE=avrdude
PORT=$(shell pavr2cmd --prog-port)

SRC = $(shell ls ./*.c) \
      $(shell ls ./libraries/*.c)

# Default values
FEATURE_SET_TIME ?= YES
FEATURE_CHARACTERS ?= YES
FEATURE_CHANGE_TWI_ADDRESS ?= YES
FEATURE_SHOW_ADDRESS_ON_STARTUP ?= YES
FEATURE_LOWERCASE ?= YES

SPECIAL_DEFS += DEMO \
	FEATURE_SET_TIME \
	FEATURE_CHARACTERS \
	FEATURE_CHANGE_TWI_ADDRESS \
	FEATURE_SHOW_ADDRESS_ON_STARTUP \
	FEATURE_LOWERCASE

OBJ = $(SRC:.c=.o)

main.hex: main.elf
	$(OBJCPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature main.elf main.hex

main.elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $@

size: main.elf
	$(SIZE) --mcu=$(MCU) --format=avr main.elf

clean:
	rm -f $(OBJ) main.elf main.hex

upload: main.hex
	avrdude -c stk500v2 -P "$(PORT)" -p $(MCU) -U flash:w:$<:i -B 0.5 

memory:
	avr-size -C --mcu=atmega328p -f 16000000 main.elf

auto: clean
	make && make upload
	avr-size -C --mcu=atmega328p main.elf