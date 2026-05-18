file(REMOVE_RECURSE
  "CMakeFiles/MLIRSlangIncGen"
  "SlangDialect.cpp.inc"
  "SlangDialect.h.inc"
  "SlangOps.cpp.inc"
  "SlangOps.h.inc"
  "SlangPasses.h.inc"
  "SlangTypes.cpp.inc"
  "SlangTypes.h.inc"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/MLIRSlangIncGen.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
