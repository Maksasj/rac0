jmpa &code

int_print_top: // (value pc)
    putda 0x0
    iretac 0x0

interrupt_table:
    _0_int_timer dw 0x0
    _0_int_invinst dw 0x0
    _0_int_priv dw 0x0
    _0_int_invint dw 0x0
    _0_int_invpacc dw 0x0
    _0_int_invdev dw 0x0
    _0_int_devamch dw 0x0

    _0_int_0 dw &int_print_top

// marker to be visible in memory dump 
_m_0 db "Page table 0 begin"
page_table_0:
    _0_pg_0 dw 0x1
    _1_pg_0 dw 0x3

_m_1 db "Page table 1 begin"
page_table_1:
    _0_pg_1 dw 0x2
    _1_pg_1 dw 0x4

code:
    // setup table
    pusha &interrupt_table
    setidtt
    drop
    
    pusha 0x9
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

    // enable paging
    pusha 0b0000000000000000000000000000000000000000000000000000000000001000
    setstt
    drop

    // store hello on first page of first page table
    pusha 'H'
    storea 0x0
    drop
    
    pusha 'E'
    storea 0x1
    drop
    
    pusha 'L'
    storea 0x2
    drop
    
    pusha 'L'
    storea 0x3
    drop
    
    pusha 'O'
    storea 0x4
    drop

    pusha '_'
    storea 0x5
    drop

    pusha '0'
    storea 0x6
    drop

    // store hello on second page of first page table
    pusha 'H'
    storea 0x1000
    drop
    
    pusha 'E'
    storea 0x1001
    drop
    
    pusha 'L'
    storea 0x1002
    drop
    
    pusha 'L'
    storea 0x1003
    drop
    
    pusha 'O'
    storea 0x1004
    drop
    
    pusha '_'
    storea 0x1005
    drop
    
    pusha '1'
    storea 0x1006
    drop


    // setup page table
    pusha &page_table_1
    setptbat 
    drop
    
    pusha 0x2
    setptst  
    drop
    
    pusha 0x1000 // 4096 bytes
    setptpst  
    drop

    // store hello on first page of second page table
    pusha 'H'
    storea 0x0
    drop
    
    pusha 'E'
    storea 0x1
    drop
    
    pusha 'L'
    storea 0x2
    drop
    
    pusha 'L'
    storea 0x3
    drop
    
    pusha 'O'
    storea 0x4
    drop

    pusha '_'
    storea 0x5
    drop

    pusha '2'
    storea 0x6
    drop

    // store hello on second page of first page table
    pusha 'H'
    storea 0x1000
    drop
    
    pusha 'E'
    storea 0x1001
    drop
    
    pusha 'L'
    storea 0x1002
    drop
    
    pusha 'L'
    storea 0x1003
    drop
    
    pusha 'O'
    storea 0x1004
    drop
    
    pusha '_'
    storea 0x1005
    drop
    
    pusha '3'
    storea 0x1006
    drop

    halt