<?php
mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
include("connect.php");

$uid = trim($_POST['uid'] ?? '');
$pin = trim($_POST['pin'] ?? '');

$result = "fail";   // default
$navn   = "Unknown";

if (!empty($uid) && !empty($pin)) {
    if (strlen($pin) < 4) {
        $navn = "PIN too short";
    } elseif (strlen($pin) > 4) {
        $navn = "PIN too long";
    } else {
        // Check UID + PIN
        $stmt = $conn->prepare("SELECT navn FROM access WHERE uid = ? AND pin = ?");
        $stmt->bind_param("ss", $uid, $pin);
        $stmt->execute();
        $stmt->bind_result($dbNavn);
        if ($stmt->fetch()) {
            $result = "ok";
            $navn   = $dbNavn;
        } else {
            $stmt->close();
            // Check UID only
            $stmt2 = $conn->prepare("SELECT navn FROM access WHERE uid = ?");
            $stmt2->bind_param("s", $uid);
            $stmt2->execute();
            $stmt2->bind_result($dbNavn2);
            if ($stmt2->fetch()){
                $navn = !empty($dbNavn2) ? $dbNavn2 : "UID known, wrong PIN";
            }else{
                $navn = "Unknown UID";
            }
            $stmt2->close();
        }   
        $stmt->close();
    }

    // Always log attempt
    $log = $conn->prepare("INSERT INTO access_log (navn, uid_entered, pin_entered, result) VALUES (?,?,?,?)");
    if (!$log){
        error_log("Prepare fail: ". $conn->error);
    }else{
        $log->bind_param("ssss", $navn, $uid, $pin, $result);
        if (!$log->execute()){
            error_log("Insert failed: ". $log->error);
        }
    }
    $log->close();

    echo $result;

} else {
    echo "Error";
}

$conn->close();
?>