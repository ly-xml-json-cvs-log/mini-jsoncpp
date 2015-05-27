###################################################################
#
#   min-jsoncpp 配置文件模板，add by tiankonguse 2014-12-25
#
###################################################################


IF(NOT mini_jsoncpp)

SET(mini_jsoncpp_path ${LIB_PATH}/mini_jsoncpp)


INCLUDE_DIRECTORIES(${mini_jsoncpp_path}/include)

ADD_LIBRARY(mini_jsoncpp STATIC IMPORTED)
SET_PROPERTY(TARGET mini_jsoncpp_path PROPERTY IMPORTED_LOCATION ${mini_jsoncpp_path}/libmini_jsoncpp.a)

ENDIF()


