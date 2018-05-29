const char SETTINGS_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<title>Waterpool Monitor - Settings</title>
	<style>
		body {
			margin: 2em auto;
			max-width: 600px;
		}

		form div {
			margin-bottom: 0.5em;
		}

			form div label, form div input {
				display: inline-block;
				margin-bottom: 0.3em;
			}

		div.form-header {
			font-weight: bold;
		}

		div.form-footer {
			font-style: italic;
		}

		input.maxval {
			width: 3em;
		}

		input.logval {
			width: 5em;
		}
		input.textval {
			width: 10em;
		}
		input.readonly {
			width: 12em;
			outline: none;
			border: 0px solid;
			margin-left: 1em;
		}
	</style>
	<script>
		(function () {
			function toJSONString(form) {
				var obj = {};
				var elements = form.querySelectorAll("input, select, textarea");
				for (var i = 0; i < elements.length; ++i) {
					var element = elements[i];
					var name = element.name;
					var value = element.value;
					if (element.type === 'checkbox')
						value = element.checked ? 'true' : 'false';

					if (name  && element.className != 'readonly') {
						obj[name] = value;
					}
				}
				return JSON.stringify(obj);
			}

			document.addEventListener("DOMContentLoaded", function () {
				var form = document.getElementById("settingsform");
				form.addEventListener("submit", function (e) {
					e.preventDefault();
					var jsonString = toJSONString(this);

					var xhttp = new XMLHttpRequest();
					xhttp.open("POST", "setsettings");
					xhttp.setRequestHeader('Content-Type', 'application/json');
					xhttp.send(jsonString);

				}, false);

			});

		})();

		function fromJSONString(jsonString) {
			var obj = JSON.parse(jsonString);
			for (var key in obj) {
				if (obj.hasOwnProperty(key)) {
					var elem = document.getElementsByName(key)[0];
					elem.value = obj[key];
					if (elem.type === 'checkbox')
						elem.checked = obj[key] == 'true';
				}
			}
		};

		function getSettings() {
			var xhttp = new XMLHttpRequest();
			xhttp.onreadystatechange = function () {
				if (this.readyState == 4 && this.status == 200) {
					var jsonStr = this.responseText;
					fromJSONString(jsonStr);
				}
			};
			xhttp.open("GET", "getsettings", true);
			xhttp.send();
		}

	</script>
</head>

<body onload="getSettings();">
	<h1>Waterpool monitor - Settings</h1>

	<form id="settingsform" action="#" method="post">
		<div class="form-header">
			<label>Temperature sensor maximums</label>
		</div>
		<div class="form-group">
			<label for="t1">1. </label>
			<input class="maxval" type="number" name="t1" id="t1" min="0" max="99" />
			<input class="readonly" type="text" name="t1id" id="t1id" readonly />
		</div>
		<div class="form-group">
			<label for="t2">2. </label>
			<input class="maxval" type="number" name="t2" id="t2" min="0" max="99" />
			<input class="readonly" type="text" name="t2id" id="t2id" readonly />
		</div>
		<div class="form-group">
			<label for="t3">3. </label>
			<input class="maxval" type="number" name="t3" id="t3" min="0" max="99" />
			<input class="readonly" type="text" name="t3id" id="t3id" readonly />
		</div>
		<div class="form-group">
			<label for="t4">4. </label>
			<input class="maxval" type="number" name="t4" id="t4" min="0" max="99" />
			<input class="readonly" type="text" name="t4id" id="t4id" readonly />
		</div>
		<div class="form-header">
			<label>Flow sensor minimums</label>
		</div>
		<div class="form-group">
			<label for="f1">Flow: </label>
			<input class="maxval" type="number" name="f1" id="f1" min="0" max="999" />
		</div>
		<div class="form-header">
			<label>Sensor data logging</label>
		</div>
		<div class="form-group">
			<label for="lp">Period, seconds: </label>
			<input class="logval" type="number" name="lp" id="lp" min="0" max="3600" />
		</div>
		<div class="form-group">
			<label for="r3">Relay 3 Name:</label>
			<input class="textval" type="text" name="r3" id="r3" maxlength="15" />
		</div>
		<div class="form-group">
			<label for="r4">Relay 4 Name:</label>
			<input class="textval" type="text" name="r4" id="r4" maxlength="15" />
		</div>
		<div class="form-footer">
			<label>Note: zero means that the sensor is OFF</label>
		</div>
		<p>
			<input type="submit" value="Save" />
		</p>
		<p>
			<br />
			<input type="button" value="Adjust time" onclick="window.location.href='/time.html'" />
		</p>
	</form>
</body>
</html>
)=====";
