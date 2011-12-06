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

LOOP:
    ADD R3, R3, -1
    BRNP LOOP

    HALT
