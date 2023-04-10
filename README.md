# MPU6050_WLED

Latest version 04/10/2023; "MPU6050_WLED_withFive_Switches.ino" no longer uses touch features; instead, it uses four switches for better user interface. 
Additionally, a potentiometer is used to adjust delay between sending of URL to WLED Controller.  Features are nearly the same.  Find the latest version 
in the folder: MPU6050_WLED_withFive_Switches. Note: must use the new variables.h file; as variable names have changed. First time use; edit variable values 
to match your network and your configuration.  Find neteork configuration in network.h.

Developed by William Lucid with an assist from OpenAI's ChatGPT.  Project is a work-in-progress.  Will need a usermod to be 
added and compiled for WLED Controller using a MPU6050, Accelerometer Gyroscope Module.  Sketch was developed to generate varialbles for Effects, Intensity, and 
Color palette variables of WLED project.  [WLED Project](https://kno.wled.ge/)  WLED runs on ESP8266 or ESP32.

[WLED "List of Effects and Palettes"](https://github.com/Aircoookie/WLED/wiki/List-of-effects-and-palettes) 

Plan is to use a wand/paddle with the MPU6050 attached; user could "wave" the wand/paddle effecting changes 
to strip led, WLED "effects," "intensity," and "color Palette."

Using MPU6050_WLED. Project:
1.  Requires WLED Controller, ESP32, MPU6050
2.  See "Operation Five Switches.pdf" on switch useage.

[RGB Strip Controller used with MPU6050_WLED project](https://www.athom.tech/blank-1/wled-esp32-music-addressable-led-strip-controller)     

[View Tom Parish's ORB Art Project using WLED Controller](https://www.craft.do/s/uEoH8zY7xPudWD)

[View ORB using MPU6050_WLED Sketch:](https://drive.google.com/file/d/1ep3-D0ZQi7GCA-WQZV0VvKzEiCXjwCIK/view?usp=share_link)

Developed by William Lucid with an assist from OpenAI's ChatGPT.  Only partially finished; 
has not been added directly to running WLED, project is a work-in-progress.  Will need a usermod to be 
added and compiled for WLED.  Sketch was developed to generate varialbles for effects, Intensity, and 
color palette variables of WLED project.  [WLED Project](https://kno.wled.ge/)  WLED runs on ESP8266 or 
ESP32.

[WLED "List of Effects and Palettes"](https://github.com/Aircoookie/WLED/wiki/List-of-effects-and-palettes) 

Plan is to use a wand/paddle with the MPU6050 attached; user could "wave" the wand/paddle effecting changes 
to strip led, WLED "effects," "intensity," and "color Palette."

Using MPU6050_WLED. Project:
1.  Requires WLED Controller, ESP32, MPU6050
2.  Touching both wires; generates URLs using data from MPU6050. Touching T4 (GPIO13) wire; release T5 (GPIO12) wire; URLs generated by random data.  
     Release both wires; ESP32 goes to deep sleep.  
     
[RGB Strip Controller used with MPU6050_WLED project](https://www.athom.tech/blank-1/wled-esp32-music-addressable-led-strip-controller)     

[View Tom Parish's ORB Art Project using WLED Controller](https://www.craft.do/s/uEoH8zY7xPudWD)

[View ORB using MPU6050_WLED Sketch:](https://drive.google.com/file/d/1ep3-D0ZQi7GCA-WQZV0VvKzEiCXjwCIK/view?usp=share_link)
