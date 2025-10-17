add_executable(bppc
	main.cpp
	../frontend/lexer.cpp
)

target_compile_features(bppc PRIVATE cxx_std_20)

target_link_libraries(bppc PRIVATE basicpp)

target_include_directories(
	bppc
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/..
)
