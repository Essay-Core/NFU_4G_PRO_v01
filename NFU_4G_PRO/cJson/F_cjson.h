#include "cJSON.h"
#include <stdlib.h>
#include <stdio.h>
using namespace std;

int getJsStr(char *json_str, const char* ch, char* ret_str);
int getJsInt(char *json_str, const char* ch, int *ret_int);

//���ļ�ת��Ϊjson��ʽ������
int insertJsonStr(char*jsonStr, const char* ch, char* subStr);