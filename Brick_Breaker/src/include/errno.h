#ifndef ERRNO_H
#define ERRNO_H

// ADDED: Error codes (Doom checks these but doesn't really use them)
extern int errno;

#define EPERM   1
#define ENOENT  2
#define EINTR   4
#define EIO     5
#define ENOMEM  12
#define EACCES  13
#define EFAULT  14
#define EBUSY   16
#define EEXIST  17
#define EINVAL  22
#define ENOSPC  28
#define ERANGE  34

#endif