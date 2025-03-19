// module definitions is a global thing, lookin at this code,
// we expected to be compiled only 0 2 messages, note it works with nested module definition

@module main {
    message0 db "0. This message should be included"

    
    @module main {
        message0 db "1. This message should NOT be included"
    }

    @module another {
        message0 db "2. This message should be included"

        @module main {
            message0 db "3. This message should NOT be included"
        }
    }
}

@module main {
    message0 db "4. This message should NOT be included"

    @module another {
        message0 db "5. This message should NOT be included"
    }
}

@module another {
    message5 db "5. This message should NOT be included"
}