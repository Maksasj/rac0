// cpu
halt
wait
setidtt
setidtst
setptbat
setptst
setptpst
settt
setstt

// stack
pusha 0x69
pushpc
pushss
pushdc
pushms
dupt
dupn
drop
swap

// memory
store
storea 0x228
load
loada 0x228

// arithmetic
add
sub
mul
div
mod

// logic
cmp
neg
not
and
or
nand
nor
xor

// flow
jmpa 0x0
jmpt
jza
jnza
jnega
jposa

// device
setda 0x0
setdt
fetchda 0x0
fetchdt
putda 0x0
putdt

// interrupt
int 0x69
iret 0x1
