/*
see ATtiny13A datasheet, section 5.3 & pp.24-25
~46 bytes for both functions and a couple of calls, just reading / writing a byte
ASM should be even lighter, I guess
*/

unsigned char EEPROM_read(unsigned char ucAddress);
void EEPROM_write(unsigned char ucAddress, unsigned char ucData);

//====================

unsigned char EEPROM_read(unsigned char ucAddress)
{
/* Wait for completion of previous write */
while(EECR & (1<<EEPE))
;
/* Set up address register */
EEARL = ucAddress;
/* Start eeprom read by writing EERE */
EECR |= (1<<EERE);
/* Return data from data register */
return EEDR;
}

void EEPROM_write(unsigned char ucAddress, unsigned char ucData)
{
/* Wait for completion of previous write */
while(EECR & (1<<EEPE))
;
/* Set Programming mode */
EECR = (0<<EEPM1)|(0>>EEPM0); // << interestingly enough, the datasheet had this semicolumn missing ))
/* Set up address and data registers */
EEARL = ucAddress;
EEDR = ucData;
/* Write logical one to EEMPE */
EECR |= (1<<EEMPE);
/* Start eeprom write by setting EEPE */
EECR |= (1<<EEPE);
}
