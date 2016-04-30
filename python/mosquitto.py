#!/usr/bin/python3.5

# The code uses the Paho Python client to connect to a MQTT broker running on localhost.

import paho.mqtt.client as mqtt
import time
import datetime

def on_connect(client, userdata, flags, rc):
	print("Connected with result code: " + mqtt.connack_string(rc) + " " + str(rc))
	client.subscribe("hello/world")		# subscribe to a topic to receive messages
	
def on_message(client, userdata, msg):
	print(msg.topic + " " + str(msg.payload))
	
client = mqtt.Client()					# create a MQTT "client" instance
client.on_connect = on_connect			# assign "on_connect" function as method
client.on_message = on_message			# assign "on_message" function as method
client.will_set("mytopic","This is my last will. Amen!",0,True)

client.connect("localhost", 1883, 60)	# connect to a broker
client.loop_start()					# call loop to maintain traffic flow

while True:
	msg = str(datetime.datetime.today())
	msg = "Time on client is: " + msg
	(rc, mid) = client.publish("mytopic", msg)
	print("Published time: " + msg)
	print("Return code: " + mqtt.error_string(rc) + "msg id: " + str(mid))
	time.sleep(20) # delays for 20 seconds
