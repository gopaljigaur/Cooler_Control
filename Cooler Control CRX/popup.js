window.onload = function() {
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
  var onlineserver = 'https://gopalji.ml/.netlify/functions/alternate';
  otherWiFi.disabled = true;
  setTimer.disabled = true;

function httpGetAsync(theUrl,callback)
{
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
            callback(xmlHttp.responseText);
          console.log(xmlHttp.responseText);
    }
    xmlHttp.open("GET", theUrl, true); // true for asynchronous 
    xmlHttp.setRequestHeader('Access-Control-Allow-Origin', '*');
    xmlHttp.send(null);
}

function whatsup(){
httpGetAsync(server.concat('/status'),function(e){
  var ison = parseInt(e.split(' ')[1])==1?true:false;
  if(ison){
    switchOnButton.disabled = true;
    switchOnButton.className = "";
    switchOffButton.disabled = false;
    switchOffButton.className = "available";
  }
  else{
    switchOnButton.disabled = false;
    switchOnButton.className = "available";
    switchOffButton.disabled = true;
    switchOffButton.className = "";
  }
});
}

httpGetAsync('http://192.168.43.146/status',function(e){
      	server = 'http://192.168.43.146';
      	whatsup();
      });

httpGetAsync('http://192.168.43.246/status',function(e){
      	server = 'http://192.168.43.246';
      whatsup();
  });

httpGetAsync(onlineserver,function(e){
	lastseen.innerHTML = e;
  lastseen.style.display='inline-block';
});

      switchOffButton.addEventListener('click', function() {
        httpGetAsync(server.concat('/off'),function(){});
      switchOnButton.disabled = false;
      switchOnButton.className = "available";
      switchOffButton.disabled = true;
      switchOffButton.className = "";
    });
      switchOnButton.addEventListener('click', function() {
    httpGetAsync(server.concat('/on'),function(){});
      switchOnButton.disabled = true;
      switchOnButton.className = "";
      switchOffButton.disabled = false;
      switchOffButton.className = "available";
    });

  homeWiFi.addEventListener('click', function() {

      server = 'http://192.168.15.100';
      console.log(server.concat(' selected'));
      homeWiFi.disabled = true;
      homeWiFi.className = "";
      coolerWiFi.disabled = false;
      coolerWiFi.className = "available";
      otherWiFi.disabled = false;
      otherWiFi.className = "available";
      whatsup();
    });
  coolerWiFi.addEventListener('click', function() {

      server = 'http://192.168.4.1';
      console.log(server.concat(' selected'));
      homeWiFi.disabled = false;
      homeWiFi.className = "available";
      coolerWiFi.disabled = true;
      coolerWiFi.className = "";
      otherWiFi.disabled = false;
      otherWiFi.className = "available";
      whatsup();
    });
  otherWiFi.addEventListener('click', function() {

      httpGetAsync('http://192.168.43.146/status',function(e){
      	server = 'http://192.168.43.146';
      	
      });
httpGetAsync('http://192.168.43.246/status',function(e){
      	server = 'http://192.168.43.246';

      });

      console.log(server.concat(' selected'));
      otherWiFi.disabled = true;
      otherWiFi.className = "";
      homeWiFi.disabled = false;
      homeWiFi.className = "available";
      coolerWiFi.disabled = false;
      coolerWiFi.className = "available";
      whatsup();
    });

  hours.addEventListener('keyup',function(){
    if((hours.value==''||hours.value=='0')&&(minutes.value==''||minutes.value=='0')&&(seconds.value==''||seconds.value=='0'))
  		{
        setTimer.disabled=true;
      setTimer.className = "";
    }
    else{
        setTimer.disabled=false;
      setTimer.className = "available";
    }
  });
  minutes.addEventListener('keyup',function(){
if((hours.value==''||hours.value=='0')&&(minutes.value==''||minutes.value=='0')&&(seconds.value==''||seconds.value=='0'))
      {
        setTimer.disabled=true;
      setTimer.className = "";
    }
    else{
        
      setTimer.disabled=false;
      setTimer.className = "available";
    }
  });
  seconds.addEventListener('keyup',function(){
  		if((hours.value==''||hours.value=='0')&&(minutes.value==''||minutes.value=='0')&&(seconds.value==''||seconds.value=='0'))
      {
         setTimer.disabled=true;
      setTimer.className = "";
    }
    else{
       
      setTimer.disabled=false;
      setTimer.className = "available";
    }
  });
  setTimer.addEventListener('click',function(){
  	if(setTimer.innerHTML!="Cancel Timer"){
  	if((hours.value==''||hours.value=='0')&&(minutes.value==''||minutes.value=='0')&&(seconds.value==''||seconds.value=='0')){
  		setTimer.className = "";
      setTimer.disabled = true;
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
      httpGetAsync(server.concat('/setTimer?secs=',value_to_send),function(){});
      //setTimer.className = "";
    	//setTimer.disabled = true;
    	setTimer.innerHTML = "Cancel Timer";
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
	}
	else{
		httpGetAsync(server.concat('/cancel'),function(){});
		setTimer.innerHTML = "Set Timer";
		setTimer.disabled = true;
		setTimer.className = "";
	}
  	});

  }