#include "rac0.h"

rac0_u8_t rac0_stack_empty(rac0_stack_t* stack) {
    return stack->top == 0;
}

void rac0_stack_push(rac0_stack_t* stack, rac0_value_t value) {
    stack->values[stack->top] = value;
    ++stack->top;
}

rac0_value_t rac0_stack_get_top(rac0_stack_t* stack) {
    if(rac0_stack_empty(stack))
        return 0;

    return stack->values[stack->top - 1];
}

rac0_value_t rac0_stack_get_next(rac0_stack_t* stack) {
    if(rac0_stack_empty(stack))
        return 0;

    if(stack->top <= 1)
        return 0;

    return stack->values[stack->top - 2];
}

void rac0_stack_drop(rac0_stack_t* stack) {
    if(rac0_stack_empty(stack))
        return;

    --stack->top;
}

void rac0_set_status_bit(rac0_cpu_t* cpu, rac0_value_t mask, rac0_value_t bool_value) {
    if(bool_value)
        cpu->status |= mask;
    else
        cpu->status &= ~mask;
}

rac0_value_t rac0_status_bit_is_set(rac0_cpu_t* cpu, rac0_value_t mask) {
    return cpu->status & mask;
}

rac0_inst_t rac0_fetch_inst(rac0_u64_t pc, rac0_memory_t* memory) {
    return *((rac0_inst_t*) &memory->memory[pc]);
}

void rac0_cpu_inst_cycle(rac0_cpu_t* cpu, rac0_memory_t* memory, rac0_device_t* devices) {
    rac0_inst_t inst = rac0_fetch_inst(cpu->pc, memory);
 
    // nikita ---------------------------------------------------------------------------
    if(inst.opcode == RAC0_HALT_OPCODE) { // cpu
        rac0_set_status_bit(cpu, RAC0_STATUS_HALTED_BIT_MASK, 1);
        goto inc;
    } else if(inst.opcode == RAC0_WAIT_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode WAIT is not implemented");
        // Todo, probably use rac0_set_status_bit function
    } else if(inst.opcode == RAC0_SETIDTT_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode SETIDTT is not implemented");
    } else if(inst.opcode == RAC0_SETIDTST_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode SETIDTST is not implemented");
    } else if(inst.opcode == RAC0_SETPTBAT_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode SETPTBAT is not implemented");
    } else if(inst.opcode == RAC0_SETPTST_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode SETPTST is not implemented");
    } else if(inst.opcode == RAC0_SETPTPST_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode SETPTPST is not implemented");
    } else if(inst.opcode == RAC0_SETTT_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode SETTT is not implemented");
    } else if(inst.opcode == RAC0_SETSTT_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode SETSTT is not implemented");
    } else if(inst.opcode == RAC0_PUSHA_OPCODE) { // stack
        rac0_stack_push(&cpu->stack, inst.value);
        goto inc;
    } else if(inst.opcode == RAC0_PUSHPC_OPCODE) {
        rac0_stack_push(&cpu->stack, cpu->pc);
        goto inc;
    } else if(inst.opcode == RAC0_PUSHSS_OPCODE) {
        rac0_stack_push(&cpu->stack, cpu->stack.top);
        goto inc;
    } else if(inst.opcode == RAC0_PUSHDC_OPCODE) {
        rac0_stack_push(&cpu->stack, cpu->device);
        goto inc;
    } else if(inst.opcode == RAC0_PUSHMS_OPCODE) {
        rac0_stack_push(&cpu->stack, RAC0_MEGABYTE_SIZE);
        goto inc;
    } else if(inst.opcode == RAC0_DUPT_OPCODE) {
        rac0_stack_push(&cpu->stack, rac0_stack_get_top(&cpu->stack));
        goto inc;
    } else if(inst.opcode == RAC0_DUPN_OPCODE) {
        rac0_stack_push(&cpu->stack, rac0_stack_get_next(&cpu->stack));
        goto inc;
    } else if(inst.opcode == RAC0_DROP_OPCODE) {
        rac0_stack_drop(&cpu->stack);
        goto inc;
    } else if(inst.opcode == RAC0_SWAP_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_drop(&cpu->stack);
        rac0_stack_drop(&cpu->stack);
        rac0_stack_push(&cpu->stack, top);
        rac0_stack_push(&cpu->stack, next);
        goto inc;
    } else if(inst.opcode == RAC0_STORE_OPCODE) { // memory
        PLUM_LOG(PLUM_ERROR, "Opcode STORE is not implemented");
    } else if(inst.opcode == RAC0_STOREA_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode STOREA is not implemented");
    } else if(inst.opcode == RAC0_LOAD_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode LOAD is not implemented");
    } else if(inst.opcode == RAC0_LOADA_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode LOADA is not implemented");
    } else if(inst.opcode == RAC0_ADD_OPCODE) { // arithmetic
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top + next);
        goto inc;
    }  else if(inst.opcode == RAC0_SUB_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top - next);
        goto inc;
    } else if(inst.opcode == RAC0_MUL_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top * next);
        goto inc;
    } else if(inst.opcode == RAC0_DIV_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top / next);
        goto inc;
    } else if(inst.opcode == RAC0_MOD_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top % next);
        goto inc;
    // maksim ---------------------------------------------------------------------------
    } else if(inst.opcode == RAC0_CMP_OPCODE) {  // logic
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        
        if(top == next) {
            rac0_stack_push(&cpu->stack, 0);
        } else if (top > next) {
            rac0_stack_push(&cpu->stack, 1);
        } else if (top < next) {
            rac0_stack_push(&cpu->stack, -1);
        }
        
        goto inc;
    } else if(inst.opcode == RAC0_NEG_OPCODE) { 
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_stack_push(&cpu->stack, -top);
        goto inc;
    } else if(inst.opcode == RAC0_NOT_OPCODE) { 
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_stack_push(&cpu->stack, ~top);
        goto inc;
    } else if(inst.opcode == RAC0_AND_OPCODE) { 
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top & next);
        goto inc;
    } else if(inst.opcode == RAC0_OR_OPCODE) { 
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top | next);
        goto inc;
    } else if(inst.opcode == RAC0_NAND_OPCODE) { 
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, ~(top & next));
        goto inc;
    } else if(inst.opcode == RAC0_NOR_OPCODE) { 
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, ~(top | next));
        goto inc;
    } else if(inst.opcode == RAC0_XOR_OPCODE) { 
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top ^ next);
        goto inc;
    } else if(inst.opcode == RAC0_JMPA_OPCODE) { // flow
        cpu->pc = inst.value;
        goto cont;
    } else if(inst.opcode == RAC0_JMPT_OPCODE) {
        cpu->pc = rac0_stack_get_top(&cpu->stack);
        goto cont;
    } else if(inst.opcode == RAC0_JZA_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        
        if(top == 0) {
            cpu->pc = inst.value;
            goto cont;
        } else
            goto inc;
    } else if(inst.opcode == RAC0_JNZA_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        
        if(top != 0) {
            cpu->pc = inst.value;
            goto cont;
        } else
            goto inc;
    } else if(inst.opcode == RAC0_JNEGA_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        
        if(top < 0) {
            cpu->pc = inst.value;
            goto cont;
        } else
            goto inc;
    } else if(inst.opcode == RAC0_JPOSA_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        
        if(top > 0) {
            cpu->pc = inst.value;
            goto cont;
        } else
            goto inc;
    } else if(inst.opcode == RAC0_SETDA_OPCODE) { // device
        cpu->device = inst.value;
        goto inc;
    } else if(inst.opcode == RAC0_SETDT_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        cpu->device = top;
        goto inc;
    } else if(inst.opcode == RAC0_FETCHDA_OPCODE) {
        rac0_device_t device = devices[cpu->device];
        rac0_value_t value = (*device.pool)(device.device_data, inst.value);
        rac0_stack_push(&cpu->stack, value);
    } else if(inst.opcode == RAC0_FETCHDT_OPCODE) {
        rac0_device_t device = devices[cpu->device];
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t value = (*device.pool)(device.device_data, top);
        rac0_stack_push(&cpu->stack, value);
    } else if(inst.opcode == RAC0_PUTDA_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_device_t device = devices[cpu->device];
        (*device.push)(device.device_data, inst.value, top);
        goto inc;
    } else if(inst.opcode == RAC0_PUTDT_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_device_t device = devices[cpu->device];
        (*device.push)(device.device_data, top, next);
    } else if(inst.opcode == RAC0_INT_OPCODE) { // interrupt
        PLUM_LOG(PLUM_ERROR,"Opcode INT is not implemented");
    } else if(inst.opcode == RAC0_IRET_OPCODE) {
        PLUM_LOG(PLUM_ERROR,"Opcode IRET is not implemented");
    } else {
        PLUM_LOG(PLUM_ERROR, "Opcode is not implemented %.4x", inst.opcode);
        return;
    }

    inc:
    cpu->pc += sizeof(rac0_inst_t);

    cont:

    // PLUM_LOG(PLUM_TRACE, "[ stack size: %llu ] [ pc: 0x%.16llx ] [ device: %llu ]",cpu->stack.top, cpu->pc, cpu->device);
    // PLUM_LOG(PLUM_TRACE, "[ 0x%.4x ] 0x%.16llx %s", inst.opcode, inst.value, RAC0_OPCODE_STRING[inst.opcode]);
}
