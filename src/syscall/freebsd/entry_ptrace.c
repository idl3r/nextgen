

/**
 * Copyright (c) 2015, Harrison Bowden, Secure Labs, Minneapolis, MN
 * 
 * Permission to use, copy, modify, and/or distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice 
 * and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **/

#include "syscall_list.h"

struct syscall_entry entry_ptrace = {

    .name_of_syscall = "ptrace",
    .syscall_symbol = SYS_ptrace,
    .number_of_args = 4,
    .status = ON,
    .requires_root = NX_YES,
    .need_alarm = NX_NO,

    .arg_type_index[FIRST_ARG] = REQUEST,
    .get_arg_index[FIRST_ARG] = &generate_request,

    .arg_type_index[SECOND_ARG] = PID,
    .get_arg_index[SECOND_ARG] = &generate_pid,

    .arg_type_index[THIRD_ARG] = VOID_BUF,
    .get_arg_index[THIRD_ARG] = &generate_buf,

    .arg_type_index[FOURTH_ARG] = INT,
    .get_arg_index[FOURTH_ARG] = &generate_int

};