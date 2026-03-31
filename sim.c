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

} // end upate_simulation() function


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

    pointer_t* p = NULL;

    int pfn = allocate(size);

    if (pfn == FRAME_ERROR) {
        simulator.frame_error += 1;
        return NULL;
    }

    int vpn = map(size);

    if (vpn == PAGE_ERROR){
        simulator.page_error += 1;
        unallocate(pfn, size);
        return NULL; 
    }

    p = (pointer_t*)malloc(sizeof(pointer_t));

    if (p == NULL){
        unallocate(pfn, size);
        unmap(vpn, size);
        return NULL;
    }
    
    for (unsigned int i = 0; i < size; i++){
        if (pfn + i < memmap.size && vpn + i < mm_struct.num_pages) {
            mm_struct.page_table[vpn + i]->present = 1;
            mm_struct.page_table[vpn + i]->pfn = pfn + i;
            memmap.frames[pfn + i] = mm_struct.page_table[vpn + i];
    
        }
    }

    p->size = size;
    p->vpn = vpn;
    p->duration = duration;


    return p;



  // TODO: COMPLETE YOUR CODE HERE.
  
 // temporary placeholder.

} // end s_malloc() function


void s_free( pointer_t* p ) {



    if (p == NULL){
        return;
    }

    unsigned int vpn = p->vpn;
    unsigned int size = p->size;
    unsigned int pfn = mm_struct.page_table[vpn]->pfn;


    unallocate(pfn, size);
    unmap(vpn, size);

    //free(p);
  // TODO: COMPLETE YOUR CODE HERE.



} // end s_free() function

int allocate( unsigned int size ) {
    
    int pfn = FRAME_ERROR;

    if (simulator.placement_algorithm == FIRST){
        pfn = first_fit(size);
    } else if (simulator.placement_algorithm == BEST) {
        pfn = best_fit(size);
    } else if (simulator.placement_algorithm == NEXT){
        pfn = next_fit(memmap.last_placement_frame, size);
    }



  // TODO: COMPLETE YOUR CODE HERE.

    return pfn;

   // temporary placeholder.


} // end allocate() function


void unallocate( unsigned int pfn, unsigned int size ) {

    for (unsigned int i = 0; i < size; i++){
        if (pfn + i < memmap.size){
            memmap.frames[pfn + i] = NULL;
        }
    }
    
    // TODO: COMPLETE YOUR CODE HERE.



} // end unallocate() function


int map( unsigned int length) {



    if (length == 0 || length > mm_struct.num_pages) {
        return PAGE_ERROR;
    }

    int cur_len = 0;
    int start = -1;

    for (unsigned int i = 0; i < mm_struct.num_pages; i++){
        if (mm_struct.page_table[i]->present == 0){
            if (start == -1){
                start = i;
            }

            cur_len++;

            if (cur_len >= length){ 
                return start;
            }
        } else {
            start = -1;
            cur_len = 0;
        }

    }

  // TODO: COMPLETE YOUR CODE HERE.



  return PAGE_ERROR; // temporary placeholder.

} // end map() function

void unmap( unsigned int vpn, unsigned int size ) {


    for (unsigned int i = 0; i < size; i++){
        if (vpn + i < mm_struct.num_pages) { 
            mm_struct.page_table[vpn + i]->present = 0;
            mm_struct.page_table[vpn + i]->pfn = 0;
        }
    }



  // TODO: COMPLETE YOUR CODE HERE.



} // end unmap() function


int first_fit( unsigned int length ) {


    if (length == 0 || length > memmap.size){
        return FRAME_ERROR;
    }

    int cur_len = 0;
    int start = -1; 

    for (unsigned int i = 0; i < memmap.size; i++){
        simulator.num_probes += 1;

        if (memmap.frames[i] == NULL){
            if (start == -1){
                start = i;
            }
            
            cur_len++;

            if (cur_len >= length){
                return start;
            }
        } else {
            start =- 1;
            cur_len = 0; 
        }
    }

  // TODO: COMPLETE YOUR CODE HERE.



  return FRAME_ERROR; // temporary placeholder.

} // end first_fit() function

int next_fit( int start_frame, unsigned int length ) {

    if (length == 0 || length > memmap.size){
        return FRAME_ERROR;
    }

    int cur_len = 0;
    int start = -1;
    int count = 0;

    int i = (start_frame >= 0) ? start_frame : 0;

    while (count < memmap.size){
        simulator.num_probes += 1;
    

        if (memmap.frames[i] == NULL) {
            if (start == -1) {
                start = i;
            }

            cur_len++;

            if (cur_len >= length) {
                memmap.last_placement_frame = i;
                return start;
            }
        } else {
            start = -1;
            cur_len = 0;
        }

        i = (i + 1) % memmap.size;
        count++;

    }

  return FRAME_ERROR; // temporary placeholder.

} // end next_fit() function


int best_fit( unsigned int length ) {
    
    if (length == 0 || length > memmap.size) {
        return FRAME_ERROR;
    }

    int best_start = -1;
    int best_size = memmap.size + 1;

    int cur_start = -1;
    int cur_len = 0;

    for (unsigned int i = 0; i < memmap.size; i++) {
        simulator.num_probes += 1;

        if (memmap.frames[i] == NULL) {
            if (cur_start == -1) {
                cur_start = i;
            }
            cur_len++;
        } else {
            if (cur_len >= (int)length && cur_len < best_size) {
                best_size = cur_len;
                best_start = cur_start;
            }
            cur_start = -1;
            cur_len = 0;
        }
    }

    if (cur_len >= (int)length && cur_len < best_size) {
        best_size = cur_len;
        best_start = cur_start;
    }

    return (best_start == -1) ? FRAME_ERROR : best_start;




} // end best_fit() function



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

