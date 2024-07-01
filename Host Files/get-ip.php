<?php
date_default_timezone_set('Asia/Kolkata');
$file = 'ip_log.txt';

$history = [];
$current_ip = 'No IP address logged yet.';

// Check if the log file exists and is readable
if (file_exists($file) && is_readable($file)) {
    $lines = file($file, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
    foreach ($lines as $line) {
        if (preg_match('/\[(.*?)\] IP: (\d+\.\d+\.\d+\.\d+)/', $line, $matches)) {
            $history[] = [
                'date_time' => $matches[1],
                'ip' => $matches[2],
            ];
        }
    }
    if (!empty($history)) {
        $current_ip = $history[count($history) - 1]['ip'];
    }
}

// Return the current IP and history as JSON
echo json_encode([
    'current_ip' => $current_ip,
    'history' => array_reverse($history), // Reverse to show the most recent first
]);
?>
