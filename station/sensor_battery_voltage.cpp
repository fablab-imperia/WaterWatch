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
  file: waterwatcher/sensor_battery_voltage.cpp
  modified: 2015/06/02 - 15:48:36
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

#include "assume.h"
#include "dbg.h"
#include "math.h"
#include "clock.h"
#include "sensor_battery_voltage.h"

static MinMaxAvg _val;

static Clock _ck;

// define approximated discharge characteristic curve as v, c[v]
// where v is voltage in mV
//       c is the % remaining charge
// we also define a max time constant that is the approximate max duration in h
// we suppose an average current of ~40mA
static const uint16 _dc_samples = 6;
static const uint16 _dc_voltage[ ] = {10500, 11000, 11500, 12000, 12500, 13000};
static const uint16 _dc_charge[ ]  = {    0,    10,    25,    50,    75,   100};
static const uint16 _dc_max_time   = 280;

// returns unfixed vcc value in mV
long readVcc( );

/*
========================================
id
========================================
*/

uint32 SensorBatteryVoltage::id( )
{
    return CFG_SENSOR_BATTERY_VOLTAGE_ID;
}

/*
========================================
type
========================================
*/

uint32 SensorBatteryVoltage::type( )
{
    return CFG_SENSOR_BATTERY_VOLTAGE_TYPE;
}

/*
========================================
chargeLeft
========================================
*/

int SensorBatteryVoltage::chargeLeft( )
{
    // retrieve current battery average voltage in mV
    int v = static_cast< uint16 >( 1000 * vAvg( ) );
    int n = _dc_samples - 1; // max index

    // resolve extreme values first
    if( v <= _dc_voltage[ 0 ] )
    {
        return _dc_charge[ 0 ]; // 0
    }
    else if( v >= _dc_voltage[ n ] )
    {
        return _dc_charge[ n ]; // 100
    }

    // interpolate cycle through [ samples - 2, 1 ]
    for( int i = n - 1; i >= 0; --i )
    {
        if( v == _dc_voltage[ i ] )
        {
            return _dc_charge[ i ]; // fast path
        }
        else if( v > _dc_voltage[ i ] )
        {
            // get low and high end points
            int vlo = _dc_voltage[ i ];
            int vhi = _dc_voltage[ i + 1 ];
            int clo = _dc_charge[ i ];
            int chi = _dc_charge[ i + 1 ];

            // interpolation coeff in [ 0, 1 ]
            float r = float( v - vlo ) / float( vhi - vlo );

            // interpolate charge
            return int( clo + r * ( chi - clo ) );
        }
    }

    assume( false ); // should never happen oO
    return 0;
}

/*
========================================
timeLeft
========================================
*/

int SensorBatteryVoltage::timeLeft( )
{
    return timeLeft( chargeLeft( ) );
}

int SensorBatteryVoltage::timeLeft
(
    int charge_left_percentage
)
{
    return charge_left_percentage * _dc_max_time / 100;
}

/*
========================================
vMin
========================================
*/

float SensorBatteryVoltage::vMin( )
{
    update( );
    return _val.getMin( );
}

/*
========================================
vMax
========================================
*/

float SensorBatteryVoltage::vMax( )
{
    update( );
    return _val.getMax( );
}

/*
========================================
vAvg
========================================
*/

float SensorBatteryVoltage::vAvg( )
{
    update( );
    return _val.getAvg( );
}

/*
========================================
reset
========================================
*/

void SensorBatteryVoltage::reset( )
{
    _val.reset( );
}

/*
========================================
update
========================================
*/

void SensorBatteryVoltage::update( )
{
    // make sure we have at least an initial value
    if( _val.getSamples( ) > 0
 && _ck.seconds( ) < CFG_SENSOR_BATTERY_VOLTAGE_SAMPLERATE )
    {
        return;
    }

    _ck.reset( );

    // read unfixed vcc value
    long vcc = readVcc( );

    // read battery A/D output
    long n = analogRead( CFG_SENSOR_BATTERY_VOLTAGE_ANALOG_PIN );

    // compute de-normalizer coefficient
    static const long kv = 1023 * CFG_SENSOR_BATTERY_VOLTAGE_DIVIDER;

    // compute and fix battery voltage
    float battery_voltage = CFG_VCC_FIX_FACTOR * n * vcc / kv / 1000.0f;

    // add to the serie
    _val.add( battery_voltage );

    // debug amenities
    DBG( "battery voltage [V]: min:" );
    DBG( vMin( ) );
    DBG( ",  max:" );
    DBG( vMax( ) );
    DBG( ",  avg:" );
    DBG( vAvg( ) );
    DBG( ",  charge left [%]:" );
    DBG( chargeLeft( ) );
    DBG( ",  time left [h]:" );
    DBGLN( timeLeft( ) );
}

/*
========================================
returns vcc in mV
see
https://code.google.com/p/tinkerit/wiki/SecretVoltmeter
http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage
========================================
*/

long readVcc( )
{
    // Read 1.1V reference against AVcc
    // set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
#else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif

    delay(2); // Wait for Vref to settle
    ADCSRA |= _BV(ADSC); // Start conversion
    while (bit_is_set(ADCSRA,ADSC)); // measuring

    uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
    uint8_t high = ADCH; // unlocks both

    long scale_constant = 1.1f * 1023 * 1000;
    long result = (high<<8) | low;

    result = scale_constant / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
    return result; // Vcc in millivolts
}
