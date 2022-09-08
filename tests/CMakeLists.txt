#reference https://matgomes.com/integrate-google-test-into-cmake/
set(BIN_NAME "ut-${PROJECT_NAME}")

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

file(GLOB TEST_SRC
  *.cpp 
)

add_executable(${BIN_NAME} 
	${TEST_SRC}
)

target_link_libraries(${BIN_NAME}
 PRIVATE
  GTest::GTest
  ${PROJECT_NAME}
)

add_test(scp_ecg_gtests ${BIN_NAME})