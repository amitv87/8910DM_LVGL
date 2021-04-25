#include <stdio.h>  
#include <stdlib.h>
#include <iostream>
#include <libxml/parser.h>  
#include <libxml/tree.h>  
#include <stdint.h>
#include <string.h>
#include <io.h>
#include "wingetopt.h"
#include "cis_config.h"


using namespace std;



typedef enum enum_cfgid
{
	cis_cfgid_init = 0x01,
	cis_cfgid_net = 0x02,
	cis_cfgid_sys = 0x03
}cfgid_t;



typedef struct st_data
{
	char* data;
	int   length;
}data_t;



#define CONFIG_MIN(a,b)             (a>b?b:a)
#define	CONFIG_SIZE			        (1024)
#define CONFIG_MAX_PATH		        (500)
#define CONFIG_BIGENDIAN            (1)


#define CONFIG_PUT_U16(p,v)          (putU16(p,(uint16_t)(v)))
#define CONFIG_PUT_U32(p,v)          (putU32(p,(uint32_t)(v)))


uint16_t makeU16(uint8_t* p);
uint32_t makeU32(uint8_t* p);

int headNodeParse(xmlNodePtr curNode, uint8_t* bufer);
int initNodeParse(xmlNodePtr curNode, uint8_t* bufer);
int netNodeParse(xmlNodePtr curNode, uint8_t* buffer);
int sysNodeParse(xmlNodePtr curNode, uint8_t* buffer);
int isFileExist(const char* path);



static int check_bigendian()
{
	union {
		unsigned long int i;
		unsigned char s[4];
	}c;

	c.i = 0x12345678;
	return (0x12 == c.s[0]);
}



void putU16(uint8_t* p, uint16_t v)
{
	uint8_t data[2] = { 0 };
#if CONFIG_BIGENDIAN
	if (check_bigendian()) {
		data[0] = (v) & 0xFF;
		data[1] = (v >> 8) & 0xFF;
	}
	else
	{
		data[0] = (v >> 8) & 0xFF;
		data[1] = (v) & 0xFF;
	}
#else
	if (check_bigendian()) {
		data[0] = (v >> 8) & 0xFF;
		data[1] = (v) & 0xFF;
	}
	else
	{
		data[0] = (v) & 0xFF;
		data[1] = (v >> 8) & 0xFF;
	}
#endif//CONFIG_BIGENDIAN
	memcpy(p, &data, 2);
}

void putU32(uint8_t* p, uint32_t v)
{
	uint8_t data[4] = { 0 };
#if CONFIG_BIGENDIAN
	if (check_bigendian()) {
		data[0] = (v) & 0xFF;
		data[1] = (v >> 8) & 0xFF;
		data[2] = (v >> 16) & 0xFF;
		data[3] = (v >> 24) & 0xFF;
	}
	else {
		data[0] = (v >> 24) & 0xFF;
		data[1] = (v >> 16) & 0xFF;
		data[2] = (v >> 8) & 0xFF;
		data[3] = (v) & 0xFF;
	}
#else
	if (check_bigendian()) {
		data[0] = (v >> 24) & 0xFF;
		data[1] = (v >> 16) & 0xFF;
		data[2] = (v >> 8) & 0xFF;
		data[3] = (v) & 0xFF;
	}
	else {
		data[0] = (v) & 0xFF;
		data[1] = (v >> 8) & 0xFF;
		data[2] = (v >> 16) & 0xFF;
		data[3] = (v >> 24) & 0xFF;
	}
#endif//CONFIG_BIGENDIAN
	memcpy(p, &data, 4);
}




static void prvHelp()
{
	printf("cmiot_configtool.exe <InputFile> [-o OutputFile] [-v] \n");
	printf("option:\n");
	printf("<InputFile> the path of the input file\n");
	printf("-o          the path of the output file,default 'out.bin'\n");
	printf("-v          the version of cmiot_configtool\n");
}

int main(int argc, char** argv) {
	xmlDocPtr xDoc;
	xmlNodePtr xCurrentNode;
	xmlChar *szNodeId;
	uint8_t *headNodeBuffer, *initNodeBuffer, *netNodeBuffer, *sysNodeBuffer;
	int32_t configLen, initNodeLen, netNodeLen, sysNodeLen, headNodeLen;
	uint8_t *configPtr;
	uint8_t * configBuffer;

	FILE *fd = NULL;
	int ch = 0;
	int result = 0;

	xDoc = NULL;
	headNodeBuffer = NULL;
	initNodeBuffer = NULL;
	netNodeBuffer = NULL;
	sysNodeBuffer = NULL;
	configBuffer = NULL;

	configLen = 0;
	initNodeLen = 0;
	netNodeLen = 0;
	sysNodeLen = 0;
	headNodeLen = 0;

	char OutputPath[CONFIG_MAX_PATH] = { 0 };

	if (argc <= 1)
	{
		prvHelp();
		goto END_TAG;
	}

	if (argv[1] == NULL || strlen(argv[1]) <= 0)
	{
		prvHelp();
		goto END_TAG;
	}

	if (isFileExist(argv[1]) <= -1)
	{
		printf("The input file is not exist.\n");
		prvHelp();
		goto END_TAG;
	}

	while ((ch = getopt(argc, argv, "o:v")) != -1)
	{
		switch (ch)
		{
		case 'v':
			printf("CMIOT_CONFIGTOOT [Version: %d.%d]\n", CONFIG_VERSION_MAJOR, CONFIG_VERSION_MINOR);
			break;
		case 'o':
		{
			memcpy(OutputPath, optarg, strlen(optarg));
		}
		break;
		default:
			prvHelp();
			goto END_TAG;
		}
	}

	printf("CMIOT_CONFIGTOOT [Version: %d.%d]\n", CONFIG_VERSION_MAJOR, CONFIG_VERSION_MINOR);

	const char *szDocName = argv[1];
	printf("InputFile:%s\n", szDocName);
	xDoc = xmlReadFile(szDocName, "UTF-8", XML_PARSE_RECOVER);
	if (NULL == xDoc)
	{
		cout << "the xmlReadFile():faild" << endl;
		goto END_TAG;
	}

	xCurrentNode = xmlDocGetRootElement(xDoc);
	if (NULL == xCurrentNode)
	{
		cout << "the xmlDocGetRootElement():failed" << endl;
		goto END_TAG;
	}

	if (xmlStrcasecmp(xCurrentNode->name, BAD_CAST "config") != 0)
	{
		cout << "the root_node is not config" << endl;
		goto END_TAG;
	}

	xmlNodePtr xmlNodeRead(xmlNodePtr Node);
	xCurrentNode = xCurrentNode->xmlChildrenNode;


	while (xCurrentNode)
	{
		if (xmlStrcasecmp(xCurrentNode->name, (const xmlChar*)"head") != 0)
		{
			xCurrentNode = xCurrentNode->next;
			continue;
		}

		headNodeBuffer = (uint8_t*)malloc(CONFIG_SIZE);
		if (NULL == headNodeBuffer) {
			printf("the head has no memory\n");
		}
		headNodeLen = headNodeParse(xCurrentNode, headNodeBuffer);
		if (headNodeLen <= 0)
		{
			printf("head parse failed.\n");
			goto END_TAG;
		}
		break;
	}

	if (headNodeBuffer == NULL)
	{
		printf("ERROR:not found config header.\n");
		goto END_TAG;
	}

	while (xCurrentNode)
	{
		if (xmlStrcasecmp(xCurrentNode->name, (const xmlChar*)"item") != 0)
		{
			xCurrentNode = xCurrentNode->next;
			continue;
		}

		int nodeid = 0;
		szNodeId = xmlGetProp(xCurrentNode, (const xmlChar*)"id");
		if (szNodeId == NULL)
		{
			continue;
		}
		nodeid = atoi((char*)szNodeId);
		if (nodeid <= 0)
		{
			continue;
		}

		switch (nodeid)
		{
		case cis_cfgid_init:
		{
			initNodeBuffer = (uint8_t*)malloc(CONFIG_SIZE);
			if (NULL == initNodeBuffer) {
				printf("ERROR:malloc failed.\n");
				goto END_TAG;
			}
			initNodeLen = initNodeParse(xCurrentNode, initNodeBuffer);
		}
		break;
		case cis_cfgid_net:
		{
			netNodeBuffer = (uint8_t*)malloc(CONFIG_SIZE);
			if (NULL == netNodeBuffer) {
				printf("ERROR:malloc failed.\n");
				goto END_TAG;
			}
			netNodeLen = netNodeParse(xCurrentNode, netNodeBuffer);
		}
		break;
		case  cis_cfgid_sys:
		{
			sysNodeBuffer = (uint8_t*)malloc(CONFIG_SIZE);
			if (NULL == sysNodeBuffer) {
				printf("ERROR:malloc failed.\n");
				goto END_TAG;
			}
			sysNodeLen = sysNodeParse(xCurrentNode, sysNodeBuffer);
		}
		break;
		}
		xmlFree(szNodeId);

		xCurrentNode = xCurrentNode->next;
	}

	configLen = headNodeLen + initNodeLen + netNodeLen + sysNodeLen;


	configBuffer = (uint8_t*)malloc(configLen);

	if (NULL == configBuffer) {
		printf("ERROR:malloc failed.\n");
		goto END_TAG;
	}
	configPtr = configBuffer;

	CONFIG_PUT_U16(headNodeBuffer + 1, configLen);
	memcpy(configPtr, headNodeBuffer, headNodeLen);
	configPtr += headNodeLen;
	memcpy(configPtr, initNodeBuffer, initNodeLen);
	configPtr += initNodeLen;
	memcpy(configPtr, netNodeBuffer, netNodeLen);
	configPtr += netNodeLen;
	memcpy(configPtr, sysNodeBuffer, sysNodeLen);
	configPtr += sysNodeLen;

	//TODO:
	//make filename and path from console;
	if (strlen(OutputPath) <= 0)
		memcpy(OutputPath, "./out.bin", strlen("./out.bin"));

	fd = fopen(OutputPath, "wb"); //argv[3] output file
	if (fd == NULL) {
		printf("output file invalid.\n");
		goto END_TAG;
	}
	fwrite(configBuffer, 1, configLen, fd);
	printf("FINISH TO FILE %s.\r\n", OutputPath);

	char *s = (char *)malloc(6);
	s[0] = '0';
	s[1] = 'x';
	printf("\n");
	for (int i = 0; i < configLen; i++) {
		sprintf_s(s + 2, 4, "%02x", configBuffer[i]);
		printf("%s", s);

		int comment_apn_last_index = 14 + configBuffer[14];
		int comment_usernamelength_last_index = comment_apn_last_index + 2;

		int comment_username_last_index = comment_usernamelength_last_index + configBuffer[comment_usernamelength_last_index];

		int comment_passwordlength_last_index = comment_username_last_index + 2;
		int comment_password_last_index = comment_passwordlength_last_index + configBuffer[comment_passwordlength_last_index];

		int comment_hostlength_last_index = comment_password_last_index + 2;
		int comment_host_last_index = comment_hostlength_last_index + configBuffer[comment_hostlength_last_index];

		int comment_userdatalength_last_index = comment_host_last_index + 2;
		int comment_userdata_last_index = comment_userdatalength_last_index + configBuffer[comment_userdatalength_last_index];

		int comment_f3_last_index = comment_userdata_last_index + 3;

		int comment_log_userdatalength_last_index = comment_f3_last_index + 5;
		int comment_log_userdata_last_index = comment_log_userdatalength_last_index + configBuffer[comment_log_userdatalength_last_index];

		//add comment
		if (i == 2 || i == 5)
		{
			printf(",\n");
		}
		else if (i == 8 || i == comment_f3_last_index)
		{
			printf(",\n    ");
		}
		else if (i == 10)
		{
			printf(" /*mtu*/, ");
		}
		else if (i == 11)
		{
			printf(" /*Link & bind type*/, ");
		}
		else if (i == 12)
		{
			printf(" /*BS DTLS ENABLED*/,\n    ");
		}
		else if (i == 14)
		{
			printf(" /*apn length*/, ");
		}
		else if (i == comment_apn_last_index)
		{
			printf(" /*apn: ");
			int j = 1;
			while (j < configBuffer[14] + 1)
			{
				printf("%c", configBuffer[14 + j]);
				j++;
			}
			printf("*/,\n    ");
		}
		else if (i == comment_usernamelength_last_index)
		{
			printf(" /*username length*/, ");
			if (comment_usernamelength_last_index == comment_username_last_index)
			{
				printf("/*username*/\n    ");
			}
		}
		else if (i == comment_username_last_index)
		{
			printf(" /*username: ");
			int j = 1;
			while (j < configBuffer[comment_usernamelength_last_index] + 1)
			{
				printf("%c", configBuffer[comment_usernamelength_last_index + j]);
				j++;
			}
			printf("*/,\n    ");
		}
		else if (i == comment_passwordlength_last_index)
		{
			printf(" /*password length*/, ");
			if (comment_passwordlength_last_index == comment_password_last_index)
			{
				printf("/*password*/\n    ");
			}
		}
		else if (i == comment_password_last_index)
		{
			printf(" /*password: ");
			int j = 1;
			while (j < configBuffer[comment_passwordlength_last_index] + 1)
			{
				printf("%c", configBuffer[comment_passwordlength_last_index + j]);
				j++;
			}
			printf("*/,\n    ");
		}
		else if (i == comment_hostlength_last_index)
		{
			printf(" /*host length*/, ");
		}
		else if (i == comment_host_last_index)
		{
			printf(" /*host: ");
			int j = 1;
			while (j < configBuffer[comment_hostlength_last_index] + 1)
			{
				printf("%c", configBuffer[comment_hostlength_last_index + j]);
				j++;
			}
			printf("*/,\n    ");
		}
		else if (i == comment_userdatalength_last_index)
		{
			printf(" /*userdata length*/, ");
		}
		else if (i == comment_userdata_last_index)
		{
			printf(" /*userdata: ");
			int j = 1;
			while (j < configBuffer[comment_userdatalength_last_index] + 1)
			{
				printf("%c", configBuffer[comment_userdatalength_last_index + j]);
				j++;
			}
			printf("*/,\n");
		}
		else if (i == comment_f3_last_index + 1)
		{
			//log config
			printf(" /*log config*/, ");
		}
		else if (i == comment_f3_last_index + 3)
		{
			//LogBufferSize
			printf(" /*LogBufferSize: %d", configBuffer[i - 1] * 256 + configBuffer[i]);
			printf("*/,\n    ");
		}
		else if (i == comment_log_userdatalength_last_index)
		{
			printf(" /*userdata length*/");
			if (comment_log_userdatalength_last_index == comment_log_userdata_last_index)
			{
				printf("/*userdata*/\n    ");
			}
			else
			{
				printf(", ");
			}
		}
		else if (i == comment_log_userdata_last_index)
		{
			printf(" /*userdata: ");
			int j = 1;
			while (j < configBuffer[comment_log_userdatalength_last_index] + 1)
			{
				printf("%c", configBuffer[comment_log_userdatalength_last_index + j]);
				j++;
			}
			printf("*/\n");
		}
		else
		{
			printf(", ");
		}

	}
	printf("\r\n");

END_TAG:

	if (fd != NULL)fclose(fd);
	if (headNodeBuffer != NULL)
		free(headNodeBuffer);
	if (initNodeBuffer != NULL)
		free(initNodeBuffer);
	if (netNodeBuffer != NULL)
		free(netNodeBuffer);
	if (sysNodeBuffer != NULL)
		free(sysNodeBuffer);

	if (configBuffer != NULL)
		free(configBuffer);

	if (xDoc != NULL)
		xmlFreeDoc(xDoc);

	printf("press enter key exit.\r\n");
	getchar();
	return 0;
}


/*
@return:
1: file path
2: dir path
-1: inavaliable path
*/
int isFileExist(const char* path)
{
	long handle;
	struct _finddata_t fileinfo;
	int rValue;

	handle = _findfirst(path, &fileinfo);
	if (handle != -1) {
		rValue = (fileinfo.attrib & _A_SUBDIR) ? 2 : 1;
	}
	else {
		rValue = -1;
	}
	_findclose(handle);

	return rValue;
}


int headNodeParse(xmlNodePtr curNode, uint8_t *buffer)
{
	int16_t ver = -1, cnt = -1;
	xmlChar* szKey;
	uint8_t *start, *buf;
	xmlNodePtr headNode = curNode->children;

	buf = buffer;
	start = buf;
	while (headNode)
	{
		if (xmlStrcasecmp(headNode->name, (const xmlChar*)"version") == 0)
		{
			szKey = xmlNodeGetContent(headNode);
			ver = atoi((const char*)szKey);
			xmlFree(szKey);
		}

		if (xmlStrcasecmp(headNode->name, (const xmlChar*)"cfgcnt") == 0)
		{
			szKey = xmlNodeGetContent(headNode);
			cnt = atoi((const char*)szKey);
			xmlFree(szKey);
		}

		headNode = headNode->next;
	}

	if (ver < 0)
	{
		printf("ERROR:not found version information.\n");
		return -1;
	}
	if (cnt < 0)
	{
		printf("ERROR:not found config count information.\n");
		return -1;
	}

	if (ver != CONFIG_VERSION_MAJOR)
	{
		printf("ERROR:version mismatch with %d vs %d\n", ver, CONFIG_VERSION_MAJOR);
		return -1;
	}


	*buf = ver << 4 | cnt;
	buf += sizeof(uint8_t);

	buf += sizeof(uint16_t);
	return buf - start;
}



int initNodeParse(xmlNodePtr curNode, uint8_t* buffer)
{
	xmlChar *szNodeName;
	uint8_t *nodeSizePtr;
	uint8_t *bufferPtr = buffer;

	xmlNodePtr initNode = curNode->children;
	while (initNode)
	{
		if (xmlStrcasecmp(initNode->name, (const xmlChar*)"data") != 0)
		{
			initNode = initNode->next;
			continue;
		}
		if (!xmlHasProp(initNode, (const xmlChar*)"name"))
		{
			initNode = initNode->next;
			continue;
		}

		szNodeName = xmlGetProp(initNode, (const xmlChar*)"name");

		//remove lifetime;
		/*
		if(xmlStrcasecmp(szNodeName,(const xmlChar*)"lifetime") == 0)
		{
		szKey = xmlNodeGetContent(initNode);
		lifetime = atoi((const char*)szKey);
		xmlFree(szKey);
		}*/

		xmlFree(szNodeName);
		initNode = initNode->next;
	}


	*bufferPtr = 0xF0 | cis_cfgid_init;
	nodeSizePtr = ++bufferPtr;
	bufferPtr += sizeof(uint16_t);

	CONFIG_PUT_U16(nodeSizePtr, bufferPtr - buffer);
	return bufferPtr - buffer;
}



int netNodeParse(xmlNodePtr curNode, uint8_t* buffer)
{
	uint8_t *bufferPtr, *nodeSizePtr;
	xmlChar *szAttrName, *szContext;
	xmlChar	*szExdataContext, *szExdataLen;

	int8_t linktype, bandtype, bootstrapEnabled, dtlsEnabled;
	int16_t mtu = 0;

	data_t dataAPN, dataUsername, dataPwd, dataHost, dataUserdata;

	linktype = -1;
	bandtype = -1;
	bootstrapEnabled = -1;
	dtlsEnabled = -1;

	szAttrName = szContext = NULL;
	szExdataContext = szExdataLen = NULL;


	bufferPtr = buffer;

	xmlNodePtr netNode = curNode->children;
	while (netNode)
	{
		if (xmlStrcasecmp(netNode->name, (const xmlChar*)"data") != 0)
		{
			netNode = netNode->next;
			continue;
		}

		if (!xmlHasProp(netNode, (const xmlChar*)"name"))
		{
			netNode = netNode->next;
			continue;
		}

		szAttrName = xmlGetProp(netNode, (const xmlChar*)"name");


		/*compare key name*/
		if (xmlStrcasecmp(szAttrName, (const xmlChar*)"mtu") == 0)
		{
			szContext = xmlNodeGetContent(netNode);
			mtu = atoi((const char*)szContext);
			xmlFree(szContext);
		}
		else if (xmlStrcasecmp(szAttrName, (const xmlChar*)"linktype") == 0)
		{
			szContext = xmlNodeGetContent(netNode);
			linktype = atoi((const char*)szContext);
			xmlFree(szContext);
		}
		else if (xmlStrcasecmp(szAttrName, (const xmlChar*)"bandtype") == 0)
		{
			szContext = xmlNodeGetContent(netNode);
			bandtype = atoi((const char*)szContext);
			xmlFree(szContext);
		}
		else if (xmlStrcasecmp(szAttrName, (const xmlChar*)"BootstrapEnabled") == 0)
		{
			szContext = xmlNodeGetContent(netNode);
			bootstrapEnabled = atoi((const char*)szContext);
			xmlFree(szContext);
		}
		else if (xmlStrcasecmp(szAttrName, (const xmlChar*)"DTLSEnabled") == 0)
		{
			szContext = xmlNodeGetContent(netNode);
			dtlsEnabled = atoi((const char*)szContext);
			xmlFree(szContext);
		}


		else if (xmlStrcasecmp(szAttrName, (const xmlChar*)"apn") == 0)
		{
			szContext = xmlNodeGetContent(netNode);
			dataAPN.data = (char*)xmlStrdup(szContext);
			dataAPN.length = strlen((const char*)szContext);

			xmlFree(szContext);
		}

		else if (xmlStrcasecmp(szAttrName, (const xmlChar*)"username") == 0)
		{
			szContext = xmlNodeGetContent(netNode);
			dataUsername.data = (char*)xmlStrdup(szContext);
			dataUsername.length = strlen((const char*)szContext);

			xmlFree(szContext);
		}

		else if (xmlStrcasecmp(szAttrName, (const xmlChar*)"password") == 0)
		{
			szContext = xmlNodeGetContent(netNode);
			dataPwd.data = (char*)xmlStrdup(szContext);
			dataPwd.length = strlen((const char*)szContext);

			xmlFree(szContext);
		}

		else if (xmlStrcasecmp(szAttrName, (const xmlChar*)"host") == 0)
		{
			szContext = xmlNodeGetContent(netNode);
			dataHost.data = (char*)xmlStrdup(szContext);
			dataHost.length = strlen((const char*)szContext);

			xmlFree(szContext);
		}
		else if (!xmlStrcasecmp(szAttrName, (const xmlChar*)"Userdata"))
		{
			dataUserdata.data = (char*)xmlNodeGetContent(netNode);
			dataUserdata.length = strlen(dataUserdata.data);
		}
		xmlFree(szAttrName);
		netNode = netNode->next;
	}


	*bufferPtr = 0xF0 | cis_cfgid_net;

	nodeSizePtr = ++bufferPtr;
	bufferPtr += sizeof(uint16_t);

	if (mtu <= 256) {
		mtu = 256;
		printf("WARNING:not found mtu.use default %d\n", mtu);
	}
	if (mtu > 1280) {
		mtu = 1280;
		printf("WARNING:invalid mtu.use default %d\n", mtu);
	}

	CONFIG_PUT_U16(bufferPtr, mtu);
	bufferPtr += sizeof(uint16_t);


	if (linktype <= -1) {
		linktype = 0;
		printf("WARNING:not found linktype.use default %d\n", linktype);
	}

	if (bandtype <= -1) {
		bandtype = 0;
		printf("WARNING:not found bandtype.use default %d\n", bandtype);
	}

	if (bootstrapEnabled <= -1) {
		bootstrapEnabled = 1;
		printf("WARNING:not found bootstrap enabled.use default %d\n", bootstrapEnabled);
	}

	if (dtlsEnabled <= -1) {
		dtlsEnabled = 0;
		printf("WARNING:not found dtls enabled.use default %d\n", dtlsEnabled);
	}

	*bufferPtr = (uint8_t)((linktype << 4) & 0xF0 | bandtype & 0x0F);
	bufferPtr += sizeof(uint8_t);

	*bufferPtr = (uint8_t)((bootstrapEnabled << 7) & 0x80 | (dtlsEnabled << 6) & 0x40);
	bufferPtr += sizeof(uint8_t);


	CONFIG_PUT_U16(bufferPtr, dataAPN.length);
	bufferPtr += sizeof(uint16_t);
	if (dataAPN.length > 0) {
		memcpy(bufferPtr, dataAPN.data, dataAPN.length);
		bufferPtr += dataAPN.length;
	}

	CONFIG_PUT_U16(bufferPtr, dataUsername.length);
	bufferPtr += sizeof(uint16_t);
	if (dataUsername.length > 0) {
		memcpy(bufferPtr, dataUsername.data, dataUsername.length);
		bufferPtr += dataUsername.length;
	}

	CONFIG_PUT_U16(bufferPtr, dataPwd.length);
	bufferPtr += sizeof(uint16_t);
	if (dataPwd.length > 0) {
		memcpy(bufferPtr, dataPwd.data, dataPwd.length);
		bufferPtr += dataPwd.length;
	}

	if (0 == dataHost.length || NULL == dataHost.data) {
		printf("ERROR:invalid or not found host info.\n");
		return 0;
	}

	CONFIG_PUT_U16(bufferPtr, dataHost.length);
	bufferPtr += sizeof(uint16_t);
	if (dataHost.length > 0) {
		memcpy(bufferPtr, dataHost.data, dataHost.length);
		bufferPtr += dataHost.length;
	}

	CONFIG_PUT_U16(bufferPtr, dataUserdata.length);
	bufferPtr += sizeof(uint16_t);
	if (dataUserdata.length > 0) {
		memcpy(bufferPtr, dataUserdata.data, dataUserdata.length);
		bufferPtr += dataUserdata.length;
	}


	CONFIG_PUT_U16(nodeSizePtr, bufferPtr - buffer);
	xmlFree(szExdataContext);
	return bufferPtr - buffer;
}



int sysNodeParse(xmlNodePtr curNode, uint8_t* buffer)
{
	uint8_t *bufferPtr, *startPtr, *nodeSizePtr;
	xmlChar *szNodeName = NULL, *szKey = NULL;
	xmlChar *szExinfoLen = NULL;
	int8_t LogEnabled, LogExtOutput, LogOutputType, LogOutputLevel;
	int16_t LogBufferSize;
	data_t dataUserdata;


	bufferPtr = buffer;
	LogEnabled = LogExtOutput = LogOutputType = LogOutputLevel = -1;
	LogBufferSize = -1;

	xmlNodePtr sysNode = curNode->children;
	while (sysNode)
	{
		if (xmlStrcasecmp(sysNode->name, (const xmlChar*)"data") != 0)
		{
			sysNode = sysNode->next;
			continue;
		}

		if (!xmlHasProp(sysNode, (const xmlChar*)"name"))
		{
			sysNode = sysNode->next;
			continue;
		}

		szNodeName = xmlGetProp(sysNode, (const xmlChar*)"name");
		if (!xmlStrcasecmp(szNodeName, (const xmlChar*)"LogEnabled"))
		{
			szKey = xmlNodeGetContent(sysNode);
			LogEnabled = atoi((const char*)szKey);
			xmlFree(szKey);
		}
		else if (!xmlStrcasecmp(szNodeName, (const xmlChar*)"LogExtOutput"))
		{
			szKey = xmlNodeGetContent(sysNode);
			LogExtOutput = atoi((const char*)szKey);
			xmlFree(szKey);
		}
		else if (!xmlStrcasecmp(szNodeName, (const xmlChar*)"LogOutputType"))
		{
			szKey = xmlNodeGetContent(sysNode);
			LogOutputType = atoi((const char*)szKey);
			xmlFree(szKey);
		}
		else if (!xmlStrcasecmp(szNodeName, (const xmlChar*)"LogOutputLevel"))
		{
			szKey = xmlNodeGetContent(sysNode);
			LogOutputLevel = atoi((const char*)szKey);
			xmlFree(szKey);
		}
		else if (!xmlStrcasecmp(szNodeName, (const xmlChar*)"LogBufferSize"))
		{
			szKey = xmlNodeGetContent(sysNode);
			LogBufferSize = atoi((const char*)szKey);
			xmlFree(szKey);
		}
		else if (!xmlStrcasecmp(szNodeName, (const xmlChar*)"Userdata"))
		{
			dataUserdata.data = (char*)xmlNodeGetContent(sysNode);
			dataUserdata.length = strlen(dataUserdata.data);
		}
		xmlFree(szNodeName);
		sysNode = sysNode->next;
	}

	startPtr = bufferPtr;
	*bufferPtr = 0xF0 | cis_cfgid_sys;

	nodeSizePtr = ++bufferPtr;
	bufferPtr += sizeof(uint16_t);

	if (LogEnabled <= -1)
	{
		LogEnabled = 0;
		printf("WARNING:not found debugMode.use default %d\n", 0);
	}

	if (LogEnabled <= -1)
	{
		LogEnabled = 0;
		printf("WARNING:not found debugMode.use default %d\n", 0);
	}
	if (LogExtOutput <= -1)
	{
		LogExtOutput = 0;
		printf("WARNING:not found ExtendMode.use default %d\n", 0);
	}
	if (LogOutputType <= -1)
	{
		LogOutputType = 0;
		printf("WARNING:not found OutputMode.use default %d\n", 0);
	}
	if (LogOutputLevel <= -1)
	{
		LogOutputLevel = 0;
		printf("WARNING:not found OutputLevel.use default %d\n", 0);
	}
	if (LogBufferSize <= -1)
	{
		LogBufferSize = 0;
		printf("WARNING:not found BufferSize.use default %d\n", 200);
	}

	*bufferPtr = LogEnabled << 7 | LogExtOutput << 6 | LogOutputType << 4 | LogOutputLevel;
	bufferPtr += sizeof(uint8_t);

	CONFIG_PUT_U16(bufferPtr, LogBufferSize);
	bufferPtr += sizeof(uint16_t);


	CONFIG_PUT_U16(bufferPtr, dataUserdata.length);
	bufferPtr += sizeof(uint16_t);
	if (dataUserdata.length > 0)
	{
		memcpy(bufferPtr, dataUserdata.data, dataUserdata.length);
		bufferPtr += dataUserdata.length;
	}

	CONFIG_PUT_U16(nodeSizePtr, bufferPtr - startPtr);

	return bufferPtr - startPtr;
}
