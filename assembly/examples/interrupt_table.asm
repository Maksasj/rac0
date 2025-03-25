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
    _0_int_timer dw 0x0
    _0_int_invinst dw 0x0
    _0_int_priv dw 0x0
    _0_int_invint dw 0x0
    _0_int_invpacc dw 0x0
    _0_int_invdev dw 0x0
    _0_int_devamch dw 0x0

    _0_int_0 dw &int0
    _0_int_1 dw &int1

interrupt_table_1:
    _1_int_timer dw 0x0
    _1_int_invinst dw 0x0
    _1_int_priv dw 0x0
    _1_int_invint dw 0x0
    _1_int_invpacc dw 0x0
    _1_int_invdev dw 0x0
    _1_int_devamch dw 0x0

    _1_int_0 dw &int2
    _1_int_1 dw &int3

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
    
    pusha 0x9
    setidtst
    drop
    
    int 0x7
    int 0x8

    pusha &interrupt_table_1
    setidtt
    drop
    
    pusha 0x9
    setidtst
    drop
    
    int 0x7
    int 0x8

    halt