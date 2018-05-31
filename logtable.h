const char LOGTABLE_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<title>Waterpool Monitor - Logging Table</title>
</head>
<body onload="getLogData();">
	<h1>Waterpool monitor - Logging Table</h1>

	<span id="data"></span>
	<span id="progress">Loading...</span>

	<script>
		function getLogData() {
			var xhttp = new XMLHttpRequest();
			xhttp.open("GET", "logs", true);
			xhttp.responseType = "arraybuffer";
			xhttp.onprogress = updateProgress;
			xhttp.onload = function (oEvent) {
  			var arrayBuffer = xhttp.response; 
  			if (arrayBuffer) {
    				var byteArray = new Uint8Array(arrayBuffer);

				var ind = 0;
				while (ind < byteArray.byteLength)
				{
					var timestamp = byteArray[ind] | byteArray[ind+1] << 8 | 
						byteArray[ind+2] << 16 | byteArray[ind+3] << 24;
					ind += 4;
					var flow = byteArray[ind];
					var temp1 = byteArray[ind+1];
					var temp2 = byteArray[ind+2];
					var temp3 = byteArray[ind+3];
					var temp4 = byteArray[ind+4];
					var state = byteArray[ind+5];
                                        ind += 6;

					var date = new Date(timestamp * 1000).toISOString();
					var tempString = date.slice(-24, -14)+" "+date.slice(-13, -5) + " &nbsp; " + 
						" Flow: " + flow.toString().padStart(3, "0") + 
						" T1: " + temp1.toString().padStart(2, "0") + 
						" T2: " + temp2.toString().padStart(2, "0") + 
						" T3: " + temp3.toString().padStart(2, "0") + 
						" T4: " + temp4.toString().padStart(2, "0") + 
						" State: " + state.toString(2).padStart(8, "0") + "</br>";
					document.getElementById("data").innerHTML += tempString;
				}


			}};
			xhttp.send(null);
		}
		function updateProgress(evt) 
		{
			document.getElementById("progress").innerHTML = 'Loaded ' + evt.loaded + ' bytes';
		}   
	</script>
</body>
</html>
)=====";
