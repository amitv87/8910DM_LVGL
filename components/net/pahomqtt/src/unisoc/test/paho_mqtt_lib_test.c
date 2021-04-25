#include "Adapter.h"
#include "../../paho.mqtt.c-master/src/Thread.h"
#include "../../paho.mqtt.c-master/src/MQTTAsync.h"
#include "../../paho.mqtt.c-master/src/MQTTClientPersistence.h"

#define PUB_TIMES (5)
#define LOG_HEAD "PahoMqtt "

struct mqtt_opts
{
	/* MQTT options */
	int MQTTVersion;
	char* topic;
	char* clientid;
	int qos;
	char* username;
	char* password;
	char* host;
	char* port;
    char* connection;
	int keepalive;
	int retained;

	/* message options */
	char* message;

	/* TLS options */
	int insecure;
	char* capath;
	char* cert;
	char* cafile;
	char* key;
	char* keypass;
	char* ciphers;
};

static struct mqtt_opts opts = {0};
static MQTTAsync pstClient = NULL;
	
static int messageArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
	char outstr[128] = {0};

	outstr[0] = '\0';
	snprintf(outstr, ((message->payloadlen)+strlen("zy_test ")), "zy_test %s", (char *)message->payload);
	OSI_LOGI(0, outstr);

	MQTTAsync_freeMessage(&message);
	MQTTAsync_free(topicName);

	return 1;
}

static int disconnected = 0;
static void onDisconnect(void* context, MQTTAsync_successData* response)
{
	char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Disconnection success.", LOG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

	disconnected = 1;
}
static void onDisconnectFailure(void* context, MQTTAsync_failureData* response)
{
	char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Disconnection fail.", LOG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

	disconnected = -1;
}

static int connected = 0;
static void onConnect(void* context, MQTTAsync_successData* response)
{
	char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Connection success.", LOG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

	connected = 1;
}
static void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
	char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Connection fail.", LOG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

	connected = -1;
}

static int published = 0;
static void onPublish(void* context, MQTTAsync_successData* response)
{
	char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Publish success.", LOG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

	published = 1;
}
static void onPublishFailure(void* context, MQTTAsync_failureData* response)
{
	char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Publish fail.", LOG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

	published = -1;
}

static int subscribed = 0;
static void onSubscribe(void* context, MQTTAsync_successData* response)
{
	char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Subscribe success.", LOG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

	subscribed = 1;
}
static void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
	char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Subscribe fail.", LOG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

	subscribed = -1;
}

static void MqttInit(void)
{
	MQTTAsync_init();

	return ;
}

static int MqttConnect(MQTTAsync client)
{
	MQTTAsync_connectOptions ConnOpts = MQTTAsync_connectOptions_initializer;
	int rc = 0;
	
	connected = 0;
	ConnOpts.onSuccess = onConnect;
	ConnOpts.onFailure = onConnectFailure;
	ConnOpts.cleansession = 1;
	
	ConnOpts.keepAliveInterval = opts.keepalive;
	ConnOpts.username = opts.username;
	ConnOpts.password = opts.password;
	ConnOpts.MQTTVersion = opts.MQTTVersion;
	ConnOpts.context = client;
	ConnOpts.automaticReconnect = 1;

	rc = MQTTAsync_connect(client, &ConnOpts);
	
	return rc;
}

static int MqttPublish(MQTTAsync client, int datalen, char* data)
{
	MQTTAsync_responseOptions PubRespOpts = MQTTAsync_responseOptions_initializer;
	int rc;

	published = 0;
	PubRespOpts.onSuccess = onPublish;
	PubRespOpts.onFailure = onPublishFailure;
	PubRespOpts.context = client;
	rc = MQTTAsync_send(client, opts.topic, datalen, data, opts.qos, opts.retained, &PubRespOpts);

	return rc;
}

static int MqttSubscribe(MQTTAsync client)
{
	MQTTAsync_responseOptions SubRespOpts = MQTTAsync_responseOptions_initializer;
	int rc;

	subscribed = 0;
	SubRespOpts.onSuccess = onSubscribe;
	SubRespOpts.onFailure = onSubscribeFailure;
	SubRespOpts.context = client;
	rc = MQTTAsync_subscribe(client, opts.topic, opts.qos, &SubRespOpts);

	return rc;
}

static int MqttDisconnect(MQTTAsync client)
{
	MQTTAsync_disconnectOptions DisconncOpts = MQTTAsync_disconnectOptions_initializer;
	int rc;
	
	disconnected = 0;
	DisconncOpts.onSuccess = onDisconnect;
	DisconncOpts.onFailure = onDisconnectFailure;
	DisconncOpts.context = client;
	rc = MQTTAsync_disconnect(client, &DisconncOpts);

	return rc;
}

static int test1()
{
	int rc = 0;					//0:success -1:fail
	char cOutstr[128] = {0};
	char* pcUrl = NULL;

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Test connection of paho mqtt lib.", LOG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);
	
	opts.MQTTVersion = MQTTVERSION_3_1_1;
	opts.host = "test.mosquitto.org";
	opts.port = "1883";
	opts.username = NULL;
	opts.password = NULL;
	opts.clientid = "UnisocClient";
	opts.keepalive = 30;
	opts.retained = 0;

	pcUrl = malloc(strlen("test.mosquitto.org") + strlen("1883") + 16);
	if (NULL == pcUrl)
	{
	    cOutstr[0] = '\0';
	    sprintf(cOutstr, "%s%s(%d):malloc falied.", LOG_HEAD, __func__, __LINE__);
	    OSI_LOGI(0, cOutstr);
		rc = -1;
		goto finish;
	}
	else
	{
		sprintf(pcUrl, "%s:%s", opts.host, opts.port);
	}

	MqttInit();
	rc = MQTTAsync_create(&pstClient, pcUrl, opts.clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	if (rc != MQTTASYNC_SUCCESS)
	{
		if (NULL != pcUrl)
		{
			free(pcUrl);
			pcUrl = NULL;
		}
		rc = -1;
		goto finish;
	}
	
	rc = MQTTAsync_setCallbacks(pstClient, pstClient, NULL, messageArrived, NULL);
	if (rc != MQTTASYNC_SUCCESS)
	{
		if (NULL != pcUrl)
		{
			free(pcUrl);
			pcUrl = NULL;
		}

		MQTTAsync_destroy(&pstClient);
		rc = -1;
		goto finish;
	}

	MqttConnect(pstClient);
	while (0 == connected)
	{
		osiThreadSleep(100);
	}

	if (1 == connected)
	{	
		rc = 0;
	}
	else
	{
		MQTTAsync_destroy(&pstClient);
		rc = -1;
	}
	
finish:
    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Test connection finish of paho mqtt lib. rc=%d", LOG_HEAD, __func__, __LINE__, rc);
    OSI_LOGI(0, cOutstr);

	if (NULL != pcUrl)
	{
		free(pcUrl);
		pcUrl = NULL;
	}
        
	return rc;
}

static int test2()
{
	int rc = 0;					//0:success -1:fail
	char cOutstr[128] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Test subscribe of paho mqtt lib.", LOG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

	opts.topic = "/unisoc/topic/test";
	opts.qos = 0;

	MqttSubscribe(pstClient);
	while (0 == subscribed)
	{
		osiThreadSleep(100);
	}
	if (1 == subscribed)
	{	
		rc = 0;
	}
	else
	{
		rc = -1;
	}

//finish:
    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Test subscribe finish of paho mqtt lib. rc=%d", LOG_HEAD, __func__, __LINE__, rc);
    OSI_LOGI(0, cOutstr);
	return rc;
}

static int test3()
{
	int rc = 0;					//0:success -1:fail
	char cOutstr[128] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Test publish of paho mqtt lib.", LOG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

	opts.topic = "/unisoc/topic/test";
	opts.message = "message.test";
	opts.qos = 0;

	MqttPublish(pstClient, strlen(opts.message), opts.message);
	while (0 == published)
	{
		osiThreadSleep(100);
	}
	if (1 == published)
	{	
		rc = 0;
	}
	else
	{
		rc = -1;
	}

//finish:
    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Test publish finish of paho mqtt lib. rc=%d", LOG_HEAD, __func__, __LINE__, rc);
    OSI_LOGI(0, cOutstr);
	return rc;
}

static int test4()
{
	int rc = 0;					//0:success -1:fail
	char cOutstr[128] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Test disconnection of paho mqtt lib.", LOG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

	MqttDisconnect(pstClient);
	while (0 == disconnected)
	{
		osiThreadSleep(100);
	}
	if (1 == disconnected)
	{	
		rc = 0;
	}
	else
	{
		rc = -1;
	}

	MQTTAsync_destroy(&pstClient);
		
//finish:
    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Test disconnection finish of paho mqtt lib. rc=%d", LOG_HEAD, __func__, __LINE__, rc);
    OSI_LOGI(0, cOutstr);
	return rc;
}

int paho_mqtt_lib_test(void)
{
	int rc = 0;

	rc = test1();
	rc += test2();
	rc += test3();
	rc += test4();

	return rc;
}


