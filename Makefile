MCU=avr128da28
F_CPU=16000000UL
CC=avr-gcc
OBJCOPY=avr-objcopy
INCLUDE_FLAGS = -Igcc/include
MACRO_DEFS = -DF_CPU=$(F_CPU)
CFLAGS  = -std=c11 -Wall -g -Os -mmcu=$(MCU) $(MACRO_DEFS)
CFLAGS += -Bgcc/dev/avr128da28 $(INCLUDE_FLAGS)
TARGET = main
SOURCES = *.c
HEADERS = *.h
AVRDUDE = avrdude -p $(MCU) -P /dev/ttyUSB0 -c jtag2updi -b 19200

build: $(HEADERS) $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET).bin $(SOURCES)

size: build
	avr-size -C $(TARGET).bin

main.hex: build
	$(OBJCOPY) -j .text -j .data -O ihex $(TARGET).bin $(TARGET).hex

flash: main.hex
	$(AVRDUDE) -V -U flash:w:$(TARGET).hex

clean:
	rm -f *.bin *.hex

# FUSES

fuse.wdtcfg.read:
	$(AVRDUDE) -U fuse0:r:-:i

fuse.bodcfg.read:
	$(AVRDUDE) -U fuse1:r:-:i

fuse.osccfg.read:
	$(AVRDUDE) -U fuse2:r:-:i

fuse.syscfg0.read:
	$(AVRDUDE) -U fuse5:r:-:i

fuse.syscfg1.read:
	$(AVRDUDE) -U fuse6:r:-:i

fuse.codesize.read:
	$(AVRDUDE) -U fuse7:r:-:i

fuse.bootsize.read:
	$(AVRDUDE) -U fuse8:r:-:i

fuse.all.read:
	@echo "Reading all fuses"
	$(AVRDUDE) \
		-U fuse0:r:-:i \
		-U fuse1:r:-:i \
		-U fuse2:r:-:i \
		-U fuse5:r:-:i \
		-U fuse6:r:-:i \
		-U fuse7:r:-:i \
		-U fuse8:r:-:i

fuse.all.write.default:
	@echo "Initializing project default fuse settings"
	$(AVRDUDE) \
		-U fuse0:w:0:m \
		-U fuse1:w:0x18:m \
		-U fuse2:w:0:m \
		-U fuse5:w:0xA8:m \
		-U fuse6:w:0:m \
		-U fuse7:w:0:m \
		-U fuse8:w:0:m

sigrow.read:
	@echo "Reading SIGROW"
	$(AVRDUDE) -U usersig:r:-:i
