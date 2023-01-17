.segment "NES_FILE_HEADER"
.byte "NES", $1A, 2

.segment	"CODE"

stop_1 = $00
stop_2 = $01
stx_1 = $03

    LDA #$FF
    STA stop_1
    LDA #$0A
    STA stop_2
    LDY #$FC
    LDX #$00
out_loop:
    STY stx_1   ;2B
    JSR in_loop ;3B
    INY         ;1B
    CPY stop_1  ;2B
    BMI *-8
shut_down:
    LDA #$01
    STA $07FF

in_loop:
    TXA        ;1B
    ADC stx_1  ;2B
    INX        ;1B
    CPX stop_2 ;2B
    BMI *-6
    RTS

.segment "INTERRUPT_VECTOR"
    .word $0000, $8000, $8000