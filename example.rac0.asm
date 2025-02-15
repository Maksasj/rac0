jmpga &code

@module main {
    @constval zero 0x0

    data:
        some_const dw 0x69
 
    code:
        pusha $zero
        addat 0x1
        jmpga &code
        halt 
}
