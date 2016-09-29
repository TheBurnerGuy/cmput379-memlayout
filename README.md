<pre>
1)     (nil)-0x8047fff NO kernel + offset
2) 0x8048000-0x8048fff RO text section
3) 0x8049000-0x8049fff RW data section
4) 0x804a000-0x804afff RO RELRO
5) 0x804b000-0x804bfff RW bss section
6) 0x804c000-0x9e9afff NO unmapped space
                          memory mapping section
		                      stack
</pre>

In all of the driver programs (with page size = 4096), the first five lines are the same. The first line allocates roughly a gigabyte of data to the kernel read as non-readable memory as well as a offset of space between the end of the kernel space and the text section. The next line is the ELF file/text section. The text section is write protected so the program cannot modify itself. The third line shows the data section which is read-writeable because the initialized variables can change during execution. The fourth line shows that the compiler allocated one page of memory as read only. This is to protect the BSS section from being overwritten by data overflowing (Ubuntu's RELRO). The fifth line is the read-writeable BSS section. The sixth line is  non-readable space between the end of the BSS section and the beginning of the heap. The seventh line is the space allocated to the heap as read-write memory. The eighth line is the non-readable unmapped memory space between the end of the heap and the memory mapping section or stack. This is followed by the stack in mem_mod1, and the memory mapping section and the stack in mem_mod2 and mem_mod3. The stack contains both read/write variables as expected of pointers and variable values. Besides the crucial memory allocations described below, much of the difference in each region is changes from read/write memory to read-only memory due to the freeing the variables used in get_mem_layout.

2) mem_mod1 uses the sbrk() command to increase the size of the heap. This causes unmapped memory to be mapped in read-writeable
pages. The change is observed in the 2nd line of the difference output which shows the region has changed from inaccessable to
read-writeable.

3) mem_mod2 uses mmap() to create a 500000 byte anonymous map. This change is observed in the memory mapping area which is located
in the address space below the stack (shown in the first line of the difference regions). This area has changed from unmapped,
unused memory into read-writeable memory pages because the PROT_READ and PROT_WRITE arguments are used.

4) mem_mod3 uses mmap() like mem_mod2 to allocate 5 000 000 bytes of read-writeable memory in the memory mapping section. It then
uses mprotect to write protect the region that is the memory mapping section. This change is seen in the second line of the difference ouput which shows 1220 pages (5 001 215 bytes) of memory in the memory segment have changed from read-writeable to read only.
