; PS/2 keyboard handling implementation
GLOBAL getPressedKey

; PS/2 keyboard controller ports
%define KBD_DATA_PORT   0x60
%define KBD_STATUS_PORT 0x64

section .text

; uint8_t getPressedKey()
; Returns the raw scan code from the keyboard
; This includes make codes (key press) and break codes (key release)
getPressedKey:
    push rbp
    mov rbp, rsp
    
    ; Read keyboard status port
    in al, KBD_STATUS_PORT
    ; Check if output buffer is full (bit 0 = 1)
    test al, 0x01
    jz .no_data
    
    ; Read keyboard data port
    in al, KBD_DATA_PORT
    jmp .done
    
.no_data:
    xor al, al    ; Return 0 if no data available
    
.done:
    leave
    ret