#include "Adapter.h"
#include "../../paho.mqtt.c-master/src/Thread.h"
#include "../../paho.mqtt.c-master/src/MQTTClient.h"
#include "../../paho.mqtt.c-master/src/MQTTClientPersistence.h"
#if 0
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
#else
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


volatile int toStop = 0;


void cfinish(int sig)
{
	signal(SIGINT, NULL);
	toStop = 1;
}


//struct pubsub_opts opts =
//{
//	1, 0, 0, 0, "\n", 100,  	/* debug/app options */
//	NULL, NULL, 1, 0, 0, /* message options */
//	MQTTVERSION_DEFAULT, NULL, "paho-cs-pub", 0, 0, NULL, NULL, "localhost", "1883", NULL, 10, /* MQTT options */
//	NULL, NULL, 0, 0, /* will options */
//	0, NULL, NULL, NULL, NULL, NULL, NULL, /* TLS options */
//	0, {NULL, NULL}, /* MQTT V5 options */
//};


int myconnect(MQTTClient* client)
{
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
	MQTTClient_willOptions will_opts = MQTTClient_willOptions_initializer;
	int rc = 0;

	if (opts.verbose)
		printf("Connecting\n");

	if (opts.MQTTVersion == MQTTVERSION_5)
	{
		MQTTClient_connectOptions conn_opts5 = MQTTClient_connectOptions_initializer5;
		conn_opts = conn_opts5;
	}

	conn_opts.keepAliveInterval = opts.keepalive;
	conn_opts.username = opts.username;
	conn_opts.password = opts.password;
	conn_opts.MQTTVersion = opts.MQTTVersion;

	if (opts.will_topic) 	/* will options */
	{
		will_opts.message = opts.will_payload;
		will_opts.topicName = opts.will_topic;
		will_opts.qos = opts.will_qos;
		will_opts.retained = opts.will_retain;
		conn_opts.will = &will_opts;
	}

	if (opts.connection && (strncmp(opts.connection, "ssl://", 6) == 0 ||
			strncmp(opts.connection, "wss://", 6) == 0))
	{
		if (opts.insecure)
			ssl_opts.verify = 0;
		ssl_opts.CApath = opts.capath;
		ssl_opts.keyStore = opts.cert;
		ssl_opts.trustStore = opts.cafile;
		ssl_opts.privateKey = opts.key;
		ssl_opts.privateKeyPassword = opts.keypass;
		ssl_opts.enabledCipherSuites = opts.ciphers;
		conn_opts.ssl = &ssl_opts;
	}

	if (opts.MQTTVersion == MQTTVERSION_5)
	{
		MQTTProperties props = MQTTProperties_initializer;
		MQTTProperties willProps = MQTTProperties_initializer;
		MQTTResponse response = MQTTResponse_initializer;

		conn_opts.cleanstart = 1;
		response = MQTTClient_connect5(client, &conn_opts, &props, &willProps);
		rc = response.reasonCode;
	}
	else
	{
		conn_opts.cleansession = 1;
		rc = MQTTClient_connect(client, &conn_opts);
	}

	if (opts.verbose && rc == MQTTCLIENT_SUCCESS)
		printf("Connected\n");
	else if (rc != MQTTCLIENT_SUCCESS && !opts.quiet)
		fprintf(stderr, "Connect failed return code: %s\n", MQTTClient_strerror(rc));

	return rc;
}


int messageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* m)
{
	/* not expecting any messages */
	return 1;
}


void trace_callback(enum MQTTCLIENT_TRACE_LEVELS level, char* message)
{
	fprintf(stderr, "Trace : %d, %s\n", level, message);
}


int main(int argc, char** argv)
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
	
	MQTTClient client;
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
	opts.keepalive = 30;
	opts.qos = 0;
	opts.retained = 0;

	url = malloc(100);
	if (NULL == url)
	{
		return -1;
	}
	sprintf(url, "%s:%s", opts.host, opts.port);

	char* buffer = NULL;

	rc = MQTTClient_createWithOptions(&client, url, opts.clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	if (MQTTASYNC_SUCCESS != rc)
	{
		if (NULL != url)
		{
			free(url);
			url = NULL;
		}
		return -1;
	}
	
	rc = MQTTAsync_setCallbacks(client, client, NULL, messageArrived, NULL);
	if (MQTTASYNC_SUCCESS != rc)
	{
		if (NULL != url)
		{
			free(url);
			url = NULL;
		}
		return -1;
	}

	rc = MqttConnect(client)
	if (MQTTCLIENT_SUCCESS != rc)
	{
		if (NULL != url)
		{
			free(url);
			url = NULL;
		}
		return -1;
	}
	//-------------------------------------	
	while (!toStop)
	{
		int data_len = 0;
		int delim_len = 0;

		if (opts.stdin_lines)
		{
			buffer = malloc(opts.maxdatalen);

			delim_len = (int)strlen(opts.delimiter);
			do
			{
				int c = getchar();

				if (c < 0)
					goto exit;
				buffer[data_len++] = c;
				if (data_len > delim_len)
				{
					if (strncmp(opts.delimiter, &buffer[data_len - delim_len], delim_len) == 0)
						break;
				}
			} while (data_len < opts.maxdatalen);
		}
		else if (opts.message)
		{
			buffer = opts.message;
			data_len = (int)strlen(opts.message);
		}
		else if (opts.filename)
		{
			buffer = readfile(&data_len, &opts);
			if (buffer == NULL)
				goto exit;
		}
		if (opts.verbose)
			fprintf(stderr, "Publishing data of length %d\n", data_len);

		if (opts.MQTTVersion == MQTTVERSION_5)
		{
			MQTTResponse response = MQTTResponse_initializer;

			response = MQTTClient_publish5(client, opts.topic, data_len, buffer, opts.qos, opts.retained, &pub_props, NULL);
			rc = response.reasonCode;
		}
		else
			rc = MQTTClient_publish(client, opts.topic, data_len, buffer, opts.qos, opts.retained, NULL);
		if (opts.stdin_lines == 0)
			break;

		if (rc != 0)
		{
			myconnect(client);
			if (opts.MQTTVersion == MQTTVERSION_5)
			{
				MQTTResponse response = MQTTResponse_initializer;

				response = MQTTClient_publish5(client, opts.topic, data_len, buffer, opts.qos, opts.retained, &pub_props, NULL);
				rc = response.reasonCode;
			}
			else
				rc = MQTTClient_publish(client, opts.topic, data_len, buffer, opts.qos, opts.retained, NULL);
		}
		if (opts.qos > 0)
			MQTTClient_yield();
	}

exit:
	if (opts.filename || opts.stdin_lines)
		free(buffer);

	if (opts.MQTTVersion == MQTTVERSION_5)
		rc = MQTTClient_disconnect5(client, 0, MQTTREASONCODE_SUCCESS, NULL);
	else
		rc = MQTTClient_disconnect(client, 0);

 	MQTTClient_destroy(&client);

	return EXIT_SUCCESS;
}

#endif
