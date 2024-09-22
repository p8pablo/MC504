; MC504 bootloader.asm - boot loader que printa "Hello, Boot!" 
; Alunos:
;  Pablo Henrique Almeida Mendes RA:230977
;  Luiz Felipe Corradini Rego Costa: RA:230613

    BITS 16                   

    org 0x7c00                ; BIOS carrega o bootloader na posição 0x7c00
start:
    mov si, hello_message      ; carrega a mensagem no si
    call print_string          ; função de print

    ; loop que mantém o bootloader rodando
hang:
    jmp hang

; função para printar a string terminada em 0
print_string:
    mov ah, 0x0e               ; função de output do bootloader
.next_char:
    lodsb                      ; carrega o proximo byte de si em al
    cmp al, 0                  ; compara al com 0 para achar o final da string
    je .done                   ; se al é 0 terminamos a string
    int 0x10                   ; interrupção para printar um caractere
    jmp .next_char             ; repete para o proximo caractere
.done:
    ret                        ; retorna a função

hello_message db 'Hello, Boot!', 0 

times 510 - ($ - $$) db 0      
dw 0xaa55                      
