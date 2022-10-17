add_executable(tests
	inMemoryFile.cpp inMemoryFile.h
	testCGLTFMeshFileLoader.cpp
)

set_target_properties(tests PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
)

target_compile_options(tests
	PRIVATE
		"$<$<CXX_COMPILER_ID:GNU>:-Wall>"
)

target_link_libraries(tests
	PRIVATE
		Catch2::Catch
		IrrlichtMt::IrrlichtMt
)
