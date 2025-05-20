jmpa &code

int_timer:
    pusha 0x0
    putda 0x0
    drop
    iretac 0x0

int_invinst:
    pusha 0x1
    putda 0x0
    drop
    halt

int_priv:
    pusha 0x2
    putda 0x0
    drop
    halt

int_invint:
    pusha 0x3
    putda 0x0
    drop
    halt

int_invpacc:
    pusha 0x4
    putda 0x0
    drop
    halt

int_invdev:
    pusha 0x5
    putda 0x0
    drop
    halt

int_devamch:
    pusha 0x6
    putda 0x0
    drop
    iretac 0x0

interrupt_table:
    _int_timer dw &int_timer
    _int_invinst dw &int_invinst
    _int_priv dw &int_priv
    _int_invint dw &int_invint
    _int_invpacc dw &int_invpacc
    _int_invdev dw &int_invdev
    _int_devamch dw &int_devamch

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
    
    int 0x7

    halt