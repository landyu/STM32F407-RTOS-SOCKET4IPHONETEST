/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  __MYJASON_H__
#define  __MYJASON_H__

#ifdef __cplusplus
extern "C" {
#endif    
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int getJsonValueAsInt(const char* json_str,const char* key, int *var);
int getJsonValueAsDouble(const char* json_str,const char* key, double * var);
int getJsonValueAsBool(const char* json_str,const char* key, int * var);
int getJsonValueAsString(const char* json_str,const char* key,char* buff,int buff_size);


#ifdef __cplusplus
}
#endif           

#endif
