<?php
  $servername = "sgcs1416.cafe24.com";

  $username = "kinkin21c";

  $password = "jenjen21c";

  $dbname = "kinkin21c";

//  date_default_timezone_set('Asia/Seoul');

  (string)$now = date("Y-m-d H:i:s");
  
  
  echo "check point\n";
//  parse_str( html_entity_decode( $_SERVER['QUERY_STRING']) , $out);

 // if ( array_key_exists( 'temperature', $out ) ) {

    // Create connection

    $conn = new mysqli($servername, $username, $password, $dbname);

    // Check connection

  if ($conn) {
	  echo " connection success\n";
	  echo "$now\n";
  }
  else{

      die("Connection failed: " . $conn->connect_error);

  }


  $floor = $_GET["floor"];
  $temp  = $_GET["temp"];
//    $humid = $out['humidity'];

    $sql = "INSERT INTO temp (floor,time , temperature) VALUES ($floor,'$now' , $temp)";

    if ($conn->query($sql) === TRUE) {

      echo "Sensed data saved.";

    } else {

      echo "Error: " . $sql . "<br>" . $conn->error;

    }

    $conn->close();

//  }

?>



