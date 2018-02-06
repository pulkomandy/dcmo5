cmake_minimum_required(VERSION 3.9)
project(dcmo5)

find_package(SDL REQUIRED)
find_package(SDL_ttf REQUIRED)

include_directories(source)
include_directories(${SDL_INCLUDE_DIR} ${SDL_TTF_INCLUDE_DIRS})
set(SOURCES_FILE 
  ${PROJECT_SOURCE_DIR}/source/dc6809emul.c
  ${PROJECT_SOURCE_DIR}/source/dcmo5boutons.c
  ${PROJECT_SOURCE_DIR}/source/dcmo5devices.c
  ${PROJECT_SOURCE_DIR}/source/dcmo5dialog.c
  ${PROJECT_SOURCE_DIR}/source/dcmo5emulation.c
  ${PROJECT_SOURCE_DIR}/source/dcmo5keyb.c
  ${PROJECT_SOURCE_DIR}/source/dcmo5main.c
  ${PROJECT_SOURCE_DIR}/source/dcmo5options.c
  ${PROJECT_SOURCE_DIR}/source/dcmo5video.c
)

add_executable(dcmo5 ${SOURCES_FILE})
target_link_libraries(dcmo5 ${SDL_LIBRARY} ${SDL_TTF_LIBRARIES})

install(TARGETS dcmo5 DESTINATION bin)

# add the uninstall support
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	@ONLY)
add_custom_target(uninstall
"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
