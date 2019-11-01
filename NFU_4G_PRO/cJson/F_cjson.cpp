#include "stdafx.h"
#include "F_cjson.h"

int getJsStr(char *json_str, const char* ch, char* ret_str)
{
	if (!json_str || !ch)
		return -1;

	//��ȡjson���ݾ��
	cJSON *json = cJSON_Parse(json_str);
	//��ȡĳ������ĳ�Ա
	cJSON *item = cJSON_GetObjectItem(json, ch);

	//�����Ա�ַ���
	if (item->type == cJSON_String)
	{
		sprintf(ret_str, "%s", item->valuestring);
	}

	return 0;
}

int getJsInt(char *json_str, const char* ch, int *ret_int)
{
	if (!json_str || !ch)
		return -1;

	//��ȡjson���ݾ��
	cJSON *json = cJSON_Parse(json_str);

	//��ȡĳ������ĳ�Ա
	cJSON *item = cJSON_GetObjectItem(json, ch);

	if (item->type == cJSON_Number)
	{
		*ret_int = item->valueint;
	}

	return 0;
}

//���ļ�ת��Ϊjson��ʽ������
int insertJsonStr(char*jsonStr, const char* ch, char* subStr)
{
	if (!jsonStr || !ch || !subStr)
	{
		return -1;
	}

	cJSON *root;
	//����json���ݶ���,������һ��{}
	root = cJSON_CreateObject();

	//�����ֵ���ַ���
	cJSON_AddStringToObject(root, ch, subStr);

	//���ݸ�ʽת��Ϊutf-8
	char *s = cJSON_PrintUnformatted(root);

	if (root)
	{
		//ɾ��json����
		cJSON_Delete(root);
	}

	strcpy(jsonStr, s);

	return 0;
}