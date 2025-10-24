# CMakeLists.txt for tran_heat Fortran module
cmake_minimum_required(VERSION 3.16)

# 启用 Fortran 语言支持
enable_language(Fortran)

# 设置变量
set(TRAN_HEAT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# 查找所有 Fortran 源文件
file(GLOB_RECURSE tran_heat_f90_sources 
    ${TRAN_HEAT_DIR}/src/*.f90
)

file(GLOB_RECURSE tran_heat_f_sources 
    ${TRAN_HEAT_DIR}/src/*.f
)

# 合并所有源文件
set(tran_heat_sources ${tran_heat_f90_sources} ${tran_heat_f_sources})

# 创建必要的目录结构
file(MAKE_DIRECTORY ${TRAN_HEAT_DIR}/build)
file(MAKE_DIRECTORY ${TRAN_HEAT_DIR}/modules)

# 如果有源文件，创建静态库
if(tran_heat_sources)
    # 创建静态库目标
    add_library(tran_heat STATIC ${tran_heat_sources})
    
    # 设置 Fortran 编译器标志
    target_compile_options(tran_heat PRIVATE
        $<$<COMPILE_LANGUAGE:Fortran>:-ffree-line-length-none>
    )
    
    # 设置模块输出目录
    set_target_properties(tran_heat PROPERTIES
        Fortran_MODULE_DIRECTORY ${TRAN_HEAT_DIR}/modules
        ARCHIVE_OUTPUT_DIRECTORY ${TRAN_HEAT_DIR}/build
        LIBRARY_OUTPUT_DIRECTORY ${TRAN_HEAT_DIR}/build
    )
    
    # 添加包含目录
    target_include_directories(tran_heat PUBLIC
        ${TRAN_HEAT_DIR}/modules
        ${TRAN_HEAT_DIR}/..
    )
    
    # 如果在 MOOSE 环境中，添加 MOOSE 相关的包含目录和标志
    if(DEFINED ENV{MOOSE_DIR})
        target_compile_definitions(tran_heat PRIVATE TRAN_HEAT_ENABLED)
        
        # 添加 libmesh 包含目录（如果可用）
        if(DEFINED ENV{LIBMESH_DIR})
            target_include_directories(tran_heat PRIVATE 
                $ENV{LIBMESH_DIR}/include
            )
        endif()
    endif()
    
    # 设置编译器特定选项
    if(CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")
        target_compile_options(tran_heat PRIVATE
            -fPIC
            -Wall
        )
    elseif(CMAKE_Fortran_COMPILER_ID STREQUAL "Intel")
        target_compile_options(tran_heat PRIVATE
            -fPIC
            -warn all
        )
    endif()
    
    # 设置构建类型相关的标志
    target_compile_options(tran_heat PRIVATE
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:RelWithDebInfo>:-g -O2>
    )
    
    # 导出目标以供父项目使用
    set(TRAN_HEAT_TARGET tran_heat PARENT_SCOPE)
    set(TRAN_HEAT_MODULES_DIR ${TRAN_HEAT_DIR}/modules PARENT_SCOPE)
    
    # 显示找到的源文件信息
    message(STATUS "tran_heat: Found ${CMAKE_CURRENT_LIST_DIR}")
    message(STATUS "tran_heat: Fortran90 sources: ${tran_heat_f90_sources}")
    message(STATUS "tran_heat: Fortran77 sources: ${tran_heat_f_sources}")
    message(STATUS "tran_heat: Module directory: ${TRAN_HEAT_DIR}/modules")
    
else()
    message(WARNING "tran_heat: No Fortran source files found in ${TRAN_HEAT_DIR}/src")
endif()
