# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("chip8-emulator")
set(PROJECT_MODULE_NAME "chip8-emulator")

# Include sub-projects.
file(GLOB_RECURSE PROJECT_SOURCE_FILES "./src/*.h" "./src/*.hpp" "./src/*.cpp")

include_directories(
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/vendors/SDL2/include"
)

add_executable(${PROJECT_MODULE_NAME} ${PROJECT_SOURCE_FILES})

target_link_libraries(${PROJECT_MODULE_NAME}
	${PROJECT_SOURCE_DIR}/vendors/SDL2/lib/x64/SDL2.lib
	${PROJECT_SOURCE_DIR}/vendors/SDL2/lib/x64/SDL2_ttf.lib
)

add_custom_command(TARGET ${PROJECT_MODULE_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/roms/ $<TARGET_FILE_DIR:${PROJECT_MODULE_NAME}>/roms)

file(GLOB DllFiles ${CMAKE_SOURCE_DIR}/vendors/SDL2/lib/x64/*.dll)
foreach(ConfigFile ${DllFiles})
  add_custom_command(TARGET ${PROJECT_MODULE_NAME} PRE_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                         copy ${ConfigFile} $<TARGET_FILE_DIR:${PROJECT_MODULE_NAME}>)
endforeach()


#add_custom_command(
#	TARGET ${PROJECT_MODULE_NAME}
#	POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/roms/ ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
#	)
