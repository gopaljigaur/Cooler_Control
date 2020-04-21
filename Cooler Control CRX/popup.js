window.onload = function() {
  var switchOnButton = document.getElementById('switchOn');
  var switchOffButton = document.getElementById('switchOff');
  var homeWiFi = document.getElementById('home');
  var coolerWiFi = document.getElementById('cooler');
  var hours = document.getElementById('hours');
  var minutes = document.getElementById('minutes');
  var seconds = document.getElementById('seconds');
  var setTimer = document.getElementById('setTimer');
  var dev = document.getElementById('dev');
  var developer = document.getElementById('developer');
  var restartN = document.getElementById('restartN');
  var restartC = document.getElementById('restartC');
  var server = 'http://192.168.15.150';

  homeWiFi.disabled = true;
  setTimer.disabled = true;
  developer.style.display = "none";

    restartN.addEventListener('click', function() {

      d = document;
      var f = d.createElement('form');
      f.action = server.concat('/restart?boot=normal');
      f.method = 'post';
      d.body.appendChild(f);
      f.submit();
      d.body.removeChild(f);
      alert("Restarting in normal mode");
    });
    restartC.addEventListener('click', function() {

      d = document;
      var f = d.createElement('form');
      f.action = server.concat('/restart?boot=program');
      f.method = 'post';
      d.body.appendChild(f);
      f.submit();
      d.body.removeChild(f);
      alert("Restarting in programming mode");
    });
    dev.addEventListener('click', function() {

      if (developer.style.display === "none") {
        developer.style.display = "block";
      } 
      else {
      developer.style.display = "none";
        }
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
  hours.addEventListener('click',function(){
  		setTimer.disabled=false;
  });
  minutes.addEventListener('click',function(){

  		setTimer.disabled=false;
  });
  seconds.addEventListener('click',function(){
  		setTimer.disabled=false;
  });
  setTimer.addEventListener('click',function(){
  	if((hours.value==''||hours.value=='0')&&(minutes.value==''||minutes.value=='0')&&(seconds.value==''||seconds.value=='0')){
  		alert('I am not an idiot like you');
  	}
  	else{
  		var c = 3;
  		if(hours.value==''){
  			hours.value='0';
  			c=c-1;
  		}
  		if(minutes.value==''){
  			minutes.value='0';
  			c=c-1;
  		}
  		if(seconds.value==''){
  			seconds.value='0';
  			c=c-1;
  		}
  		var value_to_send = hours.value*3600 + minutes.value*60 +seconds.value;
  		d = document;
  		var f = d.createElement('form');
  		f.action = server.concat('/specificArgs?secs=',value_to_send);
  		f.method = 'post';
    	d.body.appendChild(f);
    	f.submit();
    	d.body.removeChild(f);
    	setTimer.disabled = true;
    	hrs_phrase='';
    	min_phrase='';
    	sec_phrase='';
   		if(hours.value==1){
   			hrs_phrase='1 hour ';
   		}
   		if(minutes.value==1){
   			min_phrase=' 1 minute ';
   		}
   		if(seconds.value==1){
   			sec_phrase=' 1 second';
   		}

   		if(hours.value>1){
   			hrs_phrase=(hours.value).concat(' hours ');
   		}
   		if(minutes.value>1){
   			min_phrase=(minutes.value).concat(' minutes ');
   		}
   		if(seconds.value>1){
   			sec_phrase=(seconds.value).concat(' seconds');
   		}
   		var alert_str = 'Timer set for '.concat(hrs_phrase,min_phrase,sec_phrase);
   		//alert(alert_str);
    	alert_str = alert_str.trim();
    	if(c==1){
    		alert_str=alert_str.concat('.');
    	}
    	if(c==2){
    		var arr = alert_str.split(" ");
    		alert_str = arr[0].concat(" ",arr[1]," ",arr[2]," ",arr[3].concat(" ",arr[4]," and ",arr[5]," ",arr[6],"."));
    	}
    	if(c==3){
    		var arr = alert_str.split(" ");
    		alert_str = arr[0].concat(" ",arr[1]," ",arr[2]," ",arr[3].concat(" ",arr[4],", ",arr[5]," ",arr[6]," and ",arr[7]," ",arr[8],"."));
    	}
    	
    	alert(alert_str);
    	hours.value='';
    	minutes.value='';
    	seconds.value='';
		}
  	});

  }