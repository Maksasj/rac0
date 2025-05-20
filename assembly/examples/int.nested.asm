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

interrupt_table:
    _0_int_timer dw 0x0     // 0
    _0_int_invinst dw 0x0   // 1
    _0_int_priv dw 0x0      // 2
    _0_int_invint dw 0x0    // 3
    _0_int_invpacc dw 0x0   // 4
    _0_int_invdev dw 0x0    // 5
    _0_int_devamch dw 0x0   // 6
    
    _0_int_0 dw &int0       // 7
    _0_int_1 dw &int1       // 8
    _0_int_2 dw &int2       // 9
    _0_int_3 dw &int3       // 10

int0:
    pusha 0x0
    pushpc
    jmpa &print_top
    drop
    drop

    int 0x8
    
    iretac 0x0

int1:
    pusha 0x1
    pushpc
    jmpa &print_top
    drop
    drop

    int 0x9

    iretac 0x0

int2:
    pusha 0x2
    pushpc
    jmpa &print_top
    drop
    drop

    int 0xa

    iretac 0x0

int3:
    pusha 0x3
    pushpc
    jmpa &print_top
    drop
    drop

    iretac 0x0

code:
    pusha &interrupt_table
    setidtt
    drop
    
    pusha 0xb
    setidtst
    drop
    
    int 0x7

    halt