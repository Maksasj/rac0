jmpa &code

@constval sys_device_screen 0x1

@constval color_red 0b11111111111111110000000000000000
@constval color_blue 0xff0000ff

fill_screen: // (color, pc)
    setda $sys_device_screen

    swap // (pc, color)
    pusha 0x5a00
    
    loop10_l:

    // decrement
    pusha 0x1
    swap
    sub
    swap 
    drop
    swap
    drop

    putdt // send to screen

    jza &fill_screen_cont
    jmpa &loop10_l

    fill_screen_cont: 
    drop // drop iterator
    swap // (color, pc)

    // pc + sizeof(inst) * 2
    pusha 0x14
    add
    swap
    drop
    swap
    drop

    jmpt // return from procedure

code:
    // fill screen with color
    pusha $color_red
    pushpc
    jmpa &fill_screen
    drop
    drop

    halt