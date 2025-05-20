jmpa &code

int_timer:
    pusha 0x0
    putda 0x0
    drop
    iretc 0x0

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
    iretc 0x0

interrupt_table:
    _int_timer dw &int_timer
    _int_invinst dw &int_invinst
    _int_priv dw &int_priv
    _int_invint dw &int_invint
    _int_invpacc dw &int_invpacc
    _int_invdev dw &int_invdev
    _int_devamch dw &int_devamch

// marker to be visible in memory dump 
_m_0 db "Page table 0 begin"
page_table_0:
    _0_pg_0 dw 0x1
    _1_pg_0 dw 0x3

code:
    pusha &interrupt_table
    setidtt
    drop
    
    pusha 0x6
    setidtst
    drop

    // setup page table
    pusha &page_table_0
    setptbat 
    drop

    pusha 0x2
    setptst  
    drop

    pusha 0x1000 // 4096 bytes
    setptpst  
    drop

    pusha 0b0000000000000000000000000000000000000000000000000000000000001000
    setstt
    drop
    
    // store 'A' on first page of first page table
    pusha 'A'
    storea 0x0
    drop

    // store 'B' on second page of first page table
    pusha 'B'
    storea 0x1000
    drop

    // store 'C' on second page of first page table
    pusha 'C'
    storea 0x2000
    drop

    halt