# Capactive Soil Moisture Sensor with LoRa

This sensor is a capacitive soil moisture sensor with a completely 3D printable housing, which can send its data over LoRaWAN into the IoT.

## Short Description

Are you interested in a soil moisture and temperature sensor that is suitable for plant pots? If so, you might want to check out my project: the “All in One Capacitive Soil Moisture Sensor w/ LoRa”. This project combines capacitive sensing, LoRaWAN, and 3D printing technologies to create a simple, compact, and efficient sensor. It has a single PCB that serves as both the sensing element and the electronic circuit. A Heltec CubeCell module provides low-power LoRaWAN connectivity to an IoT network. The sensor also measures the actual soil temperature and calibrates the moisture readings for different soil types. A layer of epoxy protects the sensor from various environmental conditions and a rotary device helps to apply it evenly. The sensor is easy to install and use, as it only requires to be inserted into the soil. It also has a good battery life, lasting for several months without replacement.
<p align="center">
  <img src="https://github.com/SirSundays/capactive-soil-moisture-sensor-lora/blob/main/Media/Sensor%20in%20the%20Wild.jpg?raw=true" width="400">
</p>

If you want to know more about the project you can read a full explanation on hackaday: [All in One Capacitive Soil Moisture Sensor w/ LoRa](https://hackaday.io/project/194052-all-in-one-capactive-soil-moisture-sensor-w-lora)

## Files
| File Path  | Description  |
| ------------- |--------------  |
| ![3D-Models/Epoxy](https://github.com/SirSundays/capactive-soil-moisture-sensor-lora/tree/main/3D-Models/Epoxy)      | 3D-Data for an epoxy potting jig for the lower half of the housing and a turning device for evenly curing the epoxy when applied to the sensor and housing      |
| ![3D-Models/Programming](https://github.com/SirSundays/capactive-soil-moisture-sensor-lora/tree/main/3D-Models/Programming)      | 3D-Data for a simple programming jig to easily program and debug the sensor node      |
| ![3D-Models/Sensor](https://github.com/SirSundays/capactive-soil-moisture-sensor-lora/tree/main/3D-Models/Sensor)     | 3D-Data for both the standard sensor housing, as well as a split housing with a hole for antenna matching      |
| ![Cap-Test](https://github.com/SirSundays/capactive-soil-moisture-sensor-lora/tree/main/CapTest) | All Files developed and used to find the optimal values for the circuit parameters for example generated frequency and capacitor and resistor values |
| ![Code/CapacativeSoilMoisture](https://github.com/SirSundays/capactive-soil-moisture-sensor-lora/tree/main/Code/CapactiveSoilMoisture) | Developed code for the Heltec CubeCell AM01 V2. Used with Arduino IDE, Heltec Boards installed. Also depends on OneWire and DallasTemperature library. |
| ![Code/ttn-minerva-payload-decoder.js](https://github.com/SirSundays/capactive-soil-moisture-sensor-lora/blob/main/Code/ttn-minerva-payload-decoder.js) | TheThingsNetwork JavaScript Payload decoder to decode the data. |
| ![Media](https://github.com/SirSundays/capactive-soil-moisture-sensor-lora/tree/main/Media) | All important photos and videos I took while developing and testing the sensor. Just gives an inside on how some of the stuff works (or did not work). |
| ![PCB/DS18B20Z-Flip-Test](https://github.com/SirSundays/capactive-soil-moisture-sensor-lora/tree/main/PCB/DS18B20Z-Flip-Test) | Small PCB, to test out the principle of the temperature chip sensor being flipped so it sits inside the PCB. The uploaded version was the one that worked the best.
| ![PCB/LoRaSoilMoistureSensor](https://github.com/SirSundays/capactive-soil-moisture-sensor-lora/tree/main/PCB/LoRaSoilMoistureSensor) | Final PCB including Gerber Files for ordering at JLCPCB |
| ![PCB/Sensor-Test](https://github.com/SirSundays/capactive-soil-moisture-sensor-lora/tree/main/PCB/Sensor-Test) | Grove Capactive Soil Moisture Sensor heavily modified to only include the main PCB trace and no electronics, except a header in the top to be able to connect it to a breadboard.

## License
The hardware and associated design files are released under the Creative Commons CC BY-SA 4.0 license. The code is released under the MIT license.

Projects that inspired this project and which partly got worked into this project are:
- ![b-parasite by rbaron](https://github.com/rbaron/b-parasite/tree/main)
- ![Grove - Capacitive Moisture Sensor (Corrosion Resistant) by Seeed Studio](https://wiki.seeedstudio.com/Grove-Capacitive_Moisture_Sensor-Corrosion-Resistant/#resources)

## Disclaimer
This product is not officially LoRaWAN or LoRa certified through the LoRa Alliance. LoRa and LoRaWAN are trademarks of the LoRa Alliance. Heltec states that the module is compatible with the LoRaWAN standard V 1.0.2, but I did not do any verification in that regard.
