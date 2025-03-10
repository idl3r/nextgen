/**
 * Copyright (c) 2015, Harrison Bowden, Minneapolis, MN
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

#ifndef LOG_H
#define LOG_H

#include <stdint.h>

#ifdef FREEBSD

#include "log-freebsd.h"

#elif MAC_OSX

#include "log-mac.h"

#endif

struct log_obj
{
	int32_t ret_value;
	int32_t had_error;
	uint32_t number_of_args;
	uint32_t syscall_number;
	const char *err_value;
	const char *name_of_syscall;
	uint64_t **arg_value_index;

};

extern int32_t create_out_directory(char *path);

extern int32_t log_results(struct log_obj *obj);

extern int32_t log_file(char *file_path, char *file_extension);

#endif
