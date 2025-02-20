jmpa &code

@include "rac0.sys.interrupt.asm"

@module main {
    data:
        some_const dw 0x69
        message db "Hello world !"
        aaaa db ""

    code:
        int 0x0
        int 0x1
        int 0x1
        int 0x2
        int 0x3
        int 0x0
        halt 
}
