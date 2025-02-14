@module main {
    @include "rac0.sys.asm"
    
    @constval THIS_IS_A_COMPILE_TIME_CONST_VALUE 0xFFFF

    data:
        pusha 0x1
        some db "some some some"
        somesome dw 0xFFFF
        halt 
        some db "some some some"
        somesome dw 0xFFFF
    
    code:
        lop:
            pusha 0x1
            addat 0x1
            jmpga &lop
            halt 
}
