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

;Call the appropriate function
  LD A,C
  ADD A
  ADD <FUNCLIST
  LD L,A
  LD A,0
  ADC >FUNCLIST
  LD H,A
  JP HL

FUNCLIST:
.dw BIOSSTART+3*1
.dw BIOSSTART+3*3
.dw BIOSSTART+3*4
.dw AUXIN
.dw AUXOUT
.dw LPTOUT
.dw 0
.dw 0
.dw 0
.dw STROUT
.dw CONINBUF
.dw CONSTAT
.dw VERNUM
.dw DISKRES
.dw DISKSEL
.dw FILEOPEN
.dw FILECLOS
.dw SEEKFIRS
.dw SEEKNEXT
.dw FILEDEL
.dw RECREAD
.dw RECWRITE
.dw FILEMAKE
.dw FILEREN
.dw DRIVLOG
.dw DRIVTHIS
.dw ALLOCMAP
.dw DISKRO
.dw DISKROMP
.dw FILEATTR

RESET:
;Load CCP and turn over control
  RET

AUXIN:
;Get character from current auxiliary device
  RET

AUXOUT:
;Send character to current auxiliary device
  RET

LPTOUT:
;Print character on printer
  RET

STROUT:
;Send full string to console, interpreting some control characters
  RET

CONINBUF:
;Read from console, preinterpreting some control characters
  RET

CONSTAT:
;Is there text waiting to be read?
  RET

VERNUM:
;Which version of CP/M are we pretending to be?
  XOR A         ;Pre-2.0 CP/M
  LD B,A
  LD H,A
  LD L,A
  RET

DISKRES:
;Initialize Disk system to default
  RET

DISKSEL:
;Select current drive
  RET

FILEOPEN:
;Fill out the given FCB for a file
  RET

FILECLOS:
;Save FCB and stop enabling file for operations
  RET

SEEKFIRS:
;Finds first file that matches the given FCB, and load in its directory info
  RET

SEEKNEXT:
;Find the next file that matches the given FCB, as above
  RET

FILEDEL:
;Remove every file on the drive that matches this FCB
  RET

RECREAD:
;Read the next record this FCB points to
  RET

RECWRITE:
;Write the next record this FCB points to
  RET

FILEMAKE:
;Create and open the file for this FCB
  RET

FILEREN:
;Rename the file in the FCB to the name in its second half
  RET

DRIVLOG:
;Returns which drives are logged in
  LD A,$01
  LD L,A
  RET

DRIVTHIS:
;Return which drive is default
  XOR A
  LD L,A
  RET

ALLOCMAP:
;Move the DMA address to the provided value
  RET

DISKRO:
;Write protect the current disk (in software)
  RET

DISKROMP:
;Get which disks are marked as Read Only
  RET

FILEATTR:
;Set the attributes for the given file
  RET

.DEFINE BDOSSIZE $-BDOSSTART
