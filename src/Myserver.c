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
#include "jsonAction.h"
//#include "zllSocCmd.h"

//#include "json.h"
//#include "json_object_private.h"




xQueueHandle xNewConnQue = NULL;
#define MYSOCKET_THREAD_PRIO    ( tskIDLE_PRIORITY + 3 )
#define HANDLENEWCONN_TASK_PRIO   ( tskIDLE_PRIORITY + 2 )
#define JSONACTION_TASK_PRIO   ( tskIDLE_PRIORITY + 1 ) 

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
	printf("accept new connect\r\n");
	sNewconnIfo.newconn = newconn;
	if( xQueueSend( xNewConnQue, ( void * ) &sNewconnIfo, ( portTickType ) 10 ) != pdPASS )
        {
            // Failed to post the message, even after 10 ticks.
            printf("fail adding to xNewConnQue and about to close this connect\r\n");
			close(newconn);
        }
	else
		printf("send to queue successfully\r\n");
	
    //http_server_serve(newconn);
  }
}


void handleLongConnTask(void * pvParameters)
{
	NewconnIfo sNewconnIfo;
	//fd_set FdRead;
	int ret;
	int recLen = 0;
    int jasonLen = 0;
	int recStatus = REC_JASON_HEAD;
	mxgHeader smxgHeader;
	unsigned char ReCvBuf[1024] = {0};
	//unsigned char jasonReCvBuf[1024] = {0};
	struct timeval timeVal = {0};
	struct json_object*  cmd_obj = NULL;
	struct json_object*  temp_obj = NULL;
	const char* type = NULL;
	const char* addr = NULL;
	int value = 0;
	char temp[32] = {0};
	//int  tempVal;
	int headIndex, index;

	//timeVal.tv_sec = 2;
	//timeVal.tv_usec =0;
		
	printf("handleLongConnTask\r\n");
	for(;;)
	{

		if( xNewConnQue != NULL )
		    {	
				printf("wait a new connection\r\n");
				xQueueReceive( xNewConnQue, &( sNewconnIfo ), portMAX_DELAY);
				printf("get new conn\r\n");
				 //FD_ZERO(&FdRead);
				 //FD_SET(sNewconnIfo.newconn, &FdRead);
				while(1)
				{
					//printf("start rec\r\n");
				 	ret = read(sNewconnIfo.newconn, ReCvBuf, sizeof(ReCvBuf)); 
					if(ret <= 0)
					{
						printf("socket disconnect....\r\n");
						close(sNewconnIfo.newconn);
						break;
					}
					//printf("ReCvBuf len = %d ---->%s\r\n", ret, ReCvBuf)	;	
					recLen += ret;
					splitJsonTCPStr(ReCvBuf, ret);
					memset(ReCvBuf , 0x0, ret);
					//continue;
					
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

void jsonActionTask(void * pvParameters)
{
	 char *bufFilled;
	 const portTickType xDelay = 500 / portTICK_RATE_MS;
	 printf("jsonActionTask\r\n");
	 for(;;)
	 {
 		if( xJsonPakFilledQue != NULL )
	    {
			//xSemaphoreTake( xJsonQueueSemp, portMAX_DELAY);
			xQueueReceive( xJsonPakFilledQue, ( void * )&bufFilled, portMAX_DELAY);
			//printf("get json----------> %s\r\n",bufFilled);
			memset(bufFilled, 0x0, strlen(bufFilled));
			if( xQueueSend( xJsonPakEmptyQue, ( void * )&bufFilled, ( portTickType ) 10 ) != pdPASS )
	        {
	            printf("xJsonPakEmptyQue Failed to post the message, even after 10 ticks.\r\n");
	        }
			//xSemaphoreGive( xJsonQueueSemp );	
		}
		else
		{
			printf("xJsonPakFilledQue is NULL and this task is about to suspend\r\n");
			// Suspend ourselves.
			 vTaskSuspend( NULL );
			
		}

		//vTaskDelay( xDelay );
	   	
	 } 
}

//void handleNewConnTask(void * pvParameters)
//{
//	NewconnIfo sNewconnIfo;
//	int Socket;
//	fd_set FdRead;
//	int ret;
//	struct timeval timeVal = {0};
//	unsigned char ReCvBuf[128] = {0};
//
//	unsigned char *Ret = "test";
//
//	timeVal.tv_sec = 2;
//	timeVal.tv_usec = 0;
//
//	//ReCvBuf = (unsigned char *)malloc(512);
//	
//
//	//FD_ZERO(&FdRead);
//	
//
//	for(;;)
//		{
//
//			if( xNewConnQue != NULL )
//			    {
//					memset(ReCvBuf, 0x00, 128);
//			        // Receive a message on the created queue.  Block for 10 ticks if a
//			        // message is not immediately available.
//			        xQueueReceive( xNewConnQue, &( sNewconnIfo ), portMAX_DELAY);
//				      
//				   FD_ZERO(&FdRead);
//				   FD_SET(sNewconnIfo.newconn, &FdRead);
//				    
//					ret = select(sNewconnIfo.newconn + 1, &FdRead, NULL, NULL, &timeVal);
//					if(0 == ret)
//						{
//							FD_CLR(sNewconnIfo.newconn, &FdRead);
//							close(sNewconnIfo.newconn);
//							continue;
//						}
//					else if(-1 == ret)
//						{
//							FD_CLR(sNewconnIfo.newconn, &FdRead);
//							close(sNewconnIfo.newconn);
//							continue;
//						}
//					
//						
//					if(FD_ISSET(sNewconnIfo.newconn, &FdRead))
//						{
//							ret = recv(sNewconnIfo.newconn, ReCvBuf, sizeof(ReCvBuf)-1, 0);
//							if(ret < 0)
//								{
//									FD_CLR(sNewconnIfo.newconn, &FdRead);
//									close(sNewconnIfo.newconn);
//									continue;
//								}
//							//printf("------%s num = %d\r\n", ReCvBuf, ret);
//							send(sNewconnIfo.newconn, ReCvBuf, ret, 0);
//							//send(sNewconnIfo.newconn, Ret, strlen(Ret), 0);
//							FD_CLR(sNewconnIfo.newconn, &FdRead);
//							close(sNewconnIfo.newconn);
//							continue;
//						}
//
//					
//						
//					
//			    }
//			else
//				{
//					//printf("xNewConnQue is NULL and this task is about to suspend\r\n");
//					// Suspend ourselves.
//    				 vTaskSuspend( NULL );
//					
//				}
//
//		}
//	 
//}

void my_server_socket_init()
{
	int index;	

	xNewConnQue = xQueueCreate( FD_SETSIZE, sizeof(NewconnIfo) );
	if(NULL == xNewConnQue )
		{
			printf("creat xNewConnQue failed.......\r\n");
			return;
		}
	xJsonPakEmptyQue = xQueueCreate( JSONDATANUM, sizeof(unsigned char *) );
	if(NULL == xJsonPakEmptyQue )
		{
			printf("creat xJsonPakEmptyQue failed.......\r\n");
			return;
		}
	xJsonPakFilledQue = xQueueCreate( JSONDATANUM, sizeof(unsigned char *) );
	if(NULL == xJsonPakFilledQue )
		{
			printf("creat xJsonPakFilledQue failed.......\r\n");
			return;
		}


	for(index = 0; index < JSONDATANUM; index++)
	{
		//printf("&jsonData[%d][0] = 0x%x\r\n", index, &jsonData[index][0]);
		arrayForJsonDataPrt[index] =  &jsonData[index][0];
		 if( xQueueSend( xJsonPakEmptyQue, arrayForJsonDataPrt+index, ( portTickType ) 10 ) != pdPASS )
        {
            printf("xJsonPakEmptyQue Failed to post the message index = %d, even after 10 ticks.\r\n", index);
        }
	}
		
		
	xTaskCreate(handleLongConnTask, "handleLongConnTask", DEFAULT_THREAD_STACKSIZE*2, NULL,HANDLENEWCONN_TASK_PRIO, NULL);
	xTaskCreate(jsonActionTask, "jsonActionTask", configMINIMAL_STACK_SIZE, NULL,JSONACTION_TASK_PRIO, NULL);
	//xTaskCreate(handleNewConnTask, "handleNewConn", configMINIMAL_STACK_SIZE, NULL,HANDLENEWCONN_TASK_PRIO, NULL);
	//xTaskCreate(handleNewConnTask, "handleNewConn", configMINIMAL_STACK_SIZE, NULL,HANDLENEWCONN_TASK_PRIO, NULL);
  	sys_thread_new("MySocket", my_server_socket_thread, NULL, DEFAULT_THREAD_STACKSIZE, MYSOCKET_THREAD_PRIO);
}


