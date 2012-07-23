<?php
namespace target;
use Riak;

set_time_limit(10);
error_reporting(E_ALL);
define('ROOT_PATH', __DIR__ . DIRECTORY_SEPARATOR);
define('RIAK_HOME', ROOT_PATH . 'Riak' . DIRECTORY_SEPARATOR);

function autoload_riak($symbol)
{
    require(ROOT_PATH . str_replace('\\', '/', $symbol) . '.php');
    return true;
}
spl_autoload_register('target\autoload_riak');

class PageHolder
{
    protected $server = array(
        'host' => '192.168.88.2',
        'port' => '8098',
        'host' => '211.154.172.172',
        'port' => '18098',

        'host' => '127.0.0.1',
        'port' => '8098',

        'raw_name' => 'riak',
        'mapred_name' => 'mapred',
    );

    protected $buckets = array(
        'location' => 'location',
        'user' => 'usr',
        'ppl' => 'ppl',
        'ads' => 'ads',
        'para' => 'para',   // memory_minute
        'meta' => 'meta',
    );

    const R_VALUE = 1;
    const W_VALUE = 1;
    const DW_VALUE = 1;

    protected $persistBucket;

    protected $paraProp = array(
        'backend' => 'memory_minute',
    );
    protected $paraNVal = 1;

    protected $metaBucket;
    protected $metaData = array(
        'total' => 0,
        'pending' => 0,
        'reached' => array(),
    );
    protected $paraBucket;
    protected $paraData = array(
        'content' => '',
    );

    protected $curPara = array();

    public $client = null;

    protected $url = 'http://www.sina.com/';

    public function __construct()
    {
        if ($this->preparePara() === false)
        {
            exit(0);
        }
        $this->client = new Riak\Client($this->server['host'], $this->server['port'], $this->server['raw_name'], $this->server['mapred_name']);
        $this->prepareBuckets();
    }

    public function prepareBuckets()
    {
        $this->persistBucket = $this->client->bucket($this->buckets['location']);
        $this->paraBucket = $this->client->bucket($this->buckets['para']);
        $this->metaBucket = $this->client->bucket($this->buckets['meta']);
    }

    public function createParaBucket()
    {
        foreach ($this->paraProp as $name => $value)
        {
            $this->paraBucket->setProperty($name, $value);
            $this->metaBucket->setProperty($name, $value);
        }
        $this->paraBucket->setNVal($this->paraNVal);
        $this->metaBucket->setNVal($this->paraNVal);

        $this->paraBucket->newBinary('useless_key', 'useless_value')->store(self::R_VALUE, self::DW_VALUE);
        $this->metaBucket->newBinary('useless_key', 'useless_value')->store(self::R_VALUE, self::DW_VALUE);
    }

    public function preparePara()
    {
//        if (!isset($_SERVER['HTTP_REFERER']))
//        {
//            return false;
//        }
//        $this->url = urlencode(trim(urldecode($_SERVER['HTTP_REFERER'])));
        if (substr($this->url, 0, 4) !== 'http')
        {
            return false;
        }
        $this->url = base64_encode($this->url);
        $curPara = $_GET;
        if (!isset($curPara['n']) or !isset($curPara['t']) or !isset($curPara['c']))
        {
            return false;
        }
        $paraNum = intval($curPara['n']);
        $turn = intval($curPara['t']);
        if ($turn > $paraNum)
        {
            return false;
        }
        $this->curPara = array(
            'total' => $paraNum,
            'turn' => $turn,
            'content' => trim(urldecode($curPara['c'])),
        );
        return true;
    }

    public function process()
    {
        if ($this->pageExists())
        {
            // page already exists, increase count?
        }
        if ($pageContent = $this->getPageContent())
        {
            $this->savePage($pageContent);
        }
        else
        {
            $this->savePara();
        }
    }

    public function savePage($pageContent)
    {
        $this->persistBucket->newBinary($this->genPageKey(), $pageContent)->store(null, null);
    }

    public function savePara()
    {
        $this->paraBucket->newBinary($this->genParaKey($this->curPara['turn']), $this->curPara['content'])->store(null, null);
    }

    public function pageExists()
    {
        return $this->persistBucket->get($this->url, self::R_VALUE)->exists;
    }

    public function isPendingFor($turn, $info = null)
    {
        if (!$info)
        {
            $info = $this->retrieveMetaData();
        }
        if (isset($info['pending']) && $info['pending'] === 1)
        {
            return array_search($turn, $info['reached'], true) === false;
        }
        return false;
    }

    public function getPageContent()
    {
        // if ($paraNum === 1)     maybe cheats.
        $info = $this->retrieveMetaData();
        if (!$this->isPendingFor($turn, $info))
        {
            return false;
        }
        $paras = $this->retrieveParaList($info['reached']);
        if ($paras === false)
        {
            return false;
        }
        $pageContent = '';
        foreach ($paras as $para)
        {
            $pageContent .= $para;
        }
        return $pageContent;
    }

    protected function retrieveMetaData()
    {
        return $this->client->bucket($this->metaBucket)->get($this->genMetaKey())->getData();
    }

    public function retrieveParaList(array $reached)
    {
        $bucket = $this->client->bucket($this->paraBucket);
        $paras = array();
        foreach ($reached as $turn)
        {
            $obj = $bucket->get($this->genParaKey($turn));
            if (!$obj->exists)
            {
                return false;
            }
            $paras[] = $obj->getData();
        }
        return $paras;
    }

    protected function retrievePara($turn)
    {
        return $this->client->bucket($this->paraBucket)->get($this->genParaKey($turn))->getData();
    }

    protected function genPageKey()
    {
        return $this->url;
    }

    protected function genMetaKey()
    {
        return $this->url;
    }

    protected function genParaKey($turn)
    {
        return $turn . ':' . $this->url;
    }
}

function process()
{
//    fastcgi_finish_request();
    $pageHolder = new PageHolder();
    var_dump($pageHolder->client->buckets());
//    exit(0);
    $pageHolder->createParaBucket();
    $pageHolder->process();
}

process();
