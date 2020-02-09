# cmake build list

cmake_minimum_required( VERSION 2.8.3 )

include( ../cmake_config/config.cmake )
include( ../cmake_config/macros.cmake )
include( ../cmake_config/common_path.cmake )

clear_all_list()



add_include_path( ${COMMON_INCLUDE} )


#add_include_path( /root/code/3rd/boost_1_67_0 )
#add_link_path( /root/code/3rd/boost_1_67_0/stage/lib )

#add_include_path( /root/code/3rd/boost_1_69_0 )
#add_link_path( /root/code/3rd/boost_1_69_0/stage/lib )

#add_include_path( /root/code/3rd/boost_1_71_0 )
#add_link_path( /root/code/3rd/boost_1_71_0/stage/lib )

add_include_path( /root/code/3rd/boost_1_72_0 )
add_link_path( /root/code/3rd/boost_1_72_0/stage/lib )

add_include_path( ../src/*.h )
add_source_files( ../src/*.cpp )

add_link_path( ${COMMON_LIBPATH} )

add_link_lib( boost_system )
add_link_lib( boost_thread )
add_link_lib( pthread )
add_link_lib( dl )
add_link_lib( rt )

build_executable( testBoostLib ./ )
