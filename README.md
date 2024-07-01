# IP_Notifier

IP_Notifier is a versatile project designed for the ESP8266 microcontroller that automatically reports its current IP address to a specified web server. This project is ideal for applications where monitoring and logging the IP address of an ESP8266 device is crucial, such as in dynamic IP environments or remote IoT deployments.

## Features

- **Automatic IP Address Reporting:** The ESP8266 device continuously monitors its IP address and sends updates to a web server whenever it connects or reconnects to a Wi-Fi network.
- **Over-the-Air (OTA) Configuration:** Users can easily update Wi-Fi credentials and other settings without needing to re-upload the code, making the device highly flexible and adaptable to different network environments.
- **Secure Communication:** Utilizes secure HTTPS connections to send IP address data, ensuring that communication with the server is encrypted and protected.
- **IP Logging and History:** A web interface displays the current IP address and a history of previously logged IP addresses, providing a comprehensive overview of the device's network activity.
- **User-Friendly Interface:** Includes a simple web page hosted by the ESP8266, allowing users to enter new Wi-Fi credentials easily when needed.
- **Built-in Debugging:** Uses LED indicators to signal status changes, such as successful IP address updates and connection attempts, aiding in quick troubleshooting and status monitoring.
- **Flexible Configuration:** Allows for easy customization of various parameters, including the frequency of IP address updates and the server URL.

## Applications

- Remote IoT device monitoring
- Dynamic IP address environments
- Network diagnostics and logging
- Smart home systems
- Educational projects and demonstrations

## Installation

1. Clone this repository to your local machine.
2. Open the project in your Arduino IDE.
3. Install the necessary libraries:
    - ESP8266WiFi
    - ESP8266HTTPClient
    - ESP8266WebServer
    - EEPROM

4. Modify the following lines in the code to match your network credentials and server URL:

    ```cpp
    const char* ssid = "your-SSID";
    const char* password = "your-password";
    const char* serverUrl = "https://your-server-url.com/update-ip.php";
    ```

5. Upload the code to your ESP8266 device.

## Usage

1. Power on your ESP8266 device.
2. The device will attempt to connect to the specified Wi-Fi network.
3. Once connected, it will automatically send its IP address to the configured server.
4. To update Wi-Fi credentials, press and hold the flash button for 2 or more seconds to enter AP mode, where you can connect to the ESP8266's AP and enter new credentials via the hosted web page.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request if you have any improvements or bug fixes.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

Combining the power of the ESP8266 with the convenience of automated IP reporting, IP_Notifier provides a robust and user-friendly solution for a variety of applications. Whether you're managing a network of IoT devices or need a reliable way to track your ESP8266's IP address, IP_Notifier has you covered.
