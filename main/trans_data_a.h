#ifndef _TRANS_DATA_AH_H___


//WIFI LED输出配置
#define GPIO_OUTPUT_IO_0			GPIO_NUM_32
#define GPIO0_OUTPUT_PIN_SEL		(((uint64_t)1)<<GPIO_OUTPUT_IO_0) 
#define LED_WIFI_OnOff(b)			gpio_set_level(GPIO_OUTPUT_IO_0, !b)

//MQTT LED输出配置
#define GPIO_OUTPUT_IO_1			GPIO_NUM_33
#define GPIO1_OUTPUT_PIN_SEL		(((uint64_t)1)<<GPIO_OUTPUT_IO_1) 
#define LED_MQTT_OnOff(b)			gpio_set_level(GPIO_OUTPUT_IO_1, !b)

void ledInit();

//485方向
#define GPIO_485DIR_IO				GPIO_NUM_4
#define GPIO_485DIR_PIN_SEL			(((uint64_t)1)<<GPIO_485DIR_IO) 
void uart485Init();
void uart1485Send(char*buf,int len);

//
void showmqtt();
int setMqtt(char*host,int port);
int setMqttInfo(char*client_id,char*username,char*password);
//设置WIFI信息
int setConnectWifi(char*ssid,char*password);
int setOpenWifi(char*ssid,char*password);
int setCloseWifi();
//
int setMqttStartAndStop(int b);

//回复搜索硬件消息
int searchFrameData(char*sendBuf,const char* devUUID,
					const char* devname,const char* devflag,
					const char* publish,const char* subscr);

//UDP回复搜索硬件消息
void isSearchFrameData(const char* devUUID,
					const char* devname,
					const char* devflag);


//设置继电器工作状态
int saveRelayState(char* relayState,int len);
int readRelayState(char* relayState,int len);

#define _TRANS_DATA_AH_H___
#endif