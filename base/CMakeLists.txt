SET(base_SRC
	src/CharEncode.cpp
	src/commfunc.cpp
	src/EpollWatch.cpp
	src/IOBufferPtr.cpp
	src/Log.cpp
	src/MsgDeque.cpp
	src/MsgTime.cpp
	src/Mutex.cpp
	src/Socket.cpp
	src/ThreadObject.cpp
	src/ThreadPool.cpp
	src/Timeout.cpp
	src/TMap.cpp)
	
ADD_LIBRARY(base SHARED ${base_SRC})

set(CMAKE_CXX_COMPILER "g++")

SET(HEADERS
	src/CharEncode.h
	src/commfunc.h
	src/datatype.h
	src/EpollWatch.h
	src/IOBufferPtr.h
	src/Log.h
	src/logdef.h
	src/MsgDeque.h
	src/MsgTime.h
	src/Mutex.h
	src/Socket.h
	src/ThreadDefine.h
	src/ThreadObject.h
	src/ThreadPool.h
	src/Timeout.h
	src/TMap.h)
