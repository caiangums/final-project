/**
 *
 * Copyright (C) 2012  Heechul Yun <heechul@illinois.edu>
 *               2012  Zheng <zpwu@uwaterloo.ca>
 *
 * This file is distributed under the University of Illinois Open Source
 * License. See LICENSE.TXT for details.
 *
 */

/**************************************************************************
 * Included Files
 **************************************************************************/

#include <string.h>
#include <architecture/tsc.h>
#include <process.h>

using namespace EPOS;

typedef long long int64_t;
typedef unsigned long long uint64_t;

OStream cout;

/**************************************************************************
 * Public Definitions
 **************************************************************************/
#define CACHE_LINE_SIZE 64       /* cache Line size is 64 byte */
#define DEFAULT_ALLOC_SIZE_KB 16384


/**************************************************************************
 * Public Types
 **************************************************************************/
enum access_type { READ, WRITE};

/**************************************************************************
 * Global Variables
 **************************************************************************/
int g_mem_size = DEFAULT_ALLOC_SIZE_KB * 1024;       /* memory size */
int *g_mem_ptr = 0;           /* pointer to allocated memory region */

volatile uint64_t g_nread = 0;               /* number of bytes read */
volatile unsigned long long g_start;           /* starting time */
int cpuid = 0;

/**************************************************************************
 * Public Functions
 **************************************************************************/
unsigned long long get_usecs()
{
    return (TSC::time_stamp()*1000000)/TSC::frequency();
}

void quit(int param)
{
    float dur_in_sec;
    float bw;
    unsigned long long dur = get_usecs() - g_start;
    dur_in_sec = (float)dur / 1000000.;
    cout << "g_nread(bytes read) = " << (long long)g_nread << endl;
    cout << "elapsed = " << dur_in_sec << "sec ( " << dur << " usec )" << endl;
    bw = (float)g_nread / dur_in_sec / 1024 / 1024;
    cout << "CPU" << cpuid << ": B/W = " << bw << " MB/s |";
    cout << " CPU" << cpuid << ": average = " << (dur*1000.)/(g_nread/CACHE_LINE_SIZE) << " ns" << endl;
}

uint64_t bench_read()
{
    int i;    
    uint64_t sum = 0;
    for ( i = 0; i < g_mem_size/4; i+=(CACHE_LINE_SIZE/4) ) {
        sum += g_mem_ptr[i];
    }
    g_nread += g_mem_size;
    return sum;
}

int bench_write()
{
    register int i;    
    for ( i = 0; i < g_mem_size/4; i+=(CACHE_LINE_SIZE/4) ) {
        g_mem_ptr[i] = i;
    }
    g_nread += g_mem_size;
    return 1;
}

// void usage(int argc, char *argv[])
// {
//     printf("Usage: $ %s [<option>]*\n\n", argv[0]);
//     printf("-m: memory size in KB. deafult=8192\n");
//     printf("-a: access type - read, write. default=read\n");
//     printf("-n: addressing pattern - Seq, Row, Bank. default=Seq\n");
//     printf("-t: time to run in sec. 0 means indefinite. default=5. \n");
//     printf("-c: CPU to run.\n");
//     printf("-i: iterations. 0 means intefinite. default=0\n");
//     printf("-p: priority\n");
//     printf("-l: log label. use together with -f\n");
//     printf("-f: log file name\n");
//     printf("-h: help\n");
//     printf("\nExamples: \n$ bandwidth -m 8192 -a read -t 1 -c 2\n  <- 8MB read for 1 second on CPU 2\n");
//     exit(1);
// }

int FuncTask1()
{
    uint64_t sum = 0;
    //unsigned finish = 5;
    //int prio = 0;        
    int num_processors = Traits<Build>::CPUS;
    int acc_type = READ;
    //int acc_type = WRITE;
    //int opt;
    //cpu_set_t cmask;
    int iterations = 100;
    int i;

    g_mem_ptr = new int[g_mem_size];

    for (i = 0; i < g_mem_size / sizeof(int); i++) {
        g_mem_ptr[i] = i;
    }
    cout <<"memsize=" << g_mem_size/1024 << " KB, type=" << ((acc_type==READ) ?"read": "write") << ", cpuid=" << cpuid << endl; 

    g_start = get_usecs();
    for (i=0;; i++) {
        switch (acc_type) {
        case READ:
            sum += bench_read();
            break;
        case WRITE:
            sum += bench_write();
            break;
        }

        if (iterations > 0 && i >= iterations)
            break;
    }
    cout << "total sum = " << sum << endl;
    quit(0);
    return 0;
}

/******************************************************************************
 *  Main
 *****************************************************************************/

int main() {
    Thread *t = new Thread(&FuncTask1);

    cout << "bandwidth: All TASKs created with result (>0 is OK): " << t << endl;

    t->join();

    delete t;

    cout << "bandwidth | Returned from application main" << endl; 
    
    return 0;
}