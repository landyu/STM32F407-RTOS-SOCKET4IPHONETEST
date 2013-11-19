/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  __JSONACTION_H__
#define  __JSONACTION_H__

#ifdef __cplusplus
extern "C" {
#endif    
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
/* Exported types ------------------------------------------------------------*/
#define JSONDATANUM 10
#define JSONDATAMAXLEN 128
typedef struct _jsonPak
{
	unsigned int length;
	unsigned char * pjsonData;
}jsonPak;
/* Exported constants --------------------------------------------------------*/
extern xQueueHandle xJsonPakFilledQue;
extern xQueueHandle xJsonPakEmptyQue;
extern unsigned char jsonData[JSONDATANUM][JSONDATAMAXLEN];
extern unsigned char *arrayForJsonDataPrt[JSONDATANUM];
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
int splitJsonTCPStr(unsigned char *dataBuf, unsigned int dataLen);
int isHead(unsigned char *buf, unsigned int bufLen);



#ifdef __cplusplus
}
#endif           

#endif
