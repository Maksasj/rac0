jmpa &code

int_priv:
    pusha 0x2
    putda 0x0
    drop
    halt

interrupt_table:
    _int_timer dw 0x0
    _int_invinst dw 0x0
    _int_priv dw &int_priv
    _int_invint dw 0x0
    _int_invpacc dw 0x0
    _int_invdev dw 0x0
    _int_devamch dw 0x0

code:
    pusha &interrupt_table
    setidtt
    drop
    
    pusha 0x6
    setidtst
    drop

    // set to user mode
    pusha 0b0000000000000000000000000000000000000000000000000000000000000001
    setstt
    drop
    
    halt