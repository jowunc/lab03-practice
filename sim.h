
// -----------------------------------
//  COMP 530: Operating Systems
//
//  Spring 2026 - Lab 3
// -----------------------------------

// ----------------------------------------
// Global variables that CANNOT be modified
// ----------------------------------------

#define MIN_DURATION      5
#define MAX_DURATION      75

#define MIN_REQUEST_SIZE  5
#define MAX_REQUEST_SIZE  75

#define ERROR            -1
#define PAGE_ERROR       -1
#define FRAME_ERROR      -2

#define FIRST             1
#define BEST              2
#define NEXT              3

#define PASS              0
#define FAIL              1

// ----------------------------------------
// Global variables that CAN be modified
// ----------------------------------------

#define DEBUG             0


// ----------------------------------------
// Structs that CANNOT be modified
// ----------------------------------------
typedef struct {
  unsigned short pfn;       
  unsigned char present;    
} page_t;

typedef struct {            
  page_t** page_table; 
  unsigned int num_pages;               
} mm_struct_t;

typedef struct {
  page_t** frames;
  unsigned int size; 
  unsigned int last_placement_frame;                     
} memmap_t;

typedef struct {
  unsigned int vpn;
  unsigned int size;
  unsigned short duration; 
} pointer_t;

typedef struct {
  pointer_t** ptable;
  unsigned int table_size;
  unsigned int seed;
  unsigned int time_units;
  unsigned int placement_algorithm;
  unsigned int num_probes;
  unsigned int page_error;
  unsigned int frame_error;
} sim_t;

// ----------------------------------------
// Global variables that CANNOT be modified
// ----------------------------------------
extern mm_struct_t mm_struct;
extern memmap_t memmap;
extern sim_t simulator; 

// ----------------------------------------
// Functions that CANNOT be modified
// ----------------------------------------

/*
 * Initializes all data structures and variables required to run the
 * memory management simulation.
 *
 * Parameters:
 *   algorithm  - The placement algorithm to use (e.g., first-fit, best-fit, next-fit).
 *   mm_size    - The size of physical memory (number of frames).
 *   vm_size    - The size of virtual memory (number of pages).
 *   seed       - The random number seed for reproducibility.
 *   time_units - The total duration of the simulation.
 *
 * Returns:
 *   None.
 *
 * Description:
 *   This function initializes the simulator state, including allocation
 *   tracking, performance statistics, and memory structures.
 *
 *   - Allocates and initializes the pointer table used to track active allocations.
 *   - Sets simulation parameters such as the placement algorithm, seed, and duration.
 *   - Initializes counters for probes, page errors, and frame errors.
 *
 *   - Allocates and initializes the page table, setting all pages to not present.
 *   - Allocates and initializes the memory map, marking all frames as empty.
 *   - Initializes the last placement frame used by the next-fit algorithm.
 */
void init_simulation( unsigned int algorithm, unsigned int mm_size, unsigned int vm_size, unsigned int seed, unsigned int time_units );

/*
 * Frees all dynamically allocated memory used by the simulation.
 *
 * Parameters:
 *   None.
 *
 * Returns:
 *   None.
 *
 * Description:
 *   This function releases all memory allocated during the simulation,
 *   including the page table, pointer table, and memory map.
 *
 *   - Frees each page in the page table, then frees the page table itself.
 *   - Frees each active allocation stored in the pointer table, then frees the pointer table.
 *   - Frees the array of frames used by the memory map.
 *
 *   After this function completes, all simulation-related memory has been
 *   properly deallocated.
 */
void clean_simulation();

/*
 * Updates the state of the simulation for a single time unit by
 * tracking new allocations and decrementing the duration of
 * existing allocations.
 *
 * Parameters:
 *   ptr - A pointer to a newly allocated block (returned by s_malloc),
 *         or NULL if no new allocation occurred during this time step.
 *
 * Returns:
 *   None.
 *
 * Description:
 *   This function performs two main tasks:
 *
 *   - If a new allocation (ptr) is provided, it is inserted into the
 *     first available slot in the pointer table (ptable).
 *
 *   - Iterates through all active allocations in the pointer table:
 *       * Decrements the remaining duration of each allocation.
 *       * If an allocation’s duration reaches zero, it is freed using
 *         s_free and removed from the table.
 *
 *   This function simulates the passage of time by managing the
 *   lifetime of allocated memory blocks.
 */
void update_simulation( pointer_t* ptr );

/*
 * Computes the average number of probes per time unit during the simulation.
 *
 * Parameters:
 *   None.
 *
 * Returns:
 *   The average number of probes performed per time unit as a double.
 *
 * Description:
 *   This function divides the total number of probes recorded during
 *   the simulation by the total number of time units, providing a
 *   measure of the efficiency of the placement algorithm.
 */
double stats_probe();

/*
 * Computes the average number of frame allocation errors per time unit
 * during the simulation.
 *
 * Parameters:
 *   None.
 *
 * Returns:
 *   The average number of frame allocation errors per time unit as a double.
 *
 * Description:
 *   This function divides the total number of frame allocation errors
 *   recorded during the simulation by the total number of time units.
 *   It provides a measure of how often physical memory allocation fails.
 */
double stats_frame_error();

/*
 * Computes the average number of page mapping errors per time unit
 * during the simulation.
 *
 * Parameters:
 *   None.
 *
 * Returns:
 *   The average number of page mapping errors per time unit as a double.
 *
 * Description:
 *   This function divides the total number of page mapping errors
 *   recorded during the simulation by the total number of time units.
 *   It provides a measure of how often virtual memory mapping fails.
 */
double stats_page_error();

/*
 * Counts the number of external memory fragments in physical memory
 * that are smaller than a specified size.
 *
 * Parameters:
 *   frag_size - The maximum size (in frames) of a fragment to be counted.
 *
 * Returns:
 *   The number of fragments whose size is greater than 0 and less than frag_size.
 *
 * Description:
 *   This function scans the memory map and identifies contiguous regions
 *   of free frames (i.e., NULL entries). A fragment is counted if its size
 *   is smaller than the specified frag_size.
 *
 *   The function keeps track of consecutive free frames and, upon reaching
 *   an occupied frame or the end of memory, determines whether the free
 *   region qualifies as a fragment. This metric is used to evaluate
 *   external fragmentation in physical memory.
 */
int stats_fragments();

/*
 * Generates a random integer within a specified inclusive range.
 *
 * Parameters:
 *   max - The upper bound of the range.
 *   min - The lower bound of the range.
 *
 * Returns:
 *   A random integer between min and max (inclusive).
 *
 * Description:
 *   This function uses the standard rand() function to generate a
 *   pseudo-random number within the specified range. The result is
 *   uniformly distributed between min and max, inclusive.
 */
unsigned int randnum( unsigned int max, unsigned int min );

/*
 * Displays the contents of the pointer table, which tracks all active
 * memory allocations in the simulation.
 *
 * Parameters:
 *   None.
 *
 * Returns:
 *   None.
 *
 * Description:
 *   This function prints a formatted table showing each active allocation,
 *   including its starting virtual page number (VPN), size, and remaining
 *   duration.
 *
 *   It iterates through the pointer table and outputs information for
 *   all non-NULL entries, providing a snapshot of the current state of
 *   allocated memory blocks.
 */
void show_pointer_table();

// ----------------------------------------
// Functions that CAN be modified
// ----------------------------------------

/*
 * Allocates a contiguous block of pages in virtual memory and maps them
 * to physical frames, simulating heap memory allocation.
 *
 * Parameters:
 *   size     - The number of bytes (pages) to allocate.
 *   duration - The number of time units the allocation should remain active.
 *
 * Returns:
 *   A pointer to a pointer_t structure containing the allocation metadata
 *   (starting virtual page number, size, and duration) if successful.
 *   Returns NULL if the allocation fails.
 */
pointer_t* s_malloc( unsigned int size, unsigned int duration );

/*
 * Deallocates a previously allocated block of memory, unmapping the associated
 * virtual pages and freeing the corresponding physical frames.
 *
 * Parameters:
 *   pointer - A pointer to a pointer_t structure returned by s_malloc,
 *             representing the block to be freed.
 *
 * Returns:
 *   None.
 */
void s_free( pointer_t* pointer );


/*
 * Allocates a contiguous block of frames in physical memory using the
 * selected placement algorithm.
 *
 * Parameters:
 *   size - The number of frames to allocate.
 *
 * Returns:
 *   The starting frame index (physical address) of the allocated block
 *   if successful.
 *   Returns FRAME_ERROR if there is not enough contiguous free memory
 *   to satisfy the request.
 */
int allocate( unsigned int size );

/*
 * Frees a contiguous block of frames in physical memory.
 *
 * Parameters:
 *   pfn  - The starting frame index (physical address) of the block to be freed.
 *   size - The number of frames to deallocate.
 *
 * Returns:
 *   None.
 *
 * Description:
 *   This function marks each frame in the specified range as empty,
 *   making them available for future allocations.
 */
void unallocate( unsigned int pfn, unsigned int size );

/*
 * Maps a contiguous block of virtual pages in the page table.
 *
 * Parameters:
 *   length - The number of pages to map.
 *
 * Returns:
 *   The starting virtual page number (vpn) of the mapped block
 *   if successful.
 *   Returns PAGE_ERROR if there is not enough contiguous virtual
 *   memory to satisfy the request.
 */
int map( unsigned int length );


/*
 * Unmaps a contiguous block of virtual pages in the page table.
 *
 * Parameters:
 *   vpn  - The starting virtual page number of the block to be unmapped.
 *   size - The number of pages to unmap.
 *
 * Returns:
 *   None.
 *
 * Description:
 *   This function marks each page in the specified range as not present,
 *   indicating that the pages are no longer mapped in virtual memory.
 */
void unmap( unsigned int vpn, unsigned int size );


/*
 * Finds a contiguous block of free frames in physical memory using
 * the first-fit placement algorithm.
 *
 * Parameters:
 *   size - The number of contiguous frames required.
 *
 * Returns:
 *   The starting frame index (physical address) of the first block
 *   large enough to satisfy the request.
 *   Returns FRAME_ERROR if no suitable block is found.
 *
 * Description:
 *   This function scans memory from the beginning and selects the
 *   first available block of frames that is large enough for the
 *   requested length.
 */
int first_fit( unsigned int size );


/*
 * Finds a contiguous block of free frames in physical memory using
 * the next-fit placement algorithm.
 *
 * Parameters:
 *   start_frame   - The frame index to begin the search from (i.e.,
 *                   the location of the last placement).
 *   size          - The number of contiguous frames required.
 *
 * Returns:
 *   The starting frame index (physical address) of the next block
 *   large enough to satisfy the request.
 *   Returns FRAME_ERROR if no suitable block is found.
 *
 * Description:
 *   This function scans memory starting from the given start_address
 *   and continues forward to find a suitable block. If the end of
 *   memory is reached, the search resumes at the beginning and
 *   continues until it returns to the starting point.
 */
int next_fit( int start_frame, unsigned int size);


/*
 * Finds a contiguous block of free frames in physical memory using
 * the best-fit placement algorithm.
 *
 * Parameters:
 *   size - The number of contiguous frames required.
 *
 * Returns:
 *   The starting frame index (physical address) of the smallest block
 *   that is large enough to satisfy the request.
 *   Returns FRAME_ERROR if no suitable block is found.
 *
 * Description:
 *   This function scans all available blocks of free frames and selects
 *   the one that most closely matches the requested size. If a perfect
 *   fit is found, the search may terminate early.
 */
int best_fit( unsigned int size );


/*
 * These functions are provided for debugging purposes.
 *
 * Example code is included, but you are encouraged to modify 
 * it as needed to help you and your team debug and inspect the
 * information stored in the related data structures. 
 */
void debug_page( unsigned int vpn, unsigned int size );
void debug_frame( unsigned int vpn, unsigned int size );
