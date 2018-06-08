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
	<script type="text/javascript" src="https://cdnjs.cloudflare.com/ajax/libs/jqPlot/1.0.9/plugins/jqplot.canvasOverlay.min.js"></script>
	<script type="text/javascript" src="https://cdnjs.cloudflare.com/ajax/libs/jqPlot/1.0.9/plugins/jqplot.highlighter.min.js"></script>

	<script type="text/javascript" src="https://cdn.jsdelivr.net/momentjs/latest/moment.min.js"></script>
	<script type="text/javascript" src="https://cdn.jsdelivr.net/npm/daterangepicker/daterangepicker.min.js"></script>
	<link rel="stylesheet" type="text/css" href="https://cdn.jsdelivr.net/npm/daterangepicker/daterangepicker.css" />
	<script>
		var plot1 = null;
		var plot2 = null;
		var cachedBytes = null;
		var defaultStartDate = moment().startOf('hour').add(-23, 'hour');
		var defaultEndDate = moment().startOf('hour').add(+1, 'hour');


		$(function () {
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

		function fillData(byteArray, startDate, endDate) {
			var data1 = [[], [], [], [], []];
			var data2 = [[], [], [], [], [], []];
			var ind = 0;
			var pointsNo = 0;
			var mindate = Number.MAX_SAFE_INTEGER;
			var maxdate = 0;
			while (ind < byteArray.byteLength) {
				var timestamp = byteArray[ind] | byteArray[ind + 1] << 8 |
					byteArray[ind + 2] << 16 | byteArray[ind + 3] << 24;
				ind += 4;
				var flow = byteArray[ind];
				var temp1 = byteArray[ind + 1];
				var temp2 = byteArray[ind + 2];
				var temp3 = byteArray[ind + 3];
				var temp4 = byteArray[ind + 4];
				var state = byteArray[ind + 5];
				ind += 6;

				var date = new Date(timestamp * 1000);
				if (date >= startDate && date <= endDate) {
					if (date < mindate)
						mindate = date;	
					if (date > maxdate)
						maxdate = date;	
					var dateStr = date.toISOString();
					var dateString = dateStr.slice(-24, -14) + " " + dateStr.slice(-13, -5);
					data1[0].push([dateString, flow]);
					data1[1].push([dateString, temp1]);
					data1[2].push([dateString, temp2]);
					data1[3].push([dateString, temp3]);
					data1[4].push([dateString, temp4]);

					data2[0].push([dateString, +bit_test(state, 0)]);
					data2[1].push([dateString, +bit_test(state, 1) + 4]);
					data2[2].push([dateString, +bit_test(state, 2)]);
					data2[3].push([dateString, +bit_test(state, 3) + 4]);
					data2[4].push([dateString, +bit_test(state, 5) + 8]);
					data2[5].push([dateString, +bit_test(state, 6) + 12]);
					pointsNo++;
				}
			}
			if (pointsNo == 0) {
				data1 = [[null]];
				data2 = [[null]];
				document.getElementById("daterange").innerHTML = 'No data found for the selected period.';
			}
			else 
			{
				document.getElementById("daterange").innerHTML = 'Data found for period: ' + mindate.toLocaleString() + ' - ' + maxdate.toLocaleString() + ', total ' + pointsNo + ' points.';
				console.log('Number of points: ' + pointsNo);
			}

			return { chart1: data1, chart2: data2 };
		};

		function bit_test(num, bit) {
			return ((num >> bit) % 2 != 0)
		}

		function updateProgress(evt) 
		{
			document.getElementById("progress").innerHTML = 'Loaded ' + evt.loaded + ' bytes';
		}   

		function fetchLogData(url, success) {

			var xhttp = new XMLHttpRequest();
			xhttp.open("GET", url, true);
			xhttp.responseType = "arraybuffer";
			xhttp.onprogress = updateProgress;
			xhttp.onload = function (oEvent) 
			{
				if (xhttp.status != 200)
				{
					document.getElementById("progress").innerHTML = xhttp.status.toString() + " " + xhttp.statusText;
					return;
				}

				var arrayBuffer = xhttp.response;
				if (arrayBuffer) {
					var byteArray = new Uint8Array(arrayBuffer);
					success(byteArray);
					console.log('loaded');
				}
			}
			xhttp.send(null);
		}

		function rebuildPlotFromBytes(bytes, startDate, endDate) {
			var timeZoneOffset = new Date().getTimezoneOffset();
			startDate = startDate.add(-timeZoneOffset, 'minute');
			endDate = endDate.add(-timeZoneOffset, 'minute');

			var data = fillData(bytes, startDate, endDate);
			if (plot1 == null) {
				plot1 = $.jqplot('chart1', data.chart1, {
					title: "Sensors Data",
					axes: {
						xaxis: {
							renderer: $.jqplot.DateAxisRenderer//,
							//          tickOptions:{formatString:'%H:%M:%S'},
							//          tickInterval:'5 second'
						}
					},
					highlighter: { show: true },
					legend: {
						show: true,
						placement: 'outside'
					},
					series: [
						{
							fill: true,
							label: 'Flow'
						},
						{
							label: 'Temp1',
						        color: 'rgba(0, 0, 255, 0.5)',
        					  	markerOptions: { style: 'circle', size: 2 }
						},
						{
							label: 'Temp2',
					                color: 'rgba(135, 206, 235, 0.5)',
        					  	markerOptions: { style: 'circle', size: 2 }
						},
						{
							label: 'Temp3',
					                color: 'rgba(255, 0, 0, 0.5)',
						        showLine:false, 
							markerOptions: { size: 2, style:"x" }
						},
						{
							label: 'Temp4',
					                color: 'rgba(250, 128, 114, 0.5)',
						        showLine:false, 
						        markerOptions: { size: 2, style:"x" }
						}
					]
				});
				plot2 = $.jqplot('chart2', data.chart2, {
					title: "State Data",
					axes: {
						xaxis: {
							renderer: $.jqplot.DateAxisRenderer//,
							//          tickOptions:{formatString:'%H:%M:%S'},
							//          tickInterval:'5 second'
						},
						yaxis: {
							tickOptions: {
								show: false
							},
							rendererOptions: {
								drawBaseline: false
							}
						}
					},
					canvasOverlay: {
						show: true,
						objects: [
							{ rectangle: { ymin: 0, ymax: 1, xminOffset: "0px", xmaxOffset: "0px", yminOffset: "0px", ymaxOffset: "0px", color: "rgba(0, 0, 200, 0.1)" } },
							{ rectangle: { ymin: 4, ymax: 5, xminOffset: "0px", xmaxOffset: "0px", yminOffset: "0px", ymaxOffset: "0px", color: "rgba(0, 0, 200, 0.1)" } },
							{ rectangle: { ymin: 8, ymax: 9, xminOffset: "0px", xmaxOffset: "0px", yminOffset: "0px", ymaxOffset: "0px", color: "rgba(0, 0, 200, 0.1)" } },
							{ rectangle: { ymin: 12, ymax: 13, xminOffset: "0px", xmaxOffset: "0px", yminOffset: "0px", ymaxOffset: "0px", color: "rgba(0, 0, 200, 0.1)" } }
						]
					},
					legend: {
						show: true,
						placement: 'outside'
					},
					series: [
						{
							label: 'Pump Enable',
							color: 'blue'
						},
						{
							label: 'Heater Enable',
							color: 'red'
						},
						{
							label: 'Pump Relay',
							color: 'skyblue',
							linePattern: 'dotted'
						},
						{
							label: 'Heat Relay',
							color: 'salmon',
							linePattern: 'dotted'
						},
						{
							label: 'Relay3',
							color: 'yellow',
							linePattern: 'dashed'
						},
						{
							label: 'Relay4',
							color: 'yellowgreen',
							linePattern: 'dashed'
						}
					],
					seriesDefaults: {
						rendererOptions: {
							smooth: true
						},
						step: true,
						markerOptions: { show: false }
					}

					//,series:[{lineWidth:4, markerOptions:{style:'square'}}]
				});
			} else {
				plot1.replot({ data: data.chart1 });
				plot2.replot({ data: data.chart2 });
				console.log('replotting');
			}
		}

		function createPlot(url) {
			fetchLogData(url, function (bytes) {
				cachedBytes = bytes;
				rebuildPlotFromBytes(bytes, defaultStartDate, defaultEndDate)
			});
		}

	</script>

</head>
<body onload="createPlot('logs');">
	<h1>Waterpool monitor - Logging Graphs</h1>

	Date range: <input type="text" name="datetimes" style="width:200px" />
	<br/>
	<span id="daterange"></span>
	<br/>
	<div id="chart1" style="width:700px; height:400px"></div>
	<div id="chart2" style="width:700px; height:400px"></div>
	<br/>
	<span id="progress">Loading...</span>
</body>
</html>
)=====";
