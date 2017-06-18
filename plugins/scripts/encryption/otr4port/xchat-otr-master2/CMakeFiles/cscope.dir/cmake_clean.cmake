FILE(REMOVE_RECURSE
  "CMakeFiles/cscope"
  "cscope.out"
  "otr-formats.c"
  "xchat-formats.c"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/cscope.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
