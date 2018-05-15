const char LOGGRAPH_page[] PROGMEM = R"=====(
<!DOCTYPE html>
	<html>
		<head>
			<meta charset="UTF-8">
				<title>Waterpool Monitor - Logging Graphs</title>

				<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js"></script>

				<script type="text/javascript" src="https://cdnjs.cloudflare.com/ajax/libs/jqPlot/1.0.9/jquery.jqplot.min.js"></script>
				<link rel="stylesheet" type="text/css" href="https://cdnjs.cloudflare.com/ajax/libs/jqPlot/1.0.9/jquery.jqplot.min.css" />
				<script type="text/javascript" src="https://cdnjs.cloudflare.com/ajax/libs/jqPlot/1.0.9/plugins/jqplot.dateAxisRenderer.min.js"></script>

				<script type="text/javascript" src="https://cdn.jsdelivr.net/momentjs/latest/moment.min.js"></script>
				<script type="text/javascript" src="https://cdn.jsdelivr.net/npm/daterangepicker/daterangepicker.min.js"></script>
				<link rel="stylesheet" type="text/css" href="https://cdn.jsdelivr.net/npm/daterangepicker/daterangepicker.css" />

</head>
			<body onload="createPlot('d.bin');">
				<h1>Waterpool monitor - Logging Graphs</h1>

				Date range: <input type="text" name="datetimes" style="width:200px" />

				<div id="chart1" style="width:700px; height:400px"></div>

				<script>
					var plot1 = null;
			var cachedBytes = null;
			var defaultStartDate = moment().startOf('hour').add(-24, 'hour');
			var defaultEndDate = moment().startOf('hour');
	
	
$(function() {
						$('input[name="datetimes"]').daterangepicker({
							timePicker: true,
							startDate: defaultStartDate,
							endDate: defaultEndDate,
							timePicker24Hour: true,
							locale: {
								format: 'DD.MM HH:mm'
							}
						},
							function (startDate, endDate, label) {
								rebuildPlotFromBytes(cachedBytes, startDate, endDate);
							}


						);
					});
					
		function getLogData() {
			var xhttp = new XMLHttpRequest();
					xhttp.open("GET", "d.bin", true);
					xhttp.responseType = "arraybuffer";
					xhttp.send(null);
					if (xhttp.status === 200)
			{
				var arrayBuffer = xhttp.response;
				if (arrayBuffer) {
						var byteArray = new Uint8Array(arrayBuffer);
					return byteArray;
			}
		}
	}

function fillData(byteArray, startDate, endDate)
	{
		var data = [[],[]];
			
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

				var date = new Date(timestamp * 1000);
					if (date >= startDate && date <= endDate)
					{
					var dateStr = date.toISOString();
					var dateString = dateStr.slice(-24, -14)+" "+dateStr.slice(-13, -5);
						data[0].push([dateString, flow]);
						data[1].push([dateString, temp1]);
					}
				}

			return data;
	};

function fetchLogData(url, success) {

			var xhttp = new XMLHttpRequest();
					xhttp.open("GET", url, true);
					xhttp.responseType = "arraybuffer";
		
			xhttp.onload = function (oEvent) {
				var arrayBuffer = xhttp.response;
				if (arrayBuffer) {
						var byteArray = new Uint8Array(arrayBuffer);
					success(byteArray);
					console.log('loaded');
			}
		}
		xhttp.send(null);
}

function rebuildPlotFromBytes(bytes, startDate, endDate)
	{
			var data = fillData(bytes, startDate, endDate);
			if (plot1 == null) {
						plot1 = $.jqplot('chart1', data, {
							title: "Logs Data Renderer",
							axes: {
								xaxis: {
									renderer: $.jqplot.DateAxisRenderer//,
									//          tickOptions:{formatString:'%H:%M:%S'},
									//          tickInterval:'5 second'
								}
							}
							//,series:[{lineWidth:4, markerOptions:{style:'square'}}]
						});
					} else {
						plot1.replot({ data: data });
					console.log('replotting');
				}
		}
	
			function createPlot(url)
	{
						fetchLogData(url, function (bytes) {
							cachedBytes = bytes;
							rebuildPlotFromBytes(bytes, defaultStartDate, defaultEndDate)
						});
					}
				
	</script>
			</body>
</html>
)=====";
