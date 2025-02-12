# rac0

HALT	Stops program exectuion
	
PUSH	Pushes argument value on top of the stack
PUSHPC	Pushes pc value on top of the stack
DROP	Drops top value of the stack
DUPT	Duplicates top value of the stack
DUPN	Duplicates next value of the stack and pushes on top of the stack
SWAP	Swaps top and next values of the stack
	
STORE	Stores next value of the stack to the adress of top stack value
STOREA	Stores top value of the stack to the adress of argument value
STOREAC	Stores top value of the stack to the adress of argument value, consumes
STOREAA	Stores argument value to the adress of the top value of the stack
LOAD	Pushes on top of the stack value from adress of top of the stack
LOADA	Pushes on top of the stack value from adress of argument
	
ADD	    ADD operation between next and top value of the stack and pushes result on top of the stack
ADDC	ADD operation between next and top value of the stack and pushes result on top of the stack, consumes both
ADDAT	ADD operand to top stack value and
ADDAN	ADD operand to next stack value and
SUB	    SUB operation between next and top value of the stack and pushes result on top of the stack
CMP	    CMP operation between next and top value of the stack and pushes result on top of the stack
NEG	    NEG applies to top value of the stack and pushes result on top of the stack
NOT	    NOT applies to top value of the stack and pushes result on top of the stack
AND	    AND operation between next and top value of the stack and pushes result on top of the stack
OR	    OR operation between next and top value of the stack and pushes result on top of the stack
NAND	NAND operation between next and top value of the stack and pushes result on top of the stack
NOR	    NOR operation between next and top value of the stack and pushes result on top of the stack
XOR	    XOR operation between next and top value of the stack and pushes result on top of the stack
	
JMPA	Unconditional to the adress of argument value
JMP	    Unconditional to the adress of top value of the stack
JZA	    Conditional jump to the adress of argument, comparable value on top of the stack
JZ	    Conditional jump to the adress of top value of the stack, comparable value on next of the stack
JEA	    Conditional jump to the adress of argument, comparable value on top and next of the stack
JEAC	Conditional jump to the adress of argument, comparable value on top and next of the stack, consumes comparable value
JNZA	Conditional jump to the adress of argument, comparable value on top of the stack
JNZ	    Conditional jump to the adress of top value of the stack, comparable value on next of the stack
JNEA	Conditional jump to the adress of argument, comparable value on top and next of the stack
JNEGA	Conditional jump to the adress of argument, comparable value on top of the stack
JNEG	Conditional jump to the adress of top value of the stack, comparable value on next of the stack

JPOSA	Conditional jump to the adress of argument, comparable value on top of the stack
JPOS	Conditional jump to the adress of top value of the stack, comparable value on next of the stack

WAIT	Does nothing
	
DEVICE	Sets active device to the argumnet value
DEVICET	Sets active device to the top stack value