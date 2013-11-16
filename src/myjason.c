#include <string.h>
#include <stdio.h>
#include<stdlib.h>
#include "myjason.h"

//const char* json_str = "{ \"type\": \"EIBS\", \"addr\": \"0/0/0\", \"value\": 1 , \"happy\":true, \"val\":-1.005}";

int get_json_str(const char* js_str, const char* key, char* buff,int buff_size);

int getJsonValueAsInt(const char* json_str,const char* key, int * var)
{
    char temp[32] = {0};
	unsigned int len;
    int ret = get_json_str(json_str,key,temp,sizeof temp);
    if (ret)
    {
        return -1;
    }

   len = strlen(temp);
    if (len > 128)
    {
        return -1;
    }

    if (temp[0] == '"' || temp[len-1] == '"')
    {//Json值非int格式
        return -1;
    }

    *var = atol(temp);
    return 0;
}

int getJsonValueAsDouble(const char* json_str,const char* key, double * var)
{
    char temp[32] = {0};
	unsigned int len;
    int ret = get_json_str(json_str,key,temp,sizeof temp);
    if (ret)
    {
        return -1;
    }

    len = strlen(temp);
    if (len > 128)
    {
        return -1;
    }

    if (temp[0] == '"' || temp[len-1] == '"')
    {//Json值非int格式
        return -1;
    }

    *var = atof(temp);
    return 0;
}

int getJsonValueAsBool(const char* json_str,const char* key, int * var)
{
    char temp[32] = {0};
	unsigned int len;
    int ret = get_json_str(json_str,key,temp,sizeof temp);
    if (ret)
    {
        return -1;
    }

   len = strlen(temp);
    if (len > 128)
    {
        return -1;
    }

    if (temp[0] == '"' || temp[len-1] == '"')
    {//Json值非int格式
        return -1;
    }

    *var = temp[0]=='t'?0:-1;
    return 0;
}

int getJsonValueAsString(const char* json_str,const char* key,char* buff,int buff_size)
{
    char temp[32] = {0};
	unsigned int len;
    int ret = get_json_str(json_str,key,temp,sizeof temp);
    if (ret)
    {
        return -1;
    }

    len = strlen(temp);
    if (len > 128)
    {
        return -1;
    }

    if (temp[0] != '"' || temp[len-1] != '"')
    {//Json值不为字符串格式
        return -1;
    }

    strncpy(buff,temp+1,len-2);
    return 0;
}

int get_json_str(const char* js_str, const char* key, char* buff,int buff_size)
{
    char _key[32] = {0};
	const char* start;
	int value_size;
	int i, j;
	const char* ptr;

	sprintf(_key,"\"%s\"",key);
    ptr = strstr(js_str, _key);
    if (ptr)
    {
        const char* end = strstr(ptr,",");
        if (!end)
        {
            end = strstr(ptr,"}");
            if (!end)
            {
                return -1;
            }
        }

        start = ptr+strlen((const char*)_key)+1;
        memset(_key, 0, sizeof _key);

        value_size = end-start;
        if (value_size>=32)
        {//too large
            return -1;
        }

        strncpy(_key, start, value_size);
        for (i=0,j=0;i<value_size;++i)
        {
            char _c = _key[i];
            if (_c != ' ')
            {
                buff[j++] = _c;
            }
        }

        return 0;
    }

    return -1;
}

int isHead(unsigned char *buf, unsigned int bufLen)
{
	unsigned int index;	

	if(bufLen < 9)
	{
		printf("buf len = %d is less than 9\r\n", bufLen);
		return -2;
	}
	
	for(index = 0 ; index <= (bufLen - 9); index++)
   {
   		if(buf[index] == '\r')  //seems to find the head
		{
			 if(buf[index +1] == '\n' && buf[index +2] == '\r' && buf[index +3] == '\n' && buf[index +12] == '\r' && buf[index +13] == '\n' && buf[index +14] == '\r' && buf[index +15] == '\n')
			 return index;
		}
   }

   return -1;
		
}

//int main(int argc,char*[])
//{
//    //strstr(json_str,"\"type\":");
//    char temp[32] = {0};
//    bool ret = getJsonValueAsString(json_str,"type",temp,sizeof temp);
//
//    int aa = 9;
//    ret = getJsonValueAsInt(json_str,"value",aa);
//
//    double bb = 0.5;
//    ret = getJsonValueAsDouble(json_str,"val",bb);
//
//    bool cc = false;
//    ret = getJsonValueAsBool(json_str,"happy",cc);
//
//
//    return 0;
//}
