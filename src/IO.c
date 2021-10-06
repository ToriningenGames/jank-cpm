#include <stdio.h>
#include <stdint.h>

#define DISKSIZE 128128

extern int power;

static uint32_t diskhead;
uint8_t diskdata[DISKSIZE];

uint8_t portRead(uint8_t port)
{
        switch (port) {
        case 0:         //keyboard read
                return (uint8_t) getchar();
        case 5:         //disk read
                return diskdata[diskhead++];
        }
        //Floating line
        return port;
}

void portWrite(uint8_t port, uint8_t data)
{
        switch (port) {
        case 0:         //screen write
                putchar(data);
                return;
        case 1:         //disk addr 0
                diskhead = diskhead & 0xFFFFFF00 | data;
                return;
        case 2:         //disk addr 1
                diskhead = diskhead & 0xFFFF00FF | (data << 8);
                return;
        case 3:         //disk addr 2
                diskhead = diskhead & 0xFF00FFFF | (data << 16);
                return;
        case 4:         //disk addr 3
                diskhead = diskhead & 0x00FFFFFF | (data << 24);
                return;
        case 5:         //disk write
                diskdata[diskhead++] = data;
                return;
        case 0xFF:      //shutdown
                power = 0;
                return;
        }
}
