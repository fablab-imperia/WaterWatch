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
  file: station/log.cpp
  modified: 2015/06/02 - 15:25:45
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

#include <string.h>

#include "dbg.h"
#include "assume.h"
#include "sim900.h"
#include "clock.h"
#include "log.h"
#include "device.h"

static Clock _ck;

static uint32 _failed_logs = 0;

/*
========================================
logData
send values to the server via GPRS
========================================
*/

bool logData( )
{
    bool res = false;

    if( _ck.seconds( ) < CFG_LOG_TIME_SECONDS )
    {
        return true;
    }
    _ck.reset( );

    DBGLN( "log data" );

    // save value, restore if log fails
#if CFG_SENSOR_RAIN_GAUGE_ENABLED == true
    float rain = SensorRainGauge::readReset( );
#endif

    // create query string
#define QS_MAX_LEN 128
    char qs[ QS_MAX_LEN ];
    sizet n = 0;

    n += sprintf( qs + n, "%s", "data=" );
    n += sprintf( qs + n, "_%d", CFG_DEVICE_ID );
    n += sprintf( qs + n, "_%d", int(_failed_logs) );

#if CFG_SENSOR_RAIN_GAUGE_ENABLED == true
    n += sprintf( qs + n, "_s%d", int( SensorRainGauge::id( ) ) );
    n += sprintf( qs + n, "_%d", int ( SensorRainGauge::type( ) ));
    n += sprintf( qs + n, "_%d", int( 100 * rain ) );
    DBG( "rain [mm]: " );
    DBGLN( rain );
#endif

#if CFG_SENSOR_SONAR_ENABLED == true
    n += sprintf( qs + n, "_s%d", int( SensorSonar::id( ) ) );
    n += sprintf( qs + n, "_%d", int ( SensorSonar::type( ) ) );
    n += sprintf( qs + n, "_%d", int( SensorSonar::levelMin( ) ) );
    n += sprintf( qs + n, "_%d", int( SensorSonar::levelMax( ) ) );
    n += sprintf( qs + n, "_%d", int( SensorSonar::levelAvg( ) ) );
    DBG( "sonar [cm]: min:" );
    DBG( int32( SensorSonar::levelMin( ) ) );
    DBG( ",  max:" );
    DBG( int32( SensorSonar::levelMax( ) ) );
    DBG( ",  avg:" );
    DBGLN( int32( SensorSonar::levelAvg( ) ) );
#endif

#if CFG_SENSOR_TEMPERATURE_ENABLED == true
    n += sprintf( qs + n, "_s%d", int( SensorTemperature::id( ) ) );
    n += sprintf( qs + n, "_%d", int( SensorTemperature::type( ) ) );
    n += sprintf( qs + n, "_%d", int( 100 * SensorTemperature::tMin( ) ) );
    n += sprintf( qs + n, "_%d", int( 100 * SensorTemperature::tMax( ) ) );
    n += sprintf( qs + n, "_%d", int( 100 * SensorTemperature::tAvg( ) ) );
    DBG( "temperature [C]: min:" );
    DBG( SensorTemperature::tMin( ) );
    DBG( ",  max:" );
    DBG( SensorTemperature::tMax( ) );
    DBG( ",  avg:" );
    DBGLN( SensorTemperature::tAvg( ) );
#endif

#if CFG_SENSOR_BATTERY_VOLTAGE_ENABLED == true
    n += sprintf( qs + n, "_s%d", int( SensorBatteryVoltage::id( ) ) );
    n += sprintf( qs + n, "_%d", int( SensorBatteryVoltage::type( ) ) );
    n += sprintf( qs + n, "_%d", int( 100 * SensorBatteryVoltage::vMin( ) ) );
    n += sprintf( qs + n, "_%d", int( 100 * SensorBatteryVoltage::vMax( )  ) );
    n += sprintf( qs + n, "_%d", int( 100 * SensorBatteryVoltage::vAvg( )  ) );
    n += sprintf( qs + n, "_%d", int( SensorBatteryVoltage::chargeLeft( ) ) );
    n += sprintf( qs + n, "_%d", int( SensorBatteryVoltage::timeLeft( ) ) );
    DBG( "battery voltage [V]: min:" );
    DBG( SensorBatteryVoltage::vMin( ) );
    DBG( ",  max:" );
    DBG( SensorBatteryVoltage::vMax( ) );
    DBG( ",  avg:" );
    DBG( SensorBatteryVoltage::vAvg( ) );
    DBG( ",  charge left [%]:" );
    DBG( SensorBatteryVoltage::chargeLeft( ) );
    DBG( ",  time left [h]:" );
    DBGLN( SensorBatteryVoltage::timeLeft( ) );
#endif

    assume( n < QS_MAX_LEN );

    // todo: move up and append csq to sent data (create a new sensor?)
    SIM900::pwrUp( );

    #if CFG_DEBUG == true
	  int csq = SIM900::csq( );
    #endif
    DBG( "signal quality [dBm]: " );
    DBGLN( csq );

    for( int i = 0; i < CFG_LOG_ATTEMPTS; ++i )
    {
        SIM900::tcpInit( CFG_INTERNET_APN );
        SIM900::httpInit( );
        SIM900::httpGet( CFG_SERVER_NAME,
                         CFG_SERVER_PORT,
                         CFG_SERVER_PATH,
                         qs );
        SIM900::httpTerm( );
        SIM900::tcpTerm( );

        // check server response
        if( 0 == strncmp( SIM900::getData( ) + 29, "ok", 2 ) )
        {
            DBGLN( "log data OK" );
            res = true;
            break;
        }

        delay( 10000 ); // long wait before retry
    }

    SIM900::pwrDown( );

    if( ! res )
    {
#if CFG_SENSOR_RAIN_GAUGE_ENABLED == true
        SensorRainGauge::restore( rain );
        ++ _failed_logs;
#endif
    }
    else
    {

#if CFG_SENSOR_SONAR_ENABLED == true
        SensorSonar::reset( );
#endif
#if CFG_SENSOR_TEMPERATURE_ENABLED == true
        SensorTemperature::reset( );
#endif
#if CFG_SENSOR_BATTERY_VOLTAGE_ENABLED == true
        SensorBatteryVoltage::reset( );
#endif
        _failed_logs = 0;
    }

    return res;
}
