#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "fs.h"
#include "fsdata.h"
#include "string.h"
#include "Myserver.h"
#include "myjason.h"
//#include "zllSocCmd.h"

//#include "json.h"
//#include "json_object_private.h"




xQueueHandle xNewConnQue = NULL;
#define MYSOCKET_THREAD_PRIO    ( tskIDLE_PRIORITY + 3 )
#define HANDLENEWCONN_TASK_PRIO   ( tskIDLE_PRIORITY + 2 ) 

//#define FD_SETSIZE 5
#define ACCE_SOCKETNUM 5

static void my_server_socket_thread(void *arg)
{
  int sock, newconn, size;
  struct sockaddr_in address, remotehost;
  NewconnIfo sNewconnIfo;
 printf("start mySocket task\r\n");
 /* create a TCP socket */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
  {
    printf("can not create socket");
    return;
  }
  
  /* bind to port 80 at any interface */
  address.sin_family = AF_INET;
  address.sin_port = htons(80);
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
  {
    printf("can not bind socket");
    return;
  }
  
  /* listen for incoming connections (TCP listen backlog = 5) */
  listen(sock, ACCE_SOCKETNUM);
  
  size = sizeof(remotehost);
  //sNewconnIfo.newconn = newconn;
  sNewconnIfo.premotehost = &remotehost;  
  while (1) 
  {
    newconn = accept(sock, (struct sockaddr *)&remotehost, (socklen_t *)&size);
	sNewconnIfo.newconn = newconn;
	if( xQueueSend( xNewConnQue, ( void * ) &sNewconnIfo, ( portTickType ) 10 ) != pdPASS )
        {
            // Failed to post the message, even after 10 ticks.
            printf("fail adding to xNewConnQue and about to close this connect\r\n");
			close(newconn);
        }
	
    //http_server_serve(newconn);
  }
}

void handleLongConnTask(void * pvParameters)
{
	NewconnIfo sNewconnIfo;
	fd_set FdRead;
	int ret;
	int recLen = 0;
    int jasonLen = 0;
	int recStatus = REC_JASON_HEAD;
	mxgHeader smxgHeader;
	unsigned char ReCvBuf[1024] = {0};
	unsigned char jasonReCvBuf[1024] = {0};
	struct timeval timeVal = {0};
	struct json_object*  cmd_obj = NULL;
	struct json_object*  temp_obj = NULL;
	const char* type = NULL;
	const char* addr = NULL;
	int value = 0;
	char temp[32] = {0};
	int  tempVal;
	int headIndex, index;

	timeVal.tv_sec = 2;
	timeVal.tv_usec =0;
		
		printf("handleLongConnTask\r\n");
	for(;;)
	{

		if( xNewConnQue != NULL )
		    {
				xQueueReceive( xNewConnQue, &( sNewconnIfo ), portMAX_DELAY);
				printf("get new conn\r\n");
				 FD_ZERO(&FdRead);
				 FD_SET(sNewconnIfo.newconn, &FdRead);
sktselect:
				printf("start slect\r\n");
				 ret = select(sNewconnIfo.newconn + 1, &FdRead, NULL, NULL, &timeVal);
				 	printf("finish slect\r\n");
				 if(0 == ret)	 //nothing is active
				{
					printf("time out\r\n");
					FD_CLR(sNewconnIfo.newconn, &FdRead);
					//close(sNewconnIfo.newconn);
					goto sktselect;
				}
				else if(-1 == ret) //system error
				{
				 	printf("select error\r\n");
					FD_CLR(sNewconnIfo.newconn, &FdRead);
					close(sNewconnIfo.newconn);
					continue;
				}

				if(FD_ISSET(sNewconnIfo.newconn, &FdRead))
				{

					printf("start rec\r\n");
					ret = recv(sNewconnIfo.newconn, ReCvBuf, sizeof(ReCvBuf)-1, 0);
					
					if(ret <= 0)
						{
							FD_CLR(sNewconnIfo.newconn, &FdRead);
							close(sNewconnIfo.newconn);
							continue;
						}
					  recLen += ret;
headfind:
					  if(recStatus == REC_JASON_HEAD)
					  {				
						printf("head.....\r\n");	  	
					  	if(recLen >= 16)
						  {
						  	 headIndex = isHead(ReCvBuf, recLen);
							 printf("headIndex = %d\r\n", headIndex);
						  	 if(headIndex >= 0)
							 {
							 	memcpy(&smxgHeader, ReCvBuf + headIndex, 16);
//							 printf("ReCvBuf[0] = %x \r\n", ReCvBuf[0]);
//							 printf("ReCvBuf[1] = %x \r\n", ReCvBuf[1]);
//							 printf("ReCvBuf[2] = %x \r\n", ReCvBuf[2]);
//							 printf("ReCvBuf[3] = %x \r\n", ReCvBuf[3]);
//							 printf("ReCvBuf[4] = %x \r\n", ReCvBuf[4]);
//							 printf("ReCvBuf[5] = %x \r\n", ReCvBuf[5]);
//							 printf("ReCvBuf[6] = %x \r\n", ReCvBuf[6]);
//							 printf("ReCvBuf[7] = %x \r\n", ReCvBuf[7]);
//							 printf("ReCvBuf[8] = %x \r\n", ReCvBuf[8]);
							 jasonLen = smxgHeader.length;
//							 printf("len = %d\r\n", jasonLen);
							 recStatus = REC_JASON_DATA;
							 for(index = headIndex; index < recLen - 16; index++)
							 {
							 	ReCvBuf[index] = ReCvBuf[index+16];
							 }
							 ret = recLen -=16;
							 }
							 else if(headIndex == -1) //find no head
							 {
							 	 recLen = 0;
							 }
							 
						  }
					  }
doData:
					 if(recStatus == REC_JASON_DATA)
					  {
					  		printf("test1 ----> %s \r\n", ReCvBuf);
					  	  printf("data.....rec = %d jason = %d ret = %d\r\n",recLen , jasonLen, ret);
						  if(recLen > jasonLen)
						  {
						  	
							printf("111111111111111111 jasonLen = %d  reclen = %d ret = %d\r\n",jasonLen ,  recLen, ret);
						  	memcpy(jasonReCvBuf, ReCvBuf, jasonLen);
							if(!getJsonValueAsString(jasonReCvBuf,"type",temp,sizeof temp))
								printf("type : %s\r\n", temp);

							if(!getJsonValueAsString(jasonReCvBuf,"addr",temp,sizeof temp))
								printf("addr : %s\r\n", temp);

							if(!getJsonValueAsInt(jasonReCvBuf,"value",&tempVal))
								printf("value : %d\r\n", tempVal);
								

							memset(jasonReCvBuf, 0x0, sizeof jasonReCvBuf);
							recLen -= jasonLen;
							recStatus = REC_JASON_HEAD ;
							goto headfind;

						  }
						  else if(recLen == jasonLen)
						  {
						  	printf("22222222222222  rec len = %d\r\n", recLen);
						  	memcpy(jasonReCvBuf , ReCvBuf , jasonLen);
							printf("test2 ----> %s \r\n", jasonReCvBuf);
							recLen = 0;
							recStatus = REC_JASON_HEAD ;

							if(!getJsonValueAsString(jasonReCvBuf,"type",temp,sizeof temp))
								printf("type : %s\r\n", temp);

							if(!getJsonValueAsString(jasonReCvBuf,"addr",temp,sizeof temp))
								printf("addr : %s\r\n", temp);

							if(!getJsonValueAsInt(jasonReCvBuf,"value",&tempVal))
							{
								printf("value : %d\r\n", tempVal);
//								if(tempVal == 0)
//								zllSocTouchLink();
//								else if(tempVal == 1)
//								zllSocSetLevelUp();
//								else if(tempVal == 2)
//								zllSocSetLevelDown();
							}
								

								memset(jasonReCvBuf, 0x0, sizeof jasonReCvBuf);

//							if(!getJsonValueAsString(jasonReCvBuf,"type",temp,sizeof temp))
//								printf("type : %s\r\n", temp);

//							cmd_obj = json_tokener_parse((char*)jasonReCvBuf);
//							printf("3333333333333\r\n");
//							if (cmd_obj != NULL)
//							{
//								type = json_object_object_get(cmd_obj,"type")->o.c_string;
//							    addr = json_object_object_get(cmd_obj, "addr")->o.c_string;
//							   	value = json_object_object_get(cmd_obj,"value")->o.c_int;
//							    
//							   	printf("type=%s, addr=%s, value=%d\n",     type, addr, value);
//							    json_object_put(cmd_obj);
//							}
						  }
						  else
						  {
						  	printf("3333333333\r\n");
							memcpy(jasonReCvBuf + recLen - ret, ReCvBuf, recLen);
							if(recLen >= jasonLen)
							goto doData;
						  }
						  	
							
					  }
					  



					
					goto sktselect;
				}
				
			}
			else
				{
					printf("xNewConnQue is NULL and this task is about to suspend\r\n");
					// Suspend ourselves.
    				 vTaskSuspend( NULL );
					
				}

	}
}

void handleNewConnTask(void * pvParameters)
{
	NewconnIfo sNewconnIfo;
	int Socket;
	fd_set FdRead;
	int ret;
	struct timeval timeVal = {0};
	unsigned char ReCvBuf[128] = {0};

	unsigned char *Ret = "test";

	timeVal.tv_sec = 2;
	timeVal.tv_usec = 0;

	//ReCvBuf = (unsigned char *)malloc(512);
	

	//FD_ZERO(&FdRead);
	

	for(;;)
		{

			if( xNewConnQue != NULL )
			    {
					memset(ReCvBuf, 0x00, 128);
			        // Receive a message on the created queue.  Block for 10 ticks if a
			        // message is not immediately available.
			        xQueueReceive( xNewConnQue, &( sNewconnIfo ), portMAX_DELAY);
				      
				   FD_ZERO(&FdRead);
				   FD_SET(sNewconnIfo.newconn, &FdRead);
				    
					ret = select(sNewconnIfo.newconn + 1, &FdRead, NULL, NULL, &timeVal);
					if(0 == ret)
						{
							FD_CLR(sNewconnIfo.newconn, &FdRead);
							close(sNewconnIfo.newconn);
							continue;
						}
					else if(-1 == ret)
						{
							FD_CLR(sNewconnIfo.newconn, &FdRead);
							close(sNewconnIfo.newconn);
							continue;
						}
					
						
					if(FD_ISSET(sNewconnIfo.newconn, &FdRead))
						{
							ret = recv(sNewconnIfo.newconn, ReCvBuf, sizeof(ReCvBuf)-1, 0);
							if(ret < 0)
								{
									FD_CLR(sNewconnIfo.newconn, &FdRead);
									close(sNewconnIfo.newconn);
									continue;
								}
							//printf("------%s num = %d\r\n", ReCvBuf, ret);
							send(sNewconnIfo.newconn, ReCvBuf, ret, 0);
							//send(sNewconnIfo.newconn, Ret, strlen(Ret), 0);
							FD_CLR(sNewconnIfo.newconn, &FdRead);
							close(sNewconnIfo.newconn);
							continue;
						}

					
						
					
			    }
			else
				{
					//printf("xNewConnQue is NULL and this task is about to suspend\r\n");
					// Suspend ourselves.
    				 vTaskSuspend( NULL );
					
				}

		}
	 
}

void my_server_socket_init()
{
	xNewConnQue = xQueueCreate( FD_SETSIZE, sizeof(NewconnIfo) );
	if(NULL == xNewConnQue )
		{
			printf("creat xNewConnQue failed.......\r\n");
			return;
		}
		
	xTaskCreate(handleLongConnTask, "handleLongConnTask", DEFAULT_THREAD_STACKSIZE*2, NULL,HANDLENEWCONN_TASK_PRIO, NULL);
	//xTaskCreate(handleNewConnTask, "handleNewConn", configMINIMAL_STACK_SIZE, NULL,HANDLENEWCONN_TASK_PRIO, NULL);
	//xTaskCreate(handleNewConnTask, "handleNewConn", configMINIMAL_STACK_SIZE, NULL,HANDLENEWCONN_TASK_PRIO, NULL);
  	sys_thread_new("MySocket", my_server_socket_thread, NULL, DEFAULT_THREAD_STACKSIZE, MYSOCKET_THREAD_PRIO);
}


