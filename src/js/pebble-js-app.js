var IP = '1';
var PORT = '2';
var ALERT = '3';

var ws;
var alerted = false;

function getSettings(callback) {
  function loadSettings(data) {
    if(!data || !data[IP])
      throw Error();

    return {
      ip: data[IP],
      port: data[PORT] || NaN,
      alert: data[ALERT] || NaN
    };
  }
  
  Pebble.addEventListener('showConfiguration', function(e) {
    Pebble.openURL('http://x.setpebble.com/ZJPA/' + Pebble.getAccountToken());
    
    console.log("Account token: " + Pebble.getAccountToken());
  });

  Pebble.addEventListener('webviewclosed', function(e) {
    var settings;
    
    try {
      settings = loadSettings(JSON.parse(decodeURIComponent(e.response)));

      localStorage.setItem(IP, settings.ip);
      localStorage.setItem(PORT, settings.port);
      localStorage.setItem(ALERT, settings.alert);
      
    }
    catch(ex) {
      console.log("Unable to parse settings: " + decodeURIComponent(e.response));
    }
    
    callback(settings);
  });
  
  var request = new XMLHttpRequest();
  request.onload = function(xhr) {
    var settings = null;

    if(this.responseText) {
      try {
        settings = loadSettings(JSON.parse(this.responseText));

        localStorage.setItem(IP, settings.ip);
        localStorage.setItem(PORT, settings.port);
        localStorage.setItem(ALERT, settings.alert);
      }
      catch(ex) {
        console.log("Unable to parse settings: " + this.responseText);
      }
    }

    callback(settings);
  };
  request.onerror = function(xhr) {
    console.log("Unable to load settings from SetPebble");

    var settings = {
      ip: localStorage.getItem(IP),
      port: localStorage.getItem(PORT),
      alert: localStorage.getItem(ALERT)
    };

    if(settings.ip) {
      callback(settings);
    }
    else {
      callback(null);
    }
  };
  request.open('get', 'http://x.setpebble.com/api/ZJPA/' + Pebble.getAccountToken());
  request.send();
}

function connect(ip, port, callback, retry) {
  retry = retry || 1;
  
  if(ws) {
    ws.onclose = null;
    ws.close();
  }
  
  console.log("Connecting to " + ip + ":" + (port || 5100));
  ws = new WebSocket('ws://' + ip + ':' + (port || 5100));
  
  ws.onmessage = function(event) {
    callback(JSON.parse(event.data));
  };
  
  ws.onopen = function(event) {
    console.log("Connected");
  };

  ws.onclose = function(event) {
    callback(null);
    
    console.log("Disconnected, retrying in " + retry + " seconds");

    setTimeout((function(retry) {
      return function() {
        return connect(ip, port, callback, retry);
      };
    })(Math.min(retry * 2, 64)), retry * 1000);
  };
  
  ws.onerror = function(event) {
    console.log("Error:\n" + JSON.stringify(event));
  };
}

Pebble.addEventListener('ready', function(e) {
  getSettings(function(settings) {
    if(settings) {
      connect(settings.ip, settings.port, function(data) {
        if(data) {
          var temp = Math.max.apply(null, data.CpuInfo.fTemp);
          var alert = temp >= settings.alert;
          var min = 40;
          var max = data.CpuInfo.uiTjMax[0];
          var load = 0;
          var memory = data.MemoryInfo.MemoryLoad;
          
          for(var i in data.CpuInfo.uiLoad) {
            load += data.CpuInfo.uiLoad[i];
          }
          
          load /= data.CpuInfo.uiLoad.length;
          
          var message = {
              temp: temp,
              alert: (alert && !alerted ? 1 : 0),
              min: min,
              max: max,
              load: load,
              memory: memory
            };
          
          Pebble.sendAppMessage(message);

          alerted = alert;
        }
        else {
          Pebble.sendAppMessage({});
        }
      });
    }
    else {
      Pebble.sendAppMessage({alert: 1});
    }
  });
});