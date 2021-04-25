#include "Adapter.h"
#include "../../paho.mqtt.c-master/src/Thread.h"
#include "../../paho.mqtt.c-master/src/MQTTAsync.h"
#include "../../paho.mqtt.c-master/src/MQTTClientPersistence.h"

#define PUB_TIMES (5)

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
	disconnected = 1;
}
static void onDisconnectFailure(void* context, MQTTAsync_failureData* response)
{
	disconnected = -1;
}

static int connected = 0;
static void onConnect(void* context, MQTTAsync_successData* response)
{
	connected = 1;
}
static void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
	connected = -1;
}

static int published = 0;
static void onPublish(void* context, MQTTAsync_successData* response)
{
	published = 1;
}
static void onPublishFailure(void* context, MQTTAsync_failureData* response)
{
	published = -1;
}

static int subscribed = 0;
static void onSubscribe(void* context, MQTTAsync_successData* response)
{
	subscribed = 1;
}
static void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
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

//static void MqttUninit(void)
//{
//	MQTTAsync_uninit();
//}

int paho_mqtt_client(void)
{
	char strings[128] = {0};
	int times = 0;
	char *host = "183.230.40.39";//"36.110.97.243";
	char *port = "6002";//"1885";
	char *username = "173288";//"zymqtt";
	char *password = "abc123";//"zymqtt";
	char *topic = "test";
	char *message = "1234567890";
	char *clientid = "44361495";//"RDA-Mqtt-Client";
	
	MQTTAsync client;
	char* url = NULL;
	int rc = 0;

	opts.MQTTVersion = MQTTVERSION_3_1_1;
	opts.host = host;
	opts.port = port;
	opts.username = username;
	opts.password = password;
	opts.topic = topic;
	opts.message = message;
	opts.clientid = clientid;
	opts.keepalive = 999;
	opts.qos = 0;
	opts.retained = 0;

	url = malloc(100);
	if (NULL == url)
	{
		return -1;
	}
	sprintf(url, "%s:%s", opts.host, opts.port);

	//init
	MqttInit();

	rc = MQTTAsync_create(&client, url, opts.clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	if (rc != MQTTASYNC_SUCCESS)
	{
		if (NULL != url)
		{
			free(url);
			url = NULL;
		}
		return -1;
	}
	
	rc = MQTTAsync_setCallbacks(client, client, NULL, messageArrived, NULL);
	if (rc != MQTTASYNC_SUCCESS)
	{
		if (NULL != url)
		{
			free(url);
			url = NULL;
		}
		return -1;
	}

	//g_engine = engine;

	//connect
	MqttConnect(client);
	while (0 == connected)
	{
		osiThreadSleep(100);
	}
	
	//subscribe
	MqttSubscribe(client);
	while (0 == subscribed)
	{
		osiThreadSleep(100);
	}

	//publish
	do
	{
		strings[0] = '\0';
		sprintf(strings, "rda mqtt client time:%ld, times:%d.", ((long unsigned int)(time(NULL))), times);
		MqttPublish(client, strlen(strings), strings);
		while (0 == published)
		{
			osiThreadSleep(100);
		}

		osiThreadSleep(1000);
		//published = 0;
	}while ((++times) < PUB_TIMES);

	osiThreadSleep(100);

	//disconnect
	MqttDisconnect(client);
	while (0 == disconnected)
	{
		osiThreadSleep(100);
	}	

	//destroy
	MQTTAsync_destroy(&client);

	//uninit
	//MqttUninit();

	if (NULL != url)
	{
		free(url);
		url = NULL;
	}

	return 0;
}

