# net_monitor
Sketch to monitor the status of my router, internet connection and local servers.
----
My Smart Home has grown into a Frankenstein monster.

I started with Node-Red and Mosquitto MQTT broker on a single Raspberry Pi3.  At some point I wanted to add some Z-Wave
devices and someone on the Node-Red forums told me that my best route was to install Home Assistant and an Aeotec Z-Stick,
then I could control the Z-Wave devices from Node-red.  I didn't want to risk messing up my currently running Pi with
Node-red and Mosquitto, so I installed Home Assistant on a second Raspberry Pi.  I discovered that I like Home Assistant
and instead of growing my system on Node-Red, as originally planned, I spent the next months adding more devices to Home Assistant.

So now I had two Raspberry Pis running my smart home.  Then came the Tradfri lights, so add the Tradfri Gateway to the mix.
Next, I added some IP cameras.

So when my wife says that "Alexa isn't listening to me!", I have a number of places to look for the problem.
(Since I am always doing something to my stuff, I am always breaking something).

My troubleshooting process involves pinging all the likely suspects to make sure they are all online.  I wanted a faster and
easier way to determine where to start looking for a problem.  Thus was born my Net Monitor Applpiance project.

The Net Monitor Appliance just pings the IP address of the critical devices on my network, and if a good ping is found,
lights a green LED.  If a ping is not responsive, a red LED lights.  The loop repeats every 30-seconds.  So, if I look up
and see a "green board", then I know not to take the time to ping up to 10 devices.

I chose to make the Net Monitor on the NodeMCU platform because it has WiFi and works from any 5V micro-USB wall wart.
I could have easily included an LCD display, but I just wanted to have something to determine at a glance if I have a net problem.
