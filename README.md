# cmput379-memlayout
In all of the driver programs (with page size = 4096), the first 5 lines are the same. The first line (nil) - 0x837ffff allocates roughly a gigabyte of data to the kernel read as non-readable memory as well as a offset of space between the end of the kernel space and the text segment. The next four lines (0x8048000-0x8048fff, 0x8049000-0x8049fff, 0x804a000-0x804afff, 0x804b000-0x804bfff) is a combination of the text segment for the ELF file, the data segment, and the BSS segment. Containing both addresses to variables and variable values, it uses a variety of read/write spaces and read spaces. The 6th line will vary since it's the non-readable space between the end of the BSS segment and the beginning of the heap. The 7th line is the space allocated to the heap as read-write memory. The 8th line is the non-readable memory space between the end of the heap and the memory mapping segment. From the 9th line and on, it belongs to the memory mapping segment and stack; It contains both read/write variables as expected of pointers and variable values.
Besides the crucial memory allocations described below, much of the difference in each region is changes from read/write memory to read-only memory due to the freeing the variables used in get_mem_layout.

2) mem_mod1 uses the sbrk() command to increase the size of the heap. This causes unmapped memory
to be mapped in read-writeable pages. The change is observed in the 2nd line of the difference regions which
has changed from inaccessable to read-writeable.

mem_mod2 uses mmap() to create a 500000 byte anonymous map. This change is observed in the memory mapping area which is located
in the addess space below the stack (shown in the first line of the difference regions). This area has changed from unmapped, unused memory into read-writeable memo pages becaus the PROT_READ and PROT_WRITE arguments are used.

mem_mod3 uses mmap() like mem_mod2 to allocate read-writeable memory. It then uses mmap to remap this memory to read-only memory. This change is seen in the first line of the difference region and comparing it to
