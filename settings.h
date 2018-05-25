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
input.readonly{
    width: 12em;
    outline: none;
    border: 0px solid;
    margin-left: 1em;
}


</style>
<script>
(function() {
	function toJSONString( form ) {
		var obj = {};
		var elements = form.querySelectorAll( "input, select, textarea" );
		for( var i = 0; i < elements.length; ++i ) {
			var element = elements[i];
			var name = element.name;
			var value = element.value;
			if (element.type === 'checkbox')
				value = element.checked ? 'true' : 'false';

			if( name ) {
				obj[ name ] = value;
			}
		}

		return JSON.stringify( obj );
	}


	document.addEventListener( "DOMContentLoaded", function() {
		var form = document.getElementById( "test" );
		var output = document.getElementById( "output" );
		form.addEventListener( "submit", function( e ) {
			e.preventDefault();
			var json = toJSONString( this );
			output.innerHTML = json;

		}, false);

	});

})();

	function fromJSONString( jsonString ) {
		var obj = JSON.parse(jsonString); 
		for (var key in obj) {
		    if (obj.hasOwnProperty(key)) {
			//console.log(key + " -> " + obj[key]);
			var elem = document.getElementsByName(key)[0];
			elem.value = obj[key];
			if (elem.type === 'checkbox')
				elem.checked = obj[key] == 'true';
		    }
		}
	};

</script>
</head>

<body>
	<h1>Waterpool monitor - Settings</h1>

<form id="test" action="#" method="post">
    <div class="form-header">
        <label>Temperature sensor maximums</label>
    </div>
    <div class="form-group">
        <label for="temp1">1. </label>
        <input class="maxval" type="number" name="temp1" id="temp1" min="0" max="99" />
        <input class="readonly" type="text" name="temp1id" id="temp1id" readonly value="32 43 54 65 76 56 54 43"/>
    </div>
    <div class="form-group">
        <label for="temp2">2. </label>
        <input class="maxval" type="number" name="temp2" id="temp2" min="0" max="99" />
        <input class="readonly" type="text" name="temp2id" id="temp2id" readonly/>
    </div>
    <div class="form-group">
        <label for="temp3">3. </label>
        <input class="maxval" type="number" name="temp3" id="temp3" min="0" max="99" />
        <input class="readonly" type="text" name="temp3id" id="temp3id" readonly/>
    </div>
    <div class="form-group">
        <label for="temp4">4. </label>
        <input class="maxval" type="number" name="temp4" id="temp4" min="0" max="99" />
        <input class="readonly" type="text" name="temp4id" id="temp4id" readonly/>
    </div>
    <div class="form-header">
        <label>Flow sensor minimums</label>
    </div>
    <div class="form-group">
        <label for="flow1">Flow: </label>
        <input class="maxval" type="number" name="flow1" id="flow1" min="0" max="999" />
    </div>
    <div class="form-header">
        <label>Sensor data logging</label>
    </div>
    <div class="form-group">
        <label for="logging">Period, seconds: </label>
        <input class="logval" type="number" name="logging" id="logging" min="0" max="3600"/>
    </div>
    <div class="form-footer">
        <label>Note: zero means that the sensor is OFF</label>
    </div>

    <div class="form-group">
    	<label for="check">Checkbox test</label>
    	<input type="checkbox" name="check" value="true" />
    </div>
    <div class="form-group">
    	<label for="select">Combo-box test</label>
    	<select name="select" class="form-control">
    		<option value="none" selected="selected">None</option>
    		<option value="option1">Choose1</option>
    		<option value="option2">Choose2</option>
    	</select>

    </div>
    <div class="form-group">
    	<label for="message">Multiline message test</label><br/>
    	<textarea class="form-control" name="message" rows="10" cols="50"></textarea>
    </div>


    <p>
        <input type="submit" value="Save" />
        <input type="button" value="Get values (test)" onclick="fromJSONString( output.innerHTML );" />
    </p>
    <p>
	<br/>
        <input type="button" value="Adjust time" onclick="window.location.href='/time.html'" />
    </p>
</form>	
<pre id="output"></pre>
</body>
</html>
)=====";
