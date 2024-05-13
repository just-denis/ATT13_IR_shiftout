# ATT13_IR_shiftout
An example of manipulating shift register (595 chip) via IR remote control (NEC protocol decoder).

## IR library
Based on NEC proto analyzer example by Łukasz Marcin Podkalicki - the IR library was borrowed from  https://github.com/lpodkalicki/blog/tree/master/avr/attiny13/013_ir_receiver_38khz_nec_proto_analyzer. 
Please find more at https://blog.podkalicki.pl/attiny13-ir-receiver-nec-proto-analyzer/
The code is in "ir.h" - it's both a "header" and an "implementation" file.

## IR codes
The MS Excel file (IR_remote_button_codes.xlsx) has the IR remote control codes that were scanned in advance to support this demo.

## EEPROM support
The code reads the single byte of data (to be "displayed" bitwise via 595 output pins) from EEPROM (address 0); this byte is written back after each change.
The read / write subroutines were copied from the ATtiny13A datasheet, no changes.
For convenience, they are put in a separate file - "eeprom_rw.h".

## Compilation size
Sketch uses 1002 bytes (97%) of program storage space. Maximum is 1024 bytes.
Global variables use 18 bytes (28%) of dynamic memory, leaving 46 bytes for local variables. Maximum is 64 bytes.
