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

    pushiret // ((... iretac)

    loada &active_process // (... iretac active)
    pusha &process_0 // (... iretac active process_0)
    cmp // (... iretac active process_0 result)
    swap // (... iretac active result process_0)
    drop // (... iretac active result)
    swap // (... iretac result active)
    drop // (... iretac result)

    jnza &int_timer_continue_0 // active process is process 0
    drop // (... iretac)

    // set active process
    pusha &process_1
    storea &active_process
    drop

    loada &process_iret // (... iretac (new iretac))
    swap // (... (new iretac) iretac)
    storea &process_iret
    drop // (... (new iretac))
    jmpa &int_timer_return

    int_timer_continue_0:
    drop // (... iretac)

    loada &active_process // (... iretac active)
    pusha &process_1 // (... iretac active process)
    cmp // (... iretac active process result)
    swap // (... iretac active result process)
    drop // (... iretac active result)
    swap // (... iretac result active)
    drop // (... iretac result)

    jnza &int_timer_continue_1 // active process is process 1
    drop // (... iretac)

    // set active process
    pusha &process_0
    storea &active_process
    drop

    loada &process_iret // (... iretac (new iretac))
    swap // (... (new iretac) iretac)
    storea &process_iret
    drop // (... (new iretac))
    jmpa &int_timer_return

    jmpa &int_timer_return

    int_timer_continue_1:
    drop
    // process is not in a table

    int_timer_return:
    setirett
    drop
    iretac 0x0
