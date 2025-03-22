jmpa &code

print_top: // (value pc)
    swap
    putda 0x0
    swap

    pusha 0x14  
    add
    swap
    drop
    swap
    drop
    
    jmpt

interrupt_table_0:
    _int_0 dw &int0
    _int_1 dw &int1

interrupt_table_1:
    _int_2 dw &int2
    _int_3 dw &int3

int0:
    pusha 0x0
    pushpc
    jmpa &print_top
    drop
    drop
    
    iret 0x0

int1:
    pusha 0x1
    pushpc
    jmpa &print_top
    drop
    drop

    iret 0x0

int2:
    pusha 0x2
    pushpc
    jmpa &print_top
    drop
    drop

    iret 0x0

int3:
    pusha 0x3
    pushpc
    jmpa &print_top
    drop
    drop

    iret 0x0

code:
    pusha &interrupt_table_0
    setidtt
    drop
    
    pusha 0x2
    setidtst
    drop
    
    int 0x0
    int 0x1

    pusha &interrupt_table_1
    setidtt
    drop
    
    pusha 0x2
    setidtst
    drop
    
    int 0x0
    int 0x1

    halt