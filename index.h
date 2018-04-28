const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<title>Waterpool Monitor</title>
</head>
<body onload="getData();">
	<h1>Waterpool monitor</h1>

	<p>Temperatures: </br><span id="temp">-</span></p>
	<p>Flow: <span id="flow">-</span> L/min</p>
	
	<p>
	Pump enabled: <span id="pump">-</span> &nbsp;
	<button type="button" onclick="setData('pump', 'on')">ON</button>
	<button type="button" onclick="setData('pump', 'off')">OFF</button>
	</br>

	Heating enabled: <span id="heat">-</span> &nbsp;
	<button type="button" onclick="setData('heat', 'on')">ON</button>
	<button type="button" onclick="setData('heat', 'off')">OFF</button>
	</p>
	<p>
	Pump relay status: <span id="pumpRelay">-</span></br>
	Heating relay status: <span id="heatRelay">-</span>
	</p>

	<script>
		setInterval(function() {
			getData();
		}, 5000); 

		function setData(control, action) {
			var xhttp = new XMLHttpRequest();
			xhttp.onreadystatechange = function() {
				if (this.readyState == 4 && this.status == 200) {
					document.getElementById(control).innerHTML = this.responseText;
				}
			};
			xhttp.open("GET", control+"/"+action, true);
			xhttp.send();
		}

		function getData() {
			var xhttp = new XMLHttpRequest();
			xhttp.onreadystatechange = function() {
				if (this.readyState == 4 && this.status == 200) {
					obj = JSON.parse(this.responseText);
					var tempString = "";
					for (i = 0; i < obj.temp.length; i++) {
						tempString += "&nbsp;&nbsp;T" + i + ": " + obj.temp[i].toString().padStart(2, "0") + " &#0176;C</br>";
					}
					document.getElementById("temp").innerHTML = tempString;
					document.getElementById("flow").innerHTML = obj.flow;

					document.getElementById("pump").innerHTML = obj.pump;
					document.getElementById("heat").innerHTML = obj.heat;
					document.getElementById("pumpRelay").innerHTML = obj.relayPump;
					document.getElementById("heatRelay").innerHTML = obj.relayHeat;
				}
			};
			xhttp.open("GET", "status", true);
			xhttp.send();
		}
	</script>
</body>
</html>
)=====";
