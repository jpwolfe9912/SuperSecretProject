/**
 * @file        esp32.c
 * @author      Jeremy Wolfe (jpwolfe@me.com)
 * @brief       Contains functions for sending and receiving data
 *                  with the ESP32 module using AT Commands
 */

/* Includes */
#include "esp32.h"
#include "esp32Config.h"

/* Global Variables */
Wifi_t Wifi;
MQTT_t MQTT;
SNTP_t SNTP;

/* Internal Functions */
/**
 * @brief Send data over USART with DMA
 * @param data: String to send
 * @param len: Length of string
 * @return
 */
bool Wifi_SendRaw(uint8_t *data, uint16_t len)
{
    if (len <= _WIFI_TX_SIZE)
    {
        // Send the information in data through the UART of the ESP8266
        usart_transmit_dma((char *)data);
        return true;
    }
    else
        return false;
}

/**
 * @brief Send a string of data
 * @param data: String to send
 * @return `1` on success or `0` on failure
 */
bool Wifi_SendString(char *data)
{
    return Wifi_SendRaw((uint8_t *)data, strlen(data));
}

/**
 * @brief Send string of data and wait
 * @param data: String to send
 * @param DelayUs: How long to wait
 * @return `1` on success or `0` on failure
 */
bool Wifi_SendStringAndWait(char *data, uint16_t DelayUs)
{
    if (Wifi_SendRaw((uint8_t *)data, strlen(data)) == false)
        return false;
    delayMicroseconds(DelayUs);
    return true;
}

/**
 * @brief Wait for a string to appear in rx buffer
 * @param TimeOut_ms: How long to wait for
 * @param result: Index of which string it found
 * @param CountOfParameter: How many strings to search for
 * @param : List of strings
 * @return `1` on success or `0` on failure
 */
bool Wifi_WaitForString(uint32_t TimeOut_ms, uint8_t *result, uint8_t CountOfParameter, ...)
{
    /*
     * It uses the CountOfParameter and the Parameters after that to compare with the
     * information that it was received in the UART RX. If the parameter is in the
     * received string the functions return a true value and the position of the
     * parameter (according to the position in the function)
     *
     * Ex:
     * Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")
     *
     * If the ESP8266 return a AT+OK after the last command, the function is going to
     * return a true value and the result number would be 1.
     */

    if (result == NULL)
        return false;
    if (CountOfParameter == 0)
        return false;

    *result = 0;

    va_list tag;
    va_start(tag, CountOfParameter);
    char *arg[CountOfParameter];
    for (uint8_t i = 0; i < CountOfParameter; i++)
        arg[i] = va_arg(tag, char *);
    va_end(tag);

    for (uint32_t t = 0; t < TimeOut_ms; t += 20)
    {
        delayMicroseconds(20000);
        for (uint8_t mx = 0; mx < CountOfParameter; mx++)
        {
            if (strstr((char *)Buffs.RxBuffer.buff, arg[mx]) != NULL) // TODO change to lwrb_find()
            // if (strstr((char *)Buffs.RxBuffer.buff, arg[mx]) != NULL)
            {
                *result = mx + 1;
                return true;
            }
        }
    }
    // timeout
    return false;
}

/**
 * @brief Splits a string based on a delimiter
 * @param result: Split string
 * @param WantWhichOne: Which split portion you want
 * @param SplitterChars: Delimiter
 * @return `1` on success or `0` on failure
 */
bool Wifi_ReturnString(char *result, uint8_t WantWhichOne, char *SplitterChars)
{
    if (result == NULL)
        return false;
    if (WantWhichOne == 0)
        return false;

    char *str = (char *)Buffs.RxBuffer.buff;

    str = strtok(str, SplitterChars);
    if (str == NULL)
    {
        strcpy(result, "");
        return false;
    }
    while (str != NULL)
    {
        str = strtok(NULL, SplitterChars);
        if (str != NULL)
            WantWhichOne--;
        if (WantWhichOne == 0)
        {
            strcpy(result, str);
            return true;
        }
    }
    strcpy(result, "");
    return false;
}

/**
 * @brief Splits a string into smaller strings and returns them
 * @param InputString: String to split
 * @param SplitterChars: Delimiter
 * @param CountOfParameter: Amount of smaller strings to return
 * @param  : Split strings
 * @return `1` on success or `0` on failure
 */
bool Wifi_ReturnStrings(char *InputString, char *SplitterChars, uint8_t CountOfParameter, ...)
{
    if (CountOfParameter == 0)
        return false;
    va_list tag;
    va_start(tag, CountOfParameter);
    char *arg[CountOfParameter];
    for (uint8_t i = 0; i < CountOfParameter; i++)
        arg[i] = va_arg(tag, char *);
    va_end(tag);

    char *str;
    str = strtok(InputString, SplitterChars);
    if (str == NULL)
        return false;
    uint8_t i = 0;
    while (str != NULL)
    {
        str = strtok(NULL, SplitterChars);
        if (str != NULL)
            CountOfParameter--;
        strcpy(arg[i], str);
        i++;
        if (CountOfParameter == 0)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Splits a string and returns an integer after the delimiter
 * @param result: Integer to return
 * @param WantWhichOne: Which integer to return
 * @param SplitterChars: Delimiter
 * @return `1` on success or `0` on failure
 */
bool Wifi_ReturnInteger(int32_t *result, uint8_t WantWhichOne, char *SplitterChars)
{
    if ((char *)Buffs.RxBuffer.buff == NULL)
        return false;
    if (Wifi_ReturnString((char *)Buffs.RxBuffer.buff, WantWhichOne, SplitterChars) == false)
        return false;
    *result = atoi((char *)Buffs.RxBuffer.buff);
    return true;
}

/**
 * @brief Splits a string and returns a float after the delimiter
 * @param result: Float to return
 * @param WantWhichOne: Which float to return
 * @param SplitterChars: Delimiter
 * @return `1` on success or `0` on failure
 */
bool Wifi_ReturnFloat(float *result, uint8_t WantWhichOne, char *SplitterChars)
{
    if ((char *)Buffs.RxBuffer.buff == NULL)
        return false;
    if (Wifi_ReturnString((char *)Buffs.RxBuffer.buff, WantWhichOne, SplitterChars) == false)
        return false;
    *result = atof((char *)Buffs.RxBuffer.buff);
    return true;
}

/**
 * @brief Remove a character from a string
 * @param str: String to search
 * @param garbage: Character to remove
 */
void Wifi_RemoveChar(char *str, char garbage)
{
    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++)
    {
        *dst = *src;
        if (*dst != garbage)
            dst++;
    }
    *dst = '\0';
}

/**
 * @brief Reset receive buffer
 * @param
 */
void Wifi_RxClear(void)
{
    lwrb_reset(&Buffs.RxBuffer);
}

/**
 * @brief Reset transmit buffer
 * @param
 */
void Wifi_TxClear(void)
{
    memset(Buffs.TxBuffer, '\0', _WIFI_TX_SIZE);
}

/**
 * @brief Checks if ESP32 is in a state where it is waiting for
 *          a number of characters to receive. Clears this state
 *          by sending random characters.
 * @param
 */
void Wifi_Unbrick(void)
{
    uint8_t result;
    do
    {
        if (Wifi_SendString("\r\nThis is a random string to send to force the MQTTPUBRAW string length to be reached so it unbricks\r\n") == false)
            break;
        Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR");

        Wifi_RxClear();
        Wifi_TxClear();
    } while (result == 0);
}
/* External Functions */

/**
 * @brief Resets buffers and sees if there is a response
 * @param
 * @return `1` on success or `0` on failure
 */
bool Wifi_Init(void)
{
    uint8_t result;
    bool returnVal = false;
    // Clean the variables and start the interruption to work with the UART
    do
    {
        // Wifi_RxClear();

        if (Wifi_SendString("\r\nAT\r\n") == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
        {
            Wifi_Unbrick();
            break;
        }
        if (result == 2)
            break;
        returnVal = true;
        Wifi_RxClear();
        Wifi_TxClear();
        // usart1StartRxIt();
    } while (0);
    return returnVal;
}

/**
 * @brief Restart module
 * @param
 * @return `1` on success or `0` on failure
 */
bool Wifi_Restart(void)
{
    // Make a restart of the ESP8266 using the AT Commands

    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+RST\r\n");
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief ESP32 enters deep-sleep mode for a time
 * @param DelayMs: Time to sleep for
 * @return
 */
bool Wifi_DeepSleep(uint16_t DelayMs)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+GSLP=%d\r\n", DelayMs);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}
// #########################################################################################################
bool Wifi_FactoryReset(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+RESTORE\r\n");
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Update the version of AT Commands when the device is connected to Internet
 * @param
 * @return `1` on success or `0` on failure
 */
bool Wifi_Update(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+CIUPDATE\r\n");
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(1000 * 60 * 5, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Set Rf power
 * @param Power_0_to_82: Power value from 0 to 82
 * @return `1` on success or `0` on failure
 */
bool Wifi_SetRfPower(uint8_t Power_0_to_82)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+RFPOWER=%d\r\n", Power_0_to_82);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Set wifi mode
 * @param WifiMode_: Set Wifi mode to station, access point, or both
 * @return `1` on success or `0` on failure
 */
bool Wifi_SetMode(WifiMode_t WifiMode_)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+CWMODE=%d\r\n", WifiMode_);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        Wifi.Mode = WifiMode_;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Get the wifi mode and store it in Wifi variable
 * @param
 * @return `1` on success or `0` on failure
 */
bool Wifi_GetMode(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+CWMODE?\r\n");
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;

        if (Wifi_ReturnInteger((int32_t *)&result, 1, ":"))
            Wifi.Mode = (WifiMode_t)result;
        else
            Wifi.Mode = WifiMode_Error;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Get my IP address
 * @param
 * @return `1` on success or `0` on failure
 */
bool Wifi_GetMyIp(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+CIFSR\r\n");
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        sscanf((char *)Buffs.RxBuffer.buff, "AT+CIFSR\r\r\n+CIFSR:APIP,\"%[^\"]", Wifi.MyIP);
        sscanf((char *)Buffs.RxBuffer.buff, "AT+CIFSR\r\r\n+CIFSR:STAIP,\"%[^\"]", Wifi.MyIP);
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Connect to access point
 * @param SSID: Network name
 * @param Pass: Network password
 * @param MAC: Network MAC address (NULL if doesn't exist)
 * @return `1` on success or `0` on failure
 */
bool Wifi_Station_ConnectToAp(char *SSID, char *Pass, char *MAC)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        /*
         * It connects to a WiFi network who has all the parameters correctly
         */
        Wifi_RxClear();
        if (MAC == NULL)
            sprintf((char *)Buffs.TxBuffer, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, Pass);
        else
            sprintf((char *)Buffs.TxBuffer, "AT+CWJAP=\"%s\",\"%s\",\"%s\"\r\n", SSID, Pass, MAC);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_VERYHIGH, &result, 3, "\r\nOK\r\n", "\r\nERROR\r\n", "\r\nFAIL\r\n") == false)
            break;
        if (result > 1) // If the result is higher to 1 is because there were an error
            break;      // in the communication
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Disconnect the station
 * @param
 * @return `1` on success or `0` on failure
 */
bool Wifi_Station_Disconnect(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+CWQAP\r\n");
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Set the station IP address
 * @param IP: IP address to set to
 * @param GateWay: Gateway
 * @param NetMask: Net mask
 * @return `1` on success or `0` on failure
 */
bool Wifi_Station_SetIp(char *IP, char *GateWay, char *NetMask)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+CIPSTA=\"%s\",\"%s\",\"%s\"\r\n", IP, GateWay, NetMask);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        Wifi.StationDhcp = false;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Enable DHCP
 * @param Enable: `1` to enable `0` to disable
 * @return `1` on success or `0` on failure
 */
bool Wifi_Station_DhcpEnable(bool Enable)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        /*
         * It makes the DHCP from the Station Mode enable
         */
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+CWDHCP=1,%d\r\n", Enable);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        Wifi.StationDhcp = Enable;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Check if DHCP is enabled and store in Wifi variable
 * @param
 * @return `1` on success or `0` on failure
 */
bool Wifi_Station_DhcpIsEnable(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        /*
         * It makes a question to the micro-controller to know how is the status of the
         * DHCP process inside of it
         */
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+CWDHCP?\r\n");
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        if (Wifi_ReturnInteger((int32_t *)&result, 1, ":") == false)
            break; // It searches for a ':' Character to know
                   // the next integer value after that
        switch (result)
        {
        case 0:
            Wifi.StationDhcp = false;
            Wifi.SoftApDhcp = false;
            break;
        case 1:
            Wifi.StationDhcp = false;
            Wifi.SoftApDhcp = true;
            break;
        case 2:
            Wifi.StationDhcp = true;
            Wifi.SoftApDhcp = false;
            break;
        case 3:
            Wifi.StationDhcp = true;
            Wifi.SoftApDhcp = true;
            break;
        }
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Create module as soft access point
 * @param SSID: Network name
 * @param password: Network password
 * @param channel: Channel
 * @param WifiEncryptionType: Encryption type
 * @param MaxConnections_1_to_4: Max connections from 1 to 4
 * @param HiddenSSID: Hidden SSID
 * @return `1` on success or `0` on failure
 */
bool Wifi_SoftAp_Create(char *SSID, char *password, uint8_t channel, WifiEncryptionType_t WifiEncryptionType, uint8_t MaxConnections_1_to_4, bool HiddenSSID)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        /*
         * It creates the SoftAP (Small WiFi Network) that it is possible with the ESP8266
         * the data of this network is defined by the inputs of the function
         */
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+CWSAP=\"%s\",\"%s\",%d,%d,%d,%d\r\n", SSID, password, channel, WifiEncryptionType, MaxConnections_1_to_4, HiddenSSID);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;

        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Get the network the module is connected to
 * @param
 * @return `1` on success or `0` on failure
 */
bool Wifi_GetApConnection(void)
{
    bool returnVal = false;
    uint8_t result;
    do
    {
        Wifi_RxClear();
        // Get the name of the AP Connection
        sprintf((char *)Buffs.TxBuffer, "AT+CWJAP?\r\n");
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        /*
         * The previous AT Command has a response like the next example:
         * 			AT+CWJAP?
         * 			+CWJAP:<ssid>,<bssid>,<channel>,<rssi>
         * 			OK
         * The previous function checks the OK Response and the next lines search for the
         * information of the SSID, taking in count the position of that information in
         * the string. It uses pointers to get the start and the final position and the
         * value is saved in the structure of the Wifi Connection.
         */

        char *str1 = strstr((char *)Buffs.RxBuffer.buff, ":\"");
        if (str1 == NULL)
        {
            str1 = strstr((char *)Buffs.RxBuffer.buff, "No AP");
            if (str1 == NULL)
                break;
            else
            {
                memset(Wifi.SSID_Connected, (uint8_t)'\0', 20);
                returnVal = true;
            }
        }
        else
        {
            str1 = str1 + 2;
            char *str2 = strstr(str1, "\"");
            size_t len = str2 - str1;
            strncpy(Wifi.SSID_Connected, str1, len);
            Wifi.SSID_Connected[len] = '\0';
            returnVal = true;
        }
    } while (0);

    return returnVal;
}

/**
 * @brief Get devices connected to access point
 * @param
 * @return `1` on success or `0` on failure
 */
bool Wifi_SoftAp_GetConnectedDevices(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        /*
         * The ESP32 has the possibility to have in a SoftAP (Small WiFi Network) four
         * devices connected at the same time. This function save the information about
         * the devices, which is the IP address and the MAC Address of every device
         */
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+CWLIF\r\n");
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;

        Wifi_RemoveChar((char *)Buffs.RxBuffer.buff, '\r');
        Wifi_ReturnStrings((char *)Buffs.RxBuffer.buff, "\n,", 10, Wifi.SoftApConnectedDevicesIp[0], Wifi.SoftApConnectedDevicesMac[0], Wifi.SoftApConnectedDevicesIp[1], Wifi.SoftApConnectedDevicesMac[1], Wifi.SoftApConnectedDevicesIp[2], Wifi.SoftApConnectedDevicesMac[2], Wifi.SoftApConnectedDevicesIp[3], Wifi.SoftApConnectedDevicesMac[3], Wifi.SoftApConnectedDevicesIp[4], Wifi.SoftApConnectedDevicesMac[4]);

        // Search if there is any device or it does not have any
        for (uint8_t i = 0; i < 6; i++)
        {
            if ((Wifi.SoftApConnectedDevicesIp[i][0] < '0') || (Wifi.SoftApConnectedDevicesIp[i][0] > '9'))
                Wifi.SoftApConnectedDevicesIp[i][0] = 0;
            if ((Wifi.SoftApConnectedDevicesMac[i][0] < '0') || (Wifi.SoftApConnectedDevicesMac[i][0] > '9'))
                Wifi.SoftApConnectedDevicesMac[i][0] = 0;
        }

        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Get connection status
 * @param
 * @return `1` on success or `0` on failure
 */
bool Wifi_TcpIp_GetConnectionStatus(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        // Get the connections status of all the possible connection with the ESP8266
        sprintf((char *)Buffs.TxBuffer, "AT+CIPSTATUS\r\n");
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        /*
         * it searches for the string STATUS: to know what is the status of the connection
         * and save this information in the different variables of the structure. The ESP8266
         * has five possible connection at the same time, therefore it uses a for to save
         * all the needed information in the structure. If there are not other connection
         * the str return a NULL value and the loop is broken.
         */
        char *str = strstr((char *)Buffs.RxBuffer.buff, "\nSTATUS:");
        if (str == NULL)
            break;
        str = strchr(str, ':');
        str++;
        for (uint8_t i = 0; i < 5; i++)
            Wifi.TcpIpConnections[i].status = (WifiConnectionStatus_t)atoi(str);
        str = strstr((char *)Buffs.RxBuffer.buff, "+CIPSTATUS:");
        for (uint8_t i = 0; i < 5; i++)
        {
            sscanf(str, "+CIPSTATUS:%d,\"%3s\",\"%[^\"]\",%d,%d,%d", (int *)&Wifi.TcpIpConnections[i].LinkId, Wifi.TcpIpConnections[i].Type, Wifi.TcpIpConnections[i].RemoteIp, (int *)&Wifi.TcpIpConnections[i].RemotePort, (int *)&Wifi.TcpIpConnections[i].LocalPort, (int *)&Wifi.TcpIpConnections[i].RunAsServer);
            str++;
            str = strstr(str, "+CIPSTATUS:");
            if (str == NULL)
                break;
        }
        returnVal = true;

    } while (0);
    return returnVal;
}

/**
 * @brief Ping a url or IP address
 * @param PingTo: String to ping
 * @return `1` on success or `0` on failure
 */
bool Wifi_TcpIp_Ping(char *PingTo)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+PING=\"%s\"\r\n", PingTo);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_MED, &result, 3, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        if (Wifi_ReturnInteger((int32_t *)&Wifi.TcpIpPingAnswer, 2, "+") == false)
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Enable multiple connections
 * @param EnableMultiConnections: `1` to enable `0` to disable
 * @return `1` on success or `0` on failure
 */
bool Wifi_TcpIp_SetMultiConnection(bool EnableMultiConnections)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        // Enable or Disable the multiconnection possibility
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+CIPMUX=%d\r\n", EnableMultiConnections);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        Wifi.TcpIpMultiConnection = EnableMultiConnections;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Check if multiple connections is enabled
 * @param
 * @return `1` on success or `0` on failure
 */
bool Wifi_TcpIp_GetMultiConnection(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+CIPMUX?\r\n");
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        if (Wifi_ReturnInteger((int32_t *)&result, 1, ":") == false)
            break;
        Wifi.TcpIpMultiConnection = (bool)result;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Start TCP connection
 * @param LinkId: Link ID
 * @param RemoteIp: Remote IP address
 * @param RemotePort: Remote port
 * @param TimeOut: How long to wait for connection to be established
 * @return `1` on success or `0` on failure
 */
bool Wifi_TcpIp_StartTcpConnection(uint8_t LinkId, char *RemoteIp, uint16_t RemotePort, uint16_t TimeOut)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        /*
         * It makes a TCP server and then it creates a TCP Connection according to the
         * settings in the function. It uses a very high time of waiting because the
         * ESP32 takes a lot of time to create a connection with a TCP the first time.
         */
        Wifi_RxClear();
        if (Wifi.TcpIpMultiConnection == true)
        {
            sprintf((char *)Buffs.TxBuffer, "AT+CIPSERVER=1,%d\r\n", RemotePort);
            if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
                break;
            if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
                break;       // The timeout was completed and the string was not there
            if (result == 2) // It was find the "ERROR" String in the receiving information
                break;
        }
        Wifi_RxClear();
        if (Wifi.TcpIpMultiConnection == false)
            sprintf((char *)Buffs.TxBuffer, "AT+CIPSTART=\"TCP\",\"%s\",%d,%d\r\n", RemoteIp, RemotePort, TimeOut);
        else
            sprintf((char *)Buffs.TxBuffer, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d,%d\r\n", LinkId, RemoteIp, RemotePort, TimeOut);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_HIGH, &result, 3, "OK", "CONNECT", "ERROR") == false)
            break;
        if (result == 3)
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Start UDP connection
 * @param LinkId: Link ID
 * @param RemoteIp: Remote IP address
 * @param RemotePort: Remote port
 * @param LocalPort: Local port
 * @return `1` on success or `0` on failure
 */
bool Wifi_TcpIp_StartUdpConnection(uint8_t LinkId, char *RemoteIp, uint16_t RemotePort, uint16_t LocalPort)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        if (Wifi.TcpIpMultiConnection == false)
            sprintf((char *)Buffs.TxBuffer, "AT+CIPSTART=\"UDP\",\"%s\",%d,%d\r\n", RemoteIp, RemotePort, LocalPort);
        else
            sprintf((char *)Buffs.TxBuffer, "AT+CIPSTART=%d,\"UDP\",\"%s\",%d,%d\r\n", LinkId, RemoteIp, RemotePort, LocalPort);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_HIGH, &result, 3, "OK", "ALREADY", "ERROR") == false)
            break;
        if (result == 3) // If the RX String returns a ERROR the function breaks and
            break;       // send a false
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Close TCP connection
 * @param LinkId: Link ID
 * @return `1` on success or `0` on failure
 */
bool Wifi_TcpIp_Close(uint8_t LinkId)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        if (Wifi.TcpIpMultiConnection == false)
            sprintf((char *)Buffs.TxBuffer, "AT+CIPCLOSE\r\n");
        else
            sprintf((char *)Buffs.TxBuffer, "AT+CIPCLOSE=%d\r\n", LinkId);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Enable TCP server
 * @param PortNumber: Port number
 * @return `1` on success or `0` on failure
 */
bool Wifi_TcpIp_SetEnableTcpServer(uint16_t PortNumber)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        if (Wifi.TcpIpMultiConnection == false)
        {
            // it actives the Multiconnection first to make a TCP Server
            sprintf((char *)Buffs.TxBuffer, "AT+CIPMUX=1\r\n");
            if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
                break;
            if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
                break;       // The timeout was completed and the string was not there
            if (result == 2) // It was find the "ERROR" String in the receiving information
                break;
            Wifi.TcpIpMultiConnection = true;
            Wifi_RxClear();
        }
        else
            sprintf((char *)Buffs.TxBuffer, "AT+CIPSERVER=1,%d\r\n", PortNumber);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Disable TCP server
 * @param PortNumber: Port number
 * @return `1` on success or `0` on failure
 */
bool Wifi_TcpIp_SetDisableTcpServer(uint16_t PortNumber)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+CIPSERVER=0,%d\r\n", PortNumber);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Send data over UDP
 * @param LinkId: Link ID
 * @param dataLen: Data length
 * @param data: Data string
 * @return `1` on success or `0` on failure
 */
bool Wifi_TcpIp_SendDataUdp(uint8_t LinkId, uint16_t dataLen, uint8_t *data)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        if (Wifi.TcpIpMultiConnection == false)
            sprintf((char *)Buffs.TxBuffer, "AT+CIPSEND=%d\r\n", dataLen);
        else
            sprintf((char *)Buffs.TxBuffer, "AT+CIPSEND=%d,%d\r\n", LinkId, dataLen);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, ">", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        Wifi_RxClear();
        // Send in this time the information through the connection
        Wifi_SendRaw(data, dataLen);
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Send data over TCP
 * @param LinkId: Link ID
 * @param dataLen: Data length
 * @param data: Data string
 * @return
 */
bool Wifi_TcpIp_SendDataTcp(uint8_t LinkId, uint16_t dataLen, uint8_t *data)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        if (Wifi.TcpIpMultiConnection == false)
            sprintf((char *)Buffs.TxBuffer, "AT+CIPSENDBUF=%d\r\n", dataLen);
        else
            sprintf((char *)Buffs.TxBuffer, "AT+CIPSENDBUF=%d,%d\r\n", LinkId, dataLen);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 3, ">", "ERROR", "busy") == false)
            break;
        if (result > 1)
            break;
        Wifi_RxClear();
        Wifi_SendRaw(data, dataLen);
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}
/* SNTP Commands */

/**
 * @brief Initialize SNTP server connection
 * @param
 * @return `1` on success or `0` on failure
 */
bool SNTP_Init(void)
{
    SNTP.enable = 1;
    SNTP.timezone = -7;
    SNTP.server1 = "pool.ntp.org";

    SNTP.time_updated = false;
    while (!SNTP_TimeUpdated())
        SNTP_SetTimeZone(1);
    return true;
}

/**
 * @brief Set the timezone
 * @param numServers: Number of servers to connect to
 * @return `1` on success or `0` on failure
 */
bool SNTP_SetTimeZone(size_t numServers)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        switch (numServers)
        {
        case 3:
            sprintf((char *)Buffs.TxBuffer, "AT+CIPSNTPCFG=%d,%d,\"%s\",\"%s\",\"%s\"\r\n", SNTP.enable, SNTP.timezone, SNTP.server1, SNTP.server2, SNTP.server3);
            break;
        case 2:
            sprintf((char *)Buffs.TxBuffer, "AT+CIPSNTPCFG=%d,%d,\"%s\",\"%s\"\r\n", SNTP.enable, SNTP.timezone, SNTP.server1, SNTP.server2);
            break;
        default:
            sprintf((char *)Buffs.TxBuffer, "AT+CIPSNTPCFG=%d,%d,\"%s\"\r\n", SNTP.enable, SNTP.timezone, SNTP.server1);
            break;
        }
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;

        if (Wifi_WaitForString(_WIFI_WAIT_TIME_ULTRAHIGH, &result, 1, "+TIME_UPDATED") == false)
        {
            SNTP.time_updated = false;
            break;
        }
        else
            SNTP.time_updated = true;

        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Check if SNTP time was updated
 * @param
 * @return `1` on success or `0` on failure
 */
bool SNTP_TimeUpdated(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();

        sprintf((char *)Buffs.TxBuffer, "AT+CIPSNTPTIME?\r\n");
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;

        char *search_str = "+CIPSNTPTIME:";
        char *search_year = "2023";
        size_t search_idx = 0;
        lwrb_find(&Buffs.RxBuffer, (void *)search_str, strlen(search_str), 0, &search_idx);
        if (!lwrb_find(&Buffs.RxBuffer, (void *)search_year, strlen(search_year), search_idx, &search_idx))
        {
            break;
        }
        returnVal = true;
    } while (0);
    return returnVal;
}

/* MWTT Commands */
/**
 * @brief Initialize MQTT connection to AWS server
 * @param
 * @return `1` on success or `0` on failure
 */
bool MQTT_Init(void)
{
    uint32_t(*unique_id_1) = (uint32_t *)(UID_BASE);      // BASE address
    uint32_t(*unique_id_2) = (uint32_t *)(UID_BASE + 4);  // BASE address + 0x04 offset
    uint32_t(*unique_id_3) = (uint32_t *)(UID_BASE + 14); // BASE address + 0x14 0ffset

    MQTT_GetConnection();
    if (!MQTT.connected)
    {
        MQTT.link_id = 0;
        MQTT.scheme = MQTTSchemes_TlsBothCert;
        sprintf(MQTT.client_id, "%p%p%p", unique_id_1, unique_id_2, unique_id_3);
        strcpy(MQTT.username, "Jpwolfe");
        strcpy(MQTT.password, "1234567890");
        strcpy(MQTT.host, "a1k7aswhvntcg-ats.iot.us-west-1.amazonaws.com");
        MQTT.port = 8883;
        MQTT.reconnect = 1;
        strcpy(MQTT.path, "");

        MQTT_SetUserConfig();
        MQTT_Connect();
        MQTT_GetConnection();
    }
    return true;
}

/**
 * @brief Set the MQTT user configuration
 * @param
 * @return `1` on success or `0` on failure
 */
bool MQTT_SetUserConfig(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        // memset(Buffs.TxBuffer, '\0', sizeof(Buffs.TxBuffer));
        sprintf((char *)Buffs.TxBuffer, "AT+MQTTUSERCFG=%u,%d,\"%s\",\"%s\",\"%s\",0,0,\"%s\"\r\n", MQTT.link_id, MQTT.scheme, MQTT.client_id, MQTT.username, MQTT.password, MQTT.path);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Set the MQTT client ID
 * @param
 * @return `1` on success or `0` on failure
 */
bool MQTT_SetClientID(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+MQTTCLIENTID=%u,\"%s\"\r\n", MQTT.link_id, MQTT.client_id);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Set the MQTT username
 * @param
 * @return `1` on success or `0` on failure
 */
bool MQTT_SetUsername(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+MQTTUSERNAME=%u,\"%s\"\r\n", MQTT.link_id, MQTT.username);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Set the MQTT password
 * @param
 * @return `1` on success or `0` on failure
 */
bool MQTT_SetPassword(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+MQTTPASSWORD=%u,\"%s\"\r\n", MQTT.link_id, MQTT.password);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Set MQTT configuration connection
 * @param
 * @return `1` on success or `0` on failure
 */
bool MQTT_SetConnectionConfig(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+MQTTCONNCFG=0,0,0,\"\",\"\",0,0\r\n");
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Connect to MQTT broker
 * @param
 * @return `1` on success or `0` on failure
 */
bool MQTT_Connect(void)
{
    uint8_t result;
    bool returnVal = false;
    MQTT.connected = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+MQTTCONN=%u,\"%s\",%u,%d\r\n", MQTT.link_id, MQTT.host, MQTT.port, MQTT.reconnect);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        /* Wait for connection */
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_VERYHIGH, &result, 1, "+MQTTCONNECTED") == false)
            break; // The timeout was completed and the string was not there
        MQTT.connected = true;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Get MQTT broker you are connected to
 * @param
 * @return `1` on success or `0` on failure
 */
bool MQTT_GetConnection(void)
{
    uint8_t result;
    bool returnVal = false;
    static char *items[6];
    uint8_t items_size = 0;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+MQTTCONN?\r\n");
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;

        char *str = strstr((char *)Buffs.RxBuffer.buff, "+MQTTCONN:");
        // if (sscanf(str, "+MQTTCONN:0,%d,%d,\"%[^\"]\",\"%u\",\"\",%d", (int *)&MQTT.state, (int *)&MQTT.scheme, MQTT.host, (int *)&MQTT.port, (int *)&MQTT.reconnect) == false)
        //     break;
        if (!split_string(str, items, &items_size))
            break;
        MQTT.state = (MQTTStates_e)atoi(items[0]);
        MQTT.scheme = (MQTTSchemes_e)atoi(items[1]);
        strcpy(MQTT.host, items[2]);
        MQTT.port = (uint16_t)atoi(items[3]);
        MQTT.reconnect = (bool)atoi(items[5]);

        if ((MQTT.state >= MQTTStates_Established) && (MQTT.host != NULL))
            MQTT.connected = true;
        else
            MQTT.connected = false;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Publish MQTT message for command less than 256 bytes
 * @param Message: Struct containing the topic, data length, and data
 * @return `1` on success or `0` on failure
 */
bool MQTT_Publish(MQTT_Message_t Message)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+MQTTPUB=%u,\"%s\",\"%s\",0,0\r\n", MQTT.link_id, Message.topic, Message.data);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Publish raw data for command length greater than 256 bytes
 * @param Message: Struct containing the topic, data length, and data
 * @return `1` on success or `0` on failure
 */
bool MQTT_PublishRaw(MQTT_Message_t Message)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+MQTTPUBRAW=%u,\"%s\",%u,0,0\r\n", MQTT.link_id, Message.topic, Message.length);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
        {
            Wifi_Unbrick();
            break; // The timeout was completed and the string was not there
        }
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        /* Send all data */
        // usart1TransmitDma((uint8_t *)Message.data, (size_t)Message.length);
        usart_transmit_dma(Message.data);
        /* Wait for response */
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "+MQTTPUB:OK", "+MQTTPUB:FAIL") == false)
        {
            Wifi_Unbrick();
            break; // The timeout was completed and the string was not there
        }
        if (result == 2) // It was find the "FAIL" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Subscribe to a topic
 * @param Message: Struct containing the topic, data length, and data
 * @return `1` on success or `0` on failure
 */
bool MQTT_Subscribe(MQTT_Message_t Message)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+MQTTSUB=%u,\"%s\",0\r\n", MQTT.link_id, Message.topic);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_HIGH, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Unsubscribe from a topic
 * @param Message: Struct containing the topic, data length, and data
 * @return `1` on success or `0` on failure
 */
bool MQTT_Unsubscribe(MQTT_Message_t Message)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+MQTTUNSUB=%u,\"%s\"\r\n", MQTT.link_id, Message.topic);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_HIGH, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Check topics you are subscribed to
 * @param Message: Struct containing the topic, data length, and data
 * @return `1` on success or `0` on failure
 */
bool MQTT_CheckSubscription(MQTT_Message_t *Message)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+MQTTSUB?\r\n");
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;

        char *str = strstr((char *)Buffs.RxBuffer.buff, "+MQTTSUB");
        if (sscanf(str, "+MQTTSUB:0,\"%[^\"]\",%d,0", Message->topic, (int *)&MQTT.state) == false)
            break;

        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Close MQTT connections
 * @param
 * @return `1` on success or `0` on failure
 */
bool MQTT_Disconnect(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        sprintf((char *)Buffs.TxBuffer, "AT+MQTTCLEAN=%u\r\n", MQTT.link_id);
        if (Wifi_SendString((char *)Buffs.TxBuffer) == false)
            break;
        if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;       // The timeout was completed and the string was not there
        if (result == 2) // It was find the "ERROR" String in the receiving information
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Wait for a message to be received
 * @param Message: Struct containing the topic, data length, and data
 * @param waitTime: Time to wait for string to come (ms)
 * @return `1` on success or `0` on failure
 */
bool MQTT_WaitForMessage(MQTT_Message_t *Message, uint32_t waitTime)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        Wifi_RxClear();
        memset(Message->data, '\0', strlen(Message->data));
        if (Wifi_WaitForString(waitTime, &result, 1, "+MQTTSUBRECV") == false)
            break; // The timeout was completed and the string was not there

        char *str = strstr((char *)Buffs.RxBuffer.buff, "+MQTTSUBRECV");
        if (sscanf(str, "+MQTTSUBRECV:0,\"%[^\"]\",%zu,%[^\r]", Message->topic, &Message->length, Message->data) == false)
            break;

        returnVal = true;
    } while (0);
    return returnVal;
}

/**
 * @brief Check receive buffer for a received message
 * @param Message: Struct containing the topic, data length, and data
 * @param findStr: String to look for
 * @return `1` on success or `0` on failure
 */
bool MQTT_ListenForMessage(MQTT_Message_t *Message, char *findStr)
{
    bool returnVal = false;
    size_t find_idx = 0;
    // size_t end_idx = 0;
    // char* str_peek = calloc(14, 1);

    do
    {
        if (lwrb_find(&Buffs.RxBuffer, (void *)findStr, strlen(findStr), 0, &find_idx))
        {
            char *items[3];
            uint8_t items_size;                          // unused
            lwrb_skip(&Buffs.RxBuffer, find_idx);        // skip to location of data we want to read
            size_t len = lwrb_get_full(&Buffs.RxBuffer); // strlen((char *)Buffs.RxBuffer.buff + find_idx); // length of data in the buffer starting at where we want data
            if (len >= 0x3FF)
            {
                lwrb_reset(&Buffs.RxBuffer);
                serialWrite("Fucked up length\n");
                break;
            }
            char str[len]; // = calloc(len, sizeof(uint8_t)); // allocate memory
            memset(str, '\0', len);
            // *start_idx = find_idx + len - 1;                             // start new search at last index

            lwrb_read(&Buffs.RxBuffer, (void *)str, len);
            if (!split_string(str, items, &items_size))
                break;
            strcpy(Message->topic, items[0]);
            Message->length = atoi(items[1]);
            strcpy(Message->data, items[2]);

            returnVal = true;
        }

    } while (0);
    return returnVal;
}