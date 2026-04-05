#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sim.h"


// ----------------------------------
// You may add helper functions here
// -----------------------------------












// End helper function definitions




typedef int (*function_ptr)();


typedef struct {
   char *name;
   function_ptr fn;
} fn_table_entry_t;




// ----------------------------------
// Define test cases here.
// Three empty templates are provided.
// ----------------------------------




int tc1() {
   // tests for success
   int res = PASS;


   init_simulation(FIRST, 16, 16, 1, 1);
   pointer_t* p = s_malloc(3, 10);




   // test for cases
   if (p == NULL) {
       res = FAIL;
       clean_simulation();
       return res;
   }


   if ((p->vpn != 0) || (p->size != 3) || (p->duration != 10)) {
       res = FAIL;
   }




   // ensure page table entries and frames are correctly allocated
   for(int i = 0; i < 3; i++) {
       if ((mm_struct.page_table[p->vpn + i]->present != 1) ||
          (mm_struct.page_table[p->vpn + i]->pfn != i) ||
          (memmap.frames[i] != mm_struct.page_table[p->vpn + i])) {
           res = FAIL;
           break;
       }
   }


   s_free(p);
   clean_simulation();


   return res;
}


int tc2() {
   // tests for unallocation
   int res = PASS;


   init_simulation(FIRST, 16, 16, 1, 1);
   pointer_t* p = s_malloc(3, 10);


   // test for cases
   if (p == NULL) {
       clean_simulation();
       return FAIL;
   }


   if ((p->vpn != 0) || (p->size != 3) || (p->duration != 10)) {
       clean_simulation();
       return FAIL;
   }


   int vpn = p->vpn;


   s_free(p);


   // check that page table entries and frames after unallocation is perfomed
   for(int i = 0; i < 3; i++) {
       if ((mm_struct.page_table[vpn + i]->present != 0) ||
          (memmap.frames[vpn + i] != NULL)) {
           res = FAIL;
           break;
       }
   }
  
   clean_simulation();


   return res;
}


int tc3() {
   // tests for partial failure
   int res = PASS;


   init_simulation(FIRST, 4, 16, 1, 1);
   pointer_t* p = s_malloc(5, 10);


   // test for cases
   if (p != NULL) {
       res = FAIL;
   }


   if ((simulator.frame_error != 1) || (simulator.page_error != 0)) {
       res = FAIL;
   }


   // verify table page entries are still free
   for(int i = 0; i < 5; i++) {
       if ((mm_struct.page_table[i]->present != 0) ||
          (mm_struct.page_table[i]->pfn != 0)) {
           res = FAIL;
           break;
       }
   }


   // check frames are still free
   for(int i = 0; i < 4; i++) {
       if (memmap.frames[i] != NULL) {
           res = FAIL;
           break;
       }
   }


   clean_simulation();


   return res;
}




int tc4() {
   // tests for page-fail
   int res = PASS;


   init_simulation(FIRST, 16, 4, 1, 1);


   pointer_t* p = s_malloc(5, 10);


   // test for cases
   if (p != NULL) {
       res = FAIL;
   }


   if ((simulator.frame_error != 0) || (simulator.page_error != 1)) {
       res = FAIL;
   }


   // verify table page entries are still free
   for(int i = 0; i < 4; i++) {
       if ((mm_struct.page_table[i]->present != 0) ||
          (mm_struct.page_table[i]->pfn != 0)) {
           res = FAIL;
           break;
       }
   }


   // check frames are still free
   for(int i = 0; i < 16; i++) {
       if (memmap.frames[i] != NULL) {
           res = FAIL;
           break;
       }
   }


   clean_simulation();


   return res;
}


int tc5() {
   // tests first fit algorithm and expected probe count
   int res = PASS;


   init_simulation(FIRST, 8, 8, 1, 1);


   // set up memory frames with a free block in the middle
   memmap.frames[0] = mm_struct.page_table[0];
   memmap.frames[1] = mm_struct.page_table[1];
   memmap.frames[2] = NULL;
   memmap.frames[3] = NULL;
   memmap.frames[4] = NULL;
   memmap.frames[5] = mm_struct.page_table[5];
   memmap.frames[6] = NULL;
   memmap.frames[7] = NULL;


   first_fit(3);


   if (simulator.num_probes != 5) {
       res = FAIL;
   }


   clean_simulation();


   return res;
}




int tc6() {
   // tests for best fit choosing the smallest block and expected probe count
   int res = PASS;


   init_simulation(BEST, 12, 12, 1, 1);




   // set up memory frames with free blocks of size 3 and 4
   memmap.frames[0] = mm_struct.page_table[0];
   memmap.frames[1] = NULL;
   memmap.frames[2] = NULL;
   memmap.frames[3] = NULL;
   memmap.frames[4] = mm_struct.page_table[4];
   memmap.frames[5] = NULL;
   memmap.frames[6] = NULL;
   memmap.frames[7] = NULL;
   memmap.frames[8] = NULL;
   memmap.frames[9] = mm_struct.page_table[9];
   memmap.frames[10] = mm_struct.page_table[10];
   memmap.frames[11] = mm_struct.page_table[11];


   simulator.num_probes = 0;


   // perform best fit
   int pfn = best_fit(2);


   // check that the block found is at the expected index
   if (pfn != 1) {
       res = FAIL;
   }


   // check probe count
   if (simulator.num_probes != 12) {
       res = FAIL;
   }


   clean_simulation();
   return res;
}




int tc7() {
   // tests for next-fit alg + wrap around
   int res = PASS;


   init_simulation(NEXT, 10, 10, 1, 1);


   // set up memory frames with a free block of size 4 at the end and a free block of size 3 at the beginning
   memmap.frames[0] = mm_struct.page_table[0];
   memmap.frames[1] = mm_struct.page_table[1];
   memmap.frames[2] = NULL;
   memmap.frames[3] = NULL;
   memmap.frames[4] = NULL;
   memmap.frames[5] = mm_struct.page_table[5];
   memmap.frames[6] = NULL;
   memmap.frames[7] = NULL;
   memmap.frames[8] = NULL;
   memmap.frames[9] = NULL;


   simulator.num_probes = 0;
  
   // perform next fit starting from index 6, should wrap around and find the block at index 0
   int pfn = next_fit(6, 3);


   // check that the block found is correct
   if (pfn != 6) {
       res = FAIL;
   }


   // check probe count
   if (simulator.num_probes != 3) {
       res = FAIL;
   }


   clean_simulation();


   return res;
}


int tc8() {
   // checks if s_free correctly unallocates frames and updates page table entries
   int res = PASS;


   init_simulation(FIRST, 16, 16, 1, 1);
   pointer_t* p = s_malloc(3, 10);


   if (p == NULL) {
       res = FAIL;
       clean_simulation();
       return res;
   }


   // get the vpn of the allocated block to check page table entries and frames after freeing
   int vpn = p->vpn;
   s_free(p);




   // check that page table entries and frames after unallocation is perfomed
   for(int i = 0; i < 3; i++) {
       if (mm_struct.page_table[vpn + i]->present != 0 ||
          memmap.frames[vpn + i] != NULL) {
           res = FAIL;
           break;
       }
   }


   clean_simulation();
   return res;
}


int tc9() {
   // checks duration timing in update_simulation
   int res = PASS;


   init_simulation(FIRST, 16, 16, 1, 1);
   pointer_t* p = s_malloc(1, 2);


   if (p == NULL) {
       res = FAIL;
       clean_simulation();
       return res;
   }


   // call update_simulation twice, which should decrement duration to 0 and free the block
   update_simulation(p);
   update_simulation(NULL);




   // check that the block has been freed in the simulation table
   if (simulator.ptable[0] != NULL) {
       res = FAIL;
   }


   clean_simulation();
   return res;
}




int tc10() {
   // checks map using first-fit alg
   int res = PASS;


   init_simulation(FIRST, 8, 8, 1, 1);


   // set up memory with allocated pages at index 0 and 1
   mm_struct.page_table[0]->allocated = 1;
   mm_struct.page_table[1]->allocated = 1;


   // call map to find a free block of size 3, which should be at index 2
   int vpn = map(3);


   if (vpn != 2) {
       res = FAIL;
   }


   clean_simulation();
   return res;
}




int tc11() {
   // verify fragment count
   int res = PASS;


   init_simulation(FIRST, 10, 10, 1, 1);


   // set up memory frames with allocated pages to create fragments of size 2
   memmap.frames[0] = mm_struct.page_table[0];
   memmap.frames[1] = NULL;
   memmap.frames[2] = NULL;
   memmap.frames[3] = mm_struct.page_table[3];
   memmap.frames[4] = NULL;
   memmap.frames[5] = mm_struct.page_table[5];
   memmap.frames[6] = NULL;
   memmap.frames[7] = NULL;
   memmap.frames[8] = NULL;
   memmap.frames[9] = mm_struct.page_table[9];


   // check that the fragment count for size 2 is 1
   int frag_count = stats_fragments(2);
   if (frag_count != 1) {
       res = FAIL;
   }


   clean_simulation();
   return res;
}




int tc12() {
   // page-side failure test
   int res = PASS;


   init_simulation(FIRST, 8, 8, 1, 1);


   // malloc with size larger than vm size to trigger page error
   pointer_t* p = s_malloc(9, 5);


   // test for cases
   if (p != NULL) {
       res = FAIL;
   }
   if ((simulator.page_error != 1) || (simulator.frame_error != 0)) {
       res = FAIL;
   }


   clean_simulation();


   return res;
}




int tc13() {
   // perform impossible virtual mem allocation
   int res = PASS;


   init_simulation(FIRST, 16, 8, 1, 1);


   pointer_t* p = s_malloc(9, 5);


   // test for cases
   if (p != NULL) {
       res = FAIL;
   }
   if (simulator.page_error != 1) {
       res = FAIL;
   }
   if (simulator.frame_error != 0) {
       res = FAIL;
   }


   clean_simulation();
   return res;
}


// additional test cases for swap testing (honors section)


int tc14() {
   // single evication (mm < vm)
   int res = PASS;


   init_simulation(FIRST, 4, 8, 1, 1);


   // allocate 4 pages to fill up physical memory
   pointer_t* a = s_malloc(1, 10); // vpn 0, pfn 0
   pointer_t* b = s_malloc(1, 10); // vpn 1, pfn 1
   pointer_t* c = s_malloc(1, 10); // vpn 2, pfn 2
   pointer_t* d = s_malloc(1, 10); // vpn 3, pfn 3


   if (a == NULL || b == NULL || c == NULL || d == NULL) {
       res = FAIL;
       clean_simulation();
       return res;
   }


   pointer_t* e = s_malloc(1, 10); // should trigger eviction


   if (e == NULL) {
       res = FAIL;
       clean_simulation();
       return res;
   }


   // check that the evicted page is marked as swapped in the page table and has a swap slot assigned
   int swap_count = 0;
   for(int i = 0; i < 8; i++) {
       if (mm_struct.page_table[i]->swapped == 1) {
           swap_count++;
       }
   }


   if (swap_count < 1) {
       res = FAIL;
   }


   // check that the new page is allocated in physical memory
   int swap_flag = 0;
   for(int i = 0; i < 8; i++) {
       if (mm_struct.page_table[i]->swapped == 1 && mm_struct.page_table[i]->swap_slot != -1) {
           swap_flag = 1;
           break;
       }
   }


   if (swap_flag != 1) {
       res = FAIL;
   }


   s_free(a);
   s_free(b);
   s_free(c);
   s_free(d);
   s_free(e);


   clean_simulation();
   return res;
}




int tc15() {
   // multiple evictions
   int res = PASS;


   init_simulation(FIRST, 4, 8, 1, 1);


   pointer_t* p[8];


   // allocate 8 pages to fill up physical memory and trigger evictions after the 4th allocation
   for(int i = 0; i < 8; i++) {
       p[i] = s_malloc(1, 10);
   }




   // check that at least 4 pages are evicted and swapped out to make room for the new allocations
   int exist_flag = 1;
   for(int i = 0; i < 8; i++) {
       if (p[i] == NULL) {
           exist_flag = 0;
           break;
       }
   }


   // from above code at least 4 pages should be swapped
   int swap_count = 0;
   for(int i = 0; i < 8; i++) {
       if (mm_struct.page_table[i]->swapped == 1) {
           swap_count++;
       }
   }


   if (swap_count < 4) {
       res = FAIL;
   }


   for(int i = 0; i < 8; i++) {
       if (p[i] != NULL) {
           s_free(p[i]);
       }
   }


   if (!exist_flag) {
       res = FAIL;
   }


   clean_simulation();
   return res;
}




int tc16() {
   // tests no swap available
   int res = PASS;


   init_simulation(FIRST, 4, 8, 1, 1);


   // allocate 4 pages to fill up physical memory
   pointer_t* a = s_malloc(1, 10); // vpn 0, pfn 0
   pointer_t* b = s_malloc(1, 10); // vpn 1, pfn 1
   pointer_t* c = s_malloc(1, 10); // vpn 2, pfn 2
   pointer_t* d = s_malloc(1, 10); // vpn 3, pfn 3


   if (a == NULL || b == NULL || c == NULL || d == NULL) {
       res = FAIL;
       clean_simulation();
       return res;
   }


   for(int i = 0; i < memmap.swap_size; i++) {
       memmap.swap_table[i] = i; // fill up swap slots
   }


   // should trigger eviction but no swap slots available, so eviction should fail and the page should not be allocated
   int ev_flag = evict_one_fifo();


   if (ev_flag != 0) {
       res = FAIL;
   }


   s_free(a);
   s_free(b);
   s_free(c);
   s_free(d);


   clean_simulation();
   return res;
}




int tc17() {
   // stale swap slot
   int res = PASS;


   init_simulation(FIRST, 4, 8, 1, 10);


   // allocate 4 pages to fill up physical memory
   pointer_t* a = s_malloc(1, 10); // vpn 0, pfn 0
   pointer_t* b = s_malloc(1, 10); // vpn 1, pfn 1
   pointer_t* c = s_malloc(1, 10); // vpn 2, pfn 2
   pointer_t* d = s_malloc(1, 10); // vpn 3, pfn 3


   if (a == NULL || b == NULL || c == NULL || d == NULL) {
       res = FAIL;
       clean_simulation();
       return res;
   }


   // FIFO should be [0, 1, 2, 3]


   s_free(a); // vpn 0 becomes stale in queue


   if (evict_one_fifo() == 0) { // should evict vpn 1 to slot 0
       res = FAIL;
       clean_simulation();
       return res;
   }




   // check that vpn 1 is swapped out and assigned a swap slot, and that vpn 0 is not swapped out since it was stale in the queue
   int present_count = 0;
   for(int i = 0; i < 8; i++) {
       if (mm_struct.page_table[i]->present == 1) {
           present_count++;
       }
   }


   if ((present_count > 3) ||
      (mm_struct.page_table[1]->present != 0) ||
      (mm_struct.page_table[1]->swapped != 1)) {
       res = FAIL;
   }


   s_free(b);
   s_free(c);
   s_free(d);


   clean_simulation();
   return res;
}




int tc18() {
   // stale queue saturation
   int res = PASS;


   init_simulation(FIRST, 4, 8, 1, 10);


   // allocate 4 pages to fill up physical memory
   pointer_t* old[4] = {NULL, NULL, NULL, NULL};
   pointer_t* new[4] = {NULL, NULL, NULL, NULL};


   // fill memory and FIFO with vpn 0-3
   for(int i = 0; i < 4; i++) {
       old[i] = s_malloc(1, 10);
       if (old[i] == NULL) {
           res = FAIL;
           clean_simulation();
           return res;
       }
   }


   // free all stale entries
   for(int i = 0; i < 4; i++) {
       s_free(old[i]);
       old[i] = NULL;
   }




   // reallocate 4 pages
   for(int i = 0; i < 4; i++) {
       new[i] = s_malloc(1, 10);
       if (new[i] == NULL) {
           res = FAIL;
           clean_simulation();
           return res;
       }
   }


   // force eviction case
   pointer_t* extra = s_malloc(1, 10);


   if (extra == NULL) {
       res = FAIL;
   }


   // from above code at least one page should be swapped
   int swap_count = 0;


   for(int i = 0; i < 8; i++) {
       if (mm_struct.page_table[i]->swapped == 1) {
           swap_count++;
       }
   }


   if (swap_count < 1) {
       res = FAIL;
   }


   // loop through each existing element and remove them
   for (int i = 0; i < 4; i++) {
       if (new[i] != NULL) {
           s_free(new[i]);
           new[i] = NULL;
       }
   }


   if (extra != NULL) {
       s_free(extra);
       extra = NULL;
   }


   clean_simulation();
   return res;
}




// End test case function definitions


// ----------------------------------
// Every new test case function needs
// to be defined the function table.
// Update as needed.
// ----------------------------------


fn_table_entry_t fn_table[] = {
   {"tc1", tc1},
   {"tc2", tc2},
   {"tc3", tc3},
   {"tc4", tc4},
   {"tc5", tc5},
   {"tc6", tc6},
   {"tc7", tc7},
   {"tc8", tc8},
   {"tc9", tc9},
   {"tc10", tc10},
   {"tc11", tc11},
   {"tc12", tc12},
   {"tc13", tc13},
   {"tc14", tc14},
   {"tc15", tc15},
   {"tc16", tc16},
   {"tc17", tc17},
   {"tc18", tc18},
   {NULL, NULL} // mark the end
};


// End function table definitions


function_ptr lookup_function(const char *fn_name) {
   if (!fn_name) {
       return NULL;
   }
   for (int i = 0; fn_table[i].name != NULL; ++i) {
       if (!strcmp(fn_name, fn_table[i].name)) {
           return fn_table[i].fn;
       }
   }
   return NULL; // testcase not found
}


int main( int argc, char** argv ) {


   int rv = FAIL;
  
   if ( argc != 2 ) {
       printf("------------------------\n");
       printf("Test case program\n");
       printf("------------------------\n");
       printf("Usage: ./testcase <testcase name>\n\n");
       printf("Example: ./testcase tc1\n\n");
       return 0;
   }


   function_ptr func = lookup_function( argv[1] );
   if (func != NULL) {
       rv = func();
   } else {
       printf("Testcase (%s) not defined!\n", argv[1] );
   }


   return rv;
}





