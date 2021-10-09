.INCLUDE "defines.asm"

.BANK 0 SLOT 0
.ORG 0
.ORGA 0
;Print screen message
;Load in BIOS
  LD A,128
  OUT (1),A
  XOR A
  OUT (2),A
  OUT (3),A
  OUT (4),A
  LD HL,BIOSSTART
-
  IN A,(5)
  LD (HL),A
  INC L
  JP nz,-
  INC H
  JP nz,-
  JP BIOSSTART          ;Warm boot call

.ORG 100
;Disk information
        ;128 byte sectors
        ;256 sectors per track
        ;64 tracks per disk
.DW SECPERTRK   ;Sectors per track
.DB 3           ;Block shift
.DB 7           ;Block mask
.DB 0           ;Extent mask
.DW 2047        ;Block count
.DW 255         ;Directory size
.DW %0000000000000000   ;Directory allocation bitmap
.DW 0           ;Checksum
.DW 1           ;Track start offset/Reserved tracks
