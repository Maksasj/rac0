// constval - basically works as constant value macro
@constval zero 0x0
pusha $zero // pushes value of $zero meaning 0x0

@constval one 0x1 // same there 
pusha $one

// this is const block - basically const statement list
@constblock push0() {
    pusha 0x69
}

$push0() // expands to a single pusha 0x69 statement

// const block can accept const arguments, such as constvalue, 
@constblock push2x(@constval value) {
    pusha $value
    pusha $value
}

$push2(0x0)
$push2($zero)

// multiple arguments
@constblock sum(@constval a, @constval b) {
    pusha $a
    pusha $b
    add
    swap 
    drop
    swap
    drop
}

$sum(0x0, 0x0)

// other constvalues
@constval funny 0x69
$push2($funny)

some_label:
$push2(&some_label)

message db "Hello world !"
$push2(&message)

@constblock do2x(@constblock block) {
    $block()
    $block()
}

$do2x($push0())


@constblock aaa(@constblock a, @constblock b) {
    $a()
    $b()
}

$do2x($push0(), $push2($funny))