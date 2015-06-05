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
  file: waterwatcher/waterwatcher.ino
  modified: 2015/06/02 - 14:29:42
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

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "LowPower.h"

#include "cfg.h"
#include "dbg.h"
#include "log.h"
#include "clock.h"
#include "sim900.h"
#include "device.h"

#define EXIT  {Serial.flush( );abort( );}

void sleep( );

void setup( )
{
    Serial.begin( CFG_SERIAL_SPEED );
    DBGLN( "program started" ); 
}

void loop( )
{
    static Clock ck;
    
    sleep( );
	
	delay( 200 );
    
#if CFG_SENSOR_SONAR_ENABLED == true
    SensorSonar::update( );
#endif

#if CFG_SENSOR_TEMPERATURE_ENABLED == true
    SensorTemperature::update( );
#endif

#if CFG_SENSOR_BATTERY_VOLTAGE_ENABLED == true
    SensorBatteryVoltage::update( );
#endif    

    // log data (if needed)
    logData( );
    
    DBG( "time [s]: " );
    DBGLN( ck.seconds( ) );
        
}

/*
==========================
put AVR ATMega in sleep mode
==========================
*/

void sleep( )
{
    // flush serial before entering sleep mode
    #if CFG_DEBUG == true
	Serial.flush( );
	#endif 
	
    // add global offset to all clocks
    Clock::offset( 8000 );
    
    // idle mode
    LowPower.idle
    (
        SLEEP_8S, 
        ADC_OFF, 
        TIMER2_OFF, 
        TIMER1_OFF, 
        TIMER0_OFF, 
        SPI_OFF, 
        USART0_OFF, 
        TWI_OFF
    );
}