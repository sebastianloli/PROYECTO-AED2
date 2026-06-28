# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles\\quadtree_bench_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\quadtree_bench_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\quadtree_sim_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\quadtree_sim_autogen.dir\\ParseCache.txt"
  "quadtree_bench_autogen"
  "quadtree_sim_autogen"
  )
endif()
