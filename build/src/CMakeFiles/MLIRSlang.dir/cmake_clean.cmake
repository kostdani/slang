file(REMOVE_RECURSE
  "../lib/libMLIRSlang.a"
  "../lib/libMLIRSlang.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/MLIRSlang.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
