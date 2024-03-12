#include <stdio.h>
#include <string.h>

/*
 * Program to create EEPROM image files that are used in place of a combinational logic 
 * that could render an 8-bit binary number as a decimal on four seven-segment displays, 
 * one for each decimal place and one for sign. 
 * 
 * We'll be utilizing the first 256 bytes for values to render unsigned numbers and the 
 * last 256 bytes for values to render signed numbers. We're going to do this is by using 
 * three separate EEPROMs for each of the three decimal places (hundred's, ten's and one's 
 * place).
 * 
 * Each of the 256 numbers will act as input to an EEPROM which will be programmed with 
 * the digit's SSD map value at that specific address. Example, for the number 123, the 
 * first EEPROM will have 1's SSD value stored in address 123, the second EEPROM will 
 * have 2's SSD value stored in address 123 and the third EEPROM will have 3's SSD value 
 * stored in address 123.
 */

//SSD values to render the digit represented by the address.
unsigned char ssd_map[] = {0x77, 0x41, 0x3b, 0x6b, 0x4d, 0x6e, 0x7e, 0x43, 0x7f, 0x6f};

int main() {

    unsigned char i = 0;
    int k = 0;

    //three files for three SSD EEPROMS: Hunderd's, Ten's and One's place.
    FILE *h_eeprom_hex = fopen("h_eeprom_hex.txt", "wb");
    FILE *t_eeprom_hex = fopen("t_eeprom_hex.txt", "wb");
    FILE *o_eeprom_hex = fopen("o_eeprom_hex.txt", "wb");

    //file headers.
    fprintf(h_eeprom_hex, "v2.0 raw\n");
    fprintf(t_eeprom_hex, "v2.0 raw\n");
    fprintf(o_eeprom_hex, "v2.0 raw\n");

    //print values for unsigned numbers from 0 to 255 at positions 0 to 255.
    do {

	//print a newline after 16 bytes.
	if(k && k % 16 == 0) {
	    fprintf(h_eeprom_hex, "\n");
	    fprintf(t_eeprom_hex, "\n");
	    fprintf(o_eeprom_hex, "\n");
	}

	//print each digit's SSD value to the corresponding file.
	fprintf(h_eeprom_hex, "%0x ", ssd_map[i / 100]);
	fprintf(t_eeprom_hex, "%0x ", ssd_map[(i / 10) % 10]);
	fprintf(o_eeprom_hex, "%0x ", ssd_map[i % 10]);

	++i;
	++k;

    } while(i);

    //print values for signed numbers from 0 to 127 at positions 256 to 383.
    char j = 0;
    while(j >= 0) {

	//print a newline after 16 bytes.
	if(k && k % 16 == 0) {
	    fprintf(h_eeprom_hex, "\n");
	    fprintf(t_eeprom_hex, "\n");
	    fprintf(o_eeprom_hex, "\n");
	}

	//print each digit's SSD value to the corresponding file.
	fprintf(h_eeprom_hex, "%0x ", ssd_map[j / 100]);
	fprintf(t_eeprom_hex, "%0x ", ssd_map[(j / 10) % 10]);
	fprintf(o_eeprom_hex, "%0x ", ssd_map[j % 10]);

	++j;
	++k;
    }

    //print values for signed numbers from -128 to -1 at positions 384 to 511.
    while(j < 0) {

	//print a newline after 16 bytes.
	if(k && k % 16 == 0) {
	    fprintf(h_eeprom_hex, "\n");
	    fprintf(t_eeprom_hex, "\n");
	    fprintf(o_eeprom_hex, "\n");
	}

	//print each digit's SSD value to the corresponding file.
	fprintf(h_eeprom_hex, "%0x ", ssd_map[-j / 100]);
	fprintf(t_eeprom_hex, "%0x ", ssd_map[(-j / 10) % 10]);
	fprintf(o_eeprom_hex, "%0x ", ssd_map[-j % 10]);

	++j;
	++k;
    }

    fclose(h_eeprom_hex);
    fclose(t_eeprom_hex);
    fclose(o_eeprom_hex);
}

