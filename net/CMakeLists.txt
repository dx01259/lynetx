SET(net_SRC
	src/AsyncSocket.cpp
	src/InetAddr.cpp
	src/InetTable.cpp
	src/InetConnection.cpp
	src/InetConnectionPool.cpp
	src/LocalIpList.cpp
	src/SendPacket.cpp
	src/SocketOps.cpp
	src/TcpConnection.cpp
	src/TcpConnectionPool.cpp
	src/TcpService.cpp)
	
ADD_LIBRARY(net SHARED ${net_SRC})

set(CMAKE_CXX_COMPILER "g++")
	
SET(HEADERS
	src/AsyncSocket.h
	src/InetAddr.h
	src/InetTable.h
	src/InetConnection.h
	src/InetConnectionPool.h
	src/LocalIpList.h
	src/SendPacket.h
	src/SocketOps.h
	src/TcpConnection.h
	src/TcpConnectionPool.h
	src/TcpService.h)