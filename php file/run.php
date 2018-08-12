<?php
$host='localhost';
$user='cse20161644';
$pw='mipil';
$dbname='testdb';
$conn = new mysqli($host, $user, $pw, $dbname);

if($conn){
 echo "connect MYSQL successfully\n";
}
else{
 echo "failed to connect MYSQL\n";
}


$temperature = $_GET["temp"];
$date = $_GET["date"];
$time = $_GET["time"];
$sql = "insert into data values ($date,$time,$temperature)"; 
echo "$sql\n";

if($conn->query($sql) === TRUE){
	echo "New record created successfully\n";
} else {
	echo "Error: " . $sql . "<br>" . $conn->error;
}

$conn->close();

?>
