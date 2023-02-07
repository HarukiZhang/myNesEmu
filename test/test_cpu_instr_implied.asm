.segment "NES_FILE_HEADER"
.byte "NES", $1A, 2

.segment	"CODE"

.byte $1A ; unofficial instr
.byte $3A
.byte $5A
.byte $7A
.byte $DA
.byte $FA

.segment "INTERRUPT_VECTOR"
    .word $0000, $8000, $8000