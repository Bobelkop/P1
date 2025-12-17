<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: GET, POST");
header("Access-Control-Allow-Headers: Content-Type");
include("connect.php");

// Query the access_log table
$sql = "SELECT id, navn, uid_entered, pin_entered, result, timestamp 
        FROM access_log 
        ORDER BY timestamp DESC";

$result = $conn->query($sql);

$rows = [];
if ($result && $result->num_rows > 0) {
    while ($row = $result->fetch_assoc()) {
        $rows[] = $row;
    }
}

$sql1 = "SELECT id, navn
         FROM access";

$result1 = $conn->query($sql1);

$rows1 = [];
if ($result1 && $result1->num_rows > 0){
    while ($row1 = $result1->fetch_assoc()){
        $rows1[] = $row1;
    }
}

// Output as JSON
header('Content-Type: application/json');
echo json_encode([
    "logs" => $rows,
    "access" => $rows1]);

$conn->close();
?>