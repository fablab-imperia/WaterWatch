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
  file: station/sensor_sonar.cpp
  modified: 2015/06/02 - 15:10:48
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

#include "assume.h"
#include "dbg.h"
#include "math.h"
#include "clock.h"
#include "sensor_sonar.h"

// use temperature correction if correction is enabled and
// temperature sensor is available
#if CFG_SENSOR_SONAR_TEMPERATURE_CORRECTION == true
#if CFG_SENSOR_TEMPERATURE_ENABLED == true
#define _USE_TEMPERATURE
#include "sensor_temperature.h"
#endif
#endif

static MinMaxAvg _val;

static Clock _ck;

/*
========================================
initializer
========================================
*/

struct SensorSonarInit
{
    SensorSonarInit( )
    {
        // attach pin
        pinMode( CFG_SENSOR_COMMAND_PIN, OUTPUT );
        digitalWrite( CFG_SENSOR_COMMAND_PIN, LOW );

        // reset clock
        _ck.reset( );
    }
};

static SensorSonarInit _init;

/*
========================================
soundSpeed
estimate sound speed @ temperature t
========================================
*/

inline
float soundSpeed
(
    float t
)
{
    return 330.0f + 0.6f * t; // simple but effective empirical model
}

/*
========================================
id
========================================
*/

uint32 SensorSonar::id( )
{
    return CFG_SENSOR_SONAR_ID;
}

/*
========================================
type
========================================
*/

uint32 SensorSonar::type( )
{
    return CFG_SENSOR_SONAR_TYPE;
}

/*
========================================
levelMin
========================================
*/

float SensorSonar::levelMin( )
{
    update( );
    return _val.getMin( );
}

/*
========================================
levelMax
========================================
*/

float SensorSonar::levelMax( )
{
    update( );
    return _val.getMax( );
}

/*
========================================
levelAvg
========================================
*/

float SensorSonar::levelAvg( )
{
    update( );
    return _val.getAvg( );
}

/*
========================================
reset
========================================
*/

void SensorSonar::reset( )
{
    _val.reset( );
}

/*
========================================
update
TOTO: it could be used the "real" value for the Vcc
========================================
*/

void SensorSonar::update( )
{
    // make sure we have at least an initial value
    if( _val.getSamples( ) > 0
 && _ck.seconds( ) < CFG_SENSOR_SONAR_SAMPLERATE )
    {
        return;
    }

    _ck.reset( );

    // send range command
    digitalWrite( CFG_SENSOR_COMMAND_PIN, HIGH );
    delay( 250 );
    digitalWrite( CFG_SENSOR_COMMAND_PIN, LOW );
    delay( 400 ); // wait distance computation... 200 should work

    // read voltage value
    // integer in [0-1023]
    uint16 v = analogRead( CFG_SENSOR_ANALOG_PIN );
    // convert in cm: V * ( 5000mV/1023 ) / (cm/mv)
    // with XL_MaxSonic-WRMA1 v represent the distance in cm
    // ( 5.0f / ( 1023f * CFG_SENSOR_SONAR_NCM ) ) ~ 1
    // max range is limited to ~700cm
    float dist = float( v ) * ( 5000.0f / ( 1023.0f * CFG_SENSOR_SONAR_MVCM ) );

    // temperature correction (if enabled)

#ifdef _USE_TEMPERATURE
    float ct( soundSpeed( SensorTemperature::tAvg( ) ) );
    float c0( soundSpeed( CFG_SENSOR_SONAR_TEMPERATURE_REF ) );
    dist *= ct / c0;
#endif

    float level = CFG_SENSOR_SONAR_HEIGHT - dist;

    _val.add( level );

    DBG( "sonar distance [cm]: min:" );
    DBG( CFG_SENSOR_SONAR_HEIGHT - levelMin( ) );
    DBG( ",  max:" );
    DBG( CFG_SENSOR_SONAR_HEIGHT - levelMax( ) );
    DBG( ",  avg:" );
    DBGLN( CFG_SENSOR_SONAR_HEIGHT - levelAvg( ) );
    DBG( "hydrometric level [cm]: min:" );
    DBG( levelMin( ) );
    DBG( ",  max:" );
    DBG( levelMax( ) );
    DBG( ",  avg:" );
    DBGLN( levelAvg( ) );

}
