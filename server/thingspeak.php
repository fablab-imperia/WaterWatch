<?php

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
  file: server/thingspeak.php
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

namespace waterwatch;

class ThingSpeakClient {

  //Add you magic api key: es. MAGICKEY
  const  THING_SPEAK_MAGIC_APIKEY = "MAGICKEY";
  //Thingspeak url
  const  THINGSPEAK_UPDATE_URL = "https://api.thingspeak.com/update";

  //Utility function: performs an HTTP post using CURL library
  private static function postDataWithCurl($url, $params) {

    $retVal = array("code" => 0, "msg" => "Ok", "http_code" => "N/A", "reponseText" => "N/A");
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL,$url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_TIMEOUT, 60);


    //Curl is a strange thing: here we choose to do a post and set fields
    curl_setopt($ch, CURLOPT_POST, 1);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $params);

    $data = curl_exec($ch);
    $curl_error_code = curl_errno($ch);
    $http_code = curl_getinfo ( $ch, CURLINFO_HTTP_CODE );

    $retVal['http_code'] = $http_code;

    if ($curl_error_code) {

        $retVal['code'] = $curl_error_code;
        $retVal['msg'] = "Error: " . curl_error($ch);

    } else {
        $retVal['reponseText'] = $data;

    }
    curl_close($ch);

    return $retVal;
  }

  //post fields to thingspeak platform
  public static function postToThingSpeak(array $data) {

      $params = array(
        'api_key' => self::THING_SPEAK_MAGIC_APIKEY,
        'timezone' => 'Europe/Rome'
      );


      if (array_key_exists('rain', $data)) {
        $params['field1'] = $data['rain'];
      }


      if (array_key_exists('sonarAvg', $data)) {
        $params['field3'] = $data['sonarAvg'];
      }


      if (array_key_exists('tempAvg', $data)) {
        $params['field5'] = $data['tempAvg'];
      }

      if (array_key_exists('voltageAvg', $data)) {
        $params['field6'] = $data['voltageAvg'];
      }

      if (array_key_exists('chargePerc', $data)) {
        $params['field2'] = $data['chargePerc'];
      }

      if (array_key_exists('remainingHours', $data)) {
        $params['field4'] = $data['remainingHours'];
      }

      if (array_key_exists('created_at', $data)) {
        $params['created_at'] = $data['created_at'];
      } else {
        $now = date("Y-m-d H:i:s");
        $params['created_at'] = $now;
      }


      $result = self::postDataWithCurl(self::THINGSPEAK_UPDATE_URL, $params);

      if ($result['reponseText'] == '0') {
        $result['reponseText'] = 'update failed';
      }
      return   $result;

  }

}
