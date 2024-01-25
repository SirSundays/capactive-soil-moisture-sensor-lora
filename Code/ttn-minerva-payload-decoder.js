function Decoder(bytes, port) {

	var frameCounter = 0;
	var frames = [];

	var type_of_power = (bytes[0] & 0b10000000) >>> 7;
	var type_of_power_reading = (bytes[0] & 0b01000000) >>> 6;
	var bat = ((bytes[0] << 10) >> 2 || bytes[1]) / 1000;

	bytes.shift();
	bytes.shift();

	var protocol_version_major = (bytes[0] & 0b11110000) >>> 4;
	var protocol_version_minor = (bytes[0] & 0b00001110) >>> 1;
	var serial_number_bit = bytes[0] & 0b00000001;

	bytes.shift();

	while (bytes.length > 0) {
		let type_of_frame = bytes[0];
		let event_code = (bytes[1] & 0b11100000) >>> 5;
		let sensor_number = (bytes[1] & 0b00011111) >>> 0;

		bytes.shift();
		bytes.shift();

		frames[frameCounter] = {};

		frames[frameCounter].type_of_frame = type_of_frame;
		frames[frameCounter].event_code = event_code;
		frames[frameCounter].sensor_number = sensor_number;

		// Frames
		switch (type_of_frame) {
			case 1:
				frames[frameCounter].firmware_version_major = (bytes[0] & 0b11110000) >>> 4;
				frames[frameCounter].firmware_version_minor = (bytes[0] & 0b00001111) >>> 0;
				frames[frameCounter].device_type = bytes[1];
				bytes.shift();
				bytes.shift();
				break;
			case 3:
				frames[frameCounter].error_code = bytes[0];
				bytes.shift();
				break;
			case 4:
				frames[frameCounter].temperature = (bytes[0] << 8 | bytes[1]) / 100;
				bytes.shift();
				bytes.shift();
				break;
			case 5:
				frames[frameCounter].humidity = (bytes[0] << 8 | bytes[1]) / 100;
				bytes.shift();
				bytes.shift();
				break;
			case 7:
			  frames[frameCounter].adc = (bytes[0] << 8 | bytes[1]);
				bytes.shift();
				bytes.shift();
				break;
			default:
				break;
		}

		frameCounter++;
	}

	frames.sort((a, b) => a.sensor_number < b.sensor_number);

	return {
		data: {
			port: port,
			power_info: {
				bat: bat,
				type_of_power: type_of_power,
				type_of_power_reading: type_of_power_reading,
			},
			protocol_info: {
				serial_number_bit: serial_number_bit,
				protocol_version_major: protocol_version_major,
				protocol_version_minor: protocol_version_minor
			},
			frames: frames
		}
	};
}