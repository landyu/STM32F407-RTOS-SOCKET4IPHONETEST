/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  __MYSERVER_H__
#define  __MYSERVER_H__

#ifdef __cplusplus
extern "C" {
#endif    
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "semphr.h"

/* Exported types ------------------------------------------------------------*/


#define REC_JASON_HEAD 0
#define REC_JASON_DATA 1

typedef struct _newconnIfo
{
	int newconn;
	struct sockaddr_in *premotehost;
}NewconnIfo;

typedef struct _mxgHeader
{
   int headStart;
   int tag;
   int length;
   int headEnd;
}mxgHeader;



extern xQueueHandle xNewConnQue;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void my_server_socket_init(void);



#ifdef __cplusplus
}
#endif           

#endif
