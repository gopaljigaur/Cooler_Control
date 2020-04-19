window.onload = function() {
  var switchOnButton = document.getElementById('switchOn');
  var switchOffButton = document.getElementById('switchOff');
  var homeWiFi = document.getElementById('home');
  var coolerWiFi = document.getElementById('cooler');
  var seconds = document.getElementById('seconds');
  var setTimer = document.getElementById('setTimer');
  var server = 'http://192.168.15.150'
  homeWiFi.disabled = true;
  setTimer.disabled = true;
  homeWiFi.addEventListener('click', function() {

      server = 'http://192.168.15.150';
      console.log(server.concat(' selected'));
      homeWiFi.disabled = true;
      coolerWiFi.disabled = false;
    });
  coolerWiFi.addEventListener('click', function() {

      server = 'http://192.168.4.1';
      console.log(server.concat(' selected'));
      homeWiFi.disabled = false;
      coolerWiFi.disabled = true;
    });
  switchOnButton.addEventListener('click', function() {

      d = document;
      var f = d.createElement('form');
      f.action = server.concat('/on');
      f.method = 'post';
      d.body.appendChild(f);
      f.submit();
      d.body.removeChild(f);
      switchOnButton.disabled = true;
      switchOffButton.disabled = false;

    });
  switchOffButton.addEventListener('click', function() {

      d = document;
      var f = d.createElement('form');
      f.action = server.concat('/off');
      f.method = 'post';
      d.body.appendChild(f);
      f.submit();
      d.body.removeChild(f);
      switchOnButton.disabled = false;
      switchOffButton.disabled = true;
    });
  seconds.addEventListener('click',function(){
  		setTimer.disabled=false;
  });
  setTimer.addEventListener('click',function(){
  	if(seconds.value==''||seconds.value=='0'){
  		alert('I am not an idiot like you');
  	}
  	else{
  	d = document;
  	var f = d.createElement('form');
  	f.action = server.concat('/specificArgs?secs=',seconds.value);
  	f.method = 'post';
    d.body.appendChild(f);
    f.submit();
    d.body.removeChild(f);
    setTimer.disabled = true;
    alert('Timer set for '.concat(seconds.value,' seconds.'));
    seconds.value=''
	}
  });

  }