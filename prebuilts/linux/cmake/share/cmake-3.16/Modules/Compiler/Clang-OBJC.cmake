include(Compiler/Clang)
__compiler_clang(OBJC)

if(NOT CMAKE_OBJC_COMPILER_VERSION VERSION_LESS 3.4)
  set(CMAKE_OBJC90_STANDARD_COMPILE_OPTION "-std=c90")
  set(CMAKE_OBJC90_EXTENSION_COMPILE_OPTION "-std=gnu90")
  set(CMAKE_OBJC90_STANDARD__HAS_FULL_SUPPORT ON)

  set(CMAKE_OBJC99_STANDARD_COMPILE_OPTION "-std=c99")
  set(CMAKE_OBJC99_EXTENSION_COMPILE_OPTION "-std=gnu99")
  set(CMAKE_OBJC99_STANDARD__HAS_FULL_SUPPORT ON)

  set(CMAKE_OBJC11_STANDARD_COMPILE_OPTION "-std=c11")
  set(CMAKE_OBJC11_EXTENSION_COMPILE_OPTION "-std=gnu11")
  set(CMAKE_OBJC11_STANDARD__HAS_FULL_SUPPORT ON)
endif()

__compiler_check_default_language_standard(OBJC 3.4 99 3.6 11)
