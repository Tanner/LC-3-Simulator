    .ORIG x3000

    AND R1, R1, 0
    ADD R1, R1, 5
    
    AND R0, R0, 0
ME  ADD R0, R0, 15

    LEA R3, STRING
    LDR R2, R3, 0

    AND R4, R4, 0
    ADD R4, R4, 0xFFFF
    NOT R4, R4

    HALT

STRING .fill 0x2419
    .fill 0xFF00
