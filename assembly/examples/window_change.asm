jmpa &code

@constval keycode_0 0x30

@constval sys_device_debug 0x0
@constval sys_device_screen 0x1
@constval sys_device_keyboard 0x2

@constval color_red 0xffff0000
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

keycode_is_pressed: // (keycode, pc)
    setda $sys_device_keyboard

    swap // (pc, keycode)
    fetchdt // (pc, keycode, value)
    swap // (pc, value, keycode)
    drop // (pc, value)
    swap

    pusha 0x14
    add
    swap
    drop
    swap
    drop

    jmpt // return from procedure

code:
    pusha $keycode_0
    pushpc
    jmpa &keycode_is_pressed
    drop

    jza &fill_with_red
    jmpa &fill_with_blue

    fill_with_blue:
        pusha $color_blue
        jmpa &continue_loop

    fill_with_red:
        pusha $color_red
        jmpa &continue_loop


    continue_loop:
    pushpc
    jmpa &fill_screen
    drop
    drop
    drop
    drop

    jmpa &code
    halt
