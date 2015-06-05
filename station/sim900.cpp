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
  file: waterwatcher/sim900.cpp
  modified: 2015/06/02 - 15:01:22
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
#include <string.h>

#include "cfg.h"
#include "assume.h"
#include "sim900.h"

// temporary buffer used to create the message
#define TMP_BUF_SIZE 256

// software serial used for SIMCOM900 I/O and AT commands 
static SoftwareSerial _swser( CFG_SIM900_SWPIN1, CFG_SIM900_SWPIN2 );
static char _buf[ CFG_SIM900_BUFFER_CAPACITY + 1 ]; // + 1 null byte
static sizet _bufsz = 0;

/*
========================================
initializer
========================================
*/

struct SIM900Init
{
    SIM900Init( )
    {
        _swser.begin( CFG_SIM900_SWSERIAL_SPEED );
        _buf[ CFG_SIM900_BUFFER_CAPACITY ] = '\0';
    }
};

static SIM900Init _init;

// send a CTRL-Z
static void atCtrlZ( int tdelay );

// send AT command to the modem
// com: command
// tdelay: delay in ms after the command
// readsw: if true read the software serial content (available in getData)
static void at( char const * com,
                int tdelay,
                bool readsw = true );

// read back serial data OUTPUT 
static void readSerialData( );
                
                
/*
========================================
pwrPinWrite
========================================
*/

inline
void pwrPinWrite
(
    uint8 state,
    uint32 holdtime
)
{
    digitalWrite( CFG_SIM900_PWRPIN, state );
    delay( holdtime );
}

/*
========================================
pwr
========================================
*/

bool SIM900::pwr( )
{
    // verify status
    at( "AT", 200 );  // send something

    // if no output comes from the modem or if it is powered down...
    if( _bufsz < 2 || 0 == strncmp( _buf + 2, "NORMAL POWER DOWN", 17 ) )
    {
        return false;
    }

    return true;
}

/*
========================================
csq
========================================
*/

int SIM900::csq( )
{
	assume( pwr( ) );
	
	char const * p = NULL;
	
	for( int i = 0; i < 5; ++i )
	{
		// verify status
		at( "AT+CSQ", 500 );

		// check server response
		p = strstr( _buf, "+CSQ:" );
			
		if( NULL != p )
		{
			p += 6;
			int csq_value = 0;
			sscanf( p, "%d", &csq_value );
    		return csq_value;
		}
		delay( 1000 );
	}
    
	return 0;    
}

/*
========================================
pwrUp
========================================
*/

void SIM900::pwrUp( )
{
    if( ! pwr( ) )
    {
        // force an HW power ON
        pinMode( CFG_SIM900_PWRPIN, OUTPUT );
        pwrPinWrite( LOW,  1000 );
        pwrPinWrite( HIGH, 2000 );
        pwrPinWrite( LOW,  3000 );
        delay( 30000 ); // looong wait
    }
}

/*
========================================
pwrDown
========================================
*/

void SIM900::pwrDown( )
{
    if( pwr( ) )
    {
        at( "AT+CPOWD=1", 5000 );
    }

}

/*
========================================
getData
========================================
*/

char const * SIM900::getData( )
{
    return _buf;
}

/*
========================================
readSerialData
========================================
*/

void readSerialData( )
{
    _bufsz = 0;
    sizet i = 0, lenmax = CFG_SIM900_BUFFER_CAPACITY;
    while( _swser.available( ) )
    {
        while( _swser.available( ) )
        {
            char ch = _swser.read( );
            if( ch != - 1 && i < lenmax )
            {
                _buf[ i++ ] = ch;
                ++_bufsz;
            }
        }
        delay( 100 );
    }
    // _bufsz could be CFG_SIM900_BUFFER_CAPACITY which is ok
    // capped @ CFG_SIM900_BUFFER_CAPACITY + 1
    _buf[ _bufsz ] = '\0';
}

/*
========================================
printSerialData (debug)
========================================
*/

void SIM900::printSerialData( )
{
    if( 0 != _bufsz )
    {
        Serial.print( "serdata: " );
        Serial.println( _buf );
    }
}

/*
========================================
at
========================================
*/

void at
(
    char const * com,
    int tdelay,
    bool readsw
)
{
    _swser.println( com );
    delay( tdelay );

    if( readsw )
    {
        readSerialData( );
#if CFG_DEBUG == true
        SIM900::printSerialData( );
#endif
    }

}

/*
========================================
atControlZ
========================================
*/

void atCtrlZ
(
    int tdelay
)
{
    _swser.println( char( 26 ) );
    delay( tdelay );
}

/*
========================================
sendSMS
========================================
*/

void SIM900::sendSMS
(
    char const * number,
    char const * text
)
{
    assume( pwr( ) );
    char tbuf[ TMP_BUF_SIZE ];
    at( "AT+CMGF=1", 200 ); // text mode
    // use buffer to store number and text
    assume( strlen( number ) +  32 < TMP_BUF_SIZE );
    tbuf[ 0 ] = '\0';
    strcat( tbuf, "AT + CMGS = \"" );
    strcat( tbuf, number );
    strcat( tbuf, "\"" );
    at( tbuf, 200 );
    at( text, 200 );
    atCtrlZ( 100 );
    at( "", 100 );
}

/*
========================================
tcpInit
========================================
*/

bool SIM900::tcpInit
(
    char const * apn
)
{
    assume( pwr( ) );

    char tbuf[ TMP_BUF_SIZE ];

    // check signal quality
    at( "AT+CSQ", 100 );
    // attach to GPRS service
    at( "AT+CGATT?", 100 );
    // attach GPRS
    at( "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 1000 );
    // set APN
    // use buffer to store apn string (be paranoic)
    assume( strlen( apn ) +  32 < TMP_BUF_SIZE );
    tbuf[ 0 ] = '\0';
    strcat( tbuf, "AT+SAPBR=3,1,\"APN\",\"" );
    strcat( tbuf, apn );
    strcat( tbuf, "\"" );
    at( tbuf, 4000 );
    at( "AT+SAPBR=1,1", 2000 );
    return true;
}

/*
========================================
tcpTerm
========================================
*/

bool SIM900::tcpTerm( )
{
    at( "CIPCLOSE", 100, false );
}

/*
========================================
httpInit
========================================
*/

void SIM900::httpInit( )
{
    // init http service (SIMCOMM prop)
    at( "AT+HTTPINIT", 2000 );
}

/*
========================================
httpTerm
========================================
*/

void SIM900::httpTerm( )
{
    // init http service (SIMCOMM prop)
    at( "AT+HTTPTERM", 2000 );
}

/*
========================================
httpGet
========================================
*/

bool SIM900::httpGet
(
    char const * server,
    int port,
    char const * path,
    char const * params
)
{
    assume( pwr( ) );

    char tbuf[ TMP_BUF_SIZE ];

    // set http params (SIMCOMM prop)
    // use buffer to store params (be paranoic)
    sizet len = strlen( server ) + strlen ( path ) + strlen ( params );
    assume( len + 32 < TMP_BUF_SIZE );
    tbuf[ 0 ] = '\0';
    strcat( tbuf, "AT+HTTPPARA=\"URL\",\"" );
    strcat( tbuf, server );
    strcat( tbuf, path );
    strcat( tbuf, params );
    strcat( tbuf, "\"" );
    at( tbuf, 1000 );
    // submit http request (0 - get, 1 - post, 2 - head)
    at( "AT+HTTPACTION=0", 10000 ); // only get seems to work
    // read data
    at( "AT+HTTPREAD", 3000 );
    at( "", 1000, false ); // dont loose received data by resetting the buffer!
    return true;
}
