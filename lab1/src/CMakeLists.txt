cmake_minimum_required(VERSION 3.26)

set(sources
	"${CMAKE_CURRENT_SOURCE_DIR}/child.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

add_executable(
	core
	${sources})

if(MSVC)
	target_compile_options(
		core
		PRIVATE /W4
		PRIVATE /MP
	)
endif()
