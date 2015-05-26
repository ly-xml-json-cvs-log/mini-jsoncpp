###################################################################
#
#	add by tiankonguse 20:37 2015/4/15
#
###################################################################

SET(CMAKE_CXX_FLAGS " -g -Wall -o2")

SET(mini_jsoncpp_lib_path ${PROJECT_SOURCE_DIR}/lib)
SET(mini_jsoncpp_src_path ${PROJECT_SOURCE_DIR}/src)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

AUX_SOURCE_DIRECTORY(${mini_jsoncpp_src_path} X_SRC)
ADD_LIBRARY(mini_jsoncpp STATIC ${X_SRC})

SET_TARGET_PROPERTIES(mini_jsoncpp PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${mini_jsoncpp_lib_path}")
