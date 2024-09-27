cmake_minimum_required(VERSION 3.26)

set(sources
	"${CMAKE_CURRENT_SOURCE_DIR}/child.cpp"
)

add_executable(
	child
	${sources})

if(MSVC)
	target_compile_options(
		child
		PRIVATE /W4
		PRIVATE /MP
	)
endif()
