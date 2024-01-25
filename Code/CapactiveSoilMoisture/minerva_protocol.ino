// Type of Power
// 0 - battery power
// 1 - external power
bool type_of_power = 0;
// Type of Powerreading
// 0 - mV
// 1 - mAh
bool type_of_power_reading = 0;

uint8_t protocol_version_major = 0;
uint8_t protocol_version_minor = 1;

uint8_t serialnumber_presence = 0;

uint8_t firmware_version_major = 0;
uint8_t firmware_version_minor = 1;

uint8_t device_type = 3;

uint16_t generatePowerHeader(uint16_t battery_reading) {
  uint16_t header = 0;

  header = type_of_power << 15;
  header = header | type_of_power_reading << 14;

  header = header | battery_reading;

  return header;
}

uint8_t generateMetaInfoHeader() {
  uint8_t header = 0;

  header = protocol_version_major << 4;
  header = header | protocol_version_minor << 1;
  header = header | serialnumber_presence;

  return header;
}

uint16_t generateFrameHeader(uint8_t type, uint8_t event_code, uint8_t sensor) {
  uint16_t header_raw = 0;

  header_raw = type << 8;

  header_raw = header_raw | event_code << 5;

  header_raw = header_raw | sensor;

  return header_raw;
}

uint32_t generateCommandFrame(uint8_t event_code, uint8_t downlink_uid, uint8_t error_code) {
  uint16_t header;
  header = generateFrameHeader(0, event_code, 0);

  uint32_t deviceInfoFrame = 0;
  deviceInfoFrame = uint32_t(header) << 16;

  deviceInfoFrame = deviceInfoFrame | uint32_t(downlink_uid) << 8;

  deviceInfoFrame = deviceInfoFrame | uint32_t(error_code);

  return deviceInfoFrame;
} 

uint32_t generateDeviceInfoFrame(uint8_t event_code) {
  uint16_t header;
  header = generateFrameHeader(1, event_code, 0);

  uint32_t deviceInfoFrame = 0;
  deviceInfoFrame = uint32_t(header) << 16;

  deviceInfoFrame = deviceInfoFrame | uint32_t(firmware_version_major) << 12;
  deviceInfoFrame = deviceInfoFrame | uint32_t(firmware_version_minor) << 8;

  deviceInfoFrame = deviceInfoFrame | uint32_t(device_type);

  return deviceInfoFrame;
} 

uint32_t generateSettingsFrame(uint8_t event_code, uint8_t number_settings) {
  uint16_t header;
  header = generateFrameHeader(2, event_code, 0);

  uint32_t settingsFrame = 0;
  settingsFrame = uint32_t(header) << 16;

  settingsFrame = settingsFrame | uint32_t(downlink_uid) << 8;
  settingsFrame = settingsFrame | uint32_t(number_settings) << 0;

  return settingsFrame;
}

uint64_t generateIntervalSettingsFrame() {
  uint64_t interval_setting_frame = 0;

  interval_setting_frame = 0 << 32;

  interval_setting_frame = interval_setting_frame | uint64_t(appTxDutyCycle);

  return interval_setting_frame;  
}

uint32_t generateErrorFrame(uint8_t error_code, uint8_t event_code, uint8_t sensor) {
  uint32_t header;
  header = generateFrameHeader(3, event_code, sensor);

  uint32_t errorFrame = 0;
  errorFrame = uint32_t(header) << 16;

  errorFrame = errorFrame | (uint32_t(error_code) << 8);

  return errorFrame;
}

uint32_t generateTemperatureFrame(uint16_t temperature, uint8_t event_code, uint8_t sensor) {
  uint32_t header;
  header = generateFrameHeader(4, event_code, sensor);

  uint32_t temperatureFrame = 0;
  temperatureFrame = uint32_t(header) << 16;

  temperatureFrame = temperatureFrame | uint32_t(temperature);

  return temperatureFrame;
}

uint32_t generateHumidityFrame(uint16_t humidity, uint8_t event_code, uint8_t sensor) {
  uint32_t header;
  header = generateFrameHeader(5, event_code, sensor);

  uint32_t humidityFrame = 0;
  humidityFrame = header << 16;

  humidityFrame = humidityFrame | uint32_t(humidity);

  return humidityFrame;
}

uint32_t generateMvFrame(uint16_t mv, uint8_t event_code, uint8_t sensor) {
  uint32_t header;
  header = generateFrameHeader(6, event_code, sensor);

  uint32_t mvFrame = 0;
  mvFrame = header << 16;

  mvFrame = mvFrame | uint32_t(mv);

  return mvFrame;
}

uint32_t generate16AdcFrame(uint16_t adc, uint8_t event_code, uint8_t sensor) {
  uint32_t header;
  header = generateFrameHeader(7, event_code, sensor);

  uint32_t adcFrame = 0;
  adcFrame = header << 16;

  adcFrame = adcFrame | uint32_t(adc);

  return adcFrame;
}
