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
  file: station/sim900.h
  modified: 2015/06/02 - 15:13:28
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

#ifndef _2DF823C68AD351088F3808B57DE6DCB8_AC_SIM900_H_INCLUDED
#define _2DF823C68AD351088F3808B57DE6DCB8_AC_SIM900_H_INCLUDED

#include "type.h"

/*
========================================
monostate representing our GPRS modem
SIMCOM900 Arduino GPRS Shield from seedstudio
========================================
*/

struct SIM900
{
    // return power status
    static bool pwr( );

    // returns signal quality in dBm
    static int csq( );

    // power up modem
    static void pwrUp( );

    // power down modem
    static void pwrDown( );

    // returns buffer data (could be not null terminated)
    static char const * getData( );

    // send an SMS to someone
    // note that any SMS is limited to 140 characters
    static void sendSMS( char const * number,
                         char const * text );

    // init a TCP connection
    static bool tcpInit( char const * apn );

    // terminate a TCP connection
    static bool tcpTerm( );

    // init an HTTP connection (over TCP)
    static void httpInit( );

    // terminate a HTTP connection
    static void httpTerm( );

    // send an HTTP get query string
    // server: server url
    // port: should be 80
    // path: path relative to server url
    // params: quesry string
    static bool httpGet( char const * server,
                         int port,
                         char const * path,
                         char const * params );

    // print serial data for debug purpose
    static void printSerialData( );
};

#endif // end of #ifndef _2DF823C68AD351088F3808B57DE6DCB8_AC_SIM900_H_INCL...
