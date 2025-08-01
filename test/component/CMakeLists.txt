add_executable(test_banner
    test_banner.cpp
)

# 1. 指定包含路径
target_include_directories(test_banner PRIVATE
    ${CMAKE_SOURCE_DIR}/src/component
)

# 2. 链接 Qt 和组件库
target_link_libraries(test_banner
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
    component                 # 这个前提是你有 add_library(component ...)
)

# 3. MSVC 特殊选项
if (MSVC)
    target_compile_options(test_banner PRIVATE "/EHsc" "/utf-8")
endif()

# 4. 注册测试
add_test(NAME BannerTest COMMAND test_banner)
