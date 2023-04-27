# chapter 10 网络模块：Hook

## 1 前置知识

### 1.1 可变参数函数

C语言中函数的可变参数使用`va_list`数据结构描述，它是一个指向可变参数列表的指针。va_list类型的变量通常用于定义一个参数列表，使用宏`va_start`，`va_arg`和`va_end`来访问列表中的参数。

``` c
va_start(va_list arg_ptr, prev_param);
```
确定第一个不可变参数的位置。可变参数在不可变参数之后，因此只需要知道最后一个不可变参数的位置，就能知道可变参数的起始位置。这个宏将`va_list`的指针指向第一个可变参数。

``` c
va_arg(va_list arg_ptr, type);
```
获得当前指向的参数的值，可变参数的类型需要编程人员确定。

``` c
va_end(va_list arg_ptr);
```
`va_start`可能会通过动态内存分配实现，`va_end`用于释放`va_list`的内存。

### 1.2 系统IO函数

#### 1.2.1 sleep函数

``` cpp
// #include <unistd.h>
unsigned int sleep(unsigned int seconds);
int usleep(useconds_t usec);
```

`sleep`族函数用于暂停程序一段时间。`usleep`精确到微秒级，`useconds_t`类型为`unsigned int`。

``` cpp
// #include <time.h>
int nanosleep(const struct timespec *req, struct timespec *rem);
```
`nanosleep`精确到纳秒级，使用一个结构体表示时间：
``` cpp
struct timespec {
    time_t tv_sec;        /* seconds */
    long   tv_nsec;       /* nanoseconds */
};
```
参数`req`为指向要求暂停的时间的指针，参数`rem`为指向存放剩余时间的结构体指针，返回值为0表示暂停时间已经结束，返回值为-1表示出现错误。`nanosleep`可能会被信号中断，此时会返回剩余的时间，需要根据需要进行处理。

### 1.2.2 socket函数

``` cpp
// #include <sys/types.h>
// #include <sys/socket.h>
/**
 * @param domain the protocol family
 *      - AF_UNIX      Local
 *      - AF_LOCAL     Synonym for AF_UNIX
 *      - AF_INET      IPv4 Internet protocols
 *      - AF_INET6     IPv6 Internet protocols
 * @param type the communication semantics
 *      - SOCK_STREAM  sequenced, reliable, two-way, connection-based byte streams
 *      - SOCK_DGRAM   connectionless, unreliable messages of a fixed maximum length
 * @param protocol Normally only a single protocol exists to support a particular socket type within a given protocol family, in which case protocol can be specified as 0.
 * @return
 *      @retval > 0 file descriptor for the new socket
 *      @retval -1  error, errno set appropriately
 */
int socket(int domain, int type, int protocol);
```
用于创建一个新的套接字，该套接字可以进行网络通信。

``` cpp
// #include <sys/types.h>
// #include <sys/socket.h>
/**
 * @brief connects sockfd to the address specified by addr
 * @param sockfd socket fd 
 * @param addr address
 * @param addrlen specifies the size of addr
 * @return
 *      @retval 0 succeed
 *      @retval -1 error, errno set appropriately
 */
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```
用于将套接字连接到指定的网络地址。

``` cpp
// #include <sys/types.h>
// #include <sys/socket.h>
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
用于在服务端接受客户端连接请求的系统调用，建立连接的另一端点。

在使用`socket`函数创建描述符后，如果调用其他与网络通信相关的函数（connect()、bind()、listen()、accept()、recv()、send()等等），这些函数可能会阻塞进程等待数据的到达。

#### 1.2.3 read函数

``` cpp
// #include <unistd.h>
ssize_t read(int fd, void *buf, size_t count);
```
用于从指定的文件描述符中读取指定长度的数据。
- `read`函数是一个阻塞调用，当在套接字上没有数据可读时，会一直阻塞等待，直到有数据到来。
- `read`函数可能会读取到不完整的数据块，因此在使用时需要对返回值进行判断，确保读取到了完整的数据。
- 当有数据可供读取时，`read`函数会从套接字中读取尽可能多的数据，并将其存储到指定的缓冲区中。如果套接字中的数据长度小于指定的`count`参数，则`read`函数会读取所有的数据，并返回实际读取的字节数。如果套接字中的数据长度大于指定的`count`参数，则`read`函数只会读取指定长度的数据，并返回实际读取的字节数。

``` cpp
// #include <sys/uio.h>
ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
```
用于从文件描述符中读取数据到多个缓冲区中。`iov`指针表示缓冲区数组，数组大小由`iovcnt`指定。数组的元素为`iovec`缓冲区结构体：
``` cpp
struct iovec {
    void  *iov_base;    /* Starting address */
    size_t iov_len;     /* Number of bytes to transfer */
};
```
该结构体包括了缓冲区的起始地址和大小。`readv`函数是一个阻塞调用，当在套接字上没有数据可读时，会一直阻塞等待，直到有数据到来。

``` cpp
// #include <sys/types.h>
// #include <sys/socket.h>
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```
用于从已连接的socket接收数据，并存储到缓冲区中。`len`指缓冲区长度，如果缓冲区长度太小，`recv`可能不会读取所有的数据，只会读取`len`个字节的数据，剩余的数据将留在socket接收缓冲区中等待下一次读取。

recv()函数是阻塞的，如果没有接收到数据，它将等待直到有数据到达或者超时。如果需要设置超时时间，可以设置socket为非阻塞模式。

`flags`参数可选以下几个选项：
- MSG_DONTWAIT：设置socket为非阻塞模式，如果没有数据可读取，则立即返回-1，不会阻塞进程等待数据到达
- MSG_ERRQUEUE：返回错误消息，如果接收到的数据无法处理，则可以使用此选项返回错误消息
- MSG_OOB：接收带外数据，与协议相关
- MSG_PEEK：查看接收到的数据，但不从缓冲区中移除数据，可以用于检查接收到的数据是否符合期望
- MSG_TRUNC：如果接收到的数据比缓冲区长度大，则截断数据，只返回部分数据，并设置errno为ENOSPC
- MSG_WAITALL：等待所有请求的字节数都完成后再返回，如果接收到的数据少于请求的字节数，则阻塞等待更多的数据

``` cpp
// #include <sys/types.h>
// #include <sys/socket.h>
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
```
下面这两种写法等价：
``` cpp
recv(sockfd, buf, len, flags);
recvfrom(sockfd, buf, len, flags, NULL, NULL);
```
和`recv`不同的是，`recvfrom`同时也返回数据发送方的地址信息，保存在后两个参数中。`recvfrom`函数通常用于UDP协议，每个数据包都需要携带发送方的地址信息。

`recvfrom`函数是阻塞的，如果没有接收到数据，它将等待直到有数据到达或者超时。

``` cpp
// #include <sys/types.h>
// #include <sys/socket.h>
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
```
用于从套接字接收数据，并将数据存储在指定的缓冲区中。
``` cpp
struct msghdr {
    void         *msg_name;       /* Optional address */
    socklen_t     msg_namelen;    /* Size of address */
    struct iovec *msg_iov;        /* Scatter/gather array */
    size_t        msg_iovlen;     /* # elements in msg_iov */
    void         *msg_control;    /* Ancillary data, see below */
    size_t        msg_controllen; /* Ancillary data buffer len */
    int           msg_flags;      /* Flags on received message */
};
```
`msg_iov`和`msg_iovlen`与前面的`iov`和`iovcnt`一致。其他字段的含义为：
- msg_name：指向一个结构体，其中包含了发送者的地址信息
- msg_namelen：发送者地址结构体的长度
- msg_control：指向一个缓冲区，其中包含了控制信息
- msg_controllen：控制信息缓冲区的长度
- msg_flags：与`recv`函数的flags类似

`msg_control`字段包含了接收到的控制信息，其格式是由`CMSGS_FIRSTHDR`、`CMSG_NXTHDR`和`CMSG_DATA`等宏来控制的，具体取决于发送端和接收端使用的协议和选项。控制信息的格式是由发送端和接收端共同约定的，因此在使用recvmsg函数时，需要根据实际情况来解析控制信息。

#### 1.2.4 write函数

``` cpp
// #include <unistd.h>
ssize_t write(int fd, const void *buf, size_t count);
```
用于向fd指定的文件中写入至多`count`字节的内容。如果`count`比缓冲区大小还大，那么`write`函数只会将缓冲区中的一部分数据写入到指定的文件中，并返回成功写入的字节数。剩余的数据将会被截断，无法写入到文件中。

当调用`write`函数时，数据会被写入到文件描述符对应的缓冲区中，直到缓冲区已满或者所有数据都被写入完成。如果缓冲区已满，`write`函数将会一直等待，直到有足够的空间来容纳要写入的数据。

``` cpp
// #include <sys/uio.h>
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
```
与`readv`相对应。

``` cpp
#include <sys/types.h>
#include <sys/socket.h>
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
```
与`recv`一族函数相对应。

#### 1.2.5 control函数

``` cpp
// #include <unistd.h>
// #include <fcntl.h>
int fcntl(int fd, int cmd, ... /* arg */ );
```
用于对文件描述符进行操作，具体的操作由第二个参数cmd来指定。根据不同的操作类型，该参数可能会传入不同的值或结构体，用于传递操作所需的参数或结果。

``` cpp
// #include <sys/ioctl.h>
int ioctl(int fd, unsigned long request, ...);
```
用于控制设备IO，对设备进行一些特定的控制操作，例如设置设备参数、获取设备状态、读取或写入数据等。其中，fd表示设备文件描述符，request表示要执行的操作类型，后面的可选参数通常是与请求相关的一些数据或参数。

``` cpp
// #include <sys/types.h>
// #include <sys/socket.h>
/**
 * @param sockfd socket file descriptor
 * @param level the protocol number
 * @param optname the name of the option
 * @param optval access option values
 * @param optlen a value-result argument, initially containing the size of the buffer pointed to by optval
 */
int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```
可以获取套接字的各种选项，例如SO_REUSEADDR、SO_KEEPALIVE等选项的值，以及TCP_NODELAY、TCP_MAXSEG等TCP选项的值。

## 2 IO Hook

