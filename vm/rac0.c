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

void rac0_cpu_throw_interrupt(rac0_cpu_t* cpu, rac0_memory_t* memory, rac0_value_t code) {
    rac0_value_t* idt = (rac0_value_t*) (memory->memory + cpu->idt);
    rac0_value_t entry = idt[code];
    rac0_stack_push(&cpu->iret, cpu->pc);
    cpu->pc = entry;
}

void rac0_cpu_throw_interrupt_step(rac0_cpu_t* cpu, rac0_memory_t* memory, rac0_value_t code) {
    rac0_value_t* idt = (rac0_value_t*) (memory->memory + cpu->idt);
    rac0_value_t entry = idt[code];
    rac0_stack_push(&cpu->iret, cpu->pc + 1 * sizeof(rac0_inst_t));
    cpu->pc = entry;
}

rac0_inst_t rac0_fetch_inst(rac0_u64_t pc, rac0_memory_t* memory) {
    return *((rac0_inst_t*) &memory->memory[pc]);
}

rac0_value_t rac0_valid_memory_access(rac0_memory_t* memory, rac0_value_t virtual_address, rac0_value_t paging) {
    rac0_value_t ptba = memory->ptba;
    rac0_value_t pts = memory->pts;
    rac0_value_t ptps = memory->ptps;

    if(paging) {
        rac0_value_t virtual_page_number = virtual_address / ptps;
        return virtual_page_number < pts;  
    }

    return virtual_address < memory->memory_size;
}

rac0_value_t rac0_get_physical_address(rac0_memory_t* memory, rac0_value_t virtual_address, rac0_value_t paging) {
    rac0_value_t ptba = memory->ptba;
    rac0_value_t pts = memory->pts;
    rac0_value_t ptps = memory->ptps;

    if(paging) {
        rac0_value_t virtual_page_number = virtual_address / ptps;  
        rac0_value_t offset = virtual_address % ptps;
        rac0_value_t physical_frame = ((rac0_value_t*) (memory->memory + ptba))[virtual_page_number];
        rac0_value_t physical_final_address = physical_frame * ptps + offset;
        return physical_final_address;  
    }

    return virtual_address;
}

void rac0_dump_memory_fmt(rac0_memory_t* memory, const char* file_name) {
    FILE *file = fopen(file_name, "wb");

    rac0_value_t lines = memory->memory_size / 8;

    for(rac0_value_t l = 0; l < lines; ++l) {
        fprintf(file, "[ 0x%.16llx ] [", l * 8);

        for(rac0_value_t i = 0; i < 8; ++i) {
            rac0_byte_t byte = memory->memory[l * 8 + i];
            fprintf(file, "0x%.2x ", byte);
        }

        fprintf(file, " ] [ ");

        for(rac0_value_t i = 0; i < 8; ++i) {
            rac0_byte_t byte = memory->memory[l * 8 + i];
      
            if(isprint(byte)) {
                fprintf(file, "%c", byte);
            } else {
                fprintf(file, " ");
            }
        }

        fprintf(file, " ] [ ");

        for(rac0_value_t i = 0; i < 8; ++i) {
            rac0_byte_t byte = memory->memory[l * 8 + i];
            fprintf(file, "'%.3d' ", byte);
        }

        fprintf(file, "] \n");
    }

    fclose(file);
}

void rac0_cpu_inst_cycle(rac0_cpu_t* cpu, rac0_memory_t* memory, rac0_device_selector_t* device_selector) {
    rac0_inst_t inst = rac0_fetch_inst(cpu->pc, memory);
    rac0_opcode_t opcode = inst.opcode;

    rac0_value_t mode_flag = rac0_status_bit_is_set(cpu, RAC0_STATUS_MODE_BIT_MASK); 
    rac0_value_t halted_flag = rac0_status_bit_is_set(cpu, RAC0_STATUS_HALTED_BIT_MASK); 
    rac0_value_t timer_flag = rac0_status_bit_is_set(cpu, RAC0_STATUS_TIMER_MODE_BIT_MASK); 
    rac0_value_t page_flag = rac0_status_bit_is_set(cpu, RAC0_STATUS_PAGE_MODE_BIT_MASK); 
    
    if(!rac0_opcode_valid(opcode)) {
        PLUM_LOG(PLUM_ERROR, "INSTRUCTION IS NOT VALID rac0_opcode_valid() %d", opcode);
        rac0_cpu_throw_interrupt(cpu, memory, RAC0_INTERRUPT_INVINST);
        rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
        goto cont;
    }

    if(mode_flag == RAC0_USER_MODE && rac0_opcode_privileged(opcode)) {
        rac0_cpu_throw_interrupt(cpu, memory, RAC0_INTERRUPT_PRIV);
        rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
        goto cont;
    }

    if(timer_flag) {
        if(cpu->timer == 0) {
            rac0_cpu_throw_interrupt(cpu, memory, RAC0_INTERRUPT_TIMER);
            rac0_set_status_bit(cpu, RAC0_STATUS_TIMER_MODE_BIT_MASK, 0);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }
        
        --cpu->timer;
    }

    if(opcode == RAC0_HALT_OPCODE) { // cpu
        rac0_set_status_bit(cpu, RAC0_STATUS_HALTED_BIT_MASK, 1);
        goto inc;
    } else if(opcode == RAC0_WAIT_OPCODE) {
        rac0_set_status_bit(cpu, RAC0_STATUS_TIMER_MODE_BIT_MASK, 1);
        goto inc;
    } else if(opcode == RAC0_SETIDTT_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        cpu->idt = top;
        goto inc;
    } else if(opcode == RAC0_SETIDTST_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        cpu->idts = top;
        goto inc;
    } else if(opcode == RAC0_SETPTBAT_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        memory->ptba = top;
        goto inc;
    } else if(opcode == RAC0_SETPTST_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        memory->pts = top;
        goto inc;
    } else if(opcode == RAC0_SETPTPST_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        memory->ptps = top;
        goto inc;
    } else if(opcode == RAC0_SETTT_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        cpu->timer = top;
        goto inc;
    } else if(opcode == RAC0_SETIRETT_OPCODE) {
        rac0_stack_drop(&cpu->iret);
        rac0_stack_push(&cpu->iret, rac0_stack_get_top(&cpu->stack));
        goto inc;
    } else if(opcode == RAC0_SETSTT_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        cpu->status = top;
        goto inc;
    } else if(opcode == RAC0_PUSHA_OPCODE) { // stack
        rac0_stack_push(&cpu->stack, inst.value);
        goto inc;
    } else if(opcode == RAC0_PUSHPC_OPCODE) {
        rac0_stack_push(&cpu->stack, cpu->pc);
        goto inc;
    } else if(opcode == RAC0_PUSHSS_OPCODE) {
        rac0_stack_push(&cpu->stack, cpu->stack.top);
        goto inc;
    } else if(opcode == RAC0_PUSHDC_OPCODE) {
        rac0_stack_push(&cpu->stack, device_selector->device);
        goto inc;
    } else if(opcode == RAC0_PUSHMS_OPCODE) {
        rac0_stack_push(&cpu->stack, memory->memory_size);
        goto inc;
    } else if(opcode == RAC0_PUSHIRET_OPCODE) {
        rac0_stack_push(&cpu->stack, rac0_stack_get_top(&cpu->iret));
        goto inc;
    } else if(opcode == RAC0_DUPT_OPCODE) {
        rac0_stack_push(&cpu->stack, rac0_stack_get_top(&cpu->stack));
        goto inc;
    } else if(opcode == RAC0_DUPN_OPCODE) {
        rac0_stack_push(&cpu->stack, rac0_stack_get_next(&cpu->stack));
        goto inc;
    } else if(opcode == RAC0_DROP_OPCODE) {
        rac0_stack_drop(&cpu->stack);
        goto inc;
    } else if(opcode == RAC0_SWAP_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_drop(&cpu->stack);
        rac0_stack_drop(&cpu->stack);
        rac0_stack_push(&cpu->stack, top);
        rac0_stack_push(&cpu->stack, next);
        goto inc;
    } else if(opcode == RAC0_STORE_OPCODE) { // memory
        rac0_value_t value = rac0_stack_get_next(&cpu->stack);
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t virtual_address = top;

        if(!rac0_valid_memory_access(memory, virtual_address, page_flag)) {
            rac0_cpu_throw_interrupt_step(cpu, memory, RAC0_INTERRUPT_INVPACC);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }

        rac0_value_t physical_address = rac0_get_physical_address(memory, virtual_address, page_flag);
        *((rac0_value_t*) (memory->memory + physical_address)) = value;
        goto inc;
    } else if(opcode == RAC0_STOREARAC_OPCODE) {
        rac0_value_t value = rac0_stack_get_next(&cpu->stack);
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t virtual_address = inst.value;

        if(!rac0_valid_memory_access(memory, virtual_address, page_flag)) {
            rac0_cpu_throw_interrupt_step(cpu, memory, RAC0_INTERRUPT_INVPACC);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }

        rac0_stack_drop(&cpu->stack);

        rac0_value_t physical_address = rac0_get_physical_address(memory, virtual_address, page_flag);
        ((rac0_value_t*) (memory->memory + physical_address))[top] = value;
        goto inc;
    } else if(opcode == RAC0_STOREA_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t virtual_address = inst.value;

        if(!rac0_valid_memory_access(memory, virtual_address, page_flag)) {
            rac0_cpu_throw_interrupt_step(cpu, memory, RAC0_INTERRUPT_INVPACC);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }

        rac0_value_t physical_address = rac0_get_physical_address(memory, virtual_address, page_flag);
        *((rac0_value_t*) (memory->memory + physical_address)) = top;
        goto inc;
    } else if(opcode == RAC0_LOAD_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t virtual_address = top;

        if(!rac0_valid_memory_access(memory, virtual_address, page_flag)) {
            rac0_cpu_throw_interrupt_step(cpu, memory, RAC0_INTERRUPT_INVPACC);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }

        rac0_value_t physical_address = rac0_get_physical_address(memory, virtual_address, page_flag);
        rac0_value_t value = *((rac0_value_t*) (memory->memory + physical_address));
        rac0_stack_push(&cpu->stack, value);
        goto inc;
    } else if(opcode == RAC0_LOADARAC_OPCODE) {
        rac0_value_t index = rac0_stack_get_top(&cpu->stack);
        rac0_value_t virtual_address = inst.value ;
        
        if(!rac0_valid_memory_access(memory, virtual_address, page_flag)) {
            rac0_cpu_throw_interrupt_step(cpu, memory, RAC0_INTERRUPT_INVPACC);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }
        rac0_stack_drop(&cpu->stack);

        rac0_value_t physical_address = rac0_get_physical_address(memory, virtual_address, page_flag);
        rac0_value_t value = ((rac0_value_t*) (memory->memory + physical_address))[index];
        rac0_stack_push(&cpu->stack, value);
        goto inc;
    } else if(opcode == RAC0_LOADA_OPCODE) {
        rac0_value_t virtual_address = inst.value;

        if(!rac0_valid_memory_access(memory, virtual_address, page_flag)) {
            rac0_cpu_throw_interrupt_step(cpu, memory, RAC0_INTERRUPT_INVPACC);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }
        
        rac0_value_t physical_address = rac0_get_physical_address(memory, virtual_address, page_flag);
        rac0_value_t value = *((rac0_value_t*) (memory->memory + physical_address));
        rac0_stack_push(&cpu->stack, value);
        goto inc;
    } else if(opcode == RAC0_ADD_OPCODE) { // arithmetic
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top + next);
        goto inc;
    } else if(opcode == RAC0_SUB_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top - next);
        goto inc;
    } else if(opcode == RAC0_MUL_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top * next);
        goto inc;
    } else if(opcode == RAC0_DIV_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top / next);
        goto inc;
    } else if(opcode == RAC0_MOD_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top % next);
        goto inc;
    } else if(opcode == RAC0_CMP_OPCODE) {  // logic
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        
        if(top == next)
            rac0_stack_push(&cpu->stack, 0);
        else if (top > next)
            rac0_stack_push(&cpu->stack, 1);
        else if (top < next)
            rac0_stack_push(&cpu->stack, -1);
        
        goto inc;
    } else if(opcode == RAC0_NEG_OPCODE) { 
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_stack_push(&cpu->stack, -top);
        goto inc;
    } else if(opcode == RAC0_NOT_OPCODE) { 
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_stack_push(&cpu->stack, ~top);
        goto inc;
    } else if(opcode == RAC0_AND_OPCODE) { 
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top & next);
        goto inc;
    } else if(opcode == RAC0_OR_OPCODE) { 
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top | next);
        goto inc;
    } else if(opcode == RAC0_NAND_OPCODE) { 
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, ~(top & next));
        goto inc;
    } else if(opcode == RAC0_NOR_OPCODE) { 
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, ~(top | next));
        goto inc;
    } else if(opcode == RAC0_XOR_OPCODE) { 
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top ^ next);
        goto inc;
    } else if(opcode == RAC0_JMPA_OPCODE) { // flow
        cpu->pc = inst.value;
        goto cont;
    } else if(opcode == RAC0_JMPT_OPCODE) {
        cpu->pc = rac0_stack_get_top(&cpu->stack);
        goto cont;
    } else if(opcode == RAC0_JMPTC_OPCODE) {
        cpu->pc = rac0_stack_get_top(&cpu->stack);
        rac0_stack_drop(&cpu->stack);
        goto cont;
    } else if(opcode == RAC0_JZA_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        
        if(top == 0) {
            cpu->pc = inst.value;
            goto cont;
        } else
            goto inc;
    } else if(opcode == RAC0_JNZA_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        
        if(top != 0) {
            cpu->pc = inst.value;
            goto cont;
        } else
            goto inc;
    } else if(opcode == RAC0_JNEGA_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        
        if(top < 0) {
            cpu->pc = inst.value;
            goto cont;
        } else
            goto inc;
    } else if(opcode == RAC0_JPOSA_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        
        if(top > 0) {
            cpu->pc = inst.value;
            goto cont;
        } else
            goto inc;
    } else if(opcode == RAC0_SETDA_OPCODE) { // device
        rac0_value_t value = inst.value;

        if(value >= device_selector->devc) {
            rac0_cpu_throw_interrupt_step(cpu, memory, RAC0_INTERRUPT_INVDEV);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }

        device_selector->device = value;
        goto inc;
    } else if(opcode == RAC0_SETDT_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);

        if(top >= device_selector->devc) {
            rac0_cpu_throw_interrupt_step(cpu, memory, RAC0_INTERRUPT_INVDEV);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }
        
        device_selector->device = top;
        goto inc;
    } else if(opcode == RAC0_FETCHDA_OPCODE) {
        rac0_device_t device = *device_selector->devices[device_selector->device];
        rac0_value_t value = (*device.pool)(device.device_data, inst.value);
        rac0_stack_push(&cpu->stack, value);
    } else if(opcode == RAC0_FETCHDT_OPCODE) {
        rac0_device_t device = *device_selector->devices[device_selector->device];
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t value = (*device.pool)(device.device_data, top);
        rac0_stack_push(&cpu->stack, value);
    } else if(opcode == RAC0_PUTDA_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_device_t device = *device_selector->devices[device_selector->device];
        (*device.push)(device.device_data, inst.value, top);
        goto inc;
    } else if(opcode == RAC0_PUTDT_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_device_t device = *device_selector->devices[device_selector->device];
        (*device.push)(device.device_data, top, next);
    } else if(opcode == RAC0_INT_OPCODE) { // interrupt
        rac0_value_t code = inst.value;

        if(code > cpu->idts) {
            rac0_cpu_throw_interrupt_step(cpu, memory, RAC0_INTERRUPT_INVINT);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }

        rac0_cpu_throw_interrupt_step(cpu, memory, inst.value);
        rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);

        goto cont;
    } else if(opcode == RAC0_IRETAC_OPCODE) {
        rac0_value_t arg = inst.value;
        rac0_value_t iret = rac0_stack_get_top(&cpu->iret);
        rac0_stack_drop(&cpu->iret);

        if(arg) {
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 1);
        } else {
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
        }

        cpu->pc = iret;
        goto cont;
    } else if(opcode == RAC0_SETSTBTA_OPCODE) {
        rac0_value_t arg = inst.value;
        rac0_set_status_bit(cpu, arg, 1);
    } else if(opcode == RAC0_SETSTBFA_OPCODE) {
        rac0_value_t arg = inst.value;
        rac0_set_status_bit(cpu, arg, 0);
    } else if(opcode == RAC0_LOADSTA_OPCODE) {
        rac0_value_t src_virtual_address = inst.value;

        if(!rac0_valid_memory_access(memory, src_virtual_address, page_flag)) {
            rac0_cpu_throw_interrupt_step(cpu, memory, RAC0_INTERRUPT_INVPACC);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }

        rac0_value_t src_physical_address = rac0_get_physical_address(memory, src_virtual_address, page_flag);

        memcpy(
            (void*) (cpu->stack.values), 
            (void*) (memory->memory + src_physical_address), 
            RAC0_MAX_STACK_SIZE  * sizeof(rac0_value_t)
        );

        goto inc;
    } else if(opcode == RAC0_STORESTA_OPCODE) {
        rac0_value_t dst_virtual_address = inst.value;

        if(!rac0_valid_memory_access(memory, dst_virtual_address, page_flag)) {
            rac0_cpu_throw_interrupt_step(cpu, memory, RAC0_INTERRUPT_INVPACC);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }

        rac0_value_t dst_physical_address = rac0_get_physical_address(memory, dst_virtual_address, page_flag);

        memcpy(
            (void*) (memory->memory + dst_physical_address), 
            (void*) (cpu->stack.values), 
            RAC0_MAX_STACK_SIZE * sizeof(rac0_value_t)
        );

        goto inc;
    } else if(opcode == RAC0_IRETCC_OPCODE) {
        rac0_value_t iret = rac0_stack_get_top(&cpu->iret);
        rac0_stack_drop(&cpu->iret);

        rac0_value_t next = rac0_stack_get_next(&cpu->stack);     
        rac0_stack_drop(&cpu->stack);

        if(next) {
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 1);
        } else {
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
        }

        cpu->pc = iret;
        goto cont;
    } else if(opcode == RAC0_LOOP_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);

        if(top == 0) {
            goto inc;
        } else {
            rac0_stack_drop(&cpu->stack);
            rac0_stack_push(&cpu->stack, top - 1);
            cpu->pc = inst.value;
            goto cont;
        }

    } else if(opcode == RAC0_MCPYBA_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_value_t arg = inst.value;

        rac0_value_t dst_virtual_address = next;
        rac0_value_t src_virtual_address = top;

        if(!rac0_valid_memory_access(memory, dst_virtual_address, page_flag)) {
            rac0_cpu_throw_interrupt_step(cpu, memory, RAC0_INTERRUPT_INVPACC);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }

        if(!rac0_valid_memory_access(memory, src_virtual_address, page_flag)) {
            rac0_cpu_throw_interrupt_step(cpu, memory, RAC0_INTERRUPT_INVPACC);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }
        
        rac0_value_t dst_physical_address = rac0_get_physical_address(memory, dst_virtual_address, page_flag);
        rac0_value_t src_physical_address = rac0_get_physical_address(memory, src_virtual_address, page_flag);

        memcpy(
            (void*) (memory->memory + dst_physical_address), 
            (void*) (memory->memory + src_physical_address), 
            arg
        );

        goto inc;
    } else if(opcode == RAC0_SETSTSSC_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_stack_drop(&cpu->stack);
        cpu->stack.top = top;

        goto inc;
    } else if(opcode == RAC0_STORESSA_OPCODE) {
        rac0_value_t virtual_address = inst.value;

        if(!rac0_valid_memory_access(memory, virtual_address, page_flag)) {
            rac0_cpu_throw_interrupt_step(cpu, memory, RAC0_INTERRUPT_INVPACC);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }

        rac0_value_t physical_address = rac0_get_physical_address(memory, virtual_address, page_flag);
        *((rac0_value_t*) (memory->memory + physical_address)) = cpu->stack.top;
        goto inc;
    } else if(opcode == RAC0_LOADSSA_OPCODE) {
        rac0_value_t virtual_address = inst.value;

        if(!rac0_valid_memory_access(memory, virtual_address, page_flag)) {
            rac0_cpu_throw_interrupt_step(cpu, memory, RAC0_INTERRUPT_INVPACC);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }

        rac0_value_t physical_address = rac0_get_physical_address(memory, virtual_address, page_flag);
        cpu->stack.top = *((rac0_value_t*) (memory->memory + physical_address));
        goto inc;
    } else if(opcode == RAC0_CLEARST_OPCODE) {
        cpu->stack.top = 0;
        memset((rac0_byte_t*) cpu->stack.values, 0, RAC0_MAX_STACK_SIZE);
    } else if(opcode == RAC0_SETTTA_OPCODE) {
        rac0_value_t value = inst.value;
        cpu->timer = value;
        goto inc;
    } else if(opcode == RAC0_DROPIRET_OPCODE) {
        rac0_stack_drop(&cpu->iret);
        goto inc;
    } else if(opcode == RAC0_PUSHIRETC_OPCODE) {
        rac0_value_t value = rac0_stack_get_top(&cpu->stack);
        rac0_stack_drop(&cpu->stack);
        rac0_stack_push(&cpu->iret, value);
        goto inc;
    } else if(opcode == RAC0_MODAC_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_stack_drop(&cpu->stack);
        rac0_value_t value = inst.value;
        rac0_stack_push(&cpu->stack, top % value);
        goto inc;
    } else if(opcode == RAC0_ADDAC_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_stack_drop(&cpu->stack);
        rac0_value_t value = inst.value;
        rac0_stack_push(&cpu->stack, top + value);
        goto inc;
    } else if(opcode == RAC0_MULAC_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_stack_drop(&cpu->stack);
        rac0_value_t value = inst.value;
        rac0_stack_push(&cpu->stack, top * value);
        goto inc;
    } else if(opcode == RAC0_JEQAC_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);

        rac0_stack_drop(&cpu->stack);
        rac0_stack_drop(&cpu->stack);

        if(top == next) {
            cpu->pc = inst.value;
            goto cont;
        } else if (top > next)
            goto inc;
        else if (top < next)
            goto inc;
    } else if(opcode == RAC0_STOREB_OPCODE) {
        rac0_byte_t value = (rac0_byte_t) rac0_stack_get_next(&cpu->stack);
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t virtual_address = top;

        if(!rac0_valid_memory_access(memory, virtual_address, page_flag)) {
            rac0_cpu_throw_interrupt_step(cpu, memory, RAC0_INTERRUPT_INVPACC);
            rac0_set_status_bit(cpu, RAC0_STATUS_MODE_BIT_MASK, 0);
            goto cont;
        }

        rac0_value_t physical_address = rac0_get_physical_address(memory, virtual_address, page_flag);
        *((rac0_value_t*) (memory->memory + physical_address)) = value;
        goto inc;
    } else if(opcode == RAC0_SUBAC_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_stack_drop(&cpu->stack);
        rac0_value_t value = inst.value;
        rac0_stack_push(&cpu->stack, top - value);
        goto inc;
    } else {
        PLUM_LOG(PLUM_ERROR, "[ 0x%.16llx ] Opcode is not implemented %.4x", cpu->pc, opcode);
        rac0_set_status_bit(cpu, RAC0_STATUS_HALTED_BIT_MASK, 1);
        return;
    }


    inc:
    cpu->pc += sizeof(rac0_inst_t);

    cont:

    // PLUM_LOG(PLUM_TRACE, "%llu. STACK  [ stack size: %llu ] [ top: 0x%.16llx ] [next: 0x%.16llx ]", cpu->cycle, cpu->stack.top, rac0_stack_get_top(&cpu->stack), rac0_stack_get_next(&cpu->stack));
    // PLUM_LOG(PLUM_TRACE, "%llu. IRET   [ stack size: %llu ] [ top: 0x%.16llx ] [next: 0x%.16llx ]", cpu->cycle, cpu->iret.top, rac0_stack_get_top(&cpu->iret), rac0_stack_get_next(&cpu->iret));
    // PLUM_LOG(PLUM_TRACE, "%llu. INST   [ 0x%.4x ] 0x%.16llx %s", cpu->cycle, opcode, inst.value, RAC0_OPCODE_STRING[opcode]);
    // PLUM_LOG(PLUM_TRACE, "%llu. CPU    [ pc: 0x%.16llx ] [ idt: %llu ] [ idts: %llu ] [ status: %llu ] [ timer: %d ]", cpu->cycle, cpu->pc, cpu->idt, cpu->idts, cpu->status, cpu->timer);
    // PLUM_LOG(PLUM_TRACE, "%llu. MEMORY [ ptba: 0x%.16llx ] [ pts: %llu ] [ ptps: %llu ]", cpu->cycle, memory->ptba, memory->pts, memory->ptps);
    // PLUM_LOG(PLUM_TRACE, "%llu. DEVICE [ device: %llu ] [ devc: %llu ]", cpu->cycle, device_selector->device, device_selector->devc);
    // printf("\n");

    ++cpu->cycle;
}

void rac0_vm_cycle(rac0_vm_t* vm) {
    rac0_cpu_t* cpu = vm->cpu; 
    rac0_memory_t* memory = vm->memory;
    rac0_device_selector_t* device_selector = vm->device_selector;

    rac0_cpu_inst_cycle(cpu, memory, device_selector);
}   

rac0_value_t rac0_vm_halted(rac0_vm_t* vm) {
    rac0_cpu_t* cpu = vm->cpu; 
    
    return rac0_status_bit_is_set(cpu, RAC0_STATUS_HALTED_BIT_MASK);
}