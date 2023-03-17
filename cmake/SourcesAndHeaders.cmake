set(sources
    #    src/FixedSizeStringBuffer.cpp
)

set(exe_sources
		src/main.cpp
		${sources}
)

set(headers
    include/fixed_size_string_buffer.h
    include/fixed_size_queue.h
)

set(test_sources
  src/fixed_size_string_buffer_test.cpp
)
