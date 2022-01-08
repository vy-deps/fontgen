cmake_minimum_required(VERSION 3.22)
project(fontgen)

include(FetchContent)

FetchContent_Declare(bmfont
  GIT_REPOSITORY git@github.com:vy-deps/bmfont.git
  GIT_TAG f4bf54dc420f2826b4763934defe5794a3c9b858)
FetchContent_MakeAvailable(bmfont)

FetchContent_Declare(inipp
  GIT_REPOSITORY https://github.com/mcmtroffaes/inipp.git
  GIT_TAG develop)
FetchContent_MakeAvailable(inipp)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp)

add_executable(fontgen ${sources})
target_include_directories(fontgen PRIVATE src)
target_link_libraries(fontgen PRIVATE libbmfont inipp)

set_target_properties(fontgen PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  WINDOWS_EXPORT_ALL_SYMBOLS ON)
