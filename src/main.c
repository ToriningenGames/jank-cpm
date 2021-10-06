#include <stdio.h>
#include <stdint.h>
#include "8080.h"

int power = 1;
uint8_t cpmimage[65536];


int main(int argc, char **argv)
{
        FILE *cpm = fopen(argv[1], "rb");
        fread(cpmimage, 1, 65536, cpm);
        fclose(cpm);
        init();
        while (power) {
                decode();
        }
}
