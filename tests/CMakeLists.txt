
#���ӵ�Ԫ����Ŀ¼
add_subdirectory(unit_test)

add_executable(test_scp_write TestScpEcgWriter.cpp)
target_link_libraries(test_scp_write ${PROJECT_NAME})