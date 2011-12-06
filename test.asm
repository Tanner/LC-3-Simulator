    .ORIG x3000

    AND R0, R0, 0
    AND R1, R1, 0
    AND R2, R2, 0
    AND R3, R3, 0
    AND R4, R4, 0
    AND R5, R5, 0
    AND R6, R6, 0
    AND R7, R7, 0

    ADD R0, R0, 1
    ADD R1, R1, 2
    ADD R2, R0, R1

    ADD R3, R3, 10

    JSR LOOP

    ADD R4, R4, 11

LOOP:
    ADD R3, R3, -1
    BRNP LOOP

    !JSRR R0

    LD R3, NUMBER

    LD R4, NOT_NUMBER
    NOT R4, R4

    ST R3, STORE
    LD R5, STORE

    LEA R6, STORE
    ST R6, STORE_LOC
    STI R1, STORE_LOC
    LD R5, STORE

    STR R4, R3, 0

    LDI R6, STORE_LOC

    LD R5, STORE_LOC
    LDR R0, R5, 0

    LD R0, A
    OUT

    !GETC
    OUT

    LEA R0, STRING
    PUTS

    IN
    OUT

    LEA R0, LETTERS
    PUTSP

    LD R1, 1
    LD R1, -1

    HALT

NUMBER:
    .fill 1261
NOT_NUMBER:
    .fill 0x5555
STORE:
    .blkw 1
STORE_LOC:
    .blkw 1
A
    .fill 65
STRING
    .stringz "Hello World"
LETTERS
    .fill 0x4554
    .fill 0x5453
    .fill 0x0000
