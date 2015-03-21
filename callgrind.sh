# First, turn on debugging and optimisation in CMakefiles.txt
~/pubsrc/valgrind-trunk/Inst/bin/valgrind --tool=callgrind debug/tests
# Then 
# ~/pubsrc/valgrind-trunk/Inst/bin/callgrind_annotate callgrind.out.86258 > foo.txt
