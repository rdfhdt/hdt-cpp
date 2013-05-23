#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

#include <signal.h>


int interruptSignal = 0;

void signalHandler(int sig)
{
        interruptSignal = 1;
}



#ifdef __APPLE__
typedef char mincore_arr;
#else
typedef unsigned char mincore_arr;
#endif

int main(int argc, char *argv[]) {
    int fd;
    struct stat file_stat;
    void *file_mmap;
    mincore_arr *mincore_vec;
    size_t page_size = getpagesize();
    size_t page_index;
    size_t numMemory;
    size_t totalPages;
    unsigned long count=0;

    fd = open(argv[1],0);
    fstat(fd, &file_stat);
    file_mmap = mmap((void *)0, file_stat.st_size, PROT_NONE, MAP_SHARED, fd, 0);
    mincore_vec = (mincore_arr *)calloc(1, (file_stat.st_size+page_size-1)/page_size);
    printf("Cached Blocks of %s, Page size: %lu: \n",argv[1], (unsigned long)page_size);


    signal(SIGINT, &signalHandler);
	while(interruptSignal==0) {
		numMemory=totalPages=0;
		count++;
	    mincore((void *)file_mmap, file_stat.st_size, mincore_vec);
	    for (page_index = 0; page_index <= file_stat.st_size/page_size; page_index++) {
		if (mincore_vec[page_index]&1) {
			//printf("%lu ", (unsigned long)page_index);
			numMemory++;
		}
		totalPages++;
	    }
	    printf("Pages in memory %lu / %lu  Percent: %lu  MB: %lu/%lu\n", (unsigned long)numMemory, (unsigned long)totalPages, (unsigned long)(100UL*numMemory/totalPages), numMemory*page_size/(1024*1024), totalPages*page_size/(1024*1024));
	    //printf("%lu,%lu,%lu\n", count, (unsigned long)numMemory, (unsigned long)totalPages);
	    sleep(1);
	}
    free(mincore_vec);
    munmap(file_mmap, file_stat.st_size);
    close(fd);
    return 0;
}
