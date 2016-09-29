#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <stdlib.h>
#include <unistd.h>
#include "memlayout.h"

int PAGE_SIZE = 4096;

int main(){
	
	struct memregion* regionlist = malloc(sizeof(struct memregion)*35);
	if (regionlist == NULL){
		printf("Failed to allocate memory\n");
		exit(0);
	}
	struct memregion* difflist = malloc(sizeof(struct memregion)*10); //Used later for diff list
	if (difflist == NULL){
		printf("Failed to allocate memory\n");
		exit(0);
	}
	int i;
	char* mode_array[3] = {"RW","RO","NO"};
	printf("Entering get_mem_layout\n");
	int memlayout = get_mem_layout(regionlist,35);
	printf("# of regions found: %u\n",memlayout);
	printf("Printing regions found:\n");
	for(i = 0; i<35; ++i){
		printf("%p-%p %s\n",regionlist[i].from,regionlist[i].to,mode_array[regionlist[i].mode]);
	}
	
	sbrk(500000); //increase size of heap by 500000 bytes
	
	printf("Entering get_mem_diff\n");
	memlayout = get_mem_diff(regionlist, 35, difflist, 10);
	printf("# of different regions found: %u\n",memlayout);
	printf("Printing difference found:\n");
	for(i = 0; i<10; ++i){
		printf("%p-%p %s\n",difflist[i].from,difflist[i].to,mode_array[difflist[i].mode]);
	}
}
