<?php
$host='localhost';
$user='cse20161644';
$pw='mipil';
$dbname='arduino_num';
$conn = new mysqli($host, $user, $pw, $dbname);

if($conn){
 echo "connect MYSQL successfully\n";
}
else{
 echo "failed to connect MYSQL\n";
}


$num = $_GET["num"];
$sql = "insert into testnum values ($num)"; 
echo "$sql\n";

if($conn->query($sql) === TRUE){
	echo "New record created successfully\n";
} else {
	echo "Error: " . $sql . "<br>" . $conn->error;
}

$conn->close();

?>
