const char TIME_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<title>Waterpool Monitor: date and time</title>
</head>
<body onload="getData();">
	<h1>Waterpool monitor: date and time</h1>

	<form>
	<div>
		<label for="date">Date:</label>
		<input name="year" id="year" value="2018" maxlength="4" size="4"> -
		<input name="month" id="month" value="01" maxlength="2" size="2"> -
		<input name="day" id="day" value="01" maxlength="2" size="2">
	</div>
	<div>
		<label for="date">Time:</label>
		<input name="hour" id="hour" value="00" maxlength="2" size="2"> :
		<input name="minute" id="minute" value="00" maxlength="2" size="2"> :
		<input name="sec" id="sec" value="00" maxlength="2" size="2">
	</div>
	<p/>
	<div>
		<button type="button" onclick="sendData()">Set date/time</button>
	</div>
	</form>

	<script>
		function getData() {
			var xhttp = new XMLHttpRequest();
			xhttp.onreadystatechange = function() {
				if (this.readyState == 4 && this.status == 200) {
					var dateTimeStr = this.responseText;
					year.value = dateTimeStr.substr(0, 4);
					month.value = dateTimeStr.substr(4, 2);
					day.value = dateTimeStr.substr(6, 2);
					hour.value = dateTimeStr.substr(9, 2);
					minute.value = dateTimeStr.substr(11, 2);
					sec.value = dateTimeStr.substr(13, 2);
				}
			};
			xhttp.open("GET", "gettime", true);
			xhttp.send();
		}

		function sendData(data) {
			var dateTimeStr = year.value + month.value + day.value + '-' + hour.value + minute.value + sec.value;
			var xhttp = new XMLHttpRequest();
			xhttp.open("POST", "settime");
			xhttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
			xhttp.send("dt=" + dateTimeStr);
	}
	</script>
</body>
</html>
)=====";
