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
  file: station/sensor_temperature.cpp
  modified: 2015/06/02 - 14:53:23
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
#include "OneWire.h"
#include "DallasTemperature.h"

#include "assume.h"
#include "dbg.h"
#include "math.h"
#include "clock.h"
#include "sensor_temperature.h"

static Clock _ck;

static MinMaxAvg _val;

// create an instance of OneWire using the specified bus
OneWire _one_wire( CFG_SENSOR_TEMPERATURE_1WBUS_PIN );

// create Dallas Temperature
DallasTemperature _dsens( & _one_wire );

/*
========================================
initializer
========================================
*/

struct SensorTemperatureInit
{
    SensorTemperatureInit( )
    {
        // initialize bus
        _dsens.begin( );

        // reset clock
        _ck.reset( );
    }
};

static SensorTemperatureInit _init;

/*
========================================
id
========================================
*/

uint32 SensorTemperature::id( )
{
    return CFG_SENSOR_TEMPERATURE_ID;
}

/*
========================================
type
========================================
*/

uint32 SensorTemperature::type( )
{
    return CFG_SENSOR_TEMPERATURE_TYPE;
}

/*
========================================
tMin
========================================
*/

float SensorTemperature::tMin( )
{
    update( );
    return _val.getMin( );
}

/*
========================================
tMax
========================================
*/

float SensorTemperature::tMax( )
{
    update( );
    return _val.getMax( );
}

/*
========================================
tAvg
========================================
*/

float SensorTemperature::tAvg( )
{
    update( );
    return _val.getAvg( );
}

/*
========================================
reset
========================================
*/

void SensorTemperature::reset( )
{
    _val.reset( );
}

/*
========================================
update
========================================
*/

void SensorTemperature::update( )
{
    // make sure we have at least an initial value
    if( _val.getSamples( ) > 0
 && _ck.seconds( ) < CFG_SENSOR_TEMPERATURE_SAMPLERATE )
    {
        return;
    }

    _ck.reset( );

    float temperature;

    // sometimes the IC may be resetted
    // we use a spin lock and try to re-establish the "connection"
    for( int i = 0; i < 5; ++i )
    {
        _dsens.requestTemperatures( );              // request to all
        delay( 200 );
        temperature = _dsens.getTempCByIndex( 0 );  // get first
        if( temperature > 127 || temperature < -100 )
        {
            _dsens.begin( );
            delay( 1000 );
        }
        else
        {
            break;
        }
    }

    _val.add( temperature );

    DBG( "temperature [C]: min:" );
    DBG( tMin( ) );
    DBG( ",  max:" );
    DBG( tMax( ) );
    DBG( ",  avg:" );
    DBGLN( tAvg( ) );
}
