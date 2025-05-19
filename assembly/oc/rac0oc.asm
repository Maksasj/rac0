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

    jmpa &test_process_2

    halt

_m_0 db "===== KERNEL CODE ====="
rac0oc_int_timer_handler:
    halt
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

_m_3 db "==== PROCESS TABLE ===="
rac0oc_process_table:
    process_0_status dw 0x0
    process_0_identifier dw 0x0
    process_0_iret dw 0x0
    process_0_child db[0x40] // 0x40 -> 64 bits -> 8 words
    process_0_page_table dw 0x0
    process_0_stack_size dw 0x0
    process_0_stack_data db[0x2000] // 0x2000 -> 8192 bits -> 1024 words

    process_1_status dw 0x0
    process_1_identifier dw 0x0
    process_1_iret dw &test_process_1
    process_1_child db[0x40] // 0x40 -> 64 bits -> 8 words
    process_1_page_table dw 0x0
    process_1_stack_size dw 0x0
    process_1_stack_data db[0x2000] // 0x2000 -> 8192 bits -> 1024 words

    process_2_status dw 0x0
    process_2_identifier dw 0x0
    process_2_iret dw &test_process_2
    process_2_child db[0x40] // 0x40 -> 64 bits -> 8 words
    process_2_page_table dw 0x0
    process_2_stack_size dw 0x0
    process_2_stack_data db[0x2000] // 0x2000 -> 8192 bits -> 1024 words

_m_4 db "===== USER SPACE ======"
rac0oc_user_space:
    // ...

test_process_1:
    test_process_1_loop:
        // pusha 0xA
        // putda 0x0
        // drop
        // jmpa &test_process_1_loop

test_process_2:
    test_process_2_loop:
        // pusha 0x0
        // putda 0x0
        // drop
        // jmpa &test_process_2_loop