<?php
$host = 'localhost';       // or your MySQL server IP
$port = 3306;              // default MySQL port
$dbname = 'security';       // your database name
$username = 'root';        // your MySQL username
$password = '12345';            // your MySQL password

$conn = new mysqli($host, $username, $password, $dbname, $port);

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

//Haldi ikki tað er neyðugt við hesum, bara log_acces verður brúkt. 
//Men í log_acces, stendur include so tað er nokkso sikkurt tað verður brúkt
?>

