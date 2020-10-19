window.onload = function () {
	var switchOnButton = document.getElementById('switchOn');
	var switchOffButton = document.getElementById('switchOff');
	var homeWiFi = document.getElementById('home');
	var coolerWiFi = document.getElementById('cooler');
	var otherWiFi = document.getElementById('other');
	var hours = document.getElementById('hours');
	var minutes = document.getElementById('minutes');
	var seconds = document.getElementById('seconds');
	var setTimer = document.getElementById('setTimer');
	var lastseen = document.getElementById('lastseen');
	var server = '';
	var timlef = false;
	var onlineserver = 'https://gopalji.ml/.netlify/functions/alternate';
	chrome.storage.local.get(['key'], function (result) {
		console.log(result.key);
		if (result.key) {
			if (result.key == 0) {
				homeWiFi.disabled = true;
				coolerWiFi.className = "available";
				otherWiFi.className = "available";
			}
			if (result.key == 1) {
				coolerWiFi.disabled = true;
				homeWiFi.className = "available";
				otherWiFi.className = "available";
			} else {
				otherWiFi.disabled = true;
				coolerWiFi.className = "available";
				homeWiFi.className = "available";
			}
		} else {
			otherWiFi.disabled = true;
			coolerWiFi.className = "available";
			homeWiFi.className = "available";
		}
	});
	setTimer.disabled = true;

	function httpGetAsync(theUrl, callback) {
		var xmlHttp = new XMLHttpRequest();
		xmlHttp.onreadystatechange = function () {
			if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
				callback(xmlHttp.responseText);
			console.log(xmlHttp.responseText);
		}
		xmlHttp.open("GET", theUrl, true); // true for asynchronous 
		xmlHttp.setRequestHeader('Access-Control-Allow-Origin', '*');
		xmlHttp.send(null);
	}

	function whatsup() {
		httpGetAsync(server.concat('/status'), function (e) {
			var ison = parseInt(e.split(' ')[1]) == 1 ? true : false;
			timlef = parseInt(e.split(' ')[0]) > 0 ? true : false;
			if (ison) {
				switchOnButton.disabled = true;
				switchOnButton.className = "";
				switchOffButton.disabled = false;
				switchOffButton.className = "available";
			} else {
				switchOnButton.disabled = false;
				switchOnButton.className = "available";
				switchOffButton.disabled = true;
				switchOffButton.className = "";
			}
			if (timlef) {
				setTimer.innerHTML = "Cancel Timer";
				setTimer.disabled = false;
				setTimer.className = "available";
			}
		});
	}

	httpGetAsync('http://192.168.43.146/status', function (e) {
		server = 'http://192.168.43.146';
		whatsup();
	});

	httpGetAsync('http://192.168.43.246/status', function (e) {
		server = 'http://192.168.43.246';
		whatsup();
	});

	httpGetAsync(onlineserver, function (e) {
		lastseen.innerHTML = e;
		lastseen.style.display = 'inline-block';
	});

	switchOffButton.addEventListener('click', function () {
		httpGetAsync(server.concat('/off'), function () {});
		switchOnButton.disabled = false;
		switchOnButton.className = "available";
		switchOffButton.disabled = true;
		switchOffButton.className = "";
	});
	switchOnButton.addEventListener('click', function () {
		httpGetAsync(server.concat('/on'), function () {});
		switchOnButton.disabled = true;
		switchOnButton.className = "";
		switchOffButton.disabled = false;
		switchOffButton.className = "available";
	});

	homeWiFi.addEventListener('click', function () {

		server = 'http://192.168.15.100';
		console.log(server.concat(' selected'));
		homeWiFi.disabled = true;
		homeWiFi.className = "";
		coolerWiFi.disabled = false;
		coolerWiFi.className = "available";
		otherWiFi.disabled = false;
		otherWiFi.className = "available";
		chrome.storage.local.set({
			key: 0
		}, function () {
			console.log('Value is set to 0');
		});
		whatsup();
	});
	coolerWiFi.addEventListener('click', function () {

		server = 'http://192.168.4.1';
		console.log(server.concat(' selected'));
		homeWiFi.disabled = false;
		homeWiFi.className = "available";
		coolerWiFi.disabled = true;
		coolerWiFi.className = "";
		otherWiFi.disabled = false;
		otherWiFi.className = "available";
		chrome.storage.local.set({
			key: 1
		}, function () {
			console.log('Value is set to 1');
		});
		whatsup();
	});
	otherWiFi.addEventListener('click', function () {

		httpGetAsync('http://192.168.43.146/status', function (e) {
			server = 'http://192.168.43.146';

		});
		httpGetAsync('http://192.168.43.246/status', function (e) {
			server = 'http://192.168.43.246';

		});

		console.log(server.concat(' selected'));
		otherWiFi.disabled = true;
		otherWiFi.className = "";
		homeWiFi.disabled = false;
		homeWiFi.className = "available";
		coolerWiFi.disabled = false;
		coolerWiFi.className = "available";
		chrome.storage.local.set({
			key: 2
		}, function () {
			console.log('Value is set to 2');
		});
		whatsup();
	});

	function handleKeyUp() {
		if ((hours.value == '' || hours.value == '0') && (minutes.value == '' || minutes.value == '0') && (seconds.value == '' || seconds.value == '0')) {
			if (timlef) {
				setTimer.innerHTML = "Cancel Timer";
				setTimer.className = "available";
			} else {
				setTimer.disabled = true;
				setTimer.className = "";
				setTimer.innerHTML = "Set Timer";
			}
		} else {
			setTimer.disabled = false;
			setTimer.className = "available";
			setTimer.innerHTML = "Set Timer";
		}
	}

	hours.addEventListener('keyup', function () {
		handleKeyUp()
	});
	minutes.addEventListener('keyup', function () {
		handleKeyUp()
	});
	seconds.addEventListener('keyup', function () {
		handleKeyUp()
	});

	hours.addEventListener('mouseup', function () {
		handleKeyUp()
	});
	minutes.addEventListener('mouseup', function () {
		handleKeyUp()
	});
	seconds.addEventListener('mouseup', function () {
		handleKeyUp()
	});

	setTimer.addEventListener('click', function () {
		if (setTimer.innerHTML != "Cancel Timer") {
			if ((hours.value == '' || hours.value == '0') && (minutes.value == '' || minutes.value == '0') && (seconds.value == '' || seconds.value == '0')) {
				setTimer.className = "";
				setTimer.disabled = true;
			} else {
				var c = 3;
				if (hours.value == '') {
					hours.value = '0';
					c = c - 1;
				}
				if (minutes.value == '') {
					minutes.value = '0';
					c = c - 1;
				}
				if (seconds.value == '') {
					seconds.value = '0';
					c = c - 1;
				}
				var value_to_send = (parseInt(hours.value) * 3600 + parseInt(minutes.value) * 60 + parseInt(seconds.value)).toString();
				httpGetAsync(server.concat('/setTimer?secs=', value_to_send), function () {});
				setTimer.innerHTML = "Cancel Timer";
				hrs_phrase = '';
				min_phrase = '';
				sec_phrase = '';
				if (hours.value == 1) {
					hrs_phrase = '1 hour ';
				}
				if (minutes.value == 1) {
					min_phrase = ' 1 minute ';
				}
				if (seconds.value == 1) {
					sec_phrase = ' 1 second';
				}

				if (hours.value > 1) {
					hrs_phrase = (hours.value).concat(' hours ');
				}
				if (minutes.value > 1) {
					min_phrase = (minutes.value).concat(' minutes ');
				}
				if (seconds.value > 1) {
					sec_phrase = (seconds.value).concat(' seconds');
				}
				var alert_str = 'Timer set for '.concat(hrs_phrase, min_phrase, sec_phrase);
				//alert(alert_str);
				alert_str = alert_str.trim();
				if (c == 1) {
					alert_str = alert_str.concat('.');
				}
				if (c == 2) {
					var arr = alert_str.split(" ");
					alert_str = arr[0].concat(" ", arr[1], " ", arr[2], " ", arr[3].concat(" ", arr[4], " and ", arr[5], " ", arr[6], "."));
				}
				if (c == 3) {
					var arr = alert_str.split(" ");
					alert_str = arr[0].concat(" ", arr[1], " ", arr[2], " ", arr[3].concat(" ", arr[4], ", ", arr[5], " ", arr[6], " and ", arr[7], " ", arr[8], "."));
				}

				alert(alert_str);
				hours.value = '';
				minutes.value = '';
				seconds.value = '';
			}
		} else {
			httpGetAsync(server.concat('/cancel'), function () {});
			setTimer.innerHTML = "Set Timer";
			setTimer.disabled = true;
			setTimer.className = "";
		}
	});
}