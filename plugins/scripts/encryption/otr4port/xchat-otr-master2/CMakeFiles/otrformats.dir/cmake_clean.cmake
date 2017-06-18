FILE(REMOVE_RECURSE
  "CMakeFiles/otrformats"
  "otr-formats.c"
  "xchat-formats.c"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/otrformats.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
