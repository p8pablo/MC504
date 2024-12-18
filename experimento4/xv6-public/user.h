struct stat;
struct rtcdate;

#define MAX_ROUND_MEMORY_OVERHEAD 100 // Número máximo de medições de throughput por rodada

struct mem_overhead {
    int memory_access_time;   // Tempo total de acesso à memória
    int memory_alloc_time;    // Tempo total de alocação de memória
    int memory_free_time;     // Tempo total de desalocação de memória
};

// Array para armazenar o throughput temporário a cada segundo
extern struct mem_overhead mem_overhead_temp[MAX_ROUND_MEMORY_OVERHEAD];
extern int mem_overhead_count;


// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void printf(int, const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);

// custom made functions
int random();
int randomrange(int lo, int hi);
int cpu_bound_task();
int io_bound_task(int i);
void run_experiment(int cpu_count, int io_count);