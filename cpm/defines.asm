.IFNDEF defines_h
.DEFINE defines_h

.MEMORYMAP
SLOTSIZE $10000
DEFAULTSLOT 0
SLOT 0 $0000
.ENDME

.ROMBANKMAP
BANKSTOTAL 1
BANKSIZE $10000
BANKS 1
.ENDRO

.DEFINE SECPERTRK       256

.DEFINE BIOSSECTOR      1
.DEFINE BIOSTRACK       0
.DEFINE BIOSSTART       $FF00

.DEFINE BDOSSECTOR      3
.DEFINE BDOSTRACK       0
.DEFINE BDOSSTART       $FE00

.DEFINE CCPSECTOR       4
.DEFINE CCPTRACK        0
.DEFINE CCPSTART        $FD00

.ENDIF
