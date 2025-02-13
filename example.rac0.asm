@module main {
    @include "rac0.sys.asm"

    @consteval THIS_IS_A_COMPILE_TIME_CONST_VALUE 0xFFFF

    $data:
        pusha 0x1
        some db "some some some"
        somesome dw 0xFFFF
        halt 
        some db "some some some"
        somesome dw 0xFFFF
    
    $code:
        pusha 0x1
        pusha $pog
        halt 
        somesome dw 0xFFFF
}
