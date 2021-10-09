;BDOS call format:
    ;C= Function no.
    ;DE=Argument
    ;Proceed to CALL 5
    ;Values returned in A and L if 8 bit, or BA and HL if 16 bit

;Function 0:    System Reset
;Function 1:    Console input
;Function 2:    Console output
;Function 3:    Auxiliary input
;Function 4:    Auxiliary output
;Function 5:    Printer output
;Function 9:    Output string
;Function 10:   Buffered console input
;Function 11:   Console status
;Function 12:   Return version number
;Function 13:   Reset disks
;Function 14:   Select disk
;Function 15:   Open file
;Function 16:   Close file
;Function 17:   Search for first file
;Function 18:   Search for next file
;Function 19:   Delete file
;Function 20:   Read next record
;Function 21:   Write next record
;Function 22:   Create file
;Function 23:   Rename file
;Function 24:   Return bitmap of logged-in drives
;Function 25:   Return current drive
;Function 26:   Set DMA address
;Function 27:   Return address of allocation map
;Function 28:   Set disk read-only
;Function 29:   Return bitmap of read-only drives
;Function 30:   Set file attributes

.INCLUDE "defines.asm"


.BANK 0 SLOT 0
.ORG BDOSSECTOR*128+(BDOSTRACK*SECPERTRK*128)
.ORGA BDOSSTART

.DEFINE BDOSSIZE $-BDOSSTART
