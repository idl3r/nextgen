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

#include "generate.h"
#include "crypto/crypto.h"
#include "io/io.h"
#include "memory/memory.h"
#include "network/network.h"
#include "reaper/reaper.h"
#include "resource/resource.h"
#include "runtime/nextgen.h"
#include "utils/utils.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/param.h>
#include <sys/ptrace.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int32_t generate_fd(uint64_t **fd, struct child_ctx *ctx)
{
    /* Allocate the descriptor. */
    (*fd) = mem_alloc(sizeof(int32_t));
    if((*fd) == NULL)
    {
        output(ERROR, "Can't allocate a descriptor\n");
        return (-1);
    }

    /* Get a file descriptor from the descriptor pool. */
    int32_t desc = get_desc();
    if(desc < 0)
    {
        output(ERROR, "Can't get file descriptor\n");
        return (-1);
    }

    memmove((*fd), &desc, sizeof(int32_t));

    /* Set the argument size. */
    ctx->arg_size_index[ctx->current_arg] = sizeof(int32_t);

    return (0);
}

int32_t generate_socket(uint64_t **sock, struct child_ctx *ctx)
{
    (*sock) = mem_alloc(sizeof(int32_t));
    if((*sock) == NULL)
    {
        output(ERROR, "Can't allocate socket\n");
        return (-1);
    }

    int32_t sock_fd = 0;

    sock_fd = get_socket();
    if(sock_fd < 0)
    {
        output(ERROR, "Can't get socket\n");
        return (-1);
    }

    memmove((*sock), &sock_fd, sizeof(int32_t));

    ctx->arg_size_index[ctx->current_arg] = sizeof(int32_t);

    return (0);
}

int32_t generate_buf(uint64_t **buf, struct child_ctx *ctx)
{
    int32_t rtrn = 0;
    uint32_t number = 0;
    uint32_t nbytes = 0;

    rtrn = rand_range(1023, &nbytes);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random size\n");
        return (-1);
    }

    /* Add one to nbytes so that we don't have a zero.
    Which would cause mmap to fail on some platforms. */
    nbytes = nbytes + 1;

    rtrn = rand_range(2, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random number\n");
        return (-1);
    }

    switch(number)
    {
        case 0:
            (*buf) = mmap(NULL, nbytes, PROT_READ | PROT_WRITE,
                          MAP_ANON | MAP_PRIVATE, -1, 0);
            if(*buf == MAP_FAILED)
            {
                output(ERROR, "mmap: %s\n", strerror(errno));
                return (-1);
            }
            break;

        case 1:
            (*buf) =
                mmap(NULL, nbytes, PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0);
            if(*buf == MAP_FAILED)
            {
                output(ERROR, "mmap: %s\n", strerror(errno));
                return (-1);
            }
            break;

        case 2:
            (*buf) =
                mmap(NULL, nbytes, PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
            if(*buf == MAP_FAILED)
            {
                output(ERROR, "mmap: %s\n", strerror(errno));
                return (-1);
            }
            break;

        default:
            output(ERROR, "Should not get here\n");
            return (-1);
    }

    ctx->arg_size_index[ctx->current_arg] = nbytes;

    return (0);
}

int32_t generate_length(uint64_t **length, struct child_ctx *ctx)
{
    uint32_t last_arg = 0;

    if(ctx->current_arg == 0)
        last_arg = 0;
    else
        last_arg = ctx->current_arg - 1;

    /* Allocate the length buffer */
    (*length) = mem_alloc(sizeof(uint64_t));
    if((*length) == NULL)
    {
        output(ERROR, "Can't alloc length\n");
        return (-1);
    }

    /* Set the length to the length of the last syscall argument. */
    memcpy((*length), &ctx->arg_size_index[last_arg], sizeof(uint64_t));

    /* Set this argument's length. */
    ctx->arg_size_index[ctx->current_arg] = sizeof(uint64_t);

    return (0);
}

int32_t generate_path(uint64_t **path, struct child_ctx *ctx)
{
    (*path) = (uint64_t *)get_filepath();
    if((*path) == NULL)
    {
        output(ERROR, "Can't get file path\n");
        return (-1);
    }

    ctx->arg_size_index[ctx->current_arg] = strlen((char *)(*path));

    return (0);
}

int32_t generate_open_flag(uint64_t **flag, struct child_ctx *ctx)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

    rtrn = rand_range(7, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    (*flag) = mem_alloc(12);
    if((*flag) == NULL)
    {
        output(ERROR, "Can't alloc flag\n");
        return (-1);
    }

    switch(number)
    {
        case 0:
            (**flag) |= (uint64_t)O_RDONLY;
            break;

        case 1:
            (**flag) |= (uint64_t)O_WRONLY;
            break;

        case 2:
            (**flag) |= (uint64_t)O_RDWR;
            break;

        case 3:
            (**flag) |= (uint64_t)O_NONBLOCK;
            break;

        case 4:
            (**flag) |= (uint64_t)O_APPEND;
            break;

        case 5:
            (**flag) |= (uint64_t)O_CREAT;
            break;

        case 6:
            (**flag) |= (uint64_t)O_TRUNC;
            break;

        case 7:
            (**flag) |= (uint64_t)O_EXCL;
            break;

        default:
            output(ERROR, "Should not get here\n");
            return (-1);
    }

    ctx->arg_size_index[ctx->current_arg] = sizeof(int64_t);

    return (0);
}

int32_t generate_mode(uint64_t **mode, struct child_ctx *ctx)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

    rtrn = rand_range(20, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    *mode = mem_alloc(12);
    if(*mode == NULL)
    {
        output(ERROR, "Can't alloc mode\n");
        return (-1);
    }

    switch(number)
    {
        case 0:
            **mode = 001;
            break;

        case 1:
            **mode = 002;
            break;

        case 2:
            **mode = 003;
            break;

        case 3:
            **mode = 004;
            break;

        case 4:
            **mode = 005;
            break;

        case 5:
            **mode = 006;
            break;

        case 6:
            **mode = 007;
            break;

        case 7:
            **mode = 010;
            break;

        case 8:
            **mode = 020;
            break;

        case 9:
            **mode = 030;
            break;

        case 10:
            **mode = 040;
            break;

        case 11:
            **mode = 050;
            break;

        case 12:
            **mode = 060;
            break;

        case 13:
            **mode = 070;
            break;

        case 14:
            **mode = 100;
            break;

        case 15:
            **mode = 200;
            break;

        case 16:
            **mode = 300;
            break;

        case 17:
            **mode = 400;
            break;

        case 18:
            **mode = 500;
            break;

        case 19:
            **mode = 600;
            break;

        case 20:
            **mode = 700;
            break;

        default:
            output(ERROR, "Can't get mode\n");
            return (-1);
    }

    ctx->arg_size_index[ctx->current_arg] = sizeof(int64_t);

    return (0);
}

int32_t generate_fs_stat(uint64_t **stat, struct child_ctx *ctx)
{
    struct statfs *stat_buf = mem_alloc(sizeof(struct statfs));
    if(stat_buf == NULL)
    {
        output(ERROR, "Can't alloc stat\n");
        return (-1);
    }

    (*stat) = (uint64_t *)stat_buf;

    ctx->arg_size_index[ctx->current_arg] = sizeof(struct statfs);

    return (0);
}

int32_t generate_fs_stat_flag(uint64_t **flag, struct child_ctx *ctx)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

    *flag = mem_alloc(12);
    if(*flag == NULL)
    {
        output(ERROR, "Can't alloc flag\n");
        return (-1);
    }

    rtrn = rand_range(1, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    switch(number)
    {
        case 0:
            **flag = MNT_NOWAIT;
            break;

        case 1:
            **flag = MNT_WAIT;
            break;

        default:
            output(ERROR, "Can't pick stat flag\n");
            return (-1);
    }

    ctx->arg_size_index[ctx->current_arg] = sizeof(int64_t);

    return (0);
}

int32_t generate_pid(uint64_t **pid, struct child_ctx *ctx)
{
    pid_t local_pid = 0;

    *pid = mem_alloc(sizeof(unsigned long));
    if(*pid == NULL)
    {
        output(STD, "Can't alloc pid\n");
        return (-1);
    }

    local_pid = fork();
    if(local_pid == 0)
    {
        /* Loop sleep until we our killed after we are used for a syscall argument. */
        while(1)
        {
            sleep(30);
        }
    }
    else if(local_pid > 0)
    {
    }
    else
    {
        output(ERROR, "Can't create pid: %s\n", strerror(errno));
        return (-1);
    }

    (**pid) = (uint64_t)local_pid;

    ctx->arg_size_index[ctx->current_arg] = sizeof(int64_t);

    return (0);
}

int32_t generate_int(uint64_t **integer, struct child_ctx *ctx)
{
    uint32_t number;
    int32_t rtrn = 0;

    *integer = mem_alloc(sizeof(uint64_t));
    if(*integer == NULL)
    {
        output(ERROR, "Can't alloc int\n");
        return (-1);
    }

    rtrn = rand_range(INT_MAX, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random int\n");
        return (-1);
    }

    (**integer) = (uint64_t)number;

    ctx->arg_size_index[ctx->current_arg] = sizeof(int64_t);

    return (0);
}

int32_t generate_rusage(uint64_t **usage, struct child_ctx *ctx)
{
    struct rusage *buf = NULL;

    buf = mem_alloc(sizeof(struct rusage));
    if(buf == NULL)
    {
        output(ERROR, "Can't alloc rusage\n");
        return (-1);
    }

    (*usage) = (uint64_t *)buf;

    ctx->arg_size_index[ctx->current_arg] = sizeof(struct rusage);

    return (0);
}

int32_t generate_wait_option(uint64_t **option, struct child_ctx *ctx)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

    (*option) = mem_alloc(sizeof(unsigned long));
    if((*option) == NULL)
    {
        output(ERROR, "Can't alloc option\n");
        return (-1);
    }

    rtrn = rand_range(1, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    switch(number)
    {
        case 0:
            **option = WUNTRACED;
            break;

        case 1:
            **option = WNOHANG;
            break;

        default:
            output(ERROR, "Should not get here\n");
            return (-1);
    }

    ctx->arg_size_index[ctx->current_arg] = sizeof(int64_t);

    return (0);
}

int32_t generate_whence(uint64_t **whence, struct child_ctx *ctx)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

    (*whence) = mem_alloc(sizeof(uint64_t));
    if((*whence) == NULL)
    {
        output(ERROR, "mem_alloc\n");
        return (-1);
    }

#ifdef FREEBSD

    rtrn = rand_range(4, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

#else

    rtrn = rand_range(2, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

#endif

    switch(number)
    {
        case 0:
            (**whence) = SEEK_SET;
            break;

        case 1:
            **whence = SEEK_CUR;
            break;

        case 2:
            **whence = SEEK_END;
            break;

#ifdef FREEBSD

        case 3:
            **whence = SEEK_HOLE;
            break;

        case 4:
            **whence = SEEK_DATA;
            break;

#else
/* Nothing */
#endif

        default:
            output(ERROR, "Should not get here\n");
            return (-1);
    }

    ctx->arg_size_index[ctx->current_arg] = sizeof(int64_t);

    return (0);
}

int32_t generate_offset(uint64_t **offset, struct child_ctx *ctx)
{
    int32_t rtrn = 0;

    (*offset) = mem_alloc(sizeof(uint64_t));
    if((*offset) == NULL)
    {
        output(ERROR, "mem_alloc\n");
        return (-1);
    }

    rtrn = rand_range(INT_MAX, (uint32_t *)(*offset));
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    ctx->arg_size_index[ctx->current_arg] = sizeof(uint64_t);

    return (0);
}

int32_t generate_mountpath(uint64_t **path, struct child_ctx *ctx)
{
    return (0);
}

int32_t generate_mount_type(uint64_t **type, struct child_ctx *ctx)
{

    return (0);
}

int32_t generate_dirpath(uint64_t **dirpath, struct child_ctx *ctx)
{
    (*dirpath) = (uint64_t *)get_dirpath();
    if((*dirpath) == NULL)
    {
        output(ERROR, "Can't get directory path\n");
        return (-1);
    }

    ctx->arg_size_index[ctx->current_arg] = strlen((char *)(*dirpath));

    return (0);
}

int32_t generate_mount_flags(uint64_t **flag, struct child_ctx *ctx)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

#ifdef MAC_OSX

    int32_t flags[] = {MNT_RDONLY, MNT_NOEXEC,      MNT_NOSUID,  MNT_NODEV,
                       MNT_UNION,  MNT_SYNCHRONOUS, MNT_CPROTECT};

    uint32_t range = 7;

#endif
#ifdef FREEBSD

    int32_t flags[] = {MNT_RDONLY, MNT_NOEXEC, MNT_NOSUID, MNT_UNION,
                       MNT_SYNCHRONOUS};

    uint32_t range = 5;

#endif
    (*flag) = mem_alloc(sizeof(uint64_t));
    if((*flag) == NULL)
    {
        output(ERROR, "Can't alloc mount flags\n");
        return (-1);
    }

    rtrn = rand_range(range, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    /* Copy randomly chosen flag value to flag buffer. */
    memcpy((*flag), &flags[number], sizeof(int32_t));

    /* Set argument size. */
    ctx->arg_size_index[ctx->current_arg] = sizeof(uint32_t);

    return (0);
}

int32_t generate_unmount_flags(uint64_t **flag, struct child_ctx *ctx)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

#ifdef MAC_OSX

    int32_t flags[] = {MNT_RDONLY, MNT_NOEXEC,      MNT_NOSUID,  MNT_NODEV,
                       MNT_UNION,  MNT_SYNCHRONOUS, MNT_CPROTECT};

    uint32_t range = 7;

#endif
#ifdef FREEBSD

    int32_t flags[] = {MNT_RDONLY, MNT_NOEXEC, MNT_NOSUID, MNT_UNION,
                       MNT_SYNCHRONOUS};

    uint32_t range = 5;

#endif

    (*flag) = mem_alloc(sizeof(uint64_t));
    if((*flag) == NULL)
    {
        output(ERROR, "Can't alloc mount flags\n");
        return (-1);
    }

    rtrn = rand_range(range, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    /* Copy randomly chosen flag value to flag buffer. */
    memcpy((*flag), &flags[number], sizeof(flags[number]));

    /* Set arg size. */
    ctx->arg_size_index[ctx->current_arg] = sizeof(flags[number]);

    return (0);
}

int32_t generate_request(uint64_t **flag, struct child_ctx *ctx)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

#ifdef MAC_OSX

    int32_t request[] = {PT_TRACE_ME, PT_DENY_ATTACH, PT_CONTINUE,  PT_STEP,
                         PT_KILL,     PT_ATTACH,      PT_ATTACHEXC, PT_DETACH};

    uint32_t range = 7;

#endif
#ifdef FREEBSD

    int32_t request[] = {
        PT_TRACE_ME,   PT_READ_I,      PT_WRITE_I,      PT_IO,
        PT_CONTINUE,   PT_STEP,        PT_KILL,         PT_ATTACH,
        PT_DETACH,     PT_GETREGS,     PT_SETREGS,      PT_GETFPREGS,
        PT_SETFPREGS,  PT_GETDBREGS,   PT_SETDBREGS,    PT_LWPINFO,
        PT_GETNUMLWPS, PT_GETLWPLIST,  PT_SETSTEP,      PT_CLEARSTEP,
        PT_SUSPEND,    PT_RESUME,      PT_TO_SCE,       PT_TO_SCX,
        PT_SYSCALL,    PT_FOLLOW_FORK, PT_VM_TIMESTAMP, PT_VM_ENTRY};

    uint32_t range = 28;

#endif

    (*flag) = mem_alloc(sizeof(uint64_t));
    if((*flag) == NULL)
    {
        output(ERROR, "Can't alloc mount flags\n");
        return (-1);
    }

    rtrn = rand_range(range, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    memcpy((*flag), &request[number], sizeof(int32_t));

    ctx->arg_size_index[ctx->current_arg] = sizeof(int32_t);

    return (0);
}

int32_t generate_recv_flags(uint64_t **flag, struct child_ctx *ctx)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

    (*flag) = mem_alloc(sizeof(uint64_t));
    if((*flag) == NULL)
    {
        output(ERROR, "Can't alloc mount flags\n");
        return (-1);
    }

    rtrn = rand_range(2, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    switch(number)
    {
        case 0:
            (**flag) = MSG_OOB;
            break;

        case 1:
            (**flag) = MSG_PEEK;
            break;

        case 2:
            (**flag) = MSG_WAITALL;
            break;

        default:
            output(ERROR, "Should not get here\n");
            return (-1);
    }

    ctx->arg_size_index[ctx->current_arg] = sizeof(uint64_t);

    return (0);
}

int32_t generate_dev(uint64_t **dev, struct child_ctx *ctx)
{
    (*dev) = mem_alloc(sizeof(dev_t));
    if((*dev) == NULL)
    {
        output(ERROR, "Can't alloc dev\n");
        return (-1);
    }

    dev_t device;

    memmove((*dev), &device, sizeof(dev_t));

    ctx->arg_size_index[ctx->current_arg] = sizeof(dev_t);

    return (0);
}
