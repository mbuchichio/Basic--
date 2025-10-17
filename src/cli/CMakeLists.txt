add_executable(bppc
	main.cpp
)

target_compile_features(bppc PRIVATE cxx_std_20)

target_link_libraries(bppc PRIVATE basicpp_frontend)
