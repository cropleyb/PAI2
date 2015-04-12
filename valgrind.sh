# Just errors
~/pubsrc/valgrind-trunk/Inst/bin/valgrind ~/Dropbox/pai2/debug/tests

# Leaks
#~/pubsrc/valgrind-trunk/Inst/bin/valgrind --leak-check=full debug/tests

# Run specific subset of tests (inc leak check)
#  ~/pubsrc/valgrind-trunk/Inst/bin/valgrind --leak-check=full debug/tests --gtest_filter="*RunAIFixture*"
