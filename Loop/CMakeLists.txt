# If we don't need RTTI or EH, there's no reason to export anything
# from the hello plugin.
cmake_minimum_required(VERSION 2.6)
if( NOT LLVM_REQUIRES_RTTI )
  if( NOT LLVM_REQUIRES_EH )
    set(LLVM_EXPORTED_SYMBOL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Hello.exports)
  endif()
endif()

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS Core Support)
endif()

project(myPass)
add_llvm_library( LLVMLoop MODULE BUILDTREE_ONLY
  myPass.cpp

  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  )

add_executable(${PROJECT_NAME} myPass.cpp)
add_subdirectory(Loop)
