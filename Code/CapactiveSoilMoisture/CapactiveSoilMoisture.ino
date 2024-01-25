#include "LoRaWan_APP.h"
#include "Arduino.h"

#include "PWM1.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS GPIO1

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

/* OTAA para*/
uint8_t devEui[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x05, 0x99, 0x3F };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x68, 0x32, 0x33, 0x5E, 0xD7, 0xAB, 0x97, 0x44, 0xE3, 0x8D, 0x76, 0x68, 0x39, 0xB1, 0xB3, 0x72 };

/* ABP para*/
uint8_t nwkSKey[] = { 0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda, 0x85 };
uint8_t appSKey[] = { 0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef, 0x67 };
uint32_t devAddr = (uint32_t)0x007e6ae1;

/*LoraWan channelsmask, default channels 0-7*/
uint16_t userChannelsMask[6] = { 0xFF00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t loraWanClass = LORAWAN_CLASS;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 15000;
uint32_t appTxDutyCycleLong = 15 * 60 * 1000;

uint8_t uplinksToLongTx = 8;

/*OTAA or ABP*/
bool overTheAirActivation = LORAWAN_NETMODE;

/*ADR enable*/
bool loraWanAdr = LORAWAN_ADR;

/* set LORAWAN_Net_Reserve ON, the node could save the network info to flash, when node reset not need to join again */
bool keepNet = LORAWAN_NET_RESERVE;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = LORAWAN_UPLINKMODE;
bool standardIsTxConfirmed = isTxConfirmed;

uint8_t appPort = 1;

/*!
  Number of trials to transmit the frame, if the LoRaMAC layer did not
  receive an acknowledgment. The MAC performs a datarate adaptation,
  according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
  to the following table:

  Transmission nb | Data Rate
  ----------------|-----------
  1 (first)       | DR
  2               | DR
  3               | max(DR-1,0)
  4               | max(DR-1,0)
  5               | max(DR-2,0)
  6               | max(DR-2,0)
  7               | max(DR-3,0)
  8               | max(DR-3,0)

  Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
  the datarate, in case the LoRaMAC layer did not receive an acknowledgment
*/
uint8_t confirmedNbTrials = 4;

// Join Limit can be set to restart the whole MCU when no join is possible. Time in ms
uint32_t join_start = 0;
uint32_t join_limit = 3 * 60 * 1000;

/* Available Settings:
  1. Interval - 32 bit
*/
uint8_t number_settings = 1;

uint8_t uplinks = 0;
bool setupErrorDS = false;

bool commandReturn = false;
uint8_t commandType = 0x00;
uint8_t commandError = 0x00;

uint8_t downlink_uid = 0x00;

/* Prepares the payload of the frame */
static void prepareTxFrame(uint8_t port) {
  // Get Battery Level (Not available on Soil Moisture Sensor)
  uint16_t battery_voltage = 0;

  // Constuct Payload-Header
  appDataSize = 3;
  uint8_t appDataSizeCounter = 3;

  uint16_t power_header = generatePowerHeader(battery_voltage);
  appData[0] = highByte(power_header);
  appData[1] = lowByte(power_header);
  appData[2] = generateMetaInfoHeader();

  if (uplinks == 0) {
    // Common Uplink: Device Info
    appDataSize += 4;
    uint32_t device_info_frame = generateDeviceInfoFrame(1);

    appData[appDataSizeCounter] = uint8_t((device_info_frame & 0xFF000000) >> 24);
    appDataSizeCounter++;
    appData[appDataSizeCounter] = uint8_t((device_info_frame & 0x00FF0000) >> 16);
    appDataSizeCounter++;
    appData[appDataSizeCounter] = uint8_t((device_info_frame & 0x0000FF00) >> 8);
    appDataSizeCounter++;
    appData[appDataSizeCounter] = uint8_t((device_info_frame & 0x000000FF));
    appDataSizeCounter++;

    if (setupErrorDS) {
      appDataSize += 3;

      uint32_t setup_error_frame_DS = generateErrorFrame(0, 1, 1);
      appData[appDataSizeCounter] = uint8_t((setup_error_frame_DS & 0xFF000000) >> 24);
      appDataSizeCounter++;
      appData[appDataSizeCounter] = uint8_t((setup_error_frame_DS & 0x00FF0000) >> 16);
      appDataSizeCounter++;
      appData[appDataSizeCounter] = uint8_t((setup_error_frame_DS & 0x0000FF00) >> 8);
      appDataSizeCounter++;
    }
    
    uplinks++;
  } else {
    if (uplinks < uplinksToLongTx) {
      uplinks++;
    } else if (uplinks == uplinksToLongTx) {
      appTxDutyCycle = appTxDutyCycleLong;
      uplinks++;
    }

    digitalWrite(GPIO6, LOW);
    delay(10);

    sensors.begin();
    delay(40);

    // Data-Frames

    bool measurement_error_ds = 0;

    // Read DS18B20
    int samples = 5;
    float temperatures = 0;

    sensors.requestTemperatures();

    if (sensors.getTempCByIndex(0) != -127) {
      for (int i = 0; i < samples; i++) {
        sensors.requestTemperatures();
        float temperatureTemp = sensors.getTempCByIndex(0);
        temperatures += temperatureTemp;
        delay(2);
      }
    } else {
      measurement_error_ds = 1;
    }
    float temperature = temperatures / samples;

    uint16_t temp100 = temperature * 100;

    digitalWrite(GPIO6, HIGH);

    // Read Soil Moisture
    pwmEnable();
    setPWM_Frequency(PWM_CLK_FREQ_16M);
    setPWM_ComparePeriod(0x12C);
    analogWrite(PWM1, 150);
    delay(100);

    int samples_adc = 10;
    float sm_adcs = 0;

    for (int i = 0; i < samples_adc; i++) {
      uint16_t sm_adcTemp = analogRead(ADC);
      sm_adcs += sm_adcTemp;
      delay(5);
    }

    analogWrite(PWM1, 0);

    uint16_t sm_adc = sm_adcs / samples_adc;

    if (measurement_error_ds) {
      appDataSize += 3;

      uint32_t temperature_error_frame = generateErrorFrame(1, 2, 1);
      appData[appDataSizeCounter] = uint8_t((temperature_error_frame & 0xFF000000) >> 24);
      appDataSizeCounter++;
      appData[appDataSizeCounter] = uint8_t((temperature_error_frame & 0x00FF0000) >> 16);
      appDataSizeCounter++;
      appData[appDataSizeCounter] = uint8_t((temperature_error_frame & 0x0000FF00) >> 8);
      appDataSizeCounter++;
    } else {
      appDataSize += 4;

      uint32_t temperature_frame = generateTemperatureFrame(temp100, 2, 1);
      appData[appDataSizeCounter] = uint8_t((temperature_frame & 0xFF000000) >> 24);
      appDataSizeCounter++;
      appData[appDataSizeCounter] = uint8_t((temperature_frame & 0x00FF0000) >> 16);
      appDataSizeCounter++;
      appData[appDataSizeCounter] = uint8_t((temperature_frame & 0x0000FF00) >> 8);
      appDataSizeCounter++;
      appData[appDataSizeCounter] = uint8_t((temperature_frame & 0x000000FF));
      appDataSizeCounter++;
    }

    appDataSize += 4;

    uint32_t adc_frame = generate16AdcFrame(sm_adc, 2, 2);
    appData[appDataSizeCounter] = uint8_t((adc_frame & 0xFF000000) >> 24);
    appDataSizeCounter++;
    appData[appDataSizeCounter] = uint8_t((adc_frame & 0x00FF0000) >> 16);
    appDataSizeCounter++;
    appData[appDataSizeCounter] = uint8_t((adc_frame & 0x0000FF00) >> 8);
    appDataSizeCounter++;
    appData[appDataSizeCounter] = uint8_t((adc_frame & 0x000000FF));
    appDataSizeCounter++;
  }
}

void pwmEnable()
{
  uint8 enableInterrupts;
  enableInterrupts = CyEnterCriticalSection();
  PWM1_BLOCK_CONTROL_REG |= PWM1_MASK;
  CyExitCriticalSection(enableInterrupts);
  enableInterrupts = CyEnterCriticalSection();
  PWM1_COMMAND_REG = ((uint32)(PWM1_MASK << PWM1_CMD_START));
  CyExitCriticalSection(enableInterrupts);
}

void downLinkDataHandle(McpsIndication_t *mcpsIndication) {
  commandReturn = true;
  commandType = uint8_t(mcpsIndication->Buffer[0]);

  downlink_uid = uint8_t(mcpsIndication->Buffer[1]);

  switch (commandType) {
    case 0:
      {
        // Get Device Info
        prepareTxFrame(appPort);
        LoRaWAN.send();
        break;
      }
    case 1:
      {
        // Restart Device
        prepareTxFrame(appPort);
        LoRaWAN.send();
        delay(30 * 1000);
        HW_Reset(0);
        break;
      }
    case 2:
      {
        // Get Settings
        prepareTxFrame(appPort);
        LoRaWAN.send();
        break;
      }
    case 3:
      {
        // Set Settings
        uint8_t total_settings = 0x00;
        total_settings = uint8_t(mcpsIndication->Buffer[2]);

        uint8_t bufferCounter = 3;

        for (uint8_t i = 0; i < total_settings; i++) {
          uint8_t setting_id = uint8_t(mcpsIndication->Buffer[bufferCounter]);
          bufferCounter++;

          switch (setting_id) {
            // Test 030801000000AFC8 - 45000
            // Interval-Time
            case 0:
              {
                appTxDutyCycle = uint32_t(mcpsIndication->Buffer[bufferCounter]) << 24 | uint32_t(mcpsIndication->Buffer[bufferCounter + 1]) << 16 | uint32_t(mcpsIndication->Buffer[bufferCounter + 2]) << 8 | uint32_t(mcpsIndication->Buffer[bufferCounter + 3]);
                appTxDutyCycleLong = appTxDutyCycle;
                bufferCounter = bufferCounter + 4;
              }
          }
        }

        prepareTxFrame(appPort);
        LoRaWAN.send();
        break;
      }
  }
}

void setup() {
  Serial.begin(115200);
  deviceState = DEVICE_STATE_INIT;
  LoRaWAN.ifskipjoin();

  pinMode(GPIO6, OUTPUT);
  digitalWrite(GPIO6, LOW);
  delay(20);

  sensors.begin();
  sensors.requestTemperatures();

  if (sensors.getTempCByIndex(0) != -127) {
    setupErrorDS = true;
  }
  digitalWrite(GPIO6, HIGH);

  pinMode(GPIO0, ANALOG);
  pinMode(GPIO3, ANALOG);
  pinMode(GPIO4, ANALOG);
  pinMode(GPIO5, ANALOG);

  setPWM_Frequency(PWM_CLK_FREQ_16M);
  setPWM_ComparePeriod(0x12C);

  pinMode(PWM1, OUTPUT);
  pinMode(ADC, ANALOG);
}

void loop() {
  switch (deviceState) {
    case DEVICE_STATE_INIT:
      {
#if (LORAWAN_DEVEUI_AUTO)
        LoRaWAN.generateDeveuiByChipID();
#endif
#if (AT_SUPPORT)
        getDevParam();
#endif
        printDevParam();
        LoRaWAN.init(loraWanClass, loraWanRegion);
        deviceState = DEVICE_STATE_JOIN;
        break;
      }
    case DEVICE_STATE_JOIN:
      {
        join_start = millis();
        LoRaWAN.join();
        break;
      }
    case DEVICE_STATE_SEND:
      {
        join_start = 0;
        prepareTxFrame(appPort);
        LoRaWAN.send();

        deviceState = DEVICE_STATE_CYCLE;
        break;
      }
    case DEVICE_STATE_CYCLE:
      {
        // Schedule next packet transmission
        txDutyCycleTime = appTxDutyCycle + randr(0, APP_TX_DUTYCYCLE_RND);
        LoRaWAN.cycle(txDutyCycleTime);
        deviceState = DEVICE_STATE_SLEEP;
        break;
      }
    case DEVICE_STATE_SLEEP:
      {
        LoRaWAN.sleep();
        break;
      }
    default:
      {
        deviceState = DEVICE_STATE_INIT;
        break;
      }
  }
  // Check for Join Limit
  if (millis() - join_start > join_limit && !IsLoRaMacNetworkJoined) {
    Serial.println("Join Reset");
    delay(50);
    HW_Reset(0);
  }
}
