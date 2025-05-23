#include "rac0_utils.h"

int rac0_utils_read_file_size(char* file_name) {
    FILE *stream = fopen(file_name, "r");

    if(stream == NULL) {
        PLUM_LOG(PLUM_ERROR, "Failed to open file '%s'", file_name);
        return -1;
    }

    if (fseek(stream, 0L, SEEK_END) == 0) {
        long buf_size = ftell(stream);
        return buf_size;
    }

    fclose(stream);

    PLUM_LOG(PLUM_ERROR, "Failed to get file size: '%s'", file_name);

    return -1;
}

char* rac0_utils_read_file_bytes(char* file_name) {
    FILE *stream = fopen(file_name, "rb");

    if(stream == NULL) {
        PLUM_LOG(PLUM_ERROR, "Failed to open file '%s'", file_name);
        return NULL;
    }

    char *source = NULL;

    /* Go to the end of the file. */
    if (fseek(stream, 0L, SEEK_END) == 0) {
        long buf_size = ftell(stream);

        if (buf_size == -1) { 
            PLUM_LOG(PLUM_ERROR, "Failed ftell for '%s'", file_name);
            return NULL;
        }

        source = (char*) malloc(sizeof(char) * buf_size);

        if (fseek(stream, 0L, SEEK_SET) != 0)  { 
            PLUM_LOG(PLUM_ERROR, "Failed fseek for '%s'", file_name);
            return NULL;
        }

        size_t new_len = fread(source, sizeof(char), buf_size, stream);

        if (ferror( stream ) != 0 ) {
            fputs("Error reading file", stderr);
            PLUM_LOG(PLUM_ERROR, "Error reading file '%s'", file_name);
            return NULL;
        }
    }

    fclose(stream);

    return source;
}


char* rac0_utils_read_file_string(char* file_name) {
    FILE *stream = fopen(file_name, "r");

    if(stream == NULL) {
        PLUM_LOG(PLUM_ERROR, "Failed to open file '%s'", file_name);
        return NULL;
    }

    char *source = NULL;

    /* Go to the end of the file. */
    if (fseek(stream, 0L, SEEK_END) == 0) {
        long buf_size = ftell(stream);

        if (buf_size == -1) { 
            PLUM_LOG(PLUM_ERROR, "Failed ftell for '%s'", file_name);
            return NULL;
        }

        source = (char*) malloc(sizeof(char) * (buf_size + 1));

        if (fseek(stream, 0L, SEEK_SET) != 0)  { 
            PLUM_LOG(PLUM_ERROR, "Failed fseek for '%s'", file_name);
            return NULL;
        }

        size_t new_len = fread(source, sizeof(char), buf_size, stream);

        if (ferror( stream ) != 0 ) {
            fputs("Error reading file", stderr);
            PLUM_LOG(PLUM_ERROR, "Error reading file '%s'", file_name);
            return NULL;
        }

        source[new_len++] = '\0'; /* Just to be safe. */
    }

    fclose(stream);

    return source;
}

char* rac0a_string_copy(const char* input) {
    unsigned long long len = strlen(input) + 1;
    char* out =  malloc(sizeof(char) * len);
    memcpy(out, input, len);
    return out;
}

char* rac0a_string_copy_len(const char* input, unsigned long long size) {
    unsigned long long len = size + 1;
    char* out =  calloc(len, sizeof(char));
    memcpy(out, input, len);
    return out;
}

rac0a_boolean_t rac0a_in_range_include(char value, char min, char max) {
    return (value >= min) && (value <= max); 
}

rac0_value_t rac0_opcode_valid(rac0_opcode_t opcode) {
    return opcode >= 1 && opcode <= 71;
}

rac0_value_t rac0_opcode_privileged(rac0_opcode_t opcode) {
    if(opcode == RAC0_HALT_OPCODE) {
        return 1;
    } else if(opcode == RAC0_WAIT_OPCODE) {
        return 1;
    } else if(opcode == RAC0_SETIDTT_OPCODE) {
        return 1;
    } else if(opcode == RAC0_SETIDTST_OPCODE) {
        return 1;
    } else if(opcode == RAC0_SETPTBAT_OPCODE) {
        return 1;
    } else if(opcode == RAC0_SETPTST_OPCODE) {
        return 1;
    } else if(opcode == RAC0_SETPTPST_OPCODE) {
        return 1;
    } else if(opcode == RAC0_SETTT_OPCODE) {
        return 1;
    } else if(opcode == RAC0_SETIRETT_OPCODE) {
        return 1;
    } else if(opcode == RAC0_SETSTT_OPCODE) {
        return 1;
    } else if(opcode == RAC0_PUSHA_OPCODE) {
        return 0;
    } else if(opcode == RAC0_PUSHPC_OPCODE) {
        return 0;
    } else if(opcode == RAC0_PUSHSS_OPCODE) {
        return 0;
    } else if(opcode == RAC0_PUSHDC_OPCODE) {
        return 1;
    } else if(opcode == RAC0_PUSHMS_OPCODE) {
        return 1;
    } else if(opcode == RAC0_PUSHIRET_OPCODE) {
        return 1;
    } else if(opcode == RAC0_DUPT_OPCODE) {
        return 0;
    } else if(opcode == RAC0_DUPN_OPCODE) {
        return 0;
    } else if(opcode == RAC0_DROP_OPCODE) {
        return 0;
    } else if(opcode == RAC0_SWAP_OPCODE) {
        return 0;
    } else if(opcode == RAC0_STORE_OPCODE) {
        return 0;
    } else if(opcode == RAC0_STOREARAC_OPCODE) {
        return 0;
    } else if(opcode == RAC0_STOREA_OPCODE) {
        return 0;
    } else if(opcode == RAC0_LOAD_OPCODE) {
        return 0;
    } else if(opcode == RAC0_LOADA_OPCODE) {
        return 0;
    } else if(opcode == RAC0_LOADARAC_OPCODE) {
        return 0;
    } else if(opcode == RAC0_ADD_OPCODE) {
        return 0;
    } else if(opcode == RAC0_SUB_OPCODE) {
        return 0;
    } else if(opcode == RAC0_MUL_OPCODE) {
        return 0;
    } else if(opcode == RAC0_DIV_OPCODE) {
        return 0;
    } else if(opcode == RAC0_MOD_OPCODE) {
        return 0;
    } else if(opcode == RAC0_CMP_OPCODE) {
        return 0;
    } else if(opcode == RAC0_NEG_OPCODE) {
        return 0;
    } else if(opcode == RAC0_NOT_OPCODE) {
        return 0;
    } else if(opcode == RAC0_AND_OPCODE) {
        return 0;
    } else if(opcode == RAC0_OR_OPCODE) {
        return 0;
    } else if(opcode == RAC0_NAND_OPCODE) {
        return 0;
    } else if(opcode == RAC0_NOR_OPCODE) {
        return 0;
    } else if(opcode == RAC0_XOR_OPCODE) {
        return 0;
    } else if(opcode == RAC0_JMPA_OPCODE) {
        return 0;
    } else if(opcode == RAC0_JMPT_OPCODE) {
        return 0;
    } else if(opcode == RAC0_JMPTC_OPCODE) {
        return 0;
    } else if(opcode == RAC0_JZA_OPCODE) {
        return 0;
    } else if(opcode == RAC0_JNZA_OPCODE) {
        return 0;
    } else if(opcode == RAC0_JNEGA_OPCODE) {
        return 0;
    } else if(opcode == RAC0_JPOSA_OPCODE) {
        return 0;
    } else if(opcode == RAC0_SETDA_OPCODE) {
        return 1;
    } else if(opcode == RAC0_SETDT_OPCODE) {
        return 1;
    } else if(opcode == RAC0_FETCHDA_OPCODE) {
        return 1;
    } else if(opcode == RAC0_FETCHDT_OPCODE) {
        return 1;
    } else if(opcode == RAC0_PUTDA_OPCODE) {
        return 1;
    } else if(opcode == RAC0_PUTDT_OPCODE) {
        return 1;
    } else if(opcode == RAC0_INT_OPCODE) {
        return 0;
    } else if(opcode == RAC0_IRETAC_OPCODE) {
        return 1;
    } else if(opcode == RAC0_SETSTBTA_OPCODE) {
        return 1;
    } else if(opcode == RAC0_SETSTBFA_OPCODE) {
        return 1;
    } else if(opcode == RAC0_LOADSTA_OPCODE) {
        return 0;
    } else if(opcode == RAC0_STORESTA_OPCODE) {
        return 0;
    } else if(opcode == RAC0_IRETCC_OPCODE) {
        return 1;
    } else if(opcode == RAC0_LOOP_OPCODE) {
        return 0;
    } else if(opcode == RAC0_MCPYBA_OPCODE) {
        return 1;
    } else if(opcode == RAC0_SETSTSSC_OPCODE) {
        return 1;
    } else if(opcode == RAC0_STORESSA_OPCODE) {
        return 0;
    } else if(opcode == RAC0_LOADSSA_OPCODE) {
        return 0;
    } else if(opcode == RAC0_CLEARST_OPCODE) {
        return 0;
    } else if(opcode == RAC0_SETTTA_OPCODE) {
        return 1;
    } else if(opcode == RAC0_DROPIRET_OPCODE) {
        return 1;
    } else if(opcode == RAC0_PUSHIRETC_OPCODE) {
        return 1;
    } else if(opcode == RAC0_MODAC_OPCODE) {
        return 0;
    } else if(opcode == RAC0_ADDAC_OPCODE) {
        return 0;
    } else if(opcode == RAC0_MULAC_OPCODE) {
        return 0;
    } else { 
        return 0;
    }
}
