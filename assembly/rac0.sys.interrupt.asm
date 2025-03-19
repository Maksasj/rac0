interrupt_descriptor_table:
    _ dw &int_0
    _ dw &int_1
    _ dw &int_2
    _ dw &int_3

@constval sys_device_0 0x0

int_0:
    iret

int_1:
    iret

int_2:
    addat 0x1
    putda $sys_device_0
    drop
    iret
    
int_3:
    iret
    