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
	<p>Additional relays:<br/>
		<span id="r3name">-</span> <span id="r3">-</span> <button type="button" onclick="setData('r3', 'on')">ON</button><button type="button" onclick="setData('r3', 'off')">OFF</button>
		<br/>
		<span id="r4name">-</span> <span id="r4">-</span> <button type="button" onclick="setData('r4', 'on')">ON</button><button type="button" onclick="setData('r4', 'off')">OFF</button>
	</p>

	<p>
		<br />
		<input type="button" value="Log graph" onclick="window.location.href='/graph.html'" />
		<br />
		<input type="button" value="Log table" onclick="window.location.href='/table.html'" />
		<br />
		<input type="button" value="Settings" onclick="window.location.href='/settings.html'" />
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
					document.getElementById("r3name").innerHTML = obj.r3name;
					document.getElementById("r4name").innerHTML = obj.r4name;
					document.getElementById("r3").innerHTML = obj.r3;
					document.getElementById("r4").innerHTML = obj.r4;
				}
			};
			xhttp.open("GET", "status", true);
			xhttp.send();
		}
	</script>
</body>
</html>
)=====";
