# ESP8266 - MQTT

    ESP8266 MQTT projects using Arduino IDE and Adafruit Huzzah board
    Board: Adafruit Huzzah ESP8266
    IC: ESP8266 - E12
    
## GPIO pins

    V+
        schottky diode(can connect both V+ and Vbat to different voltages)

    Vbat
        schottky diode(can connect both V+ and Vbat to different voltages)

    3V3
        output from internal regulator
        
    RX
        5V compliant
        includes a level shifter
        
    TX
        3V3 output
    
    GPIO pins
        3V3, not 5V compatible!
        max current drawn 12mA
        
        GPIO #0
            boot pin - switch, no pullup
                if held low at powerup, starts bootloading
            red LED, can also be used as output
            
        GPIO #2
            boot pin - no switch, pullup
            blue LED, can also be used as output

        GPIO #15
            boot pin - no switch, pulldown
            can also be used as output
         
        GPIO #16
            wakeup from deep sleep
        
    A 
        analog input
        0 - 1V range
        10 bits, 1024 ticks

    LDO
        pin enable for LDO regulator
        10K pullup to V+ or Vbat(which is greater)
        pulldown to disable power
        5V compliant
        
    /RST
        pulled high
        5V compliant
        
    EN(CH_PD)
        enable pin for ESP8266
        pulled high
        3V3 only

## Boot info
    
    default startup baudrate: 74880 then changes to 115200
    
    Boot modes:
        Pins
        [15-0-2]    Decimal     Meaning
        000         0           Remapping … details unknown.
        001*        1           Boot from the data received from UART0. Also
                                includes flashing the flash memory for subsequent
                                normal starts.
        010         2           Jump start
        011*        3           Boot from flash
        100         4           SDIO low speed V2
        101         5           SDIO high speed V1
        110         6           SDIO low speed V1
        111         7           SDIO high speed V2

        GPIO#15 always LOW 
        GPIO#2  always HIGH
        -----
        GPIO#0 LOW  => boot from UART0, flash memory
        GPIO#0 HIGH => boot from FLASH
    
    Reset codes:
        0 -> normal startup by power on
        1 -> hardware watch dog reset
        2 -> software watch dog reset (From an exception)
        3 -> software watch dog reset system_restart (Possibly unfed wd got angry)
        4 -> soft restart (Possibly with a restart command)
        5 -> wake up from deep-sleep

    Memory map
    
        The reset vector is 40000080h, which maps to internal ROM.
        
        4096KB Flash Option 1
        
        Boot                0x00 0000 – 0x00 0FFF       4KB
        App 1 (user1.bin)   0x00 1000 – 0x07 BFFF       492KB
        User params         0x07 C000 – 0x07 FFFF       16KB
        Do not use          0x08 0000 – 0x08 0FFF       4KB
        App 2 (user2.bin)   0x08 1000 – 0x0F BFFF       492KB
        User Data           0x0F C000 – 0x3F BFFF       3072KB
        System params       0x3F C000 – 0x3F FFFF       16KB

## Arduino IDE setup and use

    https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout/using-arduino-ide
    
    select Adafruit HUZZAH ESP8266 from the Tools->Board dropdown
    
    80 MHz as the CPU frequency

    115200 baud upload speed

    ### Blink test
    
        ```javascript
        void setup() {
          pinMode(0, OUTPUT);
        }
         
        void loop() {
          digitalWrite(0, HIGH);
          delay(500);
          digitalWrite(0, LOW);
          delay(500);
        }
        ```

    put the board into bootload mode:
        Hold down the GPIO0 button, the red LED will be lit
        While holding down GPIO0, click the RESET button
        Release RESET, then release GPIO0
        When you release the RESET button, the red LED will be lit dimly, this means its ready to bootload
    
    upload the sketch via the IDE
        
    red led should blink(GPIO0)
    
    ### Connect to wifi test
        
        ```javascript
        /*
         *  This sketch demonstrates how to scan WiFi networks. 
         *  The API is almost the same as with the WiFi Shield library, 
         *  the most obvious difference being the different file you need to include:
         */
        #include "ESP8266WiFi.h"

        void setup() {
          Serial.begin(115200);

          // Set WiFi to station mode and disconnect from an AP if it was previously connected
          WiFi.mode(WIFI_STA);
          WiFi.disconnect();
          delay(100);

          Serial.println("Setup done");
        }

        void loop() {
          Serial.println("scan start");

          // WiFi.scanNetworks will return the number of networks found
          int n = WiFi.scanNetworks();
          Serial.println("scan done");
          if (n == 0)
            Serial.println("no networks found");
          else
          {
            Serial.print(n);
            Serial.println(" networks found");
            for (int i = 0; i < n; ++i)
            {
              // Print SSID and RSSI for each network found
              Serial.print(i + 1);
              Serial.print(": ");
              Serial.print(WiFi.SSID(i));
              Serial.print(" (");
              Serial.print(WiFi.RSSI(i));
              Serial.print(")");
              Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
              delay(10);
            }
          }
          Serial.println("");

          // Wait a bit before scanning again
          delay(5000);
        }
        ```

# MQTT protocol info

    ## Message structure

    CLIENT
        Publisher
        Subscriber
    BROKER
    
                CONNECT
    CLIENT -----------------------> BROKER
    CLIENT <----------------------- BROKER
                CONNACK


        CONNECT message - sent to initiate connection
            clientId                "client_1"          
            cleanSession            true                
            username(opt)           "marius"           
            password(opt)           "letmein"          
            lastWillTopic(opt)      "/marius/will"      
            lastWillQos(opt)        2                  
            lastWillMessage(opt)    "bye bye..."        
            keepAlive               60                  

            clientId        - unique per broker
            cleanSession    - "false" = persistent session = broker stores all subscriptions and missed messages when QoS 1,2.
            user/pass       - sent in plaintext
            willMessage     - notify other clients when disconnects ungracefully
        
        CONNACK message
            sessionPresent          true
            returnCode              0

            sessionPresent  - always false if  cleanSession=true. if true, no need to subscribe to topics again
            returnCode      - 0 : Connection Accepted
                              1 : Connection Refused, unacceptable protocol version
                              2 : Connection Refused, identifier rejected
                              3 : Connection Refused, Server unavailable
                              4 : Connection Refused, bad user name or password
                              5 : Connection Refused, not authorized
                              
                PUBLISH                             PUBLISH
    CLIENT -----------------------> BROKER -----------------------> CLIENT

        PUBLISH message
            packetId                4314       
            topicName               "myhome/livingroom/temperature"
            qos                     1
            retainFlag              false
            payload                 "temperature:32.5"
            dupFlag                 false
            
            
            packetId    - per msg, always 0 for QoS = 0
            qos         - 
            retainFlag  - if msg is saved by broker as last known good value for that topic
            dupFlag     - true if msg is resent, because no ACK was received previously


                SUBSCRIBE (1)                       PUBLISH (3)
    CLIENT -----------------------> BROKER <----------------------- CLIENT
                SUBACK (2)
    CLIENT <----------------------- BROKER
                PUBLISH (4)
    CLIENT <----------------------- BROKER

        SUBSCRIBE message
            packetId                4313
            qos1                    1
            topic1                  "myhome/livingroom/temperature"
            qos2                    0
            topic2                  "myhome/livingroom/temperature"
            ...                     ...
            
            packetId    - per msg, always 0 for QoS = 0
            qos1/topic1 - list of subscriptions, can also contain wildcards
    
        SUBACK message
            packetId                4313
            returnCode1             2
            returnCode2             0
            ...                     ...

            packetId    - same as in SUBSCRIBE message
            returnCode  - one returnCode for each SUBSCRIVE topic, in the same order
                        - 0   : Success - Maximum QoS 0 
                          1   : Success - Maximum QoS 1
                          2   : Success - Maximum QoS 2
                          128 : Failure

                UNSUBSCRIBE
    CLIENT -----------------------> BROKER
    CLIENT <----------------------- BROKER
                UNSUBACK
                
        UNSUBSCRIBE message
            packetId                4313
            topic1                  "myhome/livingroom/temperature"
            topic2                  "myhome/livingroom/temperature"
            ...                     ...
    
        UNSUBACK message
            packetId                4313
    
            packetId    - same as in UNSUBSCRIBE message


    ## Topics
    
    - "myhome/goundfloor/livingroom/temperature"
    - "myhome"  = topic level
    - "/"       = topic level separator
    - can contain spaces, not recommended
    - is case sensitive, use lower case only
    - "/" valid topic, root like, don`t use it at start
    - "$" special start character: $SYS/broker/uptime
        - client cannot publish to these topics

    Wildcards:
        - Single level:
            - "+":  matches any topic from that level
                    "myhome/goundfloor/+/temperature"
                    
                        "myhome/goundfloor/livingroom/temperature"      OK
                        "myhome/goundfloor/kitchen/temperature"         OK
                        "myhome/goundfloor/kitchen/brightness"          NOK
                        "myhome/firstfloor/kitchen/temperature"         NOK
                        "myhome/goundfloor/kitchen/fridge/temperature"  NOK
        - Multi level:
            - "#":  mathecs any topics from that level below
                    has to be the last character of the topic
                    "myhome/goundfloor/#"

                        "myhome/goundfloor/livingroom/temperature"      OK
                        "myhome/goundfloor/kitchen/temperature"         OK
                        "myhome/goundfloor/kitchen/brightness"          OK
                        "myhome/firstfloor/kitchen/temperature"         NOK

    ## QoS
    
        QoS for Publish:    Client --> Broker
            QoS level set by the client for a that sent msg
        Qos for Subscribe   Broker --> Client
            QoS level set by the other client, which has subscribed to broker
            
        - the QoS level can be degraded, if a client subscribes with a lower QoS
          than the one used by the client which published the msg.
    
        0 = at most once
            msg not ACK by receiver(broker)
            msg not stored and redelivered by sender(client)
            "fire and forget"
            same guarantee as the TCP underneath
            
                        PUBLISH QoS 0
            CLIENT -----------------------> BROKER
            
        1 = at least once
            guaranteed the msg is delivered at least once to receiver(broker)
            the message can also be delivered more than once
            the sender stores the message until receives PUBACK from receiver
            
                        PUBLISH QoS 1
            CLIENT -----------------------> BROKER
                        PUBACK
            CLIENT <----------------------- BROKER
            
        2 = exactly once
            guarantees that each message is received only once by the counterpart
            safest and slowest
            PUBREC  = ack message from broker -> the client can discard the copy of the msg
            PUBREL  = ack from client to broker -> the broker can discard any saved state
            PUBCOMP = the flow is complete

                        PUBLISH QoS 2
            CLIENT -----------------------> BROKER
                        PUBREC
            CLIENT <----------------------- BROKER
                        PUBREL
            CLIENT -----------------------> BROKER
                        PUBCOMP
            CLIENT <----------------------- BROKER

        Use QoS 0 when:
            - stable connection, wired network
            - don`t care if some msg are lost sometimes
            - don`t need msg queing, for disconnected clients

        Use QoS 1 when:
            - need the get every msg
            - can handle duplicates
            - you can`t bear the overhead of QoS 2
        
        Use QoS 2 when:
            - you need to receive a msg exactly once

        **Note**:   All msg sent with QoS 1 and 2 will be qued for offline clients,
                    if they subscribed with a "persisten session".

    ### Persistent/Clean session
        
        Use a Persistent session when:
            - a client must get all msg, even if it is offline
            - a client has limited resources and the broker should hold the subscriptions
        
        Use a Clean session when:
            - a client is only publishing to broker, not subscribing
            - a client does not need the msg from when it was offline

# MQTT security

    TODO...

# MQTT server - Mosquitto

	Install server and clients(pub sub):
		sudo apt-get install mosquitto
		sudo apt-get install mosquitto-clients
		--- optional
		sudo apt-get install python-pip python-dev build-essential  		or sudo apt-get install python3-pip python3-dev build-essential
		pip install paho-mqtt 		or pip3 install paho-mqtt
	Check mosquitto server is on, and using port 1883:
		sudo /etc/init.d/mosquitto status
		sudo lsof -i TCP:1883
		netstat –an (on Windows)
	Subscribe to topic 'test':
		mosquitto_sub -t test -d
	Publish to 'test' content of file
		mosquitto_pub -t test -f mqtt_pub.txt -d

	Test sequence	
	
		Stop the deamon:
			sudo /etc/init.d/mosquitto stop
		Start the deamon:
			sudo /etc/init.d/mosquitto start
		In terminal window 1:
			mosquitto_sub -d -t hello/world
		In terminal window 2:
			mosquitto_pub -d -t hello/world -m "Hello from Terminal window 2!"

	Debug tools:
		'mosquitto_tools' - command line utilities
		'mqtt.fx' - gui stuff
		'mymqtt' - android
		'wireshark' 
			filter: ip.src==192.168.1.108

	Start the Mosquitto server with a configuration file:
		sudo /etc/init.d/mosquitto stop									stop m. deamon
		sudo lsof -i TCP:1883											check deamon is stopped, by inspecting the default mqtt port. no output is expected
		sudo /etc/init.d/mosquitto start -c ~/mosquitto.conf -v			start m. deamon with config file and verbose mode		
		sudo lsof -i TCP:1883





































