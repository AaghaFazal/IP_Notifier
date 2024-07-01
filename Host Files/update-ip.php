<?php
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    // Get the IP address from the POST request
    $ip = $_POST['ip'];

    // Validate the IP address
    if (filter_var($ip, FILTER_VALIDATE_IP)) {
        // Log the IP address to a file
        $file = 'ip_log.txt';
        $currentDate = new DateTime('now', new DateTimeZone('Asia/Kolkata'));
        $logEntry = "[" . $currentDate->format('Y-m-d H:i:s') . "] IP: $ip\n";
        file_put_contents($file, $logEntry, FILE_APPEND);

        // Send a response back to the ESP8266
        echo "IP address logged successfully.";
    } else {
        echo "Invalid IP address.";
    }
} else {
    echo "Invalid request method.";
}
?>