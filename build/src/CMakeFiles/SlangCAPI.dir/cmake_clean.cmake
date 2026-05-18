file(REMOVE_RECURSE
  "../lib/libSlangCAPI.a"
  "../lib/libSlangCAPI.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/SlangCAPI.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
