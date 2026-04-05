// I pledge the COMP530 honor code.


// -----------------------------------
//  COMP 530: Operating Systems
//
//  Spring 2026 - Lab 3
// -----------------------------------


// ----------------------------------------
// The approved header fles.
// These CANNOT be modified or
// additional ones included.
// ----------------------------------------


#include <stdlib.h>  
#include <stdio.h>   
#include <stdbool.h>
#include "sim.h"




// ----------------------------------------
// Global variables that CANNOT be modified
// ----------------------------------------
mm_struct_t mm_struct;
memmap_t memmap;
sim_t simulator;






// ----------------------------------------
// Functions that CANNOT be modified
// ----------------------------------------


void init_simulation( unsigned int algorithm, unsigned int mm_size, unsigned int vm_size, unsigned int seed, unsigned int time_units ) {


 simulator.table_size = time_units;
 simulator.ptable = malloc( sizeof( pointer_t* ) * simulator.table_size );
 simulator.seed = seed;
 simulator.time_units = time_units;
 simulator.placement_algorithm = algorithm;
 simulator.num_probes = 0;
 simulator.page_error = 0;
 simulator.frame_error = 0;


 for ( unsigned int i=0; i<simulator.table_size; i++ ) {
   simulator.ptable[i] = NULL;
 }


 mm_struct.num_pages = vm_size;
 mm_struct.page_table = malloc( sizeof( page_t* ) * mm_struct.num_pages );


 for ( unsigned int i=0; i<vm_size; i++ ) {
   mm_struct.page_table[i] = (page_t*)malloc(sizeof( page_t ) );
   mm_struct.page_table[i]->pfn = 0;
   mm_struct.page_table[i]->present = 0;
 }


 memmap.size = mm_size;
 memmap.frames = malloc( sizeof( page_t* ) * memmap.size );


 for ( unsigned int i=0; i<mm_size; i++ ) {
   memmap.frames[i] = NULL;
 }


 memmap.last_placement_frame = 0;


 // honors add-on
 for(unsigned i = 0; i < mm_struct.num_pages; i++) {
   mm_struct.page_table[i]->allocated = 0;
   mm_struct.page_table[i]->swapped = 0;
   mm_struct.page_table[i]->swap_slot = -1;
 }


 memmap.fifo_queue = malloc(sizeof(int) * memmap.size);
 memmap.fifo_head = 0;
 memmap.fifo_tail = 0;
 memmap.fifo_count = 0;


 memmap.swap_size = mm_struct.num_pages; // for simplicity, swap size = number of pages
 memmap.swap_table = malloc(sizeof(int) * memmap.swap_size);


 for(unsigned int i = 0; i < memmap.swap_size; i++) {
   memmap.swap_table[i] = -1;
 }
} // end init() function




void clean_simulation() {


 for ( unsigned int i=0; i<mm_struct.num_pages; i++ ) {
   if ( mm_struct.page_table[i] != NULL ) {
     free( mm_struct.page_table[i] );
   }
 }


 free( mm_struct.page_table );


 for ( unsigned int i=0; i<simulator.table_size; i++ ) {
   if ( simulator.ptable[i] != NULL ) {
     free( simulator.ptable[i] );
   }
 }


 free( simulator.ptable );
 free( memmap.frames );


 // honors add-on
 free( memmap.fifo_queue );
 free( memmap.swap_table );


} // end unallocate() function




void update_simulation( pointer_t* ptr ) {


 if ( ptr != NULL ) {


   for ( unsigned int i=0; i<simulator.table_size; i++ ) {


     if ( simulator.ptable[i] == NULL ) {
       simulator.ptable[i] = ptr;
       break;
     }


   }


 }


 for ( unsigned int i=0; i<simulator.table_size; i++ ) {


   if ( simulator.ptable[i] != NULL ) {


     simulator.ptable[i]->duration--;


     if ( simulator.ptable[i]->duration <= 0 ) {


       s_free( simulator.ptable[i] );
       simulator.ptable[i] = NULL;


     }


   }


 }


} // end update_simulation() function




double stats_probe() {
 return (double) simulator.num_probes / (double) simulator.time_units;
} // end stats_probe() function




double stats_frame_error() {
 return (double) simulator.frame_error / (double) simulator.time_units;
} // end stats_frame_error() function




double stats_page_error() {
 return (double) simulator.page_error / (double) simulator.time_units;
} // end stats_page_error() function




int stats_fragments( int frag_size ) {


 int frag_count = 0;   
 int free_count = 0;


 for (int i = 0; i < memmap.size; i++) { 


   if (memmap.frames[i] == NULL ) free_count++;


   if (memmap.frames[i] != NULL || i == memmap.size - 1) {


     if (free_count > 0 && free_count < frag_size) frag_count++;


     free_count = 0;


   }


 }


 return frag_count;


} // stats_fragments() function


unsigned int randnum( unsigned int max, unsigned int min ) {
 return rand() % (max-min+1) + min;
} // end randnum() function




void show_pointer_table() {


 printf("\n---------------------------\n");
 printf("    POINTER TABLE\n");
 printf("---------------------------\n");
 printf("VPN\tSIZE\tDuration\n" );
 printf("---------------------------\n");


 for (unsigned int i=0; i<simulator.table_size; i++ ) {
   if ( simulator.ptable[i] != NULL ) {
     printf("%d\t%d\t%d\n", simulator.ptable[i]->vpn,
                            simulator.ptable[i]->size,
                            simulator.ptable[i]->duration );
   }


 }
 } // end show_pointer_table() function


// ----------------------------------------
// Functions that CAN be modified
// ----------------------------------------


pointer_t* s_malloc( unsigned int size, unsigned int duration ) {


 // TODO: COMPLETE YOUR CODE HERE.
 pointer_t* p = NULL;


 // allocate and map the block, and check for errors


 // first map the block
 int vpn = map(size);


 if (vpn == PAGE_ERROR) {
   simulator.page_error += 1;
   return NULL;
 }


 // do this second as it evicts pages from memory, which may be needed for the block to be mapped
 int pfn = allocate_with_swap(size);


 if (pfn == FRAME_ERROR) {
     simulator.frame_error += 1;
     unmap(vpn, size);
     return NULL;
 }


 // set the page table entries and memory frames for the allocated block
 for(int i = 0; i < size; i++) {
   int vpn_offset = vpn + i;
   int pfn_offset = pfn + i;


   mm_struct.page_table[vpn_offset]->present = 1;
   mm_struct.page_table[vpn_offset]->pfn = pfn_offset;
   memmap.frames[pfn_offset] = mm_struct.page_table[vpn_offset];


   // set allocated bit for the page table entry
   mm_struct.page_table[vpn_offset]->allocated = 1;
   mm_struct.page_table[vpn_offset]->swapped = 0;
   mm_struct.page_table[vpn_offset]->swap_slot = -1;


   // enqueue the vpn for FIFO page replacement
   fifo_push(vpn_offset);
 }


 // init pointer struct for the allocated block
 p = (pointer_t*)malloc(sizeof(pointer_t));


 if (p == NULL) {
   unallocate(pfn, size);
   unmap(vpn, size);
   return NULL;
 }


 // set fields of pointer struct
 p->vpn = vpn;
 p->size = size;
 p->duration = duration;


 return p;
} // end s_malloc() function




void s_free( pointer_t* p ) {


 // TODO: COMPLETE YOUR CODE HERE.
 if (p == NULL) {
     return;
 }


 // get the vpn, size, and pfn of the block to be freed
 int vpn = p->vpn;
 int size = p->size;


 // free the block by unallocating the frames and unmapping the pages
 for(int i = 0; i < size; i++) {
   int vpn_offset = vpn + i;


   // get the page table entry for the page to be freed
   page_t* page = mm_struct.page_table[vpn_offset];


   // if the page is swapped, free the swap slot
   if ((page->swapped == 1) && (page->swap_slot != -1)) {
     int swap_slot = page->swap_slot;
     memmap.swap_table[swap_slot] = -1;
   }


   if (page->present == 1) {
     memmap.frames[page->pfn] = NULL;
   }


   // reset the page table entry
   page->present = 0;
   page->allocated = 0;
   page->swapped = 0;
   page->swap_slot = -1;
   page->pfn = 0;
 }
  free(p);
  p = NULL;
} // end s_free() function




int allocate( unsigned int size ) {
 // TODO: COMPLETE YOUR CODE HERE.


 int pfn = FRAME_ERROR;


 // go through each placement algorithm and return the pfn of the allocated block if found
 if (simulator.placement_algorithm == FIRST) {
     pfn = first_fit(size);
 } else if (simulator.placement_algorithm == BEST) {
     pfn = best_fit(size);
 } else if (simulator.placement_algorithm == NEXT) {
     pfn = next_fit(memmap.last_placement_frame, size);
     if (pfn != FRAME_ERROR) {
         memmap.last_placement_frame = (pfn + size) % memmap.size;
     }
 }


 return pfn;
}




void unallocate( unsigned int pfn, unsigned int size ) {
 for ( unsigned int i=0; i<size; i++ ) {
   if (pfn + i < memmap.size) {
       memmap.frames[pfn + i] = NULL;
   }
 }


} // end unallocate() function




int map( unsigned int length) {


 // TODO: COMPLETE YOUR CODE HERE.


 if (length == 0 || length > mm_struct.num_pages) {
     return PAGE_ERROR;
 }


 // store the best index and length of the best fit
 int cur_dist = 0;
 int last_saved_index = -1;


 // iterate through page table
 for(int i = 0; i < mm_struct.num_pages; i++) {


     // case 1: page is not allocated
     if (mm_struct.page_table[i]->allocated == 0) {
        
         // save index
         if (last_saved_index == -1) {
             last_saved_index = i;
         }


         // increment current distance
         cur_dist += 1;


         // found a fit -> return immediately
         if (cur_dist >= length) {
             return last_saved_index;
         }
     } else {
         // case 2: page is allocated -> reset current distance and index
         last_saved_index = -1;
         cur_dist = 0;
     }
 }


 return PAGE_ERROR;


} // end map() function


void unmap( unsigned int vpn, unsigned int size ) {


 // TODO: COMPLETE YOUR CODE HERE.
 // loop through entire size of the block to be unmapped
 for(unsigned int i = 0; i < size; i++) {
   if (vpn + i < mm_struct.num_pages) {
       // reset the page table entry for the page to be unmapped
       mm_struct.page_table[vpn + i]->allocated = 0;
       mm_struct.page_table[vpn + i]->present = 0;
       mm_struct.page_table[vpn + i]->swapped = 0;
       mm_struct.page_table[vpn + i]->swap_slot = -1;
       mm_struct.page_table[vpn + i]->pfn = 0;
   }
 }


} // end unmap() function




int first_fit( unsigned int length ) {


 // TODO: COMPLETE YOUR CODE HERE.
 if (memmap.size == 0 || length == 0) {
     return FRAME_ERROR;
 }


 // store the best index and length of the best fit
 int cur_dist = 0;
 int last_saved_index = -1;


 // loop through memory frames
 for(int i = 0; i < memmap.size; i++) {
     // increment probe count for each frame checked
     simulator.num_probes += 1;
   
     // case 1: frame is free
     if (memmap.frames[i] == NULL) {


         // save index
         if (last_saved_index == -1) {
             last_saved_index = i;
         }


         // increment current distance
         cur_dist += 1;


         // found a fit -> return immediately
         if (cur_dist >= length) {
             return last_saved_index;
         }


     } else {
         // case 2: frame is occupied -> reset current distance and index
         last_saved_index = -1;
         cur_dist = 0;
     }
 }


 return FRAME_ERROR; // temporary placeholder.


} // end first_fit() function


int next_fit( int start_frame, unsigned int length ) {


 // TODO: COMPLETE YOUR CODE HERE.
 if (length == 0 || memmap.size == 0) {
   return FRAME_ERROR;
 }


 // if start frame is out of bounds, reset to 0
 if (start_frame < 0 || start_frame >= memmap.size) {
   start_frame = 0;
 }


 // store the best index and length of the best fit
 int cur_len = 0;
 int cur_index = -1;


 // loop through memory frames starting from start_frame
 for(int i = start_frame; i < memmap.size; i++) {


     // increment probe count for each frame checked
     simulator.num_probes += 1;


     // case 1: frame is free
     if (memmap.frames[i] == NULL) {


         // save index
         if (cur_index == -1) {
             cur_index = i;
         }


         // increment current length
         cur_len += 1;


         // found a fit -> return immediately
         if (cur_len >= length) {
             return cur_index;
         }
     } else {
         // case 2: frame is occupied -> reset current length and index
         cur_len = 0;
         cur_index = -1;
     }
 }


 // reset current length and index for second loop
 cur_len = 0;
 cur_index = -1;


 // loop through memory frames starting from the beginning to start_frame
 for(int i = 0; i < start_frame; i++) {


     // increment probe count for each frame checked
     simulator.num_probes += 1;


     // case 1: frame is free
     if (memmap.frames[i] == NULL) {


         // save index
         if (cur_index == -1) {
             cur_index = i;
         }


         // increment current length
         cur_len += 1;


         // found a fit -> return immediately
         if (cur_len >= length) {
             return cur_index;
         }
     } else {
         // case 2: frame is occupied -> reset current length and index
         cur_len = 0;
         cur_index = -1;
     }
 }


 return FRAME_ERROR; // temporary placeholder.


} // end next_fit() function




int best_fit( unsigned int length ) {


 // TODO: COMPLETE YOUR CODE HERE.
 if (memmap.size == 0 || length == 0) {
     return FRAME_ERROR;
 }


 // store the best index and length of the best fit
 // with additional variable to track current length of free block
 int curr_best = memmap.size + 1;
 int curr_len = 0;


 int best_index = -1;
 int curr_index = -1;


 // loop through memory frames
 for(int i = 0; i < memmap.size; i++) {


     // increment probe count for each frame checked
     simulator.num_probes += 1;
    
     // case 1: frame is free
     if (memmap.frames[i] == NULL) {
        
         // save index
         if (curr_index == -1) {
             curr_index = i;
         }


         // increment current length
         curr_len += 1;


     } else {
         // case 2: frame is occupied -> check if current block is best fit
         // then reset current length and index


         // check if current block is large enough and smaller than current best
         if ((curr_len >= length) && (curr_best > curr_len)) {
             // set current block as best fit
             curr_best = curr_len;
             best_index = curr_index;


             // found a perfect fit -> return immediately
             if (curr_best == length) {
                 return best_index;
             }
         }
        
         // reset current length and index
         curr_len = 0;
         curr_index = -1;
     }
 }


 // for the case where the best fit block is at the end of memory and loop ends
 if ((curr_len >= length) && (curr_best > curr_len)) {
     curr_best = curr_len;
     best_index = curr_index;


     if (curr_best == length) {
         return best_index;
     }
 }


 // if a best fit block was found, return its index
 if (best_index != -1) {
     return best_index;
 }


 return FRAME_ERROR;


} // end best_fit() function


// honors add-on
int allocate_with_swap( unsigned int size) {
 if (size == 0 || size > memmap.size) {
     return FRAME_ERROR;
 }


 // loop to evict pages and free frames until a block can be allocated
 while(1) {
   int pfn = allocate(size);


   if (pfn != FRAME_ERROR) {
       return pfn;
   }


   if (!evict_one_fifo()) {
       return FRAME_ERROR;
   }
 }
}


void fifo_push(int vpn) {
 clean_queue(); // clean the FIFO queue of any invalid entries before pushing a new vpn


 if (memmap.fifo_count == memmap.size) {
   return;
 }


 // enqueue the vpn in the FIFO queue for page replacement
 memmap.fifo_queue[memmap.fifo_tail] = vpn;
 memmap.fifo_tail = (memmap.fifo_tail + 1) % memmap.size;
 memmap.fifo_count += 1;
}


int fifo_pop() {
 if (memmap.fifo_count == 0) {
   return -1;
 }


 // dequeue the vpn from the FIFO queue for page replacement
 int vpn = memmap.fifo_queue[memmap.fifo_head];
 memmap.fifo_head = (memmap.fifo_head + 1) % memmap.size;
 memmap.fifo_count -= 1;


 return vpn;
}


int alloc_swap_slot() {
 // find the first free slot in the swap table and return its index
 for(unsigned int i = 0; i < memmap.swap_size; i++) {
   if (memmap.swap_table[i] == -1) {
     return i;
   }
 }


 return -1;
}


int evict_one_fifo() {
 // get a free swap slot to evict a page to
 int slot = alloc_swap_slot();


 if (slot == -1) {
   return 0;
 }


 // loop to find a page to evict from memory and swap it out
 while(memmap.fifo_count > 0) {
     int vpn = fifo_pop();


     if (vpn < 0 || vpn >= mm_struct.num_pages) {
         continue;
     }


     page_t* page = mm_struct.page_table[vpn];


     if (!page->allocated || !page->present) {
         continue;
     }


     memmap.frames[page->pfn] = NULL;
     page->present = 0;
     page->swapped = 1;
     page->swap_slot = slot;
     memmap.swap_table[slot] = vpn;


     page->pfn = 0;
     return 1;
   }


 return 0;
}




void clean_queue() {
 if (memmap.fifo_count == 0) {
   return;
 }


 // create new queue
 int* new_queue = (int*)malloc(sizeof(int) * memmap.size);


 if (new_queue == NULL) {
   return;
 }


 // track variables that helps structure queue
 int kept = 0;
 int index = memmap.fifo_head;


 for(int i = 0; i < memmap.fifo_count; i++) {


   // get vpn of the current page
   int vpn = memmap.fifo_queue[index];
   index = (index + 1) % memmap.size;


   // invalid entry -> don't include
   if (vpn < 0 || vpn >= mm_struct.num_pages) {
     continue;
   }


   // get page corresponding to vpn
   page_t* page = mm_struct.page_table[vpn];


   // valid entry -> include
   if (page->allocated && page->present) {
     new_queue[kept] = vpn;
     kept += 1;
   }
 }


 // free former queue and set pointer to new queue
 free(memmap.fifo_queue);
 memmap.fifo_queue = new_queue;
 memmap.fifo_head = 0;
 memmap.fifo_tail = kept % memmap.size;
 memmap.fifo_count = kept;
}


void debug_page( unsigned int vpn, unsigned int size ) {


 if ( mm_struct.page_table != NULL ) {


   printf("\n---------------------------\n");
   printf("    PAGE TABLE\n");
   printf("---------------------------\n");
   printf("VPN\tPFN\tPresent\n" );
   printf("---------------------------\n");


   for ( unsigned int i=vpn; i<vpn+size; i++ ) {
     if ( mm_struct.page_table[i] != NULL ) {
       printf("%d\t%d\t%d\n", i, mm_struct.page_table[i]->pfn,
                                 mm_struct.page_table[i]->present );
     }
   }


 }


} // end debug_page() function


void debug_frame( unsigned int vpn, unsigned int size ) {


 printf("\n---------------------------\n");
 printf("      FRAMES\n");
 printf("---------------------------\n");
 printf("PFN\tVPN\tPresent\n" );
 printf("---------------------------\n");


 int pfn = 0;


 for ( unsigned int i=vpn; i<(vpn+size); i++ ) {
   if ( mm_struct.page_table[i] != NULL ) {
     pfn = mm_struct.page_table[i]->pfn;
     printf("%d\t%d\t%d\n", pfn, i, memmap.frames[pfn]->present );
   }
 }


} // end debug_frame() function
