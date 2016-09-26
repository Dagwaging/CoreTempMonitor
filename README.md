# CoreTempMonitor
A simple Pebble app for tracking your computer's CPU temperature in real time.

## Usage
1. Install the Core Temp Monitor app on your Pebble smartwatch via the [Pebble Store](https://apps.getpebble.com/en_US/application/55fdac750a5482833d000085) or Developer settings
2. Install [Core Temp](http://www.alcpu.com/CoreTemp/)
3. Download the [Core Temp plugin](https://github.com/zanecodes/CoreTempMonitor/raw/master/assets/CoreTempWebsockets.zip)
4. Extract the Core Temp plugin to `C:\Program Files\Core Temp\plugins`
5. Configure the Core Temp Monitor Pebble app with your computer's IP address and port number (or leave blank for 5100, the default)
6. Optionally, configure your router to forward port 5100 to your computer so it doesn't need to be on the same network as your phone
