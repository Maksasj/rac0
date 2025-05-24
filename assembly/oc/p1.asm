_ dw 0xFFFFFFFFFFFFFFFF

jmpa &code

code:

    l:
        pusha 0x1
        putda 0x0
        drop

        jmpa &l
        halt
