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

  // TODO: COMPLETE YOUR CODE HERE.


  return NULL; // temporary placeholder.

} // end s_malloc() function


void s_free( pointer_t* p ) {

  // TODO: COMPLETE YOUR CODE HERE.



} // end s_free() function

int allocate( unsigned int size ) {

  // TODO: COMPLETE YOUR CODE HERE.



  return FRAME_ERROR; // temporary placeholder.


} // end allocate() function


void unallocate( unsigned int pfn, unsigned int size ) {

  // TODO: COMPLETE YOUR CODE HERE.



} // end unallocate() function


int map( unsigned int length) {

  // TODO: COMPLETE YOUR CODE HERE.



  return PAGE_ERROR; // temporary placeholder.

} // end map() function

void unmap( unsigned int vpn, unsigned int size ) {

  // TODO: COMPLETE YOUR CODE HERE.



} // end unmap() function


int first_fit( unsigned int length ) {

  // TODO: COMPLETE YOUR CODE HERE.



  return FRAME_ERROR; // temporary placeholder.

} // end first_fit() function

int next_fit( int start_frame, unsigned int length ) {

  // TODO: COMPLETE YOUR CODE HERE.



  return FRAME_ERROR; // temporary placeholder.

} // end next_fit() function


int best_fit( unsigned int length ) {

  // TODO: COMPLETE YOUR CODE HERE.

 

  return FRAME_ERROR; // temporary placeholder.

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

