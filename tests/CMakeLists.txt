# Test CMakeLists.txt for FlexonDB
# Comprehensive testing infrastructure

# Only configure tests if testing is enabled
if(ENABLE_TESTING)
    enable_testing()
    
    # Test utilities library
    add_library(test_utils STATIC
        test_utils.c
    )
    target_include_directories(test_utils PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    target_link_libraries(test_utils flexondb_common flexondb_platform)
    
    # Unit tests
    add_executable(test_schema_enhanced unit/test_schema.c)
    target_link_libraries(test_schema_enhanced flexondb_core test_utils)
    add_test(NAME schema_tests COMMAND test_schema_enhanced)
    
    add_executable(test_writer_enhanced unit/test_writer.c)
    target_link_libraries(test_writer_enhanced flexondb_core test_utils)
    add_test(NAME writer_tests COMMAND test_writer_enhanced)
    
    add_executable(test_reader_enhanced unit/test_reader.c)
    target_link_libraries(test_reader_enhanced flexondb_core test_utils)
    add_test(NAME reader_tests COMMAND test_reader_enhanced)
    
    add_executable(test_data_types unit/test_data_types.c)
    target_link_libraries(test_data_types flexondb_core test_utils)
    add_test(NAME data_types_tests COMMAND test_data_types)
    
    add_executable(test_enhanced_schema unit/test_enhanced_schema.c)
    target_link_libraries(test_enhanced_schema flexondb_core test_utils)
    add_test(NAME enhanced_schema_tests COMMAND test_enhanced_schema)
    
    add_executable(test_command_processor unit/test_command_processor.c)
    target_link_libraries(test_command_processor flexondb_core flexondb_common test_utils)
    add_test(NAME command_processor_tests COMMAND test_command_processor)
    
    # Integration tests
    add_executable(test_cli_shell_parity integration/test_cli_shell_parity.c)
    target_link_libraries(test_cli_shell_parity flexondb_core flexondb_cli flexondb_shell test_utils)
    add_test(NAME cli_shell_parity COMMAND test_cli_shell_parity)
    
    add_executable(test_cross_platform integration/test_cross_platform.c)
    target_link_libraries(test_cross_platform flexondb_core test_utils)
    add_test(NAME cross_platform_tests COMMAND test_cross_platform)
    
    add_executable(test_full_workflow integration/test_full_workflow.c)
    target_link_libraries(test_full_workflow flexondb_core test_utils)
    add_test(NAME full_workflow_tests COMMAND test_full_workflow)
    
    # Benchmarks (if enabled)
    if(BUILD_BENCHMARKS)
        add_executable(benchmark_insert benchmarks/benchmark_insert.c)
        target_link_libraries(benchmark_insert flexondb_core test_utils)
        
        add_executable(benchmark_read benchmarks/benchmark_read.c)
        target_link_libraries(benchmark_read flexondb_core test_utils)
        
        add_executable(benchmark_query benchmarks/benchmark_query.c)
        target_link_libraries(benchmark_query flexondb_core test_utils)
        
        add_executable(benchmark_formats benchmarks/benchmark_formats.c)
        target_link_libraries(benchmark_formats flexondb_core test_utils)
        
        add_custom_target(benchmarks DEPENDS 
            benchmark_insert 
            benchmark_read 
            benchmark_query 
            benchmark_formats
        )
    endif()
    
    # Custom test targets
    add_custom_target(test-all
        DEPENDS test_schema_enhanced test_writer_enhanced test_reader_enhanced test_data_types
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMENT "Running all FlexonDB tests"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    
    # Install test data
    file(COPY data/ DESTINATION ${CMAKE_BINARY_DIR}/tests/data/)
    
endif()