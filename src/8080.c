#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "IO.h"
#include "8080.h"



struct {
        uint8_t a;
        union {
                uint8_t f;
                struct {
                        bool carry:1;
                        bool f1:1;
                        bool parity:1;
                        bool f3:1;
                        bool aux:1;
                        bool f5:1;
                        bool zero:1;
                        bool sign:1;
                };
        };
        union {
                uint16_t bc;
                struct {
                        uint8_t c;
                        uint8_t b;
                };
        };
        union {
                uint16_t de;
                struct {
                        uint8_t e;
                        uint8_t d;
                };
        };
        union {
                uint16_t hl;
                struct {
                        uint8_t l;
                        uint8_t h;
                };
        };
        uint16_t ip;
        uint16_t sp;
        uint8_t i:1;
} cpu;

uint8_t memory[65536];


uint8_t calcParity(uint8_t val)
{
        return !((!(val & 0x01))
              ^ (!(val & 0x02))
              ^ (!(val & 0x04))
              ^ (!(val & 0x08))
              ^ (!(val & 0x10))
              ^ (!(val & 0x20))
              ^ (!(val & 0x40))
              ^ (!(val & 0x80))
        );
}

void setFlags(uint8_t res)
{
        cpu.sign = !!(res & 0x80);
        cpu.zero = !res;
        cpu.parity = calcParity(res);
}

void init()
{
        memcpy(memory, cpmimage, 65536);
        //Variable initialization is mandatory
        cpu.ip = 0;
        cpu.sp = 0;
        cpu.a = 0;
        cpu.f = 0;
        cpu.bc = 0;
        cpu.de = 0;
        cpu.hl = 0;
        cpu.i = 0;
}

//TODO:
        //0x27: DAA
        //0x76: HALT
void decode()
{
        uint8_t inst = memory[cpu.ip++];
        uint8_t temp;
        uint16_t addr;
        switch (inst) {
        case 0x08:
        case 0x18:
        case 0x28:
        case 0x38:
        case 0x00:      //NOP
                return;
        case 0x01:      //LD BC,nn
                cpu.c = memory[cpu.ip++];
                cpu.b = memory[cpu.ip++];
                return;
        case 0x02:      //LD (BC),A
                memory[cpu.bc] = cpu.a;
                return;
        case 0x03:      //INC BC
                cpu.bc++;
                return;
        case 0x04:      //INC B
                setFlags(++cpu.b);
                cpu.aux = !(cpu.b & 0x0F);
                return;
        case 0x05:      //DEC B
                setFlags(--cpu.b);
                cpu.aux = !(cpu.b & 0x0F ^ 0x0F);
                return;
        case 0x06:      //LD B,n
                cpu.b = memory[cpu.ip++];
                return;
        case 0x07:      //RLCA
                cpu.a = (cpu.a << 1) | !!(cpu.a & 0x80);
                cpu.carry = cpu.a & 0x01;
                return;
        case 0x09:      //ADD HL,BC
                cpu.hl += cpu.bc;
                cpu.carry = cpu.hl < cpu.bc;
                return;
        case 0x0A:      //LD A,(BC)
                cpu.a = memory[cpu.bc];
                return;
        case 0x0B:      //DEC BC
                cpu.bc--;
                return;
        case 0x0C:      //INC C
                setFlags(++cpu.c);
                cpu.aux = !(cpu.c & 0x0F);
                return;
        case 0x0D:      //DEC C
                setFlags(--cpu.c);
                cpu.aux = !(cpu.c & 0x0F ^ 0x0F);
                return;
        case 0x0E:      //LD C,n
                cpu.c = memory[cpu.ip++];
                return;
        case 0x0F:      //RRCA
                cpu.a = (cpu.a >> 1) | (cpu.a & 0x01) << 7;
                cpu.carry = !!(cpu.a & 0x80);
                return;
        case 0x11:      //LD DE,nn
                cpu.e = memory[cpu.ip++];
                cpu.d = memory[cpu.ip++];
                return;
        case 0x12:      //LD (DE),A
                memory[cpu.de] = cpu.a;
                return;
        case 0x13:      //INC DE
                cpu.de++;
                return;
        case 0x14:      //INC D
                setFlags(++cpu.d);
                cpu.aux = !(cpu.d & 0x0F);
                return;
        case 0x15:      //DEC D
                setFlags(--cpu.d);
                cpu.aux = !(cpu.d & 0x0F ^ 0x0F);
                return;
        case 0x16:      //LD D,n
                cpu.d = memory[cpu.ip++];
                return;
        case 0x17:      //RLA
                temp = !!(cpu.a & 0x80);
                cpu.a = (cpu.a << 1) | cpu.carry;
                cpu.carry = temp;
                return;
        case 0x19:      //ADD HL,DE
                cpu.hl += cpu.de;
                cpu.carry = cpu.hl < cpu.de;
                return;
        case 0x1A:      //LD A,(DE)
                cpu.a = memory[cpu.de];
                return;
        case 0x1B:      //DEC DE
                cpu.de--;
                return;
        case 0x1C:      //INC E
                setFlags(++cpu.e);
                cpu.aux = !(cpu.e & 0x0F);
                return;
        case 0x1D:      //DEC E
                setFlags(--cpu.e);
                cpu.aux = !(cpu.e & 0x0F ^ 0x0F);
                return;
        case 0x1E:      //LD E,n
                cpu.e = memory[cpu.ip++];
                return;
        case 0x1F:      //RRA
                temp = cpu.a & 0x01;
                cpu.a = (cpu.a >> 1) | (cpu.carry << 7);
                cpu.carry = temp;
                return;
        case 0x21:      //LD HL,nn
                cpu.l = memory[cpu.ip++];
                cpu.h = memory[cpu.ip++];
                return;
        case 0x22:      //LD (nn),HL
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip++] << 8;
                memory[addr++] = cpu.l;
                memory[addr] = cpu.h;
                return;
        case 0x23:      //INC HL
                cpu.hl++;
                return;
        case 0x24:      //INC H
                setFlags(++cpu.h);
                cpu.aux = !(cpu.h & 0x0F);
                return;
        case 0x25:      //DEC H
                setFlags(--cpu.h);
                cpu.aux = !(cpu.h & 0x0F ^ 0x0F);
                return;
        case 0x26:      //LD H,n
                cpu.h = memory[cpu.ip++];
                return;
        case 0x27:      //DAA
                return;
        case 0x29:      //ADD HL,HL
                addr = cpu.hl;
                cpu.hl *= 2;
                cpu.carry = cpu.hl < addr;
                return;
        case 0x2A:      //LD HL,(nn)
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip++] << 8;
                cpu.l = memory[addr++];
                cpu.h = memory[addr];
                return;
        case 0x2B:      //DEC HL
                cpu.hl--;
                return;
        case 0x2C:      //INC L
                setFlags(++cpu.l);
                cpu.aux = !(cpu.l & 0x0F);
                return;
        case 0x2D:      //DEC L
                setFlags(--cpu.l);
                cpu.aux = !(cpu.l & 0x0F ^ 0x0F);
                return;
        case 0x2E:      //LD L,n
                cpu.l = memory[cpu.ip++];
                return;
        case 0x2F:      //CPL
                cpu.a ^= 0xFF;
                return;
        case 0x31:      //LD SP,nn
                cpu.sp = memory[cpu.ip++];
                cpu.sp |= memory[cpu.ip++] << 8;
                return;
        case 0x32:      //LD (nn),A
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip++] << 8;
                memory[addr] = cpu.a;
                return;
        case 0x33:      //INC SP
                cpu.sp++;
                return;
        case 0x34:      //INC (HL)
                temp = ++memory[cpu.hl];
                setFlags(temp);
                cpu.aux = !(temp & 0x0F);
                return;
        case 0x35:      //DEC (HL)
                temp = -- memory[cpu.hl];
                setFlags(temp);
                cpu.aux = !(temp & 0x0F ^ 0x0F);
                return;
        case 0x36:      //LD (HL),n
                temp = memory[cpu.ip++];
                memory[cpu.hl] = temp;
                return;
        case 0x37:      //SCF
                cpu.carry = 1;
                return;
        case 0x39:      //ADD HL,SP
                cpu.hl += cpu.sp;
                cpu.carry = cpu.hl < cpu.sp;
                return;
        case 0x3A:      //LD A,(nn)
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip++] << 8;
                cpu.a = memory[addr];
                return;
        case 0x3B:      //DEC SP
                cpu.sp--;
                return;
        case 0x3C:      //INC A
                setFlags(++cpu.a);
                cpu.aux = !(cpu.a & 0x0F);
                return;
        case 0x3D:      //DEC A
                setFlags(--cpu.a);
                cpu.aux = !(cpu.a & 0x0F ^ 0x0F);
                return;
        case 0x3E:      //LD A,n
                cpu.a = memory[cpu.ip++];
                return;
        case 0x3F:      //CCF
                cpu.carry ^= 1;
                return;
        case 0x40:      //LD B,B
                cpu.b = cpu.b;
                return;
        case 0x41:      //LD B,C
                cpu.b = cpu.c;
                return;
        case 0x42:      //LD B,D
                cpu.b = cpu.d;
                return;
        case 0x43:      //LD B,E
                cpu.b = cpu.e;
                return;
        case 0x44:      //LD B,H
                cpu.b = cpu.h;
                return;
        case 0x45:      //LD B,L
                cpu.b = cpu.l;
                return;
        case 0x46:      //LD B,(HL)
                cpu.b = memory[cpu.hl];
                return;
        case 0x47:      //LD B,A
                cpu.b = cpu.a;
                return;
        case 0x48:      //LD C,B
                cpu.c = cpu.b;
                return;
        case 0x49:      //LD C,C
                cpu.c = cpu.c;
                return;
        case 0x4A:      //LD C,D
                cpu.c = cpu.d;
                return;
        case 0x4B:      //LD C,E
                cpu.c = cpu.e;
                return;
        case 0x4C:      //LD C,H
                cpu.c = cpu.h;
                return;
        case 0x4D:      //LD C,L
                cpu.c = cpu.l;
                return;
        case 0x4E:      //LD C,(HL)
                cpu.c = memory[cpu.hl];
                return;
        case 0x4F:      //LC C,A
                cpu.c = cpu.a;
                return;
        case 0x50:      //LD D,B
                cpu.d = cpu.b;
                return;
        case 0x51:      //LD D,C
                cpu.d = cpu.c;
                return;
        case 0x52:      //LD D,D
                cpu.d = cpu.d;
                return;
        case 0x53:      //LD D,E
                cpu.d = cpu.e;
                return;
        case 0x54:      //LD D,H
                cpu.d = cpu.h;
                return;
        case 0x55:      //LD D,L
                cpu.d = cpu.l;
                return;
        case 0x56:      //LD D,(HL)
                cpu.d = memory[cpu.hl];
                return;
        case 0x57:      //LD D,A
                cpu.d = cpu.a;
                return;
        case 0x58:      //LD E,B
                cpu.e = cpu.b;
                return;
        case 0x59:      //LD E,C
                cpu.e = cpu.c;
                return;
        case 0x5A:      //LD E,D
                cpu.e = cpu.d;
                return;
        case 0x5B:      //LD E,E
                cpu.e = cpu.e;
                return;
        case 0x5C:      //LD E,H
                cpu.e = cpu.h;
                return;
        case 0x5D:      //LD E,L
                cpu.e = cpu.l;
                return;
        case 0x5E:      //LD E,(HL)
                cpu.e = memory[cpu.hl];
                return;
        case 0x5F:      //LD E,A
                cpu.e = cpu.a;
                return;
        case 0x60:      //LD H,B
                cpu.h = cpu.b;
                return;
        case 0x61:      //LD H,C
                cpu.h = cpu.c;
                return;
        case 0x62:      //LD H,D
                cpu.h = cpu.d;
                return;
        case 0x63:      //LD H,E
                cpu.h = cpu.e;
                return;
        case 0x64:      //LD H,H
                cpu.h = cpu.h;
                return;
        case 0x65:      //LD H,L
                cpu.h = cpu.l;
                return;
        case 0x66:      //LD H,(HL)
                cpu.h = memory[cpu.hl];
                return;
        case 0x67:      //LD H,A
                cpu.h = cpu.a;
                return;
        case 0x68:      //LD L,B
                cpu.l = cpu.b;
                return;
        case 0x69:      //LD L,C
                cpu.l = cpu.c;
                return;
        case 0x6A:      //LD L,D
                cpu.l = cpu.d;
                return;
        case 0x6B:      //LD L,E
                cpu.l = cpu.e;
                return;
        case 0x6C:      //LD L,H
                cpu.l = cpu.h;
                return;
        case 0x6D:      //LD L,L
                cpu.l = cpu.l;
                return;
        case 0x6E:      //LD L,(HL)
                cpu.l = memory[cpu.hl];
                return;
        case 0x6F:      //LD L,A
                cpu.l = cpu.a;
                return;
        case 0x70:      //LD (HL),B
                memory[cpu.hl] = cpu.b;
                return;
        case 0x71:      //LD (HL),C
                memory[cpu.hl] = cpu.c;
                return;
        case 0x72:      //LD (HL),D
                memory[cpu.hl] = cpu.d;
                return;
        case 0x73:      //LD (HL),E
                memory[cpu.hl] = cpu.e;
                return;
        case 0x74:      //LD (HL),H
                memory[cpu.hl] = cpu.h;
                return;
        case 0x75:      //LD (HL),L
                memory[cpu.hl] = cpu.l;
                return;
        case 0x76:      //HALT
                return;
        case 0x77:      //LD (HL),A
                memory[cpu.hl] = cpu.a;
                return;
        case 0x78:      //LD A,B
                cpu.a = cpu.b;
                return;
        case 0x79:      //LD A,C
                cpu.a = cpu.c;
                return;
        case 0x7A:      //LD A,D
                cpu.a = cpu.d;
                return;
        case 0x7B:      //LD A,E
                cpu.a = cpu.e;
                return;
        case 0x7C:      //LD A,H
                cpu.a = cpu.h;
                return;
        case 0x7D:      //LD A,L
                cpu.a = cpu.l;
                return;
        case 0x7E:      //LD A,(HL)
                cpu.a = memory[cpu.hl];
                return;
        case 0x7F:      //LD A,A
                cpu.a = cpu.a;
                return;
        case 0x90:      //SUB B
                cpu.b *= -1;
        case 0x80:      //ADD B
                setFlags(cpu.a += cpu.b);
                cpu.carry = cpu.a < cpu.b;
                cpu.aux = cpu.a & 0x0F < cpu.b & 0x0F;
                return;
        case 0x91:      //SUB C
                cpu.c *= -1;
        case 0x81:      //ADD C
                setFlags(cpu.a += cpu.c);
                cpu.carry = cpu.a < cpu.c;
                cpu.aux = cpu.a & 0x0F < cpu.c & 0x0F;
                return;
        case 0x92:      //SUB D
                cpu.d *= -1;
        case 0x82:      //ADD D
                setFlags(cpu.a += cpu.d);
                cpu.carry = cpu.a < cpu.d;
                cpu.aux = cpu.a & 0x0F < cpu.d & 0x0F;
                return;
        case 0x93:      //SUB E
                cpu.e *= -1;
        case 0x83:      //ADD E
                setFlags(cpu.a += cpu.e);
                cpu.carry = cpu.a < cpu.e;
                cpu.aux = cpu.a & 0x0F < cpu.e & 0x0F;
                return;
        case 0x94:      //SUB H
                cpu.h *= -1;
        case 0x84:      //ADD H
                setFlags(cpu.a += cpu.h);
                cpu.carry = cpu.a < cpu.h;
                cpu.aux = cpu.a & 0x0F < cpu.h & 0x0F;
                return;
        case 0x95:      //SUB L
                cpu.l *= -1;
        case 0x85:      //ADD L
                setFlags(cpu.a += cpu.l);
                cpu.carry = cpu.a < cpu.l;
                cpu.aux = cpu.a & 0x0F < cpu.l & 0x0F;
                return;
        case 0x96:      //SUB (HL)
                temp = -memory[cpu.hl];
                setFlags(cpu.a += temp);
                cpu.carry = cpu.a < temp;
                cpu.aux = cpu.a & 0x0F < temp & 0x0F;
                return;
        case 0x86:      //ADD (HL)
                temp = memory[cpu.hl];
                setFlags(cpu.a += temp);
                cpu.carry = cpu.a < temp;
                cpu.aux = cpu.a & 0x0F < temp & 0x0F;
                return;
        case 0x97:      //SUB A
                cpu.a = 0;
                setFlags(cpu.a);
                cpu.carry = 0;
                cpu.aux = 0;
        case 0x87:      //ADD A
                temp = cpu.a;
                setFlags(cpu.a *= 2);
                cpu.carry = !!(temp & 0x80);
                cpu.aux = !!(temp & 0x08);
                return;
        case 0x98:      //SBC B
                cpu.b ^= 0xFF;
                cpu.carry ^= 0x01;
        case 0x88:      //ADC B
                setFlags(cpu.a += cpu.b + cpu.carry);
                cpu.carry = cpu.a < cpu.b;
                cpu.aux = cpu.a & 0x0F < cpu.b & 0x0F;
                return;
        case 0x99:      //SBC C
                cpu.c ^= 0xFF;
                cpu.carry ^= 0x01;
        case 0x89:      //ADC C
                setFlags(cpu.a += cpu.c + cpu.carry);
                cpu.carry = cpu.a < cpu.c;
                cpu.aux = cpu.a & 0x0F < cpu.c & 0x0F;
                return;
        case 0x9A:      //SBC D
                cpu.d ^= 0xFF;
                cpu.carry ^= 0x01;
        case 0x8A:      //ADC D
                setFlags(cpu.a += cpu.d + cpu.carry);
                cpu.carry = cpu.a < cpu.d;
                cpu.aux = cpu.a & 0x0F < cpu.d & 0x0F;
                return;
        case 0x9B:      //SBC E
                cpu.e ^= 0xFF;
                cpu.carry ^= 0x01;
        case 0x8B:      //ADC E
                setFlags(cpu.a += cpu.e + cpu.carry);
                cpu.carry = cpu.a < cpu.e;
                cpu.aux = cpu.a & 0x0F < cpu.e & 0x0F;
                return;
        case 0x9C:      //SBC H
                cpu.h ^= 0xFF;
                cpu.carry ^= 0x01;
        case 0x8C:      //ADC H
                setFlags(cpu.a += cpu.h + cpu.carry);
                cpu.carry = cpu.a < cpu.h;
                cpu.aux = cpu.a & 0x0F < cpu.h & 0x0F;
                return;
        case 0x9D:      //SBC L
                cpu.l ^= 0xFF;
                cpu.carry ^= 0x01;
        case 0x8D:      //ADC L
                setFlags(cpu.a += cpu.l + cpu.carry);
                cpu.carry = cpu.a < cpu.l;
                cpu.aux = cpu.a & 0x0F < cpu.l & 0x0F;
                return;
        case 0x9E:      //SBC (HL)
                temp = ~memory[cpu.hl];
                cpu.carry ^= 0x01;
                setFlags(cpu.a += temp + cpu.carry);
                cpu.carry = cpu.a < temp;
                cpu.aux = cpu.a & 0x0F < temp & 0x0F;
                return;
        case 0x8E:      //ADC (HL)
                temp = memory[cpu.hl];
                setFlags(cpu.a += temp + cpu.carry);
                cpu.carry = cpu.a < temp;
                cpu.aux = cpu.a & 0x0F < temp & 0x0F;
                return;
        case 0x9F:      //SBC A
                cpu.a = cpu.carry ? -1 : 0;
                setFlags(cpu.a);
                cpu.aux = cpu.carry;
                return;
        case 0x8F:      //ADC A
                temp = cpu.a;
                setFlags(cpu.a = cpu.a * 2 + cpu.carry);
                cpu.carry = !!(temp & 0x80);
                cpu.aux = !!(temp & 0x08);
                return;
        case 0xA0:      //AND B
                setFlags(cpu.a &= cpu.b);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xA1:      //AND C
                setFlags(cpu.a &= cpu.c);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xA2:      //AND D
                setFlags(cpu.a &= cpu.d);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xA3:      //AND E
                setFlags(cpu.a &= cpu.e);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xA4:      //AND H
                setFlags(cpu.a &= cpu.h);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xA5:      //AND L
                setFlags(cpu.a &= cpu.l);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xA6:      //AND (HL)
                setFlags(cpu.a &= memory[cpu.hl]);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xA7:      //AND A
                setFlags(cpu.a &= cpu.a);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xA8:      //XOR B
                setFlags(cpu.a ^= cpu.b);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xA9:      //XOR C
                setFlags(cpu.a ^= cpu.c);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xAA:      //XOR D
                setFlags(cpu.a ^= cpu.d);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xAB:      //XOR E
                setFlags(cpu.a ^= cpu.e);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xAC:      //XOR H
                setFlags(cpu.a ^= cpu.h);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xAD:      //XOR L
                setFlags(cpu.a ^= cpu.l);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xAE:      //XOR (HL)
                setFlags(cpu.a ^= memory[cpu.hl]);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xAF:      //XOR A
                setFlags(cpu.a ^= cpu.a);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xB0:      //OR B
                setFlags(cpu.a |= cpu.b);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xB1:      //OR C
                setFlags(cpu.a |= cpu.c);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xB2:      //OR D
                setFlags(cpu.a |= cpu.d);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xB3:      //OR E
                setFlags(cpu.a |= cpu.e);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xB4:      //OR H
                setFlags(cpu.a |= cpu.h);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xB5:      //OR L
                setFlags(cpu.a |= cpu.l);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xB6:      //OR (HL)
                setFlags(cpu.a |= memory[cpu.hl]);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xB7:      //OR A
                setFlags(cpu.a |= cpu.a);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xB8:      //CP B
                setFlags(cpu.a - cpu.b);
                cpu.carry = cpu.a < cpu.b;
                cpu.aux = cpu.a & 0x0F < cpu.b & 0x0F;
                return;
        case 0xB9:      //CP C
                setFlags(cpu.a - cpu.c);
                cpu.carry = cpu.a < cpu.c;
                cpu.aux = cpu.a & 0x0F < cpu.c & 0x0F;
                return;
        case 0xBA:      //CP D
                setFlags(cpu.a - cpu.d);
                cpu.carry = cpu.a < cpu.d;
                cpu.aux = cpu.a & 0x0F < cpu.d & 0x0F;
                return;
        case 0xBB:      //CP E
                setFlags(cpu.a - cpu.e);
                cpu.carry = cpu.a < cpu.e;
                cpu.aux = cpu.a & 0x0F < cpu.e & 0x0F;
                return;
        case 0xBC:      //CP H
                setFlags(cpu.a - cpu.h);
                cpu.carry = cpu.a < cpu.h;
                cpu.aux = cpu.a & 0x0F < cpu.h & 0x0F;
                return;
        case 0xBD:      //CP L
                setFlags(cpu.a - cpu.l);
                cpu.carry = cpu.a < cpu.l;
                cpu.aux = cpu.a & 0x0F < cpu.l & 0x0F;
                return;
        case 0xBE:      //CP (HL)
                temp = memory[cpu.hl];
                setFlags(cpu.a - temp);
                cpu.carry = cpu.a < temp;
                cpu.aux = cpu.a & 0x0F < temp & 0x0F;
                return;
        case 0xBF:      //CP A
                setFlags(cpu.a - cpu.a);
                cpu.carry = cpu.a < cpu.a;
                cpu.aux = cpu.a & 0x0F < cpu.a & 0x0F;
                return;
        case 0xC0:      //RET nz
                if (cpu.zero) return;
                cpu.ip = memory[cpu.sp++];
                cpu.ip |= memory[cpu.sp++] << 8;
                return;
        case 0xC1:      //POP BC
                cpu.b = memory[cpu.sp++];
                cpu.c = memory[cpu.sp++];
                return;
        case 0xC2:      //JP nz,nn
                if (cpu.zero) {
                        cpu.ip+=2;
                        return;
                }
        case 0xCB:
        case 0xC3:      //JP nn
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip] << 8;
                cpu.ip = addr;
                return;
        case 0xC4:      //CALL nz,nn
                if (cpu.zero) {
                        cpu.ip+=2;
                        return;
                }
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip++] << 8;
                memory[--cpu.sp] = (uint8_t)cpu.ip;
                memory[--cpu.sp] = (uint8_t)(cpu.ip >> 8);
                cpu.ip = addr;
                return;
        case 0xC5:      //PUSH BC
                memory[--cpu.sp] = cpu.c;
                memory[--cpu.sp] = cpu.b;
                return;
        case 0xC6:      //ADD n
                temp = memory[cpu.ip++];
                setFlags(cpu.a += temp);
                cpu.carry = cpu.a < temp;
                cpu.aux = cpu.a & 0x0F < temp & 0x0F;
                return;
        case 0xC7:      //RST $00
                memory[--cpu.sp] = (uint8_t)cpu.ip;
                memory[--cpu.sp] = (uint8_t)(cpu.ip >> 8);
                cpu.ip = 0x0000;
                return;
        case 0xC8:      //RET z
                if (!cpu.zero) return;
        case 0xD9:
        case 0xC9:      //RET
                cpu.ip = memory[cpu.sp++];
                cpu.ip |= memory[cpu.sp++] << 8;
                return;
        case 0xCA:      //JP z,nn
                if (!cpu.zero) {
                        cpu.ip+=2;
                        return;
                }
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip] << 8;
                cpu.ip = addr;
                return;
        case 0xCC:      //CALL z,nn
                if (!cpu.zero) {
                        cpu.ip+=2;
                        return;
                }
        case 0xDD:
        case 0xED:
        case 0xFD:
        case 0xCD:      //CALL nn
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip++] << 8;
                memory[--cpu.sp] = (uint8_t)cpu.ip;
                memory[--cpu.sp] = (uint8_t)(cpu.ip >> 8);
                cpu.ip = addr;
                return;
        case 0xCE:      //ADC n
                temp = memory[cpu.ip++];
                setFlags(cpu.a += temp + cpu.carry);
                cpu.carry = cpu.a < temp;
                cpu.aux = cpu.a & 0x0F < temp & 0x0F;
                return;
        case 0xCF:      //RST $08
                memory[--cpu.sp] = (uint8_t)cpu.ip;
                memory[--cpu.sp] = (uint8_t)(cpu.ip >> 8);
                cpu.ip = 0x0008;
                return;
        case 0xD0:      //RET nc
                if (cpu.carry) return;
                cpu.ip = memory[cpu.sp++];
                cpu.ip |= memory[cpu.sp++] << 8;
                return;
        case 0xD1:      //POP DE
                cpu.d = memory[cpu.sp++];
                cpu.e = memory[cpu.sp++];
                return;
        case 0xD2:      //JP nc,nn
                if (cpu.carry) {
                        cpu.ip+=2;
                        return;
                }
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip] << 8;
                cpu.ip = addr;
                return;
        case 0xD3:      //OUT (n),A
                temp = memory[cpu.ip++];
                portWrite(temp, cpu.a);
                return;
        case 0xD4:      //CALL nc,nn
                if (cpu.carry) {
                        cpu.ip+=2;
                        return;
                }
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip++] << 8;
                memory[--cpu.sp] = (uint8_t)cpu.ip;
                memory[--cpu.sp] = (uint8_t)(cpu.ip >> 8);
                cpu.ip = addr;
                return;
        case 0xD5:      //PUSH DE
                memory[--cpu.sp] = cpu.e;
                memory[--cpu.sp] = cpu.d;
                return;
        case 0xD6:      //SUB n
                temp = -memory[cpu.hl];
                setFlags(cpu.a += temp);
                cpu.carry = cpu.a < temp;
                cpu.aux = cpu.a & 0x0F < temp & 0x0F;
                return;
        case 0xD7:      //RST $10
                memory[--cpu.sp] = (uint8_t)cpu.ip;
                memory[--cpu.sp] = (uint8_t)(cpu.ip >> 8);
                cpu.ip = 0x0010;
                return;
        case 0xD8:      //RET c
                if (!cpu.carry) return;
                cpu.ip = memory[cpu.sp++];
                cpu.ip |= memory[cpu.sp++] << 8;
                return;
        case 0xDA:      //JP c,nn
                if (!cpu.carry) {
                        cpu.ip+=2;
                        return;
                }
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip] << 8;
                cpu.ip = addr;
                return;
        case 0xDB:      //IN A,(n)
                temp = memory[cpu.ip++];
                cpu.a = portRead(temp);
                return;
        case 0xDC:      //CALL c,nn
                if (!cpu.carry) {
                        cpu.ip+=2;
                        return;
                }
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip++] << 8;
                memory[--cpu.sp] = (uint8_t)cpu.ip;
                memory[--cpu.sp] = (uint8_t)(cpu.ip >> 8);
                cpu.ip = addr;
                return;
        case 0xDE:      //SBC n
                temp = ~memory[cpu.hl];
                cpu.carry ^= 0x01;
                setFlags(cpu.a += temp + cpu.carry);
                cpu.carry = cpu.a < temp;
                cpu.aux = cpu.a & 0x0F < temp & 0x0F;
        case 0xDF:      //RST $18
                memory[--cpu.sp] = (uint8_t)cpu.ip;
                memory[--cpu.sp] = (uint8_t)(cpu.ip >> 8);
                cpu.ip = 0x0018;
                return;
        case 0xE0:      //RET po
                if (cpu.parity) return;
                cpu.ip = memory[cpu.sp++];
                cpu.ip |= memory[cpu.sp++] << 8;
                return;
        case 0xE1:      //POP HL
                cpu.h = memory[cpu.sp++];
                cpu.l = memory[cpu.sp++];
                return;
        case 0xE2:      //JP po,nn
                if (cpu.parity) {
                        cpu.ip+=2;
                        return;
                }
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip] << 8;
                cpu.ip = addr;
                return;
        case 0xE3:      //EX (SP),HL
                addr = memory[cpu.sp] << 8;
                addr |= memory[cpu.sp+1];
                memory[cpu.sp] = cpu.h;
                memory[cpu.sp+1] = cpu.l;
                cpu.hl = addr;
                return;
        case 0xE4:      //CALL po,nn
                if (cpu.parity) {
                        cpu.ip+=2;
                        return;
                }
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip++] << 8;
                memory[--cpu.sp] = (uint8_t)cpu.ip;
                memory[--cpu.sp] = (uint8_t)(cpu.ip >> 8);
                cpu.ip = addr;
                return;
        case 0xE5:      //PUSH HL
                memory[--cpu.sp] = cpu.l;
                memory[--cpu.sp] = cpu.h;
                return;
        case 0xE6:      //AND n
                setFlags(cpu.a &= memory[cpu.hl]);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xE7:      //RST $20
                memory[--cpu.sp] = (uint8_t)cpu.ip;
                memory[--cpu.sp] = (uint8_t)(cpu.ip >> 8);
                cpu.ip = 0x0020;
                return;
        case 0xE8:      //RET pe
                if (!cpu.parity) return;
                cpu.ip = memory[cpu.sp++];
                cpu.ip |= memory[cpu.sp++] << 8;
                return;
        case 0xE9:      //JP HL
                cpu.ip = cpu.hl;
                return;
        case 0xEA:      //JP pe,nn
                if (!cpu.parity) {
                        cpu.ip+=2;
                        return;
                }
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip] << 8;
                cpu.ip = addr;
                return;
        case 0xEB:      //EX DE,HL
                addr = cpu.de;
                cpu.de = cpu.hl;
                cpu.hl = addr;
                return;
        case 0xEC:      //CALL pe,nn
                if (!cpu.parity) {
                        cpu.ip+=2;
                        return;
                }
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip++] << 8;
                memory[--cpu.sp] = (uint8_t)cpu.ip;
                memory[--cpu.sp] = (uint8_t)(cpu.ip >> 8);
                cpu.ip = addr;
                return;
        case 0xEE:      //XOR n
                setFlags(cpu.a ^= memory[cpu.hl]);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xEF:      //RST $28
                memory[--cpu.sp] = (uint8_t)cpu.ip;
                memory[--cpu.sp] = (uint8_t)(cpu.ip >> 8);
                cpu.ip = 0x0028;
                return;
        case 0xF0:      //RET p
                if (cpu.sign) return;
                cpu.ip = memory[cpu.sp++];
                cpu.ip |= memory[cpu.sp++] << 8;
                return;
        case 0xF1:      //POP AF
                cpu.a = memory[cpu.sp++];
                cpu.f = memory[cpu.sp++];
                return;
        case 0xF2:      //JP p,nn
                if (cpu.sign) {
                        cpu.ip+=2;
                        return;
                }
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip] << 8;
                cpu.ip = addr;
                return;
        case 0xF3:      //DI
                cpu.i = 0;
                return;
        case 0xF4:      //CALL p,nn
                if (cpu.sign) {
                        cpu.ip+=2;
                        return;
                }
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip++] << 8;
                memory[--cpu.sp] = (uint8_t)cpu.ip;
                memory[--cpu.sp] = (uint8_t)(cpu.ip >> 8);
                cpu.ip = addr;
                return;
        case 0xF5:      //PUSH AF
                memory[--cpu.sp] = cpu.f & 0xD5 | 0x02;
                memory[--cpu.sp] = cpu.a;
                //Mask the unused bits
                return;
        case 0xF6:      //OR n
                setFlags(cpu.a |= memory[cpu.hl]);
                cpu.carry = 0;
                cpu.aux = 0;
                return;
        case 0xF7:      //RST $30
                memory[--cpu.sp] = (uint8_t)cpu.ip;
                memory[--cpu.sp] = (uint8_t)(cpu.ip >> 8);
                cpu.ip = 0x0030;
                return;
        case 0xF8:      //RET m
                if (!cpu.sign) return;
                cpu.ip = memory[cpu.sp++];
                cpu.ip |= memory[cpu.sp++] << 8;
                return;
        case 0xF9:      //LD SP,HL
                cpu.sp = cpu.hl;
                return;
        case 0xFA:      //JP m,nn
                if (!cpu.sign) {
                        cpu.ip+=2;
                        return;
                }
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip] << 8;
                cpu.ip = addr;
                return;
        case 0xFB:      //EI
                cpu.i = 1;
                return;
        case 0xFC:      //CALL m,nn
                if (!cpu.sign) {
                        cpu.ip+=2;
                        return;
                }
                addr = memory[cpu.ip++];
                addr |= memory[cpu.ip++] << 8;
                memory[--cpu.sp] = (uint8_t)cpu.ip;
                memory[--cpu.sp] = (uint8_t)(cpu.ip >> 8);
                cpu.ip = addr;
                return;
        case 0xFE:      //CP n
                temp = memory[cpu.hl];
                setFlags(cpu.a - temp);
                cpu.carry = cpu.a < temp;
                cpu.aux = cpu.a & 0x0F < temp & 0x0F;
                return;
        case 0xFF:      //RST $38
                memory[--cpu.sp] = (uint8_t)cpu.ip;
                memory[--cpu.sp] = (uint8_t)(cpu.ip >> 8);
                cpu.ip = 0x0038;
                return;
        }
}
