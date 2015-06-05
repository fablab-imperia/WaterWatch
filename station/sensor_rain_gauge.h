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
  file: waterwatcher/sensor_rain_gauge.h
  modified: 2015/06/02 - 14:49:50
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

#ifndef _9057C428B356D30E45654B89542B74CF_AC_SENSOR_RAIN_GAUGE__INCLUDED 
#define _9057C428B356D30E45654B89542B74CF_AC_SENSOR_RAIN_GAUGE__INCLUDED

#include <Arduino.h>

#include "cfg.h"
#include "type.h"

/*
========================================
monostate representing our rain gauge sensor
========================================
*/

struct SensorRainGauge
{
    // returns sensor id 
    static uint32 id( );
    
    // returns sensor type
    static uint32 type( );
    
    // returns rain in mm
    static float read( );
    
    // returns rain in mm and reset the counter
    static float readReset( );
    
    // restore rain (add amount)
    static float restore( float amount );
    
};

#endif // end of #ifndef _9057C428B356D30E45654B89542B74CF_AC_SENSOR_RAIN_G...