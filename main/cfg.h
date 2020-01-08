#ifndef __ZH_CFG__H__
#define __ZH_CFG__H__

//
#define INIT_HARDWARE_FLAG			1111112
//数据储存位置
#define SECTOR_CFG_DATA			400		//0x190000
#define SECTOR_CFG_JSON			511		//0x1FF000
#define SECTOR_WEBPAGE_1		512		//0x200000
#define SECTOR_WEBPAGE_2		514		//0x202000
#define SECTOR_WEBPAGE_3		516		//0x204000

typedef enum _EzhWifiMode
{
	ezhWifiModeUndo, //不初始化WIFI
	ezhWifiModeAP,
	ezhWifiModeSTA
}EzhWifiMode;


typedef struct _TagCfgData{
	int devInitFlag;  //此值固定为INIT_HARDWARE_FLAG
	EzhWifiMode mode;
	char apSSID[96];
	char apPassword[34];
	char staSSID[96];
	char staPassword[34];
	int uart1_baudRate;
	int uart2_baudRate;
	//TCP服务器的端口
	int tcpserv1_port;
	int tcpserv2_port;
	//
	char yun_host[128];
	int yun_port;
	int is_yun_start;
	//
	char devuuid[64];
	char devname[32];
	char devflag[16];
	//
	int is_mqtt_start;
	char mqtt_host[128];
	int mqtt_port;
	char mqtt_clientid[64];
	char mqtt_username[64];
	char mqtt_password[64];
}TagCfgData;

//
void initFactoryCfgFlash();
//执行指令
void do_command(char* Cmdline);

#endif