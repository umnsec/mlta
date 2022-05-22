set (DumperSourceCodes
  IRDumper.h
  IRDumper.cpp
)

add_library (DumperObj OBJECT ${DumperSourceCodes})
add_library (Dumper SHARED $<TARGET_OBJECTS:DumperObj>)
add_library (DumperStatic STATIC $<TARGET_OBJECTS:DumperObj>)

set (EXECUTABLE_OUTPUT_PATH ${UNISAN_BINARY_DIR})
link_directories (${UNISAN_BINARY_DIR}/lib)
