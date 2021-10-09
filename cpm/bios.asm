.INCLUDE "defines.asm"

.IF (BIOSSTART & $FF) != 0
.FAIL "BIOS must start on a page boundry"
.ENDIF

;BIOS use of Low Storage:
;$40: Console peek buffer
;$41: Current Sector
;$42: Current Track
;$44: DMA address

.BANK 0 SLOT 0
.ORG BIOSSECTOR*128+(BIOSTRACK*SECPERTRK*128)
.ORGA BIOSSTART
;BIOS entry points:
  JP Boot
  JP WarmBoot
  JP ConStat
  JP ConIn
  JP ConOut
  JP LptOut
  JP TapeOut
  JP TapeIn
  JP ResetDisk
  JP SelDisk
  JP SetTrack
  JP SetSector
  JP SetDMA
  JP ReadSector
  JP WriteSector

Boot:
;Print screen message
;Load BDOS (max size 32k)
  LD C,0
  LD E,0
  CALL SelDisk
  LD B,0
  CALL SetTrack
  LD C,2
  CALL SetSector
  LD BC,BDOSSTART
  CALL SetDMA
  LD C,BDOSSIZE
-
  CALL ReadSector
  DEC C
  JP nz,-
;Init Low Storage
  LD HL,0
  LD (HL),<BIOSSTART
  INC L
  LD (HL),>BIOSSTART+3
  INC L
  LD (HL),0     ;IO byte
  INC L
  LD (HL),0     ;DSK byte
  INC L
  LD (HL),<BDOSSTART
  INC L
  LD (HL),>BDOSSTART
  LD L,$40
  LD (HL),$FF   ;Clear character buffer
;Perform warm boot
WarmBoot:
;Load CCP
;Jump to CCP

ConStat:
  ;Character in peek buffer?
  LD A,($0040)
  INC A
  LD A,$FF
  RET nz
  ;Character in wait?
  IN A,(0)
  LD ($0040),A
  INC A
  LD A,$FF
  RET nz
  INC A
  RET

ConIn:
  CALL ConStat
  OR A
  JP z,ConIn
  LD A,($0040)
  LD C,A
  LD A,$FF
  LD ($0040),A
  LD A,C
  RET

ConOut:
  LD A,C
  OUT (0),A
LptOut:
TapeOut:
  RET

TapeIn:
  XOR A
  RET

ResetDisk:
  XOR A
  OUT (1),A
  OUT (2),A
  OUT (3),A
  OUT (4),A
  RET

SelectDisk:     ;Help
  XOR A
  LD H,A
  LD L,A
  CP C
  RET nz
  LD HL,ParamBlock
  RET

SetTrack:
  LD A,C
  LD ($0042),A
  LD A,B
  LD ($0043),A
  RET

SetSector:
  LD ($0041),A
  RET

SetDMA:
  LD A,C
  LD ($0044),A
  LD A,B
  LD ($0045),A
  RET

SectorSetup:
  OR A      ;Clear carry
  LD A,($0043)
  RRA
  LD B,A
  LD A,($0042)
  RRA
  LD C,A
  LD A,($0041)
  RRA
  LD L,A
  LD A,0
  ADC 0
  RRCA
  OUT (1),A
  LD A,L
  OUT (2),A
  LD A,C
  OUT (3),A
  LD A,B
  OUT (4),A
  LD A,($0044)
  LD L,A
  LD A,($0045)
  LD H,A
  LD C,128
  RET

ReadSector:
  CALL SectorSetup
-
  IN A,(5)
  LD (HL),A
  INC HL
  DEC C
  JP nz,-
  XOR A
  RET

WriteSector:
  CALL SectorSetup
-
  LD A,(HL)
  OUT (5),A
  INC HL
  DEC C
  JP nz,-
  XOR A
  RET

ParamBlock:
;Space for the Disk Parameter Header
.DSB 16
DriveBlock:
;Space for the Disk Parameter Block
.DSB 15
VectorBlock:
;Space for the Allocation Vector
.DSB 256

.DEFINE BIOSSIZE $-BIOSSTART
