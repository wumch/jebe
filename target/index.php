<?php
namespace target;
use Riak;

set_time_limit(10);
error_reporting(E_ALL);
define('ROOT_PATH', __DIR__ . DIRECTORY_SEPARATOR);
define('RIAK_HOME', ROOT_PATH . 'Riak' . DIRECTORY_SEPARATOR);
define('IMAGE_FILE_PATH', ROOT_PATH . 'px.gif');

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
        'raw_name' => 'riak',
        'mapred_name' => 'mapred',
        // test only
        'host' => '211.154.172.172',
        'port' => '18098',
        'host' => '127.0.0.1',
        'port' => '8098',
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
    protected $metaObject;

    protected $paraBucket;

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

    // test only
    public function createParaBucket()
    {
        foreach ($this->paraProp as $name => $value)
        {
            $this->paraBucket->setProperty($name, $value);
            $this->metaBucket->setProperty($name, $value);
            $this->persistBucket->setProperty($name, $value);
        }
        $this->paraBucket->setNVal($this->paraNVal);
        $this->metaBucket->setNVal($this->paraNVal);
        $this->persistBucket->setNVal($this->paraNVal);

        $this->paraBucket->newBinary('useless_key', 'useless_value')->store(self::W_VALUE, self::DW_VALUE);
        $this->metaBucket->newBinary('useless_key', 'useless_value')->store(self::W_VALUE, self::DW_VALUE);
        $this->persistBucket->newBinary('useless_key', 'useless_value')->store(self::W_VALUE, self::DW_VALUE);
        var_dump($this->metaBucket->get('useless_key', self::W_VALUE));
        var_dump($this->persistBucket->get('useless_key', self::W_VALUE));
    }

    public function preparePara()
    {
        if (!isset($_SERVER['HTTP_REFERER']))
        {
            return false;
        }
        $this->url = urlencode(trim(urldecode($_SERVER['HTTP_REFERER'])));
        if (substr($this->url, 0, 4) !== 'http')
        {
            return false;
        }
        $curPara = $_GET;
        if (!isset($curPara['n']) or !isset($curPara['t']) or !isset($curPara['c']))
        {
            return false;
        }
        // TODO: prevent from unreachable `paragraph-count` cheats by message-queue.
        $paraCount = intval($curPara['n']);
        $turn = intval($curPara['t']);
        if ($turn > $paraCount)
        {
            return false;
        }
        $this->curPara = array(
            'total' => $paraCount,
            'turn' => $turn,
            'content' => trim(urldecode($curPara['c'])),
        );
        return true;
    }

    public function process()
    {
        $metaData = $this->retrieveMetaData();
        if ($metaData === null)
        {
            $metaData = $this->saveMeta();
        }
        if (($pageContent = $this->getPageContent($metaData)) !== false)
        {
            $this->savePage($pageContent);
            $this->clean($metaData);
        }
        else
        {
            if (array_search($this->curPara['turn'], $metaData['reached'], true) === false)
            {
                $this->savePara();
                $this->updateMeta($metaData);
            }
        }
    }

    // TODO: solve parallel save one day...
    public function savePage($pageContent)
    {
        $this->persistBucket->newBinary($this->genPageKey(), $pageContent)->store(self::W_VALUE, self::DW_VALUE);
    }

    public function savePara()
    {
        $this->paraBucket->newBinary($this->genParaKey($this->curPara['turn']), $this->curPara['content'])->store(self::W_VALUE, self::DW_VALUE);
    }

    public function clean()
    {
        if ($this->metaObject->exists())
        {
            $data = $this->metaObject->getData();
            foreach ($data['reached'] as $turn)
            {
                $para = $this->paraBucket->get($this->genParaKey($turn), self::R_VALUE);
                if ($para->exists())
                {
                    $para->delete();
                }
            }
            $this->metaObject->delete();
        }
    }

    public function saveMeta()
    {
        $metaData = array(
            'total' => $this->curPara['total'],
            'reached' => array(),
        );
        $this->metaObject = $this->metaBucket->newObject($this->genMetaKey(), $metaData);
        $this->metaObject->store(self::W_VALUE, self::DW_VALUE);
        return $this->metaObject->getData();
    }

    public function updateMeta(array $metaData)
    {
        array_push($metaData['reached'], $this->curPara['turn']);
        $this->metaObject->setData($metaData);
        $this->metaObject->store(self::W_VALUE, self::DW_VALUE);
    }

    public function pageExists()
    {
        return $this->persistBucket->get($this->genPageKey(), self::R_VALUE)->exists();
    }

    public function isPendingFor($turn, $info = null)
    {
        if ($info === null)
        {
            $info = $this->retrieveMetaData();
        }
        if (isset($info['pending']) && $info['pending'] === 1)
        {
            return (count($info['reached']) + 1 === $info['total'])
                    and array_search($turn, $info['reached'], true) === false;
        }
        return false;
    }

    public function getPageContent($metaData = null)
    {
        // if ($paraNum === 1)     maybe cheats.
        if ($metaData === null)
        {
            if ($this->curPara['total'] === 1)
            {
                return $this->curPara['content'];
            }
            else
            {
                $metaData = $this->retrieveMetaData();
            }
        }
        if (!$this->isPendingFor($this->curPara['turn'], $metaData))
        {
            return false;
        }
        $paras = $this->retrieveParaList($metaData['reached']);
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
        $this->metaObject = $this->metaBucket->get($this->genMetaKey(), self::R_VALUE);
        return $this->metaObject->exists() ? $this->metaObject->getData() : null;
    }

    public function retrieveParaList(array $reached)
    {
        $bucket = $this->client->bucket($this->paraBucket);
        $paras = array();
        foreach ($reached as $turn)
        {
            $obj = $bucket->get($this->genParaKey($turn), self::R_VALUE);
            if (!$obj->exists())
            {
                return false;
            }
            $paras[] = $obj->getData();
        }
        return $paras;
    }

    protected function retrievePara($turn)
    {
        return $this->paraBucket->get($this->genParaKey($turn), self::R_VALUE)->getData();
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

// reply client with an error to notify something like that the page already exists in database.
function responseError($exit = false)
{
    header('Status:', true, 404);
    if ($exit !== false)
    {
        exit($exit);
    }
}

function responseSuccess()
{header ('Content-Type: image/png');
$im = @imagecreatetruecolor(120, 20)
      or die('Cannot Initialize new GD image stream');
$text_color = imagecolorallocate($im, 233, 14, 91);
imagestring($im, 1, 5, 5,  'A Simple Text String', $text_color);
imagepng($im);
imagedestroy($im);
    exit(0);
    header('Content-Type: image/png');
//    header('Cache-Control: no-store, no-cache, must-revalidate');
    $image = imagecreatetruecolor(100,100);
//    imageColorAllocate($image,0,0,0);
    imagepng($image);
    imagedestroy($image);
//    $filesize = filesize('px.gif');
//    header('Last-Modified: ' . gmdate('D, d M Y H:i:s') . ' GMT');
//    header('Cache-Control: no-store, no-cache, must-revalidate');
//    header('Content-length: '.$filesize);
//    readfile('px.gif');
}

function process()
{
    responseSuccess();
//    fastcgi_finish_request();
    exit(0);
    $pageHolder = new PageHolder();
    if ($pageHolder->pageExists())
    {
        // TODO: page already exists, increase count? currently nothing to do.
        responseError(0);
    }
    else
    {
        responseSuccess();
//        fastcgi_finish_request();
        $pageHolder->process();
    }
    // test only
    $pageHolder->process();
}

process();
