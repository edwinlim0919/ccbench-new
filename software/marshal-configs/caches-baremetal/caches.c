////////////////////////////////////////////////////////////////////////////////
// Caches (time a pointer chase to get cache capacity, cache acces latency) 
// Author: Christopher Celio
// Date  : 2010 Nov 15
//
// Designed to assist in emperically discovering the cache size of a processor
// by repeatedly running this kernel with varying input sizes. 
//
// Create an array of size <input>, where each array element is a pointer to
// another element in the array (circular linked list). Arrange pointers
// randomly to minimize cache-prefetching effects, and walk the array for set
// period of elements to normalize for work/array-size. Thus, any increase in
// run-time should be localized to caching effects.
//
// The input is (at the moment) in "number of elements in the array", 
// which will be rounded to the nearest cache-line amount. This means that
// the graphing code should pay attention to the report file, and not the 
// input file when looking at where to plot a data point. 

//#define DEBUG 
//#define PRINT_ARRAY
#define PRINT_SCRIPT_FRIENDLY
 
//force benchmark to run for some minimum duration in cycles
// advantages: hopefully smoothes out noise of tests that run too quickly
// disadvantages: adds additional code to critical loop (empirically unnoticable)
#define USE_MIN_CYCLES
#define MIN_CYCLES 100000UL
                      

// stride in # of elements, most processors have a 64byte cache line, so 16 elements
// would stride across cache lines.
#define CACHELINE_IN_BYTES_SZ (64)
#define CACHELINE_SZ (CACHELINE_IN_BYTES_SZ/sizeof(uint32_t))

// in # bytes
#define PAGE_SZ (4096) 

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cctimer.h>
#include <cclfsr.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <float.h>

// Global Variables
double MB_per_sec;

int g_stride;
int g_run_type;  // choose between stride size, or random stride 

cccycles_t run_cycles;


// Function Declarations
uint32_t initializeGlobalArrays(uint32_t* arr_n_ptr, uint32_t num_elements, uint32_t stride);
uint32_t threadMain();

uint32_t printArray(uint32_t iter, uint32_t *arr_ptr, uint32_t num_elements, uint32_t stride);
uint32_t verifyArray(uint32_t *arr_ptr, uint32_t num_elements, uint32_t stride);

int main(int argc, char* argv[])
{
  printf("\nBegin Test\n");
  g_run_type = 0; // set manually for baremetal version

  if(g_run_type > 0) {
    g_stride = g_run_type;
  }

  // random access should hit cache-line addresses, to eliminate
  // spatial locality and force more misses.
  if(g_run_type == 0) {
    g_stride = CACHELINE_SZ;
  }

  // this volatile ret_val is crucial, otherwise the entire run-loop 
  // gets optimized away! :(
  uint32_t volatile ret_val = threadMain();  

  printf("Done. Exiting...\n");
  return 0;
}

uint32_t runFunc(uint32_t num_elements, uint32_t num_iterations) {
  uint32_t* arr_n_ptr;

  // make the array size a multiple of the stride; round up
  // actually stride*2 so we can insure that no consecutive accesses are adjacent, and this makes implementing this easier
  if (num_elements % (2*g_stride) != 0) {
    num_elements = num_elements + ((2*g_stride) - (num_elements % (2*g_stride)));
  }

  // pad out memory to next PAGE_SZ to make initialization easier...
  uint32_t num_total_elements_per_page = PAGE_SZ/sizeof(uint32_t);
  int num_elements_allocated = num_elements + (num_total_elements_per_page - (num_elements % num_total_elements_per_page));

  arr_n_ptr = (uint32_t *) malloc(num_elements_allocated * sizeof(uint32_t));
  initializeGlobalArrays(arr_n_ptr, num_elements, g_stride);

  double const clk_freq = 1e9;
  uint32_t performed_iterations = num_iterations;
  //printf("RUNFUNC: [num_elements=%u], [num_iterations=%u]\n", num_elements, num_iterations);

  /** CRITICAL SECTION **/
  cccycles_t start_cycles = cc_get_cycles(clk_freq);
  
#ifdef USE_MIN_CYCLES
  // run for num_iterations or until MIN_CYCLES reached, whichever comes last
  cccycles_t estimated_end_cycles = start_cycles + MIN_CYCLES;
  intptr_t idx = 0;

  // we have to run for AT LEAST num_iterations, so dont muck up critical section
  // with unnecessary code
  for (uint32_t k = 0; k < num_iterations; k++) {
    idx = arr_n_ptr[idx];
  }

  while (cc_get_cycles(clk_freq) < estimated_end_cycles) {
    performed_iterations += num_iterations;
    for (uint32_t k = 0; k < num_iterations; k++) {
      idx = arr_n_ptr[idx];
    }
  }
#else
  // run for num_iterations...
  intptr_t idx = 0;

  for (uint32_t k = 0; k < num_iterations; k++) {
    idx = arr_n_ptr[idx];
  }
#endif

  cccycles_t stop_cycles = cc_get_cycles(clk_freq);
  run_cycles = stop_cycles - start_cycles;
  printf("App:[caches], AppSize:[%u], NumCycles:[%lu], NumIterations:[%u], RunType:[%d]\n",
    num_elements,
    run_cycles,
    performed_iterations,
    g_run_type);
  free(arr_n_ptr);

  // prevent compiler from removing ptr chasing...
  // although the receiver must put idx into a volatile variable as well!
  return (uint32_t) idx;
}

uint32_t threadMain() {
  uint32_t size = 32;
  uint32_t prev_size = 0;
  uint32_t div_amount = 10;
  //uint32_t max_size = 8388608;
  uint32_t max_size = 1024;
  uint32_t num_iterations = 100;
  uint32_t run_count = 10;
  uint32_t size_diff; 
  uint32_t new_size;

  uint32_t volatile ret_sum = 0;
 
  // sweep exponentially across array sizes
  while (size <= max_size) {
    for (int i = 0; i < run_count; i++) {
      ret_sum += runFunc(size, num_iterations);
    }

    new_size = size + (size / div_amount);
    size_diff = new_size - size;
    prev_size = size;

    if (size_diff < 32) {
      size = prev_size + 32;
    } else {
      size = new_size;
    }
  }

  return ret_sum;
}


//generate a random linked list of a PAGE_SZ, starting at address arr_n_ptr[page_offset]
//the calling function is in charge of stitching the page together with the other pages
//num_accesses is the number of pointers we are going to add (not the number of
//elements spanned by the page)
//returns the index to the last index (to help the calling function stitch pages together)
//interleaving_space is the size between entries (i.e., 2 means generated lines are even/odd)
//assumes num_accesses can be divided by interleaving_space
uint32_t initializePage(uint32_t* arr_n_ptr, uint32_t page_offset, uint32_t num_accesses, uint32_t stride, uint32_t interleaving_space)
{
   if (interleaving_space > 2)
      printf("ERROR: unsupported interleaving_space\n");

#ifdef DEBUG
   printf("\n--(offset = %d) Generating Page-- num_el=%d, pageSz=%d, strd=%d\n\n",
      page_offset, num_accesses, num_accesses*stride*sizeof(int32_t), stride);
#endif

   cc_lfsr_t lfsr;
   uint32_t lfsr_init_val = 1; //TODO provide different streams different starting positions?
   //(-1) because we are going to loop through twice, once for odd and once for even entries 
   uint32_t lfsr_width = (log(num_accesses) / log(2)) - (interleaving_space - 1); //TODO EVENODD


   uint32_t max_accesses = (0x1 << lfsr_width)*interleaving_space;

//   printf("width=%d || max_accesses = %d, num_elements = %d\n", lfsr_width, max_accesses, num_accesses);

   // special case to handle non-powers of 2 num_accesses
   if (max_accesses < num_accesses)
      lfsr_width+=interleaving_space;


   uint32_t idx;
   uint32_t curr_idx, next_idx;

   curr_idx = page_offset;

//   printf("lfsr_width = %d\n", lfsr_width);

   // special cases to handle the VERY small pages (too small to use a LFSR)
   if (lfsr_width < 2)
   {
      // lfsr width too small, forget about randomizing this page...
      for (int i=0; i < num_accesses-1; i++)
      {
         arr_n_ptr[curr_idx] = curr_idx + stride;
         curr_idx += stride; 
      }
      
      arr_n_ptr[curr_idx] = page_offset;
      return curr_idx;
   }

   
   cc_lfsr_init(&lfsr, lfsr_init_val, lfsr_width);


   // "-1" because we do the last part separately (lfsr's don't generate 0s)
   for (int i=0; i < num_accesses-interleaving_space; i+=interleaving_space)
   {
//      printf("i=%d, i < %d-%d = %d\n", i, num_accesses, interleaving_space, (num_accesses-interleaving_space));
      //TODO do both even, odd at same time...
      next_idx = lfsr.value * (interleaving_space*stride) + page_offset; //TODO EVENODD
      
      arr_n_ptr[curr_idx] = next_idx;
      if (interleaving_space == 2)
         arr_n_ptr[curr_idx+stride] = next_idx+stride;


#ifdef DEBUG
      printf("   array[%4d] = %4d,   lfsr.value = %4d, i = %4d EVEN\n", 
                  curr_idx, arr_n_ptr[curr_idx], lfsr.value, i); 
      if (interleaving_space == 2)
         printf("   array[%4d] = %4d,   lfsr.value = %4d, i = %4d ODD\n", 
                     curr_idx+stride, arr_n_ptr[curr_idx+stride], lfsr.value, i); 
#endif

      curr_idx = next_idx;
      lfsr.value = cc_lfsr_next(&lfsr);

      //handle non powers of 2 num_accesses
//      while (lfsr.value > (num_accesses-1))
      while (lfsr.value > ((num_accesses-1)/interleaving_space))
      {
         lfsr.value = cc_lfsr_next(&lfsr);
      }
   }

   //handle last index, wrap back to the start of the page
   //let calling function stitch to next pages...
   if (interleaving_space==1)
   {
      arr_n_ptr[curr_idx] = page_offset;
   }
   else
   {
      arr_n_ptr[curr_idx] = page_offset+stride;
      arr_n_ptr[curr_idx+stride] = page_offset;
#ifdef DEBUG
      printf("  *array[%4d] = %4d,   lfsr.value = %4d, \n", 
                  curr_idx, arr_n_ptr[curr_idx], lfsr.value); 
      printf("  *array[%4d] = %4d,   lfsr.value = %4d, \n", 
                  curr_idx+stride, arr_n_ptr[curr_idx+stride], lfsr.value); 
#endif
      curr_idx += stride;
   }


   return curr_idx;
}


// this is the future initializeGlobalArrays, but it has one or two bugs so it's commented out for now
// it will more accurately provide the L3 access times on some processors
uint32_t initializeGlobalArrays(uint32_t* arr_n_ptr, uint32_t num_elements, uint32_t stride)
{
   // create a strided access array (no randomization)
   if(g_run_type != 0)
   {
      for (uint32_t i = 0; i < num_elements-1; i++)
      {
         arr_n_ptr[i % num_elements] = (i+stride) % num_elements;
      }
   }
   else
   {
      //randomize array on cacheline strided boundaries
#ifdef DEBUG
      printf("-==== Begin two-level randomization of the chase array ====-\n");
#endif
 
      // check to see if the array is 1-element long...
      if (num_elements == stride)
      {
#ifdef DEBUG
         printf("\nArray is 1 element long, returning....\n");
#endif
         // points back on itself
         arr_n_ptr[0] = 0;
         return 0;
      }
      
      uint32_t num_elements_per_page = PAGE_SZ/sizeof(uint32_t);

#ifdef DEBUG
      printf("\nnum_elements_per_page = %lu\n\n", PAGE_SZ/sizeof(uint32_t));
#endif
                    
      
      uint32_t last_idx;

      // two-level randomization (this is the outer level for-loop)
      // for each page...
      for(int i=0; i < num_elements; i+=num_elements_per_page)
      {
#ifdef DEBUG
         printf("\nStarting New Page: i=%d, page offset = %d\n", i, i); 
#endif
         
         uint32_t page_offset = i;


         //TODO test for partial page (which is always the last page)...
         if ((int32_t) i >= (int32_t) (num_elements - num_elements_per_page)  //is last page?
            && ((num_elements - i) % num_elements_per_page != 0))  //and last page is partial
         {
//            printf("PARTIAL PAGE: i=%d >= %d/%d - %d\n", i, num_elements, stride, num_elements_per_page);
            num_elements_per_page = num_elements - i; //num_elements % num_elements_per_page;
//            printf("PARTIAL PAGE: num_elements_per_page = %d,  total_num_elements = %d, i=%d\n",
//               num_elements_per_page,  num_elements, i);
         }



         last_idx = initializePage(arr_n_ptr, page_offset, num_elements_per_page/stride, stride, 1);
//         last_idx = initializePage(arr_n_ptr, page_offset, num_elements_per_page/stride, stride, 2);
         // tie this page to the next page...
         arr_n_ptr[last_idx] = (i+num_elements_per_page);
//         printf(" *array[%4d] = %4d, \n", last_idx, arr_n_ptr[last_idx]);
      }


      //handle the last page ...
      //wrap the array back to the start
      arr_n_ptr[last_idx] = 0;



   } //end of randomized array creation

#ifdef DEBUG
   printf("\n\n");
#endif

#ifdef PRINT_ARRAY
   printArray(0, arr_n_ptr, num_elements, stride);
#endif

   verifyArray(arr_n_ptr, num_elements, stride);
  
   return 0;
}


uint32_t printArray(uint32_t iter, uint32_t *arr_ptr, uint32_t num_elements, uint32_t stride )
{
   printf("Chasing through Array (run thru for 2x*num_el/stride) after iteration: %d\n", iter);
   uint32_t idx = 0;
   for (uint32_t i = 0; i < 2*num_elements/stride; i++)
   {
      printf("%3d, ", arr_ptr[idx]);
      idx = arr_ptr[idx];
   }
   printf("\n");
   
   printf("arr_ptr: ");
   for (uint32_t i = 0; i < num_elements; i++)
   {
      if (i % stride == 0) printf("\n");
      printf("%3d, ", arr_ptr[i]);
   }
   printf("\n\n\n");

   return 0;
}


uint32_t verifyArray(uint32_t *arr_ptr, uint32_t num_elements, uint32_t stride)
{
   uint32_t idx = 0;
   uint32_t counter = 0;

   uint32_t first_idx = 0;
   uint32_t finished = 0;

   uint32_t* verify_array;
   verify_array = (uint32_t *) calloc(num_elements, sizeof(uint32_t));
   
   while (!finished)
   {
      verify_array[idx] = verify_array[idx] + 1;
      counter++;
      idx = arr_ptr[idx];

      if(counter >= num_elements || idx == first_idx)
         finished = 1;
   }

   uint32_t error = 0;

   for (uint32_t i = 0; i < num_elements; i+=stride)
   {
      if(verify_array[i] != 1)  
      {
#ifdef DEBUG
         printf("Error at Element [%d], accessed %d times\n", 
            i,
            verify_array[i]
            );
#endif
         error = 1;
      }
   }

   if (counter==(num_elements/stride) && !error) {
#ifdef DEBUG
      printf("Array verified\n\n");
#else
      int x=0; //here for compiler reasons
#endif
   } 
   else 
   {
      printf("Error: Array size:%d, Stride size:%d, Loops in:%d (wants to loop in %d)\n\n",
         num_elements,
         stride,
         counter,
         num_elements/stride);
   }

   return error | (counter==(num_elements/stride));
}

