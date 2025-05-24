_ dw 0xFFFFFFFFFFFFFFFF

jmpa &code

code:
    pusha 0x1
    pusha 0x2
    pusha 0x3
    pusha 0x4
    pusha 0x5
    pusha 0x6

    l:
        pusha 0x0
        putda 0x0
        drop

        jmpa &l
        halt
