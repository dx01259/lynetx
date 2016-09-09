SET(TcpService_SRC
	src/TcpServiceMain.cpp)

LINK_DIRECTORIES(${lynetx_SOURCE_DIR}/cmake/lib)
#INCLUDE_DIRECTORIES()
	
ADD_EXECUTABLE(TcpService ${TcpService_SRC})



#target_link_libraries(TcpService boost_thread)
#target_link_libraries(TcpService boost_system)
#target_link_libraries(TcpService boost_filesystem)
TARGET_LINK_LIBRARIES(TcpService base)
TARGET_LINK_LIBRARIES(TcpService net)
TARGET_LINK_LIBRARIES(TcpService pthread)
