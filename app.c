#include "app.h"

/************************** 
 * Error Functions
 **************************/
void unixError(char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}

void posixError(int code, char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(code));
    exit(0);
}

void dnsError(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(0);
}

void gaiError(int code, char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, gai_strerror(code));
    exit(0);
}

void appError(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(0);
}

/*********************************************
 * Unix process control functions
 ********************************************/
pid_t Fork(void) 
{
    pid_t pid;

    if ((pid = fork()) < 0)
	unixError("Fork error");
    return pid;
}

void Execve(const char *filename, char *const argv[], char *const envp[]) 
{
    if (execve(filename, argv, envp) < 0)
	unixError("Execve error");
}

pid_t Wait(int *status) 
{
    pid_t pid;

    if ((pid  = wait(status)) < 0)
	unixError("Wait error");
    return pid;
}

pid_t Waitpid(pid_t pid, int *iptr, int options) 
{
    pid_t retpid;

    if ((retpid  = waitpid(pid, iptr, options)) < 0) 
	unixError("Waitpid error");
    return(retpid);
}

void Kill(pid_t pid, int signum) 
{
    int rc;

    if ((rc = kill(pid, signum)) < 0)
	unixError("Kill error");
}

void Pause() 
{
    (void)pause();
    return;
}

unsigned int Sleep(unsigned int secs) 
{
    unsigned int rc;

    if ((rc = sleep(secs)) < 0)
	unixError("Sleep error");
    return rc;
}

unsigned int Alarm(unsigned int seconds) {
    return alarm(seconds);
}
 
void Setpgid(pid_t pid, pid_t pgid) {
    int rc;

    if ((rc = setpgid(pid, pgid)) < 0)
	unixError("Setpgid error");
    return;
}

pid_t Getpgrp(void) {
    return getpgrp();
}

/************************************
 * Unix signal functions 
 ***********************************/

handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;

    if (sigaction(signum, &action, &old_action) < 0)
	unixError("Signal error");
    return (old_action.sa_handler);
}

void Sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
    if (sigprocmask(how, set, oldset) < 0)
	unixError("Sigprocmask error");
    return;
}

void Sigemptyset(sigset_t *set)
{
    if (sigemptyset(set) < 0)
	unixError("Sigemptyset error");
    return;
}

void Sigfillset(sigset_t *set)
{ 
    if (sigfillset(set) < 0)
	unixError("Sigfillset error");
    return;
}

void Sigaddset(sigset_t *set, int signum)
{
    if (sigaddset(set, signum) < 0)
	unixError("Sigaddset error");
    return;
}

void Sigdelset(sigset_t *set, int signum)
{
    if (sigdelset(set, signum) < 0)
	unixError("Sigdelset error");
    return;
}

int Sigismember(const sigset_t *set, int signum)
{
    int rc;
    if ((rc = sigismember(set, signum)) < 0)
	unixError("Sigismember error");
    return rc;
}

int Sigsuspend(const sigset_t *set)
{
    int rc = sigsuspend(set);
    if (errno != EINTR)
        unixError("Sigsuspend error");
    return rc;
}

/*************************************************************
 * The Signal-safe I/O package
 *************************************************************/

static void signalsafe_reverse(char s[])
{
    int c, i, j;

    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

static void signalsafe_ltoa(long v, char s[], int b)
{
    int c, i = 0;
    
    do {  
        s[i++] = ((c = (v % b)) < 10)  ?  c + '0' : c - 10 + 'a';
    } while ((v /= b) > 0);
    s[i] = '\0';
    signalsafe_reverse(s);
}

static size_t signalsafe_strlen(char s[])
{
    int i = 0;

    while (s[i] != '\0')
        ++i;
    return i;
}

ssize_t signalsafe_puts(char s[]) 
{
    return write(STDOUT_FILENO, s, signalsafe_strlen(s));
}

ssize_t signalsafe_putl(long v)
{
    char s[128];
    
    signalsafe_ltoa(v, s, 10);
    return signalsafe_puts(s);
}

void signalsafe_error(char s[])
{
    signalsafe_puts(s);
    _exit(1);
}

/*******************************
 * Signal Safe I/O routines
 ******************************/
ssize_t Signalsafe_putl(long v)
{
    ssize_t n;
  
    if ((n = signalsafe_putl(v)) < 0)
	signalsafe_error("Signalsafe_putl error");
    return n;
}

ssize_t Signalsafe_puts(char s[])
{
    ssize_t n;
  
    if ((n = signalsafe_puts(s)) < 0)
	signalsafe_error("Signalsafe_puts error");
    return n;
}

void Signalsafe_error(char s[])
{
    signalsafe_error(s);
}

/********************************
 * Unix I/O routines
 ********************************/
int Open(const char *pathname, int flags, mode_t mode) 
{
    int rc;

    if ((rc = open(pathname, flags, mode))  < 0)
	unixError("Open error");
    return rc;
}

ssize_t Read(int fd, void *buf, size_t count) 
{
    ssize_t rc;

    if ((rc = read(fd, buf, count)) < 0) 
	unixError("Read error");
    return rc;
}

ssize_t Write(int fd, const void *buf, size_t count) 
{
    ssize_t rc;

    if ((rc = write(fd, buf, count)) < 0)
	unixError("Write error");
    return rc;
}

off_t Lseek(int fildes, off_t offset, int whence) 
{
    off_t rc;

    if ((rc = lseek(fildes, offset, whence)) < 0)
	unixError("Lseek error");
    return rc;
}

void Close(int fd) 
{
    int rc;

    if ((rc = close(fd)) < 0)
	unixError("Close error");
}

int Select(int  n, fd_set *readfds, fd_set *writefds,
	   fd_set *exceptfds, struct timeval *timeout) 
{
    int rc;

    if ((rc = select(n, readfds, writefds, exceptfds, timeout)) < 0)
	unixError("Select error");
    return rc;
}

int Dup2(int fd1, int fd2) 
{
    int rc;

    if ((rc = dup2(fd1, fd2)) < 0)
	unixError("Dup2 error");
    return rc;
}

void Stat(const char *filename, struct stat *buf) 
{
    if (stat(filename, buf) < 0)
	unixError("Stat error");
}

void Fstat(int fd, struct stat *buf) 
{
    if (fstat(fd, buf) < 0)
	unixError("Fstat error");
}

/*********************************
 * Directory function
 *********************************/
DIR *Opendir(const char *name) 
{
    DIR *dirp = opendir(name); 

    if (!dirp)
        unixError("opendir error");
    return dirp;
}

struct dirent *Readdir(DIR *dirp)
{
    struct dirent *dep;
    
    errno = 0;
    dep = readdir(dirp);
    if ((dep == NULL) && (errno != 0))
        unixError("readdir error");
    return dep;
}

int Closedir(DIR *dirp) 
{
    int rc;

    if ((rc = closedir(dirp)) < 0)
        unixError("closedir error");
    return rc;
}

/***************************************
 * Memory mapping functions
 ***************************************/
void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset) 
{
    void *ptr;

    if ((ptr = mmap(addr, len, prot, flags, fd, offset)) == ((void *) -1))
	unixError("mmap error");
    return(ptr);
}

void Munmap(void *start, size_t length) 
{
    if (munmap(start, length) < 0)
	unixError("munmap error");
}

/***************************************************
 * Dynamic storage allocation functions
 ***************************************************/

void *Malloc(size_t size) 
{
    void *p;

    if ((p  = malloc(size)) == NULL)
	unixError("Malloc error");
    return p;
}

void *Realloc(void *ptr, size_t size) 
{
    void *p;

    if ((p  = realloc(ptr, size)) == NULL)
	unixError("Realloc error");
    return p;
}

void *Calloc(size_t nmemb, size_t size) 
{
    void *p;

    if ((p = calloc(nmemb, size)) == NULL)
	unixError("Calloc error");
    return p;
}

void Free(void *ptr) 
{
    free(ptr);
}

/******************************************
 * Standard I/O functions.
 ******************************************/
void Fclose(FILE *fp) 
{
    if (fclose(fp) != 0)
	unixError("Fclose error");
}

FILE *Fdopen(int fd, const char *type) 
{
    FILE *fp;

    if ((fp = fdopen(fd, type)) == NULL)
	unixError("Fdopen error");

    return fp;
}

char *Fgets(char *ptr, int n, FILE *stream) 
{
    char *rptr;

    if (((rptr = fgets(ptr, n, stream)) == NULL) && ferror(stream))
	appError("Fgets error");

    return rptr;
}

FILE *Fopen(const char *filename, const char *mode) 
{
    FILE *fp;

    if ((fp = fopen(filename, mode)) == NULL)
	unixError("Fopen error");

    return fp;
}

void Fputs(const char *ptr, FILE *stream) 
{
    if (fputs(ptr, stream) == EOF)
	unixError("Fputs error");
}

size_t Fread(void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
    size_t n;

    if (((n = fread(ptr, size, nmemb, stream)) < nmemb) && ferror(stream)) 
	unixError("Fread error");
    return n;
}

void Fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
    if (fwrite(ptr, size, nmemb, stream) < nmemb)
	unixError("Fwrite error");
}


/**************************** 
 * Sockets interface wrappers
 ****************************/

int Socket(int domain, int type, int protocol) 
{
    int rc;

    if ((rc = socket(domain, type, protocol)) < 0)
	unixError("Socket error");
    return rc;
}

void Setsockopt(int s, int level, int optname, const void *optval, int optlen) 
{
    int rc;

    if ((rc = setsockopt(s, level, optname, optval, optlen)) < 0)
	unixError("Setsockopt error");
}

void Bind(int sockfd, struct sockaddr *my_addr, int addrlen) 
{
    int rc;

    if ((rc = bind(sockfd, my_addr, addrlen)) < 0)
	unixError("Bind error");
}

void Listen(int s, int backlog) 
{
    int rc;

    if ((rc = listen(s,  backlog)) < 0)
	unixError("Listen error");
}

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen) 
{
    int rc;

    if ((rc = accept(s, addr, addrlen)) < 0)
	unixError("Accept error");
    return rc;
}

void Connect(int sockfd, struct sockaddr *serv_addr, int addrlen) 
{
    int rc;

    if ((rc = connect(sockfd, serv_addr, addrlen)) < 0)
	unixError("Connect error");
}

/*******************************
 * Protocol-independent wrappers
 *******************************/
void Getaddrinfo(const char *node, const char *service, 
                 const struct addrinfo *hints, struct addrinfo **res)
{
    int rc;

    if ((rc = getaddrinfo(node, service, hints, res)) != 0) 
        gaiError(rc, "Getaddrinfo error");
}

void Getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host, 
                 size_t hostlen, char *serv, size_t servlen, int flags)
{
    int rc;

    if ((rc = getnameinfo(sa, salen, host, hostlen, serv, 
                          servlen, flags)) != 0) 
        gaiError(rc, "Getnameinfo error");
}

void Freeaddrinfo(struct addrinfo *res)
{
    freeaddrinfo(res);
}

void Inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    if (!inet_ntop(af, src, dst, size))
        unixError("Inet_ntop error");
}

void Inet_pton(int af, const char *src, void *dst) 
{
    int rc;

    rc = inet_pton(af, src, dst);
    if (rc == 0)
	appError("inet_pton error: invalid dotted-decimal address");
    else if (rc < 0)
        unixError("Inet_pton error");
}

/*******************************************
 * DNS interface wrappers. 
 *
 * NOTE: These are obsolete because they are not thread safe. Use
 * getaddrinfo and getnameinfo instead
 ***********************************/
struct hostent *Gethostbyname(const char *name) 
{
    struct hostent *p;

    if ((p = gethostbyname(name)) == NULL)
	dnsError("Gethostbyname error");
    return p;
}

struct hostent *Gethostbyaddr(const char *addr, int len, int type) 
{
    struct hostent *p;

    if ((p = gethostbyaddr(addr, len, type)) == NULL)
	dnsError("Gethostbyaddr error");
    return p;
}

/************************************************
 * Pthreads thread control functions
 ************************************************/

void Pthread_create(pthread_t *tidp, pthread_attr_t *attrp, 
		    void * (*routine)(void *), void *argp) 
{
    int rc;

    if ((rc = pthread_create(tidp, attrp, routine, argp)) != 0)
	posixError(rc, "Pthread_create error");
}

void Pthread_cancel(pthread_t tid) {
    int rc;

    if ((rc = pthread_cancel(tid)) != 0)
	posixError(rc, "Pthread_cancel error");
}

void Pthread_join(pthread_t tid, void **thread_return) {
    int rc;

    if ((rc = pthread_join(tid, thread_return)) != 0)
	posixError(rc, "Pthread_join error");
}

void Pthread_detach(pthread_t tid) {
    int rc;

    if ((rc = pthread_detach(tid)) != 0)
	posixError(rc, "Pthread_detach error");
}

void Pthread_exit(void *retval) {
    pthread_exit(retval);
}

pthread_t Pthread_self(void) {
    return pthread_self();
}
 
void Pthread_once(pthread_once_t *once_control, void (*init_function)()) {
    pthread_once(once_control, init_function);
}

/********************
 * Posix semaphores
 ********************/
void Sem_init(sem_t *sem, int pshared, unsigned int value) 
{
    if (sem_init(sem, pshared, value) < 0)
	unixError("Sem_init error");
}

void P(sem_t *sem) 
{
    if (sem_wait(sem) < 0)
	unixError("P error");
}

void V(sem_t *sem) 
{
    if (sem_post(sem) < 0)
	unixError("V error");
}

/****************************************
 * The Robust_io package - Robust I/O functions
 ****************************************/
ssize_t robust_io_readn(int fd, void *usrbuf, size_t n) 
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;

    while (nleft > 0) {
	if ((nread = read(fd, bufp, nleft)) < 0) {
	    if (errno == EINTR)
		nread = 0;
	    else
		return -1;
	} 
	else if (nread == 0)
	    break;   
	nleft -= nread;
	bufp += nread;
    }
    return (n - nleft); 
}

ssize_t robust_io_writen(int fd, void *usrbuf, size_t n) 
{
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = usrbuf;

    while (nleft > 0) {
	if ((nwritten = write(fd, bufp, nleft)) <= 0) {
	    if (errno == EINTR) 
		nwritten = 0;
	    else
		return -1;    
	}
	nleft -= nwritten;
	bufp += nwritten;
    }
    return n;
}


static ssize_t robust_io_read(robust_io_t *rp, char *usrbuf, size_t n)
{
    int cnt;

    while (rp->robust_io_cnt <= 0) { 
	rp->robust_io_cnt = read(rp->robust_io_fd, rp->robust_io_buf, 
			   sizeof(rp->robust_io_buf));
	if (rp->robust_io_cnt < 0) {
	    if (errno != EINTR)
		return -1;
	}
	else if (rp->robust_io_cnt == 0)
	    return 0;
	else 
	    rp->robust_io_bufptr = rp->robust_io_buf;
    }

    cnt = n;          
    if (rp->robust_io_cnt < n)   
	cnt = rp->robust_io_cnt;
    memcpy(usrbuf, rp->robust_io_bufptr, cnt);
    rp->robust_io_bufptr += cnt;
    rp->robust_io_cnt -= cnt;
    return cnt;
}

void robust_io_readinitb(robust_io_t *rp, int fd) 
{
    rp->robust_io_fd = fd;  
    rp->robust_io_cnt = 0;  
    rp->robust_io_bufptr = rp->robust_io_buf;
}

ssize_t robust_io_readnb(robust_io_t *rp, void *usrbuf, size_t n) 
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;
    
    while (nleft > 0) {
	if ((nread = robust_io_read(rp, bufp, nleft)) < 0) 
            return -1;  
	else if (nread == 0)
	    break;
	nleft -= nread;
	bufp += nread;
    }
    return (n - nleft);
}

ssize_t robust_io_readlineb(robust_io_t *rp, void *usrbuf, size_t maxlen) 
{
    int n, rc;
    char c, *bufp = usrbuf;

    for (n = 1; n < maxlen; n++) { 
        if ((rc = robust_io_read(rp, &c, 1)) == 1) {
	    *bufp++ = c;
	    if (c == '\n') {
                n++;
     		break;
            }
	} else if (rc == 0) {
	    if (n == 1) {
		return 0;
            }
	    else {
		break;
            }
	} else
	    return -1;
    }
    *bufp = '\0';
    return n-1;
}

/**********************************
 * Robust I/O routines
 **********************************/
ssize_t Robust_io_readn(int fd, void *ptr, size_t nbytes) 
{
    ssize_t n;
  
    if ((n = robust_io_readn(fd, ptr, nbytes)) < 0)
	unixError("Robust_io_readn error");
    return n;
}

void Robust_io_writen(int fd, void *usrbuf, size_t n) 
{
    if (robust_io_writen(fd, usrbuf, n) != n)
	unixError("Robust_io_writen error");
}

void Robust_io_readinitb(robust_io_t *rp, int fd)
{
    robust_io_readinitb(rp, fd);
} 

ssize_t Robust_io_readnb(robust_io_t *rp, void *usrbuf, size_t n) 
{
    ssize_t rc;

    if ((rc = robust_io_readnb(rp, usrbuf, n)) < 0)
	unixError("Robust_io_readnb error");
    return rc;
}

ssize_t Robust_io_readlineb(robust_io_t *rp, void *usrbuf, size_t maxlen) 
{
    ssize_t rc;

    if ((rc = robust_io_readlineb(rp, usrbuf, maxlen)) < 0)
	unixError("Robust_io_readlineb error");
    return rc;
} 

/******************************** 
 * Client/server helper functions
 ********************************/
int open_clientfd(char *hostname, char *port) {
    int clientfd;
    struct addrinfo hints, *listp, *p;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_flags |= AI_ADDRCONFIG;
    Getaddrinfo(hostname, port, &hints, &listp);
  
    for (p = listp; p; p = p->ai_next) {

        if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
            continue;
        if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1) 
            break;
        Close(clientfd); 
    } 

    Freeaddrinfo(listp);
    if (!p) 
        return -1;
    else   
        return clientfd;
}

int open_listenfd(char *port) 
{
    struct addrinfo hints, *listp, *p;
    int listenfd, optval=1;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_flags = AI_PASSIVE;
    hints.ai_flags |= AI_NUMERICSERV;
    hints.ai_flags |= AI_ADDRCONFIG;
    Getaddrinfo(NULL, port, &hints, &listp);

    for (p = listp; p; p = p->ai_next) {

        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
            continue;

        Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
                   (const void *)&optval , sizeof(int));

        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break; 
        Close(listenfd);
    }

    Freeaddrinfo(listp);
    if (!p) 
        return -1;

    if (listen(listenfd, LISTENQ) < 0)
	return -1;
    return listenfd;
}


/****************************************************
 * Reentrant protocol-independent helpers
 ****************************************************/
int Open_clientfd(char *hostname, char *port) 
{
    int rc;

    if ((rc = open_clientfd(hostname, port)) < 0) 
	unixError("Open_clientfd error");
    return rc;
}

int Open_listenfd(char *port) 
{
    int rc;

    if ((rc = open_listenfd(port)) < 0)
	unixError("Open_listenfd error");
    return rc;
}




