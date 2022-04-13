# chapter 07 网络模块：sockaddr

## 7.1 套接字地址结构

### 一、sockaddr

#### 1. 通用套接字地址sockaddr

sockaddr相关的结构体用于处理网络通信的地址。

``` cpp
// #include <sys/socket.h>
struct sockaddr {  
    sa_family_t sin_family;     /* address family */
    char sa_data[14];           /* address data */
};
```

``` cpp
typedef unsigned short int sa_family_t;
```

`sa_family_t`是`uint16`类型，常用值如下：

|Value|Description|
|-|-|
|AF_UNIX|Local communication|
|AF_INET|IPv4 Internet protocols|
|AF_INET6|IPv6 Internet protocols|

`sa_data`是通用地址数据，以确保不同的套接字地址结构能够强制转化为通用地址结构。

#### 2. IPv4套接字地址sockaddr_in

IPv4套接字地址结构体`sockaddr_in`为了保持大小与`sockaddr`一致，最后采用0填充。

``` cpp
// #include <arpa/inet.h>
struct sockaddr_in
{
    sa_family_t sin_family;         /* address family   uint16_t */
    in_port_t sin_port;			    /* port number      uint16_t */
    struct in_addr sin_addr;		/* internet address uint32_t */
    unsigned char sin_zero[8];      /* zero byte */
};
```

``` cpp
/* Internet address. */
typedef uint32_t in_addr_t;
struct in_addr
{
    in_addr_t s_addr;
};
```

#### 3. IPv4套接字地址sockaddr_in6

``` cpp
struct sockaddr_in6
{
    sa_family_t sin6_family;        /* address family           uint16_t */
    in_port_t sin6_port;	        /* Transport layer port     uint16_t */
    uint32_t sin6_flowinfo;	        /* IPv6 flow information    uint32_t */
    struct in6_addr sin6_addr;	    /* IPv6 address             128-bit  */
    uint32_t sin6_scope_id;	        /* IPv6 scope-id            uint32_t */
};
```

- `sin6_flowinfo`：低20位作为flow label，高12位保留
- `sin6_scope_id`：标识具有范围的地址

``` cpp
struct in6_addr     /* 128-bit IPv6 address */
{
    union
    {
        uint8_t	__u6_addr8[16];
        uint16_t __u6_addr16[8];
        uint32_t __u6_addr32[4];
    } __in6_u;
};
```

#### 4. Unix域套接字地址sockaddr_un

`sockaddr_un`结构体是定长的，但其中的信息是变长的。

``` cpp
struct sockaddr_un
{
    sa_family_t sun_family;
    char sun_path[108];		/* file path name.  */
};
```

### 二、IP地址操作

#### 1. 字节序转换

``` cpp
// #include <arpa/inet.h>
/**
 * @return values in net byte order
 */
uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);
/**
 * @return in host byte order
 */
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);
```

#### 2. 点分十进制串转换

``` cpp
// #include <arpa/inet.h>
/**
 * @brief pointer to network byte orderconvert IPv4 and IPv6 addresses from text to binary form
 * @param af    AF_INET or AF_INET6
 * @param src   a character string containing an IPv4 network address in dotted-decimal format
 * @param dst   the network address structure written in network byte order
 */
int inet_pton(int af, const char *src, void *dst);

/**
 * @brief convert IPv4 and IPv6 addresses from binary to text form
 * @param af    AF_INET or AF_INET6
 * @param src   the network address structure
 * @param dst   the resulting string buffer
 * @param size  the number of bytes available in the string buffer
 */
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```

### 三、主机和服务的转换

#### 1. 生成套接字地址结构

``` cpp
// #include <netdb.h>
/**
 * @brief network address and service translation
 * @param node      identify an Internet host and a service
 * @param service   identify an Internet host and a service
 * @param hints     specifies criteria for selecting the socket address structures
 * @param res       the socket address structures list
 */
int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);

/**
 * @brief frees the memory that was allocated for the dynamically allocated linked list res
 * @param res   the linked list
 */ 
void freeaddrinfo(struct addrinfo *res);

/**
 * @brief return error information
 * @param errcode error code
 */
const char *gai_strerror(int errcode);
```

``` cpp
struct addrinfo {
    int ai_flags;       /* hints argument flags */
    int ai_family;      /* first arg to socket function */
    int ai_socktype;    /* second arg to socket function */
    int ai_protocol;    /* third arg to socket function */
    char *ai_cannonname;/* canonical hostname */
    size_t ai_addrlen;  /* size of ai_addr struct */
    struct sockaddr *ai_addr;   /* pointer to socket address structure */
    struct addrinfo *ai_next;   /* pointer to next item in linked list */        
}
```