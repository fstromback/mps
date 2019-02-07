/* 
TEST_HEADER
 id = $Id$
 summary = reset ld again, in destroyed arena
 language = c
 link = myfmt.o testlib.o
OUTPUT_SPEC
 abort = true
END_HEADER
*/

#include "testlib.h"
#include "mpscamc.h"
#include "myfmt.h"

static void test(void *stack_pointer)
{
 mps_arena_t arena;
 mps_ld_s ld;

 cdie(mps_arena_create(&arena, mps_arena_class_vm(), mmqaArenaSIZE), "create arena");

 mps_ld_reset(&ld, arena);
 comment("Reset ld."); 

 mps_arena_destroy(arena);
 comment("Destroyed arena.");

 mps_ld_reset(&ld, arena);
 comment("Reset ld again."); 

}

int main(void)
{
 run_test(test);
 return 0;
}

