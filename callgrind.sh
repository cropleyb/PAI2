# First, turn on debugging and optimisation in CMakefiles.txt
~/pubsrc/valgrind-trunk/Inst/bin/valgrind --tool=callgrind --dsymutil=yes debug/tests
# Then 
# ~/pubsrc/valgrind-trunk/Inst/bin/callgrind_annotate callgrind.out.86258 board_strip.h > foo.txt
