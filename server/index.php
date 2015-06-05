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
  file: server/index.php

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

define( 'KARMA', 'ON' ); // here we start

require "cfg.php";
include 'thingspeak.php';


$data_for_thingspeak = array();

// get data from query string
$data = isset( $_GET[ "data" ] ) ? $_GET[ "data" ] : null;

/*
test string
echo "<pre>";
$data = "_1_0_s1_100_0_s2_200_382_382_382_s3_300_-12700_-12700_-12700_s4_400_674_674_674_100_200";
*/

if( null == $data )
{
    die;
}

$ip = getIp( );
$time = time( );


/*
// test purge
purgeOldRecords( "rain", $time - 10 );
purgeOldRecords( "sonar", $time - 10 );
purgeOldRecords( "temperature", $time - 10 );
purgeOldRecords( "battery_voltage", $time - 10 );
*/


// explode values
$data = explode( "_", trim( $data, "_" ) );

// read device and failed logs (this could be logged)
$device_id = getParam( $data, 0 );
$failed_logs = getParam( $data, 1 );


if( null == $device_id || null == $failed_logs )
{
    die;
}

// parse data
for( $i = 2; $i < count( $data ); )
{
    if( $data[ $i ][ 0 ] == "s" )
    {
        // pattern: s[id], [type], [data0], [data1], ...
        $id = trim( $data[ $i ], "s" );
        // TODO: verify sensor id
        $type = getParam( $data, $i + 1 );

        if( null == $type )
        {
            die; // invalid: no type
        }
        $i = logSensor( $id, $type, $data, $i + 2 );
    }
}



if ($cfg['data-loggers']['thingspeak']) {
  $postdata = \waterwatch\ThingSpeakClient::postToThingSpeak($data_for_thingspeak);
}


// $res = purgeOldRecords( time( ) - 10 );

// log correctly received and saved
die ( "ok" );

/*
==========================================
getParam
returns value of element $i from array $data
null if not defined
==========================================
*/

function getParam( $data, $i )
{
    if( isset( $data[ $i ] ) )
    {
        return $data[ $i ];
    }
    else
    {
        return null;
    }
}

/*
==========================================
getParam3
returns value of element $i, $i+1 and $i+2 from array $data
null if one or more are not defined
==========================================
*/

function getParam3( $data, $i )
{
    $v1 = getParam( $data, $i + 0 );
    $v2 = getParam( $data, $i + 1 );
    $v3 = getParam( $data, $i + 2 );

    if( null == $v1 || null == $v2 || null == $v3 )
    {
        return null;
    }

    return array( $v1, $v2, $v3 );
}

/*
==========================================
getParam3
returns value of element $i, $i+1, ..., and $i+4 from array $data
null if one or more are not defined
==========================================
*/

function getParam5( $data, $i )
{
    $v1 = getParam( $data, $i + 0 );
    $v2 = getParam( $data, $i + 1 );
    $v3 = getParam( $data, $i + 2 );
    $v4 = getParam( $data, $i + 3 );
    $v5 = getParam( $data, $i + 4 );

    if( null == $v1 ||
        null == $v2 ||
        null == $v3 ||
        null == $v4 ||
        null == $v5 )
    {
        return null;
    }

    return array( $v1, $v2, $v3, $v4, $v5 );
}

/*
==========================================
logSensor
route log to the specific functions
params: $id: sensor id
        $type: type of sensor
        $data: array with data
        $i: start of sensor data
==========================================
*/

function logSensor( $id, $type, $data, $i )
{
    global $cfg;
    foreach( $cfg[ "sensors" ][ "type" ] as $k => $v )
    {

        if( $v == $type )
        {
            $cbk = "logSensor";

            $parts = explode( "_", $k );
            foreach( $parts as $p )
            {
                $cbk .= ucfirst( $p );
            }
            if( function_exists( $cbk ) )
            {
                return call_user_func_array( $cbk, array( $id, $data, $i ) );
            }
        }
    }
    die;
}

/*
==========================================
logSensorRain
pattern: 1 integer representing 100 * rain in mm
==========================================
*/

function logSensorRain( $id, $data, $i )
{
    global $data_for_thingspeak;
    $v1 = getParam( $data, $i );


    if( null == $v1 )
    {
        die;
    }

    $v1 = round( (float)$v1 / 100, 2 ); // scale to mm

    // echo "Logged rain: $id $v1\n";

    if( ! writeRaw( $id, "rain", array( $v1 ) ) )
    {
        die;
    }

    $data_for_thingspeak['rain'] = strval($v1);

    return $i + 1;
}

/*
==========================================
logSensorSonar
pattern: 1 integer representing level in cm
==========================================
*/

function logSensorSonar( $id, $data, $i )
{
    global $data_for_thingspeak;
    $v = getParam3( $data, $i );

    if( $v == null )
    {
        die;
    }

    list( $v1, $v2, $v3 ) = $v;
    $v1 = strval( $v1 );  // cm
    $v2 = strval( $v2 );  // cm
    $v3 = strval( $v3 );  // cm

    // echo "Logged sonar: $id $v1 $v2 $v3\n";

    if( ! writeRaw( $id, "sonar", array( $v1, $v2, $v3 ) ) )
    {
        die;
    }

    $data_for_thingspeak['sonarAvg'] = $v3;

    return $i + 3;
}

/*
==========================================
logSensorTemperature
pattern: 3 integers representing 100 * celsius degrees (min, max and avg)
==========================================
*/

function logSensorTemperature( $id, $data, $i )
{
    global $data_for_thingspeak;
    $v = getParam3( $data, $i );

    if( $v == null )
    {
        die;
    }

    list( $v1, $v2, $v3 ) = $v;
    $v1 = strval( round( (float)$v1 / 100, 2 ) ); // scale to degree celsius
    $v2 = strval( round( (float)$v2 / 100, 2 ) ); // scale to degree celsius
    $v3 = strval( round( (float)$v3 / 100, 2 ) ); // scale to degree celsius

    // echo "Logged temperature: $id $v1 $v2 $v3\n";

    if( ! writeRaw( $id, "temperature", array( $v1, $v2, $v3 ) ) )
    {
        die;
    }

    $data_for_thingspeak['tempAvg'] = $v3;

    return $i + 3;
}

/*
==========================================
logSensorBatteryVoltage
pattern: 3 integers representing 100 * V (min, max and avg)
==========================================
*/

function logSensorBatteryVoltage( $id, $data, $i )
{
    global $data_for_thingspeak;
    $v = getParam5( $data, $i );

    if( $v == null )
    {
        die;
    }

    list( $v1, $v2, $v3, $v4, $v5 ) = $v;
    $v1 = strval( round( (float)$v1 / 100, 2 ) ); // scale to V
    $v2 = strval( round( (float)$v2 / 100, 2 ) ); // scale to V
    $v3 = strval( round( (float)$v3 / 100, 2 ) ); // scale to V
    // $v4 is the percentage of charge left
    // $v5 is time left in hours

    // echo "Logged battery voltage: $id $v1 $v2 $v3 $v4 $v5\n";

    if( ! writeRaw( $id, "battery_voltage", array( $v1, $v2, $v3, $v4, $v5 ) ) )
    {
        die;
    }

    $data_for_thingspeak['voltageAvg'] = $v3;
    $data_for_thingspeak['chargePerc'] = $v4;
    $data_for_thingspeak['remainingHours'] = $v5;

    return $i + 5;
}

/*
==========================================
writeRaw
==========================================
*/

function writeRaw( $id, $type, $data )
{
    // generate file and lock names
    // we have to use a second file as lock because it seems there is
    // a bug that prevents a locked file to be truncated oO
    // we need to truncate the record file in purgeOldRecords function
    $basename = __DIR__ . "/data/$type";
    $flockfilename   = "$basename.lock";
    $filename = "$basename.txt";

    // format record and filename
    global $ip;
    global $time;
    $record = "$time;$id;$ip;" . implode( $data, ";" );

    // open file in "binary" append/create mode
    $fp = @fopen( $filename, 'ab'); // append
    $flock = @fopen( $flockfilename, 'w+');

    if( !$fp || !$flock )
    {
        return false;
    }

    //  acquire an exclusive access
    $ok = false;
    for( $i = 0; $i < 5; ++$i ) // spinlock
    {
        $ok = @flock( $flock, LOCK_EX );
        if( $ok )
        {
            // write the record and unlock
            @fwrite( $fp, "$record\n" ); // ? sometimes returns false
            @flock( $flock, LOCK_UN ); // release the lock
            break;
        }
        sleep( 1 );
    }

    @fclose( $fp );
    @fclose( $flock );
    return $ok;
}

/*
==========================================
getIP
==========================================
*/

function getIP( )
{
	$ip = "0";
	if( ! empty( $_SERVER[ 'HTTP_CLIENT_IP' ] ) )
	{
		$ip = $_SERVER[ 'HTTP_CLIENT_IP' ];
	}
	else if( ! empty( $_SERVER[ 'HTTP_X_FORWARDED_FOR' ] ) )
	{
		$ip = $_SERVER[ 'HTTP_X_FORWARDED_FOR' ];
	}
	else
	{
		$ip = $_SERVER[ 'REMOTE_ADDR' ];
	}
	return $ip;
}

/*
==========================================
purgeOldRecords
delete all records older (<=) than time $time
and backup in a file
returns path to backup file
==========================================
*/

function purgeOldRecords( $time )
{
    global $cfg;
    $res = array( );
    foreach( $cfg[ "sensors" ][ "type" ] as $k => $v )
    {
        $res[ ] = _purgeOldRecords( $k, $time );
    }
    return $res;
}

/*
==========================================
purgeOldRecords
delete all records of type $type older (<=) than time $time
return path to backup file, false on failure
==========================================
*/

function _purgeOldRecords( $type, $time )
{
    /*
        lock record file (file .lock)
        copy recent records to a tmp file
        truncate original record file
        copy back all records from the tmp file
    */

    // generate file names
    $basename = __DIR__ . "/data/$type";              // basename
    $src_filename  = "$basename.txt";                 // original
    $tmp_filename  = "$basename.tmp.txt";             // temp
    $backup_filename = "$basename.back.$time.txt";    // backup filename
    $flockfilename = "$basename.lock";                // lock file

    // open files
    $fps = @fopen( $src_filename, 'rb');              // phase 1: read only
    $fpt = @fopen( $tmp_filename, 'w+b');             // read write
    $fback = @fopen( $backup_filename, "ab" );        // write only
    $flock = @fopen( $flockfilename, 'w+');           // read write

    // exit if something gone wrong
    if( !$fps || !$fpt || !$flock || !$fback )
    {
        return false;
    }

    // acquire an exclusive access to record file
    // we use an external lock file because for some reason we cannot
    // truncate a locked file
    for( $i = 0; $i < 5; ++$i ) // spinlock
    {
        if( @flock( $flock, LOCK_EX ) )
        {
            while( ! feof( $fps ) )
            {
                $record = @fgets( $fps );

                // copy to backup
                @fwrite( $fback, $record );

                // copy to tmp only if the record is recent
                list( $rtime ) = sscanf( $record, "%d" );
                if( $rtime > $time )
                {
                    @fwrite( $fpt, $record );
                }
            }

            @fseek( $fpt, 0 );
            @fflush( $fps );

            // phase2: truncate record file and copy back recent records
            // this will not invalidate file pointer to record file
            // if opened by other processes

            // close, reopen and truncate
            @fclose( $fps );
            $fps = @fopen( $src_filename, 'wb'); // write mode + truncate

            // copy back from tmp
            while( ! feof( $fpt ) )
            {
                @fwrite( $fps, @fgets( $fpt ) );
            }

            // flush all
            @fflush( $fps );
            @fflush( $fpt);
            @fflush( $fback);

            // release the lock and exit
            @flock( $flock, LOCK_UN );
            break;
        }
        sleep( 1 );
    }

    // close all
    @fclose( $fps );
    @fclose( $fpt );
    @fclose( $fback );
    @fclose( $flock );

    // delete tmp_filename and exit
    unlink( $tmp_filename );

    return $backup_filename;
}
