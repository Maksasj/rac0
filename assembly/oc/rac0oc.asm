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

@constval STATUS_MODE_BIT_MASK 0b0000000000000000000000000000000000000000000000000000000000000001
@constval STATUS_HALTED_BIT_MASK 0b0000000000000000000000000000000000000000000000000000000000000010
@constval STATUS_TIMER_MODE_BIT_MASK 0b0000000000000000000000000000000000000000000000000000000000000100
@constval STATUS_PAGE_MODE_BIT_MASK 0b0000000000000000000000000000000000000000000000000000000000001000

@constval PROCESS_DEAD 0x0 
@constval PROCESS_ALIVE 0x1
@constval PROCESS_BLOCKED 0x2

_m_3 db "==== PROCESS SCHEDULING ===="
    active_process_index dw 0x0

    tmp_process_stack_size dw 0x0
    tmp_process_stack_data db[0x2000]

rac0oc_schedule_routine:
    // Firstly we reset timer and disable it for now
    setstbfa $STATUS_TIMER_MODE_BIT_MASK
    settta 0x12C // 0x12C (16) = 300 (10)

    // backup stack
    // firstly we backup into a temporary variable
    storessa &tmp_process_stack_size // store stack size
    storesta &tmp_process_stack_data // store stack data
    clearst

    // lets load process stack
    // loadssa &tmp_process_stack_size 
    // loadsta &tmp_process_stack_data
    // halt

    // restore stack
    // jump to active process
    loada &active_process_index // ( index )
    loadarac &process_iret_table // push(process_iret_table[index]) | ( iret ) 
    setirett

    setstbta $STATUS_TIMER_MODE_BIT_MASK // enable timer
    jmptc
    // halt

// Interrupt handlers
rac0oc_int_timer_handler:
    // timer interrupt delegates timer handling to schedule routine 
    // (iret)
    halt
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
    rac0oc_int_timer dw &rac0oc_int_timer_handler
    rac0oc_int_invinst dw &rac0oc_int_invinst_handler
    rac0oc_int_priv dw &rac0oc_int_priv_handler
    rac0oc_int_invint dw &rac0oc_int_invint_handler
    rac0oc_int_invpacc dw &rac0oc_int_invpacc_handler
    rac0oc_int_invdev dw &rac0oc_int_invdev_handler
    rac0oc_int_devamch dw &rac0oc_int_devamch_handler

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

        jmpa &test_process_1_loop
        halt

_m_13 db "!test_process_1!"
test_process_2:
    test_process_2_loop:
        // print 2
        pusha 0x2
        putda 0x0
        drop
 
        jmpa &test_process_2_loop
        halt

_m_14 db "!test_process_1!"
test_process_3:
    test_process_3_loop:
        // print 3
        pusha 0x3
        putda 0x0
        drop

        jmpa &test_process_3_loop
        halt