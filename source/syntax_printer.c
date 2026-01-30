#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "syntax_printer.h"

// Static buffer for test strings
static char snap_buf[4096]; 
static int snap_pos = 0;

// Helper to append to the buffer like printf
void buf_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    // Write to buffer at current position
    snap_pos += vsprintf(snap_buf + snap_pos, format, args);
    va_end(args);
}

// Forward declaration
void snap_factors(Factors *fs);

void snap_factor(Factor *f) {
    if (!f) { buf_printf("NULL"); return; }
    switch (f->type) {
        case FACTOR_NUM:  buf_printf("%.2f", f->data.num); break;
        case FACTOR_STR:  buf_printf("\"%s\"", f->data.str); break;
        case FACTOR_VAR:  buf_printf("%s", f->data.var); break;
        case FACTOR_JUMP: 
            if (f->data.jump.type == JUMP_FORWARD) buf_printf("(=> %d)", f->data.jump.lines);
            else buf_printf("(%d <=)", f->data.jump.lines);
            break;
        case FACTOR_FUNC:
            buf_printf("(%s ", f->data.func->name);
            snap_factors(f->data.func->factors);
            buf_printf(")");
            break;
    }
}

void snap_factors(Factors *fs) {
    if (!fs || fs->count == 0) { buf_printf("[]"); return; }
    buf_printf("[");
    for (int i = 0; i < fs->count; i++) {
        snap_factor(fs->items[i]);
        if (i < fs->count - 1) buf_printf(", ");
    }
    buf_printf("]");
}

// The Main Function to call from tests
char* statement_to_string(Statement *stmt) {
    // Reset buffer
    snap_pos = 0;
    snap_buf[0] = '\0';

    if (!stmt) return "NULL";

    switch (stmt->type) {
        case PINCH_VAR:
            buf_printf("(ASSIGN %s ", stmt->content.pinch_var->name);
            snap_factors(stmt->content.pinch_var->factors);
            buf_printf(")");
            break;
        case PINCH_FUNC_S:
            buf_printf("(CALL %s ", stmt->content.pinch_func->name);
            snap_factors(stmt->content.pinch_func->factors);
            buf_printf(")");
            break;
        case FACTOR:
            buf_printf("(FACTOR ");
            snap_factor(stmt->content.factor);
            buf_printf(")");
            break;
    }
    return snap_buf;
}
