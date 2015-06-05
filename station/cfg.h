/*//////////////////////////////////////////////////////////////////////////////
                    __      __         __
                   /  \    /  \_____ _/  |_  ___________
                   \   \/\/   /\__  \\   __\/ __ \_  __ \
                    \        /  / __ \|  | \  ___/|  | \/
                     \__/\  /  (____  /__|  \___  >__|
                          \/        \/          \/
             __      __         __         .__
            /  \    /  \_____ _/  |_  ____ |  |__   ___________
            \   \/\/   /\__  \\   __\/ ___\|  |  \_/ __ \_  __ \
             \        /  / __ \|  | \  \___|   Y  \  ___/|  | \/
              \__/\  /  (____  /__|  \___  >___|  /\___  >__|
                   \/        \/          \/     \/     \/
                      _______________  ____ .________
                      \_____  \   _  \/_   ||   ____/
                       /  ____/  /_\  \|   ||____  \
                      /       \  \_/   \   |/       \
                      \_______ \_____  /___/______  /
                              \/     \/           \/

  project: WaterWatcher
  author: Andrea Cazzadori (mrwolf.fablabimperia.org)
  license: WTFPL
  file: station/cfg.h
  modified: 2015/06/02 - 16:08:38
  header created: 2015/06/02 - 16:17:07
________________________________________________________________________________

              DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                    Version 2, December 2004

 Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>

 Everyone is permitted to copy and distribute verbatim or modified
 copies of this license document, and changing it is allowed as long
 as the name is changed.

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. You just DO WHAT THE FUCK YOU WANT TO.
________________________________________________________________________________

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; BUSINESS
INTERRUPTION; OR INJURIES, DEATHS OR DAMAGE TO PROPERTIES) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//////////////////////////////////////////////////////////////////////////////*/

#ifndef _D1F431A2A8D424FB77906DDD05B2F145_AC_CFG_H_INCLUDED
#define _D1F431A2A8D424FB77906DDD05B2F145_AC_CFG_H_INCLUDED

/*
========================================
GENERAL
========================================
*/

// should be true
#define CFG_ARDUINO_UNO   true

// if true print debug string to serial monitor
#define CFG_DEBUG         false

// serial speed
#define CFG_SERIAL_SPEED  9600

// types of sensors
// do not modify
#define CFG_SENSOR_RAIN_GAUGE_TYPE      100
#define CFG_SENSOR_SONAR_TYPE           200
#define CFG_SENSOR_TEMPERATURE_TYPE     300
#define CFG_SENSOR_BATTERY_VOLTAGE_TYPE 400

// device id
#define CFG_DEVICE_ID     1

// fix factor for AVR internal voltage reference
// if fix factor = 1.0 it will be used the nominal value of 1100mV
// internal voltage reference 8and the fix factor) varies from chip to chip
#define CFG_VCC_FIX_FACTOR              1.0076f

/*
========================================
SENSORS
========================================
*/

// ------------ RAIN GAUGE

// enable raing gauge sensor
#define CFG_SENSOR_RAIN_GAUGE_ENABLED  true

// rain gauge sensor id
#define CFG_SENSOR_RAIN_GAUGE_ID      1

// rain gauge pin, must be 2 or 3 on Arduino UNO
#define CFG_SENSOR_RAIN_GAUGE_PIN     2

// tips to mm (l/mq) conversion (mm/tip)
#define CFG_SENSOR_RAIN_GAUGE_NMM     0.36f

// ------------ SONAR

// enable sonar sensor
#define CFG_SENSOR_SONAR_ENABLED  true

// sonar sensor id
#define CFG_SENSOR_SONAR_ID           2

// digital pin to send range command
#define CFG_SENSOR_COMMAND_PIN        4

// analog pin to read
#define CFG_SENSOR_ANALOG_PIN         2

// mV/cm conversion
#define CFG_SENSOR_SONAR_MVCM        4.89f

// height in cm from the sonar to the bottom (hydrometric zero)
#define CFG_SENSOR_SONAR_HEIGHT     700.0f

// time between two consecutive samples in seconds
#define CFG_SENSOR_SONAR_SAMPLERATE 30

// use temperature correction
// CFG_SENSOR_TEMPERATURE_ENABLED must be true
#define CFG_SENSOR_SONAR_TEMPERATURE_CORRECTION true

// sonar reference temperature (used in correction)
#define CFG_SENSOR_SONAR_TEMPERATURE_REF   22.0f

// ------------ TEMPERATURE

// enable temperature sensor
#define CFG_SENSOR_TEMPERATURE_ENABLED  true

#define CFG_SENSOR_TEMPERATURE_ID  3

#define CFG_SENSOR_TEMPERATURE_1WBUS_PIN  5

#define CFG_SENSOR_TEMPERATURE_SAMPLERATE 20

// ------------ BATTERY

// see sensor_battery_voltage.cpp for discharge characteristic

// enable battery voltage divider
#define CFG_SENSOR_BATTERY_VOLTAGE_ENABLED true

// sensor id
#define CFG_SENSOR_BATTERY_VOLTAGE_ID     4

// analog pin
#define CFG_SENSOR_BATTERY_VOLTAGE_ANALOG_PIN   3

// battery voltage divider coefficient
#define CFG_SENSOR_BATTERY_VOLTAGE_DIVIDER  10.0f/( 10 + 22 )

// time between two consecutive samples in seconds
#define CFG_SENSOR_BATTERY_VOLTAGE_SAMPLERATE   60

/*
========================================
SIM900 GPRS
========================================
*/

// software serial pin 1
#define CFG_SIM900_SWPIN1 	       7

// software serial pin 2
#define CFG_SIM900_SWPIN2 	       8

// power pin
#define CFG_SIM900_PWRPIN          9

// buffer capacity
#define CFG_SIM900_BUFFER_CAPACITY 128

// software serial speed
#define CFG_SIM900_SWSERIAL_SPEED  19200

// apn name: Replace with your GPRS AP (eg ap.example.com)
//This should be changed depending on your SIM and carrier
#define CFG_INTERNET_APN  "ap.example.com"

/*
========================================
logs
========================================
*/

// server name
// example: "http://www.mysite.com"
#define CFG_SERVER_NAME  "http://www.mysite.com"

// server port (must be 80)
#define CFG_SERVER_PORT  80

// server path relative to server name
// example: "/log/index.php?"
#define CFG_SERVER_PATH  "/log/index.php?"

// time between logs in seconds
#define CFG_LOG_TIME_SECONDS      600

// number of attempts to send a log to the server
#define CFG_LOG_ATTEMPTS           3

#endif // end of #ifndef _D1F431A2A8D424FB77906DDD05B2F145_AC_CFG_H_INCLUDED
