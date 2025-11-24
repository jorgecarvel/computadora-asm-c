; factorial_no_labels.asm - generado por c_to_asm
; MEM[100] = N
; MEM[101] = contador
; MEM[200] = resultado
; MEM[2]   = const 1 (usada para decrementar)

        LOADI 5        ; 0..1   A=5
        STORE 100      ; 2..3   MEM[100]=5

        LOADI 1        ; 4..5   A=1
        STORE 200      ; 6..7   MEM[200]=1

        LOADI 1        ; 8..9   A=1
        STORE 2        ; 10..11   MEM[2]=1

        LOADM 100      ; 12..13 A = MEM[100]
        STORE 101      ; 14..15 MEM[101] = N (contador)

        LOADM 101      ; 16..17 A = contador
        JMPZ 34        ; 18..19 salto a fin

        LOADM 200      ; 20..21 A = resultado
        MUL 101        ; 22..23 A = resultado * contador
        STORE 200      ; 24..25 MEM[200] = A

        LOADM 101      ; 26..27 A = contador
        SUB 2          ; 28..29 A = A - MEM[2]
        STORE 101      ; 30..31 MEM[101] = nuevo contador

        JMP 16         ; 32..33 volver al inicio del bucle

        HALT           ; 34 fin
