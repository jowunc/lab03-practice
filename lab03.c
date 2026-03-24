#include <stdio.h>  
#include <stdlib.h> 
#include "sim.h"

/*
  The main program accepts five parameters from the command line:

  - The first parameter specifies the placement algorithm 
    (1 = first-fit, 2 = best-fit, 3 = next-fit).
  - The second parameter specifies the size of main memory 
    (i.e., the total number of physical addresses).
  - The third parameter specifies the size of virtual memory 
    (i.e., the total number of virtual addresses).
  - The fourth parameter specifies the simulation duration 
    (in time units).
  - The fifth parameter specifies the random number seed.

  Example command:

    ./lab03 1 512 512 200 1234

  This example runs the simulation using the first-fit algorithm,
  with 512 physical addresses, 512 virtual addresses, a duration
  of 200 time units, and a random seed of 1234.
*/
int main(int argc, char** argv) {
  
  if (argc != 6) {
    printf("USAGE: %s <algorithm> <mm_size> <vm_size> <sim_time> <rand_seed>\n", argv[0]);
    return 1;
  }

  init_simulation( atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[5]), atoi(argv[4]) );

  srand( simulator.seed );

  for ( int i=0; i<simulator.time_units; i++ ) {

    pointer_t* p = s_malloc( randnum( MAX_REQUEST_SIZE, MIN_REQUEST_SIZE ),
                             randnum( MAX_DURATION, MIN_DURATION ) );

    update_simulation( p );
    show_pointer_table();

    if ( p != NULL ) {

      // -------------------------------------------------
      // You may uncomment to debug your coding solution
      // -------------------------------------------------
      //debug_page( p->vpn, p->size );
      //debug_frame( p->vpn, p->size );
    }

  }

  printf("\n---------------------------------\n");
  printf("Simulation statistics\n");
  printf("---------------------------------\n");

  if (simulator.placement_algorithm == FIRST)
    printf("Algorithm = First-fit\n");
  else if (simulator.placement_algorithm == NEXT)
    printf("Algorithm = Next-fit\n");
  else
    printf("Algorithm = Best-fit\n");

  printf("Average probes = %.2f\n", stats_probe() );
  printf("Frame error rate = %.2f\n", stats_frame_error() );
  printf("Page error rate = %.2f\n", stats_page_error() );
  printf("Fragment count = %d\n", stats_fragments(MIN_REQUEST_SIZE));

  printf("---------------------------------\n");
  clean_simulation();

  return 0;
}
