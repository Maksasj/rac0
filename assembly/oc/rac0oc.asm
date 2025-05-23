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

    // start process scheduler
    jmpa &rac0oc_schedule_routine_select_next_process

    halt

_m_0 db "===== KERNEL CODE ====="

@constval STATUS_MODE_BIT_MASK 0b0000000000000000000000000000000000000000000000000000000000000001
@constval STATUS_HALTED_BIT_MASK 0b0000000000000000000000000000000000000000000000000000000000000010
@constval STATUS_TIMER_MODE_BIT_MASK 0b0000000000000000000000000000000000000000000000000000000000000100
@constval STATUS_PAGE_MODE_BIT_MASK 0b0000000000000000000000000000000000000000000000000000000000001000

@constval STACK_SIZE 0x400

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

    rac0oc_schedule_routine_save_process:
        // backup stack
        rac0oc_schedule_routine_save_process_stack:
            // firstly we backup into a temporary variable
            storessa &tmp_process_stack_size // store stack size
            storesta &tmp_process_stack_data // store stack data
            clearst
        
            // copy stack size in table
            loada &tmp_process_stack_size
            loada &active_process_index // (iret) ( index )
            storearac &process_stack_size_table 
            drop
        
            // copy stack data in table
            loada &active_process_index // (index)
            mulac $STACK_SIZE // (index * 1024)
            addac &process_stack_data_table // (process_stack_data_table + index * 1024)
            pusha &tmp_process_stack_data
            mcpyba $STACK_SIZE // memcpy(next, top)
            drop 
            drop

        rac0oc_schedule_routine_save_process_iret:
            // get iret
            pushiret // (iret)
            dropiret
            loada &active_process_index // (iret) ( index )
            storearac &process_iret_table 
            drop

    rac0oc_schedule_routine_select_next_process:
        loada &active_process_index
        addac 0x1
        modac 0x3
        storea &active_process_index
        drop

    rac0oc_schedule_routine_start_selected_process:
        rac0oc_schedule_routine_start_selected_process_get_iret:
            loada &active_process_index
            loadarac &process_iret_table // push(process_iret_table[index]) | ( iret ) 
            pushiretc // we save iret to the iret stack
            drop

        // since iret stack already stores process iret we can prepare stack and other stuff
        rac0oc_schedule_routine_start_selected_process_restore_stack:
            loada &active_process_index // ( index )
            loadarac &process_stack_size_table 
            storea &tmp_process_stack_size
            drop
            
            pusha &tmp_process_stack_data
            loada &active_process_index // (index)
            mulac $STACK_SIZE // (index * 1024)
            addac &process_stack_data_table // (process_stack_data_table + index * 1024)
            mcpyba $STACK_SIZE // memcpy(next, top)
            drop 
            drop

            // replaced current stack with process stack
            loadssa &tmp_process_stack_size
            loadsta &tmp_process_stack_data
        
        settta 0x12C // 0x12C (16) = 300 (10)
        setstbta $STATUS_TIMER_MODE_BIT_MASK // enable timer

        iretac 0x0

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

    // _m_7 db "!process_identifier_table!"
    // process_identifier_table db[0x10][0x8]

    _m_8 db "!process_iret_table!"
    process_iret_table db[0x10][0x8]

    // _m_9 db "!process_child_table!"
    // process_child_table db[0x10][0x40] // 0x40 -> 64 bits -> 8 words // -> process_status_table dw[16][8]
    // process_0_page_table dw 0x0

    _m_10 db "!process_stack_size_table!"
    process_stack_size_table db[0x10][0x8]

    _m_11 db "!process_stack_data_table!"
    process_stack_data_table db[0x10][0x2000] // 0x2000 -> 8192 bits -> 1024 words

_m_5 db "===== USER SPACE ======"
rac0oc_user_space:
    _m_12 db "!test_process_1!"
    test_process_1:
        pusha 0x1
        pusha 0x2
        pusha 0x3
        pusha 0x4
        pusha 0x5
        pusha 0x6

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