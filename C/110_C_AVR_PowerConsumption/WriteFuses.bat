"avrdude" -p atmega328p -c usbasp -C "avrdude.conf" -P com1 -b 115200 -U lfuse:w:0xef:m -U hfuse:w:0xd9:m -U efuse:w:0x07:m
pause