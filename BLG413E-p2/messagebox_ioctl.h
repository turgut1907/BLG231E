#ifndef __MESSAGEBOX_H
#define __MESSAGEBOX_H

#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */
#define MESSAGEBOX_MAJOR 0
#define MES_SIZE 50
#define USERLIMIT 20
#define USERNAME_SIZE 20
#define DEF_BOXLIMIT 5
#define EXCLUDE_READ 1
#define INCLUDE_READ 2

#define SET_READ 3
#define SET_LIMIT 4
#define DELETE_MSG 5

#endif
