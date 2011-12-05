    .ORIG x3000

    AND R0, R0, 0
ME  ADD R0, R0, 15
    
    BRp ME

    LDR R1, R6, 0

    HALT
