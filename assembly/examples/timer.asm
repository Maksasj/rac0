jmpa &code

// user
process_0:
    pusha 0x0
    putda 0x0
    drop
    jmpa &process_0

process_1:
    pusha 0xaaaaaaaaaaaaaaaa
    putda 0x0
    drop
    jmpa &process_1

code:
    // setup table
    pusha &interrupt_table_0
    setidtt
    drop
    
    pusha 0x7
    setidtst
    drop

    // setup timer
    pusha 0x12c // 300
    settt
    drop

    pusha 0b0000000000000000000000000000000000000000000000000000000000000100
    setstt
    drop

    jmpa &process_0

// system
interrupt_table_0:
    _int_timer dw &int_timer
    _int_invinst dw 0x0
    _int_priv dw 0x0
    _int_invint dw 0x0
    _int_invpacc dw 0x0
    _int_invdev dw 0x0
    _int_devamch dw 0x0

process_table:
    active_process dw &process_0
    process_iret dw &process_1

int_timer:
    // reset timer
    pusha 0x12c // 300
    settt
    drop

    pusha 0b0000000000000000000000000000000000000000000000000000000000000100
    setstt
    drop

    pushiret // ((... iret)

    loada &active_process // (... iret active)
    pusha &process_0 // (... iret active process_0)
    cmp // (... iret active process_0 result)
    swap // (... iret active result process_0)
    drop // (... iret active result)
    swap // (... iret result active)
    drop // (... iret result)

    jnza &int_timer_continue_0 // active process is process 0
    drop // (... iret)

    // set active process
    pusha &process_1
    storea &active_process
    drop

    loada &process_iret // (... iret (new iret))
    swap // (... (new iret) iret)
    storea &process_iret
    drop // (... (new iret))
    jmpa &int_timer_return

    int_timer_continue_0:
    drop // (... iret)

    loada &active_process // (... iret active)
    pusha &process_1 // (... iret active process)
    cmp // (... iret active process result)
    swap // (... iret active result process)
    drop // (... iret active result)
    swap // (... iret result active)
    drop // (... iret result)

    jnza &int_timer_continue_1 // active process is process 1
    drop // (... iret)

    // set active process
    pusha &process_0
    storea &active_process
    drop

    loada &process_iret // (... iret (new iret))
    swap // (... (new iret) iret)
    storea &process_iret
    drop // (... (new iret))
    jmpa &int_timer_return

    jmpa &int_timer_return

    int_timer_continue_1:
    drop
    // process is not in a table

    int_timer_return:
    setirett
    drop
    iret 0x0
