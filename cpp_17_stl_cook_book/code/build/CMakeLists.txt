# cmake build list

cmake_minimum_required( VERSION 2.8.3 )

include( ../../cmake_config/config.cmake )
include( ../../cmake_config/macros.cmake )
include( ../../cmake_config/common_path.cmake )


clear_all_list()

add_include_path( ${COMMON_INCLUDE} )
add_include_path( ${MYSQL_INCLUDE}  )
add_include_path( ../src/*.h )
add_source_files( ../src/*.cpp )

add_link_path( ${COMMON_LIBPATH} )
add_link_path( ${MYSQL_LIBPATH} )

add_link_lib( pthread )
add_link_lib( dl )
add_link_lib( rt )

build_executable( test ./ )
