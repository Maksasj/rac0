// comment
halt // instruction wihtout argument
pusha 0x0 // instruction with argument
message db "Hello world !" // byte definition
number dw 0x101 // word definiion
l: // label definition

@constv one 0x1 // constval definition
@constblock push0() { } // constblock definition
@module main { } // module definition.
$push0() // constblock usage
@include "pog.asm" // include
