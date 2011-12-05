    .ORIG x3000

    AND R0, R0, 0
ME  ADD R0, R0, 15

    JSRR R0
    JMP R0
    
    BRp ME

    LDR R1, R6, 0

    HALT
