#include <string.h>
#include "jsonAction.h"




xQueueHandle xJsonPakFilledQue = NULL;
xQueueHandle xJsonPakEmptyQue = NULL;
unsigned char jsonData[JSONDATANUM][JSONDATAMAXLEN] = {0};
unsigned char *arrayForJsonDataPrt[JSONDATANUM];


int splitJsonTCPStr(unsigned char *dataBuf, unsigned int dataLen)
{
	unsigned int readIndex = 0;
	unsigned char seps[] = "$";
	char *token;
	char *lenToken = NULL;
	char *bufToBeFilled;
	unsigned int pakLen;
	int ret;
	{
		token = strtok( dataBuf, seps ); // C4996
	    // Note: strtok is deprecated; consider using strtok_s instead
	    while( token != NULL )
	    {
	        // While there are tokens in "string"
			lenToken = strstr(token, "packagelen=");
			if(lenToken)
			{
				lenToken+=strlen("packagelen=");
				pakLen = atoi(lenToken);
				//printf(" len = %d\r\n",pakLen);
				token = strtok( NULL, seps ); // C4996
				continue;
			}
			if(strlen(token) == pakLen)
			{
				//printf( "%s\r\n", token );
				//if( xSemaphoreTake( xJsonQueueSemp, ( portTickType ) 10 ) == pdTRUE )
				{
					if(xQueueReceive( xJsonPakEmptyQue, &bufToBeFilled, ( portTickType ) 10) != pdFALSE)
					{
						   //get an empty buf to store this json package
					   	memcpy(bufToBeFilled, token, pakLen);
					   //bufToBeFilled = 	&jsonData[0][0];
					   //printf("bufToBeFilled = 0x%x\r\n", bufToBeFilled);
					   if( xQueueSend( xJsonPakFilledQue, ( void * )&bufToBeFilled, ( portTickType ) 10 ) != pdPASS )
				        {
				            printf("xJsonPakFilledQue Failed to post the message, even after 10 ticks.\r\n");
				        }
					}
					else 
					{
						printf("xJsonPakEmptyQue is empty\r\n");
						//printf( "%s\r\n", &jsonData[9][0] );
					}
					//xSemaphoreGive( xJsonQueueSemp );
				}
			}
	        
	        // Get next token:
	        token = strtok( NULL, seps ); // C4996
	    }

	}


}
int isHead(unsigned char *buf, unsigned int bufLen)
{
	unsigned int index;	

	if(bufLen < 16)
	{
		printf("buf len = %d is less than 16\r\n", bufLen);
		return -2;
	}
	
	for(index = 0 ; index <= (bufLen - 16); index++)
   {
   		if(buf[index] == '\r')  //seems to find the head
		{
			 if(buf[index +1] == '\n' && buf[index +2] == '\r' && buf[index +3] == '\n' && buf[index +12] == '\r' && buf[index +13] == '\n' && buf[index +14] == '\r' && buf[index +15] == '\n')
			 return index;
		}
   }

   return -1;
		
}