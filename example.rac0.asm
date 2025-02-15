jmpga &code

@module main {
    @constval zero 0x0
    @constval one 0x1
    @constval one 0xFFFFAAAAFFFFAAAA

    @constval sys_device_zero 0x0

    data:
        some_const dw 0x69
 
    code:
        pusha $zero

    lo:        
        addat $one
        pushda $sys_device_zero
        jmpga &lo
        halt 
}
