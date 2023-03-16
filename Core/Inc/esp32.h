#ifndef _ESP32_H
#define _ESP32_H

#include "super_secret_project.h"
#include "esp32Config.h"

// ###################################################################################################
typedef enum
{
    WifiMode_Error = 0,
    WifiMode_Station = 1,
    WifiMode_SoftAp = 2,
    WifiMode_StationAndSoftAp = 3,
} WifiMode_t;

typedef enum
{
    WifiEncryptionType_Open = 0,
    WifiEncryptionType_WPA_PSK = 2,
    WifiEncryptionType_WPA2_PSK = 3,
    WifiEncryptionType_WPA_WPA2_PSK = 4,
} WifiEncryptionType_t;
typedef enum
{
    WifiConnectionStatus_Error = 0,
    WifiConnectionStatus_GotIp = 2,
    WifiConnectionStatus_Connected = 3,
    WifiConnectionStatus_Disconnected = 4,
    WifiConnectionStatus_ConnectionFail = 5,
} WifiConnectionStatus_t;

typedef enum
{
    MQTTSchemes_Tcp = 1,
    MQTTSchemes_TlsNoCert,
    MQTTSchemes_TlsServerCert,
    MQTTSchemes_TlsClientCert,
    MQTTSchemes_TlsBothCert,
    MQTTSchemes_WsTcp,
    MQTTSchemes_WsTlsNoCert,
    MQTTSchemes_WsTlsServerCert,
    MQTTSchemes_WsTlsClientCert,
    MQTTSchemes_WsTlsBothCert
} MQTTSchemes_e;

typedef enum
{
    MQTTStates_Uninitialized = 0,
    MQTTStates_AlreadySetAtUserCfg,
    MQTTStates_AlreadySetAtConnCfg,
    MQTTStates_Disconnected,
    MQTTStates_Established,
    MQTTStates_ConnectedNoSub,
    MQTTStates_ConnectedSubbed
} MQTTStates_e;

typedef enum
{
    MQTT_Status_ERROR = 0,
    MQTT_Status_NO_DATA,
    MQTT_Status_RECV_GOOD
} MQTT_Status_e;

typedef struct
{
    WifiConnectionStatus_t status;
    uint8_t LinkId;
    char Type[4];
    char RemoteIp[17];
    uint16_t RemotePort;
    uint16_t LocalPort;
    bool RunAsServer;
} WifiConnection_t;
// ###################################################################################################
typedef struct
{
    //----------------Usart	Paremeter
    uint8_t usartBuff;
    uint8_t RxBuffer[_WIFI_RX_SIZE];
    uint8_t TxBuffer[_WIFI_TX_SIZE];
    uint16_t RxIndex;
    bool RxIsData;
    //----------------General	Parameter
    WifiMode_t Mode;
    char MyIP[16];
    char MyGateWay[16];
    //----------------Station	Parameter
    char SSID_Connected[20];
    bool StationDhcp;
    char StationIp[16];
    //----------------SoftAp Parameter
    bool SoftApDhcp;
    char SoftApConnectedDevicesIp[6][16];
    char SoftApConnectedDevicesMac[6][18];
    //----------------TcpIp Parameter
    bool TcpIpMultiConnection;
    uint16_t TcpIpPingAnswer;
    WifiConnection_t TcpIpConnections[5];
    //----------------
} Wifi_t;

typedef struct
{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} SNTP_Clocktime_t;

typedef struct
{
    char day_of_week[3];
    char month[3];
    uint8_t day;
    uint16_t year;
    SNTP_Clocktime_t clocktime;
} SNTP_Time_t;

typedef struct
{
    bool enable;
    bool time_updated;
    int8_t timezone;
    char *server1;
    char *server2;
    char *server3;
    SNTP_Time_t time;
} SNTP_t;

typedef struct
{
    char client_id[256];
    char username[64];
    char password[64];
    char host[128];
    char path[32];
    MQTTStates_e state;
    MQTTSchemes_e scheme;
    uint16_t port;
    uint8_t link_id;
    bool reconnect;
    bool connected;
} MQTT_t;

typedef struct
{
    char topic[128];
    char* data; //[_MQTT_SIZE];
    uint16_t length;
} MQTT_Message_t;

// ###################################################################################################
extern Wifi_t Wifi;
extern MQTT_t MQTT;
extern SNTP_t SNTP;
// ###################################################################################################
void Wifi_RxClear(void);
bool Wifi_SendString(char *data);
bool Wifi_WaitForString(uint32_t TimeOut_ms, uint8_t *result, uint8_t CountOfParameter, ...);
void Wifi_RxCallBack(void);
// ###################################################################################################
//  Basic functions of ESP8266
bool Wifi_Init(void);
void Wifi_Enable(void);
void Wifi_Disable(void);
bool Wifi_Restart(void);
bool Wifi_DeepSleep(uint16_t DelayMs);
bool Wifi_FactoryReset(void);
bool Wifi_Update(void);
bool Wifi_SetRfPower(uint8_t Power_0_to_82);
// ###################################################################################################
//  Mode of the ESP8266: Station or SoftAP or both
bool Wifi_SetMode(WifiMode_t WifiMode_);
bool Wifi_GetMode(void);
bool Wifi_GetMyIp(void);
// ###################################################################################################
bool Wifi_Station_ConnectToAp(char *SSID, char *Pass, char *MAC);
bool Wifi_Station_Disconnect(void);
bool Wifi_Station_DhcpEnable(bool Enable);
bool Wifi_Station_DhcpIsEnable(void);
bool Wifi_Station_SetIp(char *IP, char *GateWay, char *NetMask);
// ###################################################################################################
bool Wifi_SoftAp_Create(char *SSID, char *password, uint8_t channel,
                        WifiEncryptionType_t WifiEncryptionType, uint8_t MaxConnections_1_to_4,
                        bool HiddenSSID);
bool Wifi_GetApConnection(void);
bool Wifi_SoftAp_GetConnectedDevices(void);
// ###################################################################################################
bool Wifi_TcpIp_GetConnectionStatus(void);
bool Wifi_TcpIp_Ping(char *PingTo);
bool Wifi_TcpIp_SetMultiConnection(bool EnableMultiConnections);
bool Wifi_TcpIp_GetMultiConnection(void);
bool Wifi_TcpIp_StartTcpConnection(uint8_t LinkId, char *RemoteIp, uint16_t RemotePort,
                                   uint16_t TimeOut_S);
bool Wifi_TcpIp_StartUdpConnection(uint8_t LinkId, char *RemoteIp, uint16_t RemotePort,
                                   uint16_t LocalPort);
bool Wifi_TcpIp_Close(uint8_t LinkId);
bool Wifi_TcpIp_SetEnableTcpServer(uint16_t PortNumber);
bool Wifi_TcpIp_SetDisableTcpServer(uint16_t PortNumber);
bool Wifi_TcpIp_SendDataUdp(uint8_t LinkId, uint16_t dataLen, uint8_t *data);
bool Wifi_TcpIp_SendDataTcp(uint8_t LinkId, uint16_t dataLen, uint8_t *data);

bool SNTP_Init(void);
bool SNTP_SetTimeZone(size_t numServers);
bool SNTP_QueryTime(void);

bool MQTT_Init(void);
bool MQTT_SetUserConfig(void);
bool MQTT_SetClientID(void);
bool MQTT_SetUsername(void);
bool MQTT_SetPassword(void);
bool MQTT_SetConnectionConfig(void);
bool MQTT_Connect(void);
bool MQTT_GetConnection(void);
bool MQTT_Publish(MQTT_Message_t Message);
bool MQTT_PublishRaw(MQTT_Message_t Message);
bool MQTT_Subscribe(MQTT_Message_t Message);
bool MQTT_Unsubscribe(MQTT_Message_t Message);
bool MQTT_Disconnect(void);
bool MQTT_WaitForMessage(MQTT_Message_t *Message, uint32_t waitTime);
bool MQTT_ListenForMessage(MQTT_Message_t *Message);

#endif