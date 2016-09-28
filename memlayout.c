#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <stdlib.h>
#include <unistd.h>
#include "memlayout.h"

/*get_mem_layout()
 * Should check at every interval of PAGE_SIZE if it's readable/writable.
 * Keep track of currrent/previous region state. If it's different, 
 * set to to address-1 and create a new region its from being the current address.
 * Keep looping until memory reaches max_size 0xffffffff or regions reaches size.
 */
int get_mem_layout(struct memregion *regions, unsigned int size){
	int current_region = 0;
	int state = 3; //Initially set to 3, since current region is not found yet
	unsigned int temp; //Used to store the value if value is readable
	
	// Initializing nested signal handler function
	jmp_buf env;
	void signal_handler(int sig){siglongjmp(env, 1);}
	
	//Setting up the signal handler
	struct sigaction segv;
	segv.sa_handler = signal_handler;
	sigemptyset(&segv.sa_mask);
	segv.sa_flags = 0;
	sigaction(SIGSEGV,&segv,0);
	
	regions[0].from = (void*)0; //Initializes first region
	
	//Run a for loop until end of memory address is reached or size reached
	unsigned int i;
	for(i = 0; i < 0xffffffff; i+=PAGE_SIZE){
		//Base Case: All memory addresses have been searched, i = 0
		if(i==0 && state!=3) break;
		//Check if regions changed
		//printf("%u\n",i); //Test if i is going through all
		int checkpoint = sigsetjmp(env,1);
		unsigned int* pointer_check = (unsigned int*)i;
		if(!checkpoint){ temp = *pointer_check; }
		else{
			//If checkpoint has changed from 0 to 1, proof that it's unreadable
			if(state == 3) state = MEM_NO; //Special case for the first region
			if(state != MEM_NO){
				//Check if current region has exceeded max size
				if(current_region<size){
					regions[current_region].to = (void*)i-1;
					regions[current_region].mode = state;
				}
				state = MEM_NO;
				++current_region;
				if(current_region<size){ //Check again if incrementing regions has exceeded its maximum size
					regions[current_region].from = (void*)i;
				}
			}
			continue;
		}
		//Assumption: File is readable past this point
		//Now try writing ( be very careful )
		checkpoint = sigsetjmp(env,1);
		if(!checkpoint){ *pointer_check = temp; }
		else{
			//If checkpoint has changed from 0 to 1, proof that it's unwritable
			//But it's readable, check if region changed
			if(state == 3) state = MEM_RO; //Special case for the first region
			if(state != MEM_RO){
				//Check if current region has exceeded max size
				if(current_region<size){
					regions[current_region].to = (void*)i-1;
					regions[current_region].mode = state;
				}
				state = MEM_RO;
				++current_region;
				if(current_region<size){ //Check again if incrementing regions has exceeded its maximum size
					regions[current_region].from = (void*)i;
				}
				continue;
			}
		}
		//Assumption: File is writable, check if region changed
		if(state == 3) state = MEM_RW; //Special case for the first region
		if(state != MEM_RW){
			//Check if current region has exceeded max size
			if(current_region<size){
				regions[current_region].to = (void*)i-1;
				regions[current_region].mode = state;
			}
			state = MEM_RW;
			++current_region;
			if(current_region<size){ //Check again if incrementing regions has exceeded its maximum size
				regions[current_region].from = (void*)i;
			}
		}
	}
	//Set an end to the last region if for loop ended without a base case
	regions[current_region].to = (void*)0xffffffff;
	regions[current_region].mode = state;
	return ++current_region; //Returns all regions found
}

/*get_mem_diff()
 * Scans all memories like get_mem_layout
 * While scanning, compare scanned region to *regions and check for difference
 * If difference found, create a region in thediff and keep scanning.
 * To end, there are two possible outcomes:
 * 1. Changed region ends and goes back to an unchanged region.
 * Set region in thediff's to address to current address - 1 and start a fresh scan.
 * 2. Changed region ends but goes into a different changed region
 * Set region in thediff's to address to current address - 1
 * then create a new region in thediff and start scanning.
 * When ending, there are two possible outcomes:
 * 1. If scanning a changed region, set thediff's to address to current address -1
 * then return.
 *  2. If scanning a region that stayed the same, do nothing and return.
 */
int get_mem_diff (struct memregion *regions, unsigned int howmany, struct memregion *thediff, unsigned int diffsize){
	int current_region = 0;
	int diff_region = -1; //Initially set to -1, so it starts at 0 when function adds 1
	int state = 3; //Initially set to 3, since current region is not found yet
	int previous_state = 3;
	int change = 0; //1 if state has changed, 0 if not
	unsigned int temp; //Used to store the value if value is readable
	
	// Initializing nested signal handler function
	jmp_buf env;
	void signal_handler(int sig){siglongjmp(env, 1);}
	
	//Setting up the signal handler
	struct sigaction segv;
	segv.sa_handler = signal_handler;
	sigemptyset(&segv.sa_mask);
	segv.sa_flags = 0;
	sigaction(SIGSEGV,&segv,0);
	
	//Run a for loop until end of memory address is reached or size reached
	unsigned int i;
	for(i = 0; i < 0xffffffff; i+=PAGE_SIZE){
		//Base Case: All memory addresses have been searched, i = 0
		if(i==0 && state!=3) break;
		//Keep track of the current_region of regions*
		//If found the end address of current region, change regions
		if(regions[current_region].to == (void*)(i-1)) ++current_region;
		//Base Case: Searching past maximum size entries in regions*
		if(current_region >= howmany) break;
		//Scan for current region's state
		//printf("%u\n",i); //Test if i is going through all
		int checkpoint = sigsetjmp(env,1);
		unsigned int* pointer_check = (unsigned int*)i;
		if(!checkpoint){ temp = *pointer_check; }
		else{
			//If checkpoint has changed from 0 to 1, proof that it's unreadable
			state = MEM_NO;
			//Check if there is a difference in region 'mode'
			if(state != regions[current_region].mode){
				if(!change){
					//If was not expecting a new region, create region in thediff
					//as long as diff_region < diffsize
					++diff_region;
					if(diff_region < diffsize){
						thediff[diff_region].from = (void*)i; 
						thediff[diff_region].mode = state;
					}
					change = 1;
				}else{
					//If expected a change, check if its still the same state in the diff
					if(state != previous_state){
						//If not the same state, a new region that is different has been entered
						thediff[diff_region].to = (void*)i-1;
						++diff_region;
						if(diff_region < diffsize){
							thediff[diff_region].from = (void*)i;
							thediff[diff_region].mode = state;
						}
					}
				}
			}else{//If state is the same, check if diff was scanning
				if(change){
					//If scanning, end diff region and stop scanning
					thediff[diff_region].to = (void*)i-1;
					change = 0;
				}
			}
			previous_state = state;
			continue;
		}
		//Assumption: File is readable past this point
		//Now try writing ( be very careful )
		checkpoint = sigsetjmp(env,1);
		if(!checkpoint){ *pointer_check = temp; }
		else{
			//If checkpoint has changed from 0 to 1, proof that it's unwritable
			state = MEM_RO;
			//But it's readable,
			//Check if there is a difference in region 'mode'
			if(state != regions[current_region].mode){
				if(!change){
					//If was not expecting a new region, create region in thediff
					//as long as diff_region < diffsize
					++diff_region;
					if(diff_region < diffsize){
						thediff[diff_region].from = (void*)i; 
						thediff[diff_region].mode = state;
					}
					change = 1;
				}else{
					//If expected a change, check if its still the same state in the diff
					if(state != previous_state){
						//If not the same state, a new region that is different has been entered
						thediff[diff_region].to = (void*)i-1;
						++diff_region;
						if(diff_region < diffsize){
							thediff[diff_region].from = (void*)i;
							thediff[diff_region].mode = state;
						}
					}
				}
			}else{//If state is the same, check if diff was scanning
				if(change){
					//If scanning, end diff region and stop scanning
					thediff[diff_region].to = (void*)i-1;
					change = 0;
				}
			}
			previous_state = state;
			continue;
		}
		//Assumption: File is writable, check if region changed
		state = MEM_RW;
		//Check if there is a difference in region 'mode'
		if(state != regions[current_region].mode){
			if(!change){
				//If was not expecting a new region, create region in thediff
				//as long as diff_region < diffsize
				++diff_region;
				if(diff_region < diffsize){
					thediff[diff_region].from = (void*)i; 
					thediff[diff_region].mode = state;
				}
				change = 1;
			}else{
				//If expected a change, check if its still the same state in the diff
				if(state != previous_state){
					//If not the same state, a new region that is different has been entered
					thediff[diff_region].to = (void*)i-1;
					++diff_region;
					if(diff_region < diffsize){
						thediff[diff_region].from = (void*)i;
						thediff[diff_region].mode = state;
					}
				}
			}
		}else{//If state is the same, check if diff was scanning
			if(change){
				//If scanning, end diff region and stop scanning
				thediff[diff_region].to = (void*)i-1;
				change = 0;
			}
		}
		previous_state = state;
		//continue
	}
	//Set an end to the last diff region if still scanning
	if(change){
		if(diff_region < diffsize){
			thediff[diff_region].to == (void*)0xffffffff;
		}
	}
	return ++diff_region; //Returns all regions found
}
