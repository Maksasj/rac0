@module main {
    data:
        some_const dw 0x69
 
    code:
        pusha 0x0
        addat 0x1
        jmpga &code
        halt 
}
