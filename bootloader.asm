# PASSO A PASSO (inicial)

# BIOS pega os devices e os carrega em 0x7C00
# Checa se a assinatura 0xAA55 está lá, e assim começa a exxecutar

ORG 0x7C00 # Seta a origem para esse endereço

BITS 16 # Number of Bits to OS 
        # Even 64-bits start at 16 bits

main:
  HLT # Pauses the CPU

halt: # Waits for execution 
  JMP halt

# Writing the signature
  # We have to get to the last two bytes and write 0xAA55

  TIMES 510 -($ - $$) DB 0
    # ($ - $$) tells how many bytes this program takes so far
    # So, 510 - that gets us to the memory position 510
    # Then, we fill it with zeroes
  
  DW 0AA55h # Writes signature to the end