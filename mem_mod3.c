#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include "memlayout.h"

int PAGE_SIZE = 4096;

int main(){
	
	struct memregion* regionlist = malloc(sizeof(struct memregion)*35);
	if (regionlist == NULL){
		printf("Failed to allocate memory\n");
		exit(0);
	}
	struct memregion* difflist = malloc(sizeof(struct memregion)*10);
		if (difflist == NULL){
		printf("Failed to allocate memory\n");
		exit(0);
	}
	int i;
	char* mode_array[3] = {"RW","RO","NO"};
	char *place = mmap(0, 5000000, PROT_READ | PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0); //place 5 000 000 bytes of anonymous mapping into the memory mapping segment
	if (place == (void*)0xffffffff){
		//Failed to allocate space
		printf("Failed to allocate memory\n");
		exit(0);
	}
	
	printf("Entering get_mem_layoutn\n");
	int memlayout = get_mem_layout(regionlist,35);
	printf("# of regions found: %u\n",memlayout);
	printf("Printing regions found:\n");
	for(i = 0; i<35; ++i){
		printf("%p-%p %s\n",regionlist[i].from,regionlist[i].to,mode_array[regionlist[i].mode]);
	}
	
   	mprotect(place, 5000000, PROT_READ);
	
	printf("Entering get_mem_diff\n");
	memlayout = get_mem_diff(regionlist, 35, difflist, 10);
	printf("# of different regions found: %u\n",memlayout);
	printf("Printing difference found:\n");
	for(i = 0; i<10; ++i){
		printf("%p-%p %s\n",difflist[i].from,difflist[i].to,mode_array[difflist[i].mode]);
	}
	munmap(place, 5000000);
}
