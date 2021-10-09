#include <stdio.h>
#include <stdint.h>
#include "8080.h"

int power = 1;
uint8_t cpmimage[65536];


int main(int argc, char **argv)
{
        //Set unbuffered input/output
        setvbuf(stdin, NULL, _IONBF, 0);
        setvbuf(stdout, NULL, _IONBF, 0);
        if (argc == 1) {
                puts("Usage: jankcpm diskimage");
                return 1;
        };
        FILE *cpm = fopen(argv[1], "rb");
        fread(cpmimage, 1, 128, cpm);   //First sector only
        fclose(cpm);
        init();
        while (power) {
                decode();
        }
}
