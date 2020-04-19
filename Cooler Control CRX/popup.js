document.addEventListener('DOMContentLoaded', function() {
  var switchOnButton = document.getElementById('switchOn');
  var switchOffButton = document.getElementById('switchOff');
  var homeWiFi = document.getElementById('home');
  var coolerWiFi = document.getElementById('cooler');
  var server = 'http://192.168.15.150'
  var firstTime = true;
  homeWiFi.disabled = true;

  homeWiFi.addEventListener('click', function() {

      server = 'http://192.168.15.150';
      console.log(server.concat(' selected'));
      homeWiFi.disabled = true;
      coolerWiFi.disabled = false;
      if(firstTime){
      switchOnButton.disabled = false;
      switchOffButton.disabled = false;
      firstTime=false;
    }
    });
  coolerWiFi.addEventListener('click', function() {

      server = 'http://192.168.4.1';
      console.log(server.concat(' selected'));
      homeWiFi.disabled = false;
      coolerWiFi.disabled = true;
      if(firstTime){
      switchOnButton.disabled = false;
      switchOffButton.disabled = false;
      firstTime=false;
    }
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


  }, false);