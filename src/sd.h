#pragma once

void sd_init(void);

#define O_RDONLY  0X00  ///< Open for reading only.
#define O_WRONLY  0X01  ///< Open for writing only.
#define O_RDWR    0X02  ///< Open for reading and writing.
#define O_AT_END  0X04  ///< Open at EOF.
#define O_APPEND  0X08  ///< Set append mode.
#define O_CREAT   0x10  ///< Create file if it does not exist.
#define O_TRUNC   0x20  ///< Truncate file to zero length.
#define O_EXCL    0x40  ///< Fail if the file exists.
#define O_SYNC    0x80  ///< Synchronized write I/O operations.

int sd_open(const char *path, int oflag);
void sd_close(int fd);
void sd_flush(int fd);
int sd_read(int fd, void *buf, size_t count);
int sd_write(int fd, void *buf, size_t count);
int sd_seek(int fd, uint64_t pos);
