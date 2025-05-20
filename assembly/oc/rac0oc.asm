jmpa &rac0oc_entry

_m_1 db "===== KERNEL ENTRY ===="
rac0oc_entry:
    // set interrupt table
    pusha &rac0oc_interrupt_table
    setidtt
    drop

    // set number of interrupts
    pusha 0x7
    setidtst
    drop

    // TODO setuping staring processes
    pusha &test_process_1 // (iret)
    pusha 0x0 // (iret) (0x0)
    storearac &process_iret_table // process_iret_table[0x0] = iret | (iret)
    drop

    pusha &test_process_2 // (iret)
    pusha 0x1 // (iret) (0x0)
    storearac &process_iret_table // process_iret_table[0x0] = iret | (iret)
    drop

    pusha &test_process_3 // (iret)
    pusha 0x2 // (iret) (0x0)
    storearac &process_iret_table // process_iret_table[0x0] = iret | (iret)
    drop

    jmpa &rac0oc_schedule_routine

    halt

_m_0 db "===== KERNEL CODE ====="
rac0oc_schedule_routine:
    // TODO actually choose an active process

    // restore stack
    // jump to active process
    loada &active_process_index // ( index )
    loadarac &process_iret_table // push(process_iret_table[index]) | ( iret ) 
    jmptc
    // halt

// Interrupt handlers
rac0oc_int_timer_handler:
    // timer interrupt delegates timer handling to schedule routine 
    jmpa &rac0oc_schedule_routine

rac0oc_int_invinst_handler:
    halt

rac0oc_int_priv_handler:
    halt
rac0oc_int_invint_handler:
    halt

rac0oc_int_invpacc_handler:
    halt

rac0oc_int_invdev_handler:
    halt

rac0oc_int_devamch_handler:
    halt

_m_2 db "=== INTERRUPT TABLE ==="
rac0oc_interrupt_table:
    rac0oc_int_timer dw &rac0oc_int_timer
    rac0oc_int_invinst dw &rac0oc_int_invinst
    rac0oc_int_priv dw &rac0oc_int_priv
    rac0oc_int_invint dw &rac0oc_int_invint
    rac0oc_int_invpacc dw &rac0oc_int_invpacc
    rac0oc_int_invdev dw &rac0oc_int_invdev
    rac0oc_int_devamch dw &rac0oc_int_devamch

_m_3 db "==== PROCESS SCHEDULING ===="
    active_process_index dw 0x0

@constval PROCESS_DEAD 0x0 
@constval PROCESS_ALIVE 0x1
@constval PROCESS_BLOCKED 0x2

_m_4 db "==== PROCESS TABLE ===="
rac0oc_process_table:
    _m_6 db "!process_status_table!"
    process_status_table db[0x10][0x8] // -> process_status_table dw[16]

    _m_7 db "!process_identifier_table!"
    process_identifier_table db[0x10][0x8]

    _m_8 db "!process_iret_table!"
    process_iret_table db[0x10][0x8]

    _m_9 db "!process_child_table!"
    process_child_table db[0x10][0x40] // 0x40 -> 64 bits -> 8 words // -> process_status_table dw[16][8]
    // process_0_page_table dw 0x0

    _m_10 db "!process_stack_size_table!"
    process_stack_size_table db[0x10][0x8]

    _m_11 db "!process_stack_data_table!"
    process_stack_data_table db[0x10][0x2000] // 0x2000 -> 8192 bits -> 1024 words

_m_5 db "===== USER SPACE ======"
rac0oc_user_space:
    // ...

_m_12 db "!test_process_1!"
test_process_1:
    test_process_1_loop:
        // print 1
        pusha 0x1
        putda 0x0
        drop

        // jmpa &test_process_1_loop
        halt

_m_13 db "!test_process_1!"
test_process_2:
    test_process_2_loop:
        // print 2
        pusha 0x2
        putda 0x0
        drop
 
        //jmpa &test_process_2_loop
        halt

_m_14 db "!test_process_1!"
test_process_3:
    test_process_3_loop:
        // print 3
        pusha 0x3
        putda 0x0
        drop

        // jmpa &test_process_3_loop
        halt