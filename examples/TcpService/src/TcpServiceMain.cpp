/*
 * TcpServiceMain.cpp
 *
 *  Created on: 2015年9月9日
 *      Author: dengxu
 */

#include"../../../base/src/logdef.h"
#include"../../../net/src/TcpService.h"
#include"../../../net/src/SocketOps.h"
#include<unistd.h>

static void sig_exit(int signo);
lynetx::net::CTcpService g_service;

int main(int argc, char *argv[])
{
	//SET_CYCLE(1);
	//TRACE_OPEN("log", 5, 1, 60);
	signal(SIGINT, sig_exit);
	signal(SIGABRT, sig_exit);
	signal(SIGIO, sig_exit);
	signal(SIGSEGV, sig_exit);
	printf("This is main Start\n");
	if(argc < 3)
	{
		printf("Parameter error\n");
		printf("sample:\n");
		printf("\t./TcpService 8000 10\n");
		return -1;
	}
	else {
		short 	port = atoi(argv[1]);
		int 	threadNum = atoi(argv[2]);
		printf("port:%d, threadNum:%d\n", port, threadNum);
		g_service.Start(port, threadNum);
	}
	while(1){
		printf("程序后续循环\n");
		sleep(60);
	}
	printf("This is main end\n");
	return 0;
}

void sig_exit(int signo)
{
	lynetx::TRACE_MSG(lynetx::LOG_DEBUGS, 100, "获取信号量:%d", signo);
	g_service.Stop();
	//usleep(1000);
	exit(0);
}


