all: main.hex

main.o:
	avr-gcc -mmcu=atmega324a  -Wall -Os -fdata-sections -ffunction-sections -c main.c -Wl,--gc-sections

ov7670.o:
	avr-gcc -mmcu=atmega324a -Wall -Os -fdata-sections -ffunction-sections -c ov7670.c -Wl,--gc-sections
	
main.elf: main.o ov7670.o 
	avr-gcc -mmcu=atmega324a -Wall -Os -fdata-sections -ffunction-sections -o main.elf main.o ov7670.o  -Wl,--gc-sections
 
main.hex: main.elf 
	avr-objcopy  -j .text -j .data -O ihex  main.elf main.hex
	avr-size main.elf
 
clean:
	rm main.elf main.hex main.o ov7670.o