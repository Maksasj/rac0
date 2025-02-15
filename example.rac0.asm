@include "rac0.sys.asm"

@module main {
    @constval zero 0x0
    @constval one 0x1
    @constval other 0xFFFFAAAAFFFFAAAA

    @constblock push2() {
        pusha $one
        pusha $one
    }

    $push2()

    data:
        some_const dw 0x69
 
    code:
        pusha $zero

    lo:        
        addat $one
        pushda $sys_device_0
        jmpga &lo
        halt 
}
