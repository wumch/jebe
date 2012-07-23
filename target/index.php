<?php
namespace target;

error_reporting(E_ALL);
define('ROOT_PATH', __DIR__ . DIRECTORY_SEPARATOR);
define('RIAK_HOME', ROOT_PATH . 'Riak' . DIRECTORY_SEPARATOR);

function autoload_riak($symbol)
{
    require(ROOT_PATH . str_replace('\\', '/', $symbol) . '.php');
    return true;
}
spl_autoload_register('target\autoload_riak');

use Riak;

$server = array(
    'host' => '192.168.88.2',
    'port' => '8098',
    'host' => '211.154.172.172',
    'port' => '18098',
    'raw_name' => 'riak',
    'mapred_name' => 'mapred',
);

$bukets = array(
    'location' => 'location',
    'user' => 'usr',
    'ppl' => 'ppl',
    'ads' => 'ads',
);

$riak = new Riak\Client($server['host'], $server['port'], $server['raw_name'], $server['mapred_name']);
var_export($riak->bucket($bukets['ppl'])->('test_value'));

