# espduino_mqtt

    ESP8266 MQTT projects using Arduino IDE and Adafruit Huzzah board
    Board: Adafruit Huzzah ESP8266
    IC: ESP8266 - E12

# GPIO pins

    GPIO #0
        boot pin - switch, no pullup
            if held low, starts bootloading
        red LED
        
    GPIO #2
        boot pin - no switch, pullup
        blue LED

    GPIO #15
        boot pin - no switch, pulldown
     
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
        
    /RST
        pulled high
        5V compliant
        
    EN(CH_PD)
        enable pin for ESP8266
        pulled high
        3V3 only

# Boot info
    
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

# Arduino IDE setup and use

    https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout/using-arduino-ide
    
    select Adafruit HUZZAH ESP8266 from the Tools->Board dropdown
    
    80 MHz as the CPU frequency

    115200 baud upload speed

    Blink test:
        void setup() {
          pinMode(0, OUTPUT);
        }
         
        void loop() {
          digitalWrite(0, HIGH);
          delay(500);
          digitalWrite(0, LOW);
          delay(500);
        }

    put the board into bootload mode:
        Hold down the GPIO0 button, the red LED will be lit
        While holding down GPIO0, click the RESET button
        Release RESET, then release GPIO0
        When you release the RESET button, the red LED will be lit dimly, this means its ready to bootload
    
    upload the sketch via the IDE
        
    red led should blink(GPIO0)

# MQTT















