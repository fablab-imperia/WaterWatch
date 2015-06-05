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
  file: station/sensor_rain_gauge.cpp
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

#include "intr.h"
#include "assume.h"
#include "dbg.h"
#include "sensor_rain_gauge.h"
#include "clock.h"

// make rain volatile because it is modified in the ISR
static volatile float _rain = 0;

void attachISR( );
void raingaugeISR( );

/*
========================================
initializer
========================================
*/

struct SensorRainGaugeInit
{
    SensorRainGaugeInit( )
    {
        attachISR( );
    }
};

static SensorRainGaugeInit _init;

/*
========================================
id
========================================
*/

uint32 SensorRainGauge::id( )
{
    return CFG_SENSOR_RAIN_GAUGE_ID;
}

/*
========================================
type
========================================
*/

uint32 SensorRainGauge::type( )
{
    return CFG_SENSOR_RAIN_GAUGE_TYPE;
}

/*
========================================
read
========================================
*/

float SensorRainGauge::read( )
{
    float res;
    BEGIN_CRITICAL_SECTION
    {
        res = _rain;
    }
    END_CRITICAL_SECTION
    return res;
}

/*
========================================
readReset
========================================
*/

float SensorRainGauge::readReset( )
{
    float res;
    BEGIN_CRITICAL_SECTION
    {
        res = _rain;
        _rain = 0;
    }
    END_CRITICAL_SECTION
    return res;
}

/*
========================================
restore
========================================
*/

float SensorRainGauge::restore( float amount )
{
    float res;
    BEGIN_CRITICAL_SECTION
    {
        res = _rain;
        _rain += amount;
    }
    END_CRITICAL_SECTION
    return res;
}

/*
========================================
attachISR
========================================
*/

void attachISR( )
{
    // on Arduino UNO pin2 -> int0, pin3 -> int1
    uint8 pin = CFG_SENSOR_RAIN_GAUGE_PIN;
    assume( ( 2 == pin ) || ( 3 == pin ) );
    pinMode( pin, INPUT_PULLUP ); // use internal pull-up
    uint8 intrn = 2 == pin ? 0 : 1;
    attachInterrupt( intrn, raingaugeISR, RISING );
}

/*
========================================
raingaugeISR
========================================
*/

void raingaugeISR( )
{
    static Clock ck;
    if( 0 == _rain || ck.milliseconds( ) > 100 )
    {
        _rain += CFG_SENSOR_RAIN_GAUGE_NMM;
        ck.reset( );
    }
}
