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
        'tokenizer' => array('http://192.168.88.2:10086/split', 'http://192.168.88.4:10086/split'),
        'host' => '192.168.88.2',
        'port' => '8098',
        'raw_name' => 'riak',
        'mapred_name' => 'mapred',
        // test only
        'tokenizer' => array('http://127.0.0.1:10086/split'),
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
    const DW_VALUE = 0;

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

    protected $url;

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

        $this->paraBucket->newBinary('useless_key', 'useless_value', 'text/plain')->store(self::W_VALUE, self::DW_VALUE);
        $this->metaBucket->newBinary('useless_key', 'useless_value', 'text/plain')->store(self::W_VALUE, self::DW_VALUE);
        $this->persistBucket->newBinary('useless_key', 'useless_value', 'text/plain')->store(self::W_VALUE, self::DW_VALUE);
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
            if (!in_array($this->curPara['turn'], $metaData['reached'], true))
            {
                $this->savePara();
                $this->updateMeta($metaData);
            }
        }
    }

    // TODO: solve parallel save one day...
    public function savePage($pageContent)
    {
        $words = $this->tokenize($pageContent);
        $pageData = array(
            'location' => $this->url,
            'words' => strlen($words) ? $words : ' ',
        );
        $this->persistBucket->newObject($this->genPageKey(), $pageData)->store(self::W_VALUE, self::DW_VALUE);
    }

    public function savePara()
    {
        // forced `json_decode($this->data, true)` call sucks, newBinary doesnot work...
        $data = array('content' => $this->curPara['content'], );
        $this->paraBucket->newObject($this->genParaKey($this->curPara['turn']), $data)->store(self::W_VALUE, self::DW_VALUE);
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
        return (count($info['reached']) + 1 === $info['total'])
                and !in_array($turn, $info['reached'], true);
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
        $paras[$this->curPara['turn']] = $this->curPara['content'];
        ksort($paras, SORT_NUMERIC | SORT_ASC);
        $pageContent = implode('', $paras);
        if (strlen($pageContent) === 0)
        {
            return false;
        }
        return $pageContent;
    }

    protected function tokenize($content)
    {
        foreach ($this->server['tokenizer'] as $url)
        {
            $ch = curl_init($url);
            curl_setopt($ch, CURLOPT_URL, $url);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
            curl_setopt($ch, CURLOPT_POST, 1);
            curl_setopt($ch, CURLOPT_POSTFIELDS, $content);
            curl_setopt($ch, CURLOPT_CONNECTTIMEOUT_MS, 150);
            curl_setopt($ch, CURLOPT_TIMEOUT_MS, 2000);
            curl_setopt($ch, CURLOPT_FORBID_REUSE, true);
            $response = curl_exec($ch);
            $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
            curl_close($ch);
            if ($httpCode === 200)
            {
                return $response;
            }
        }
        return false;
    }

    protected function retrieveMetaData()
    {
        $this->metaObject = $this->metaBucket->get($this->genMetaKey(), self::R_VALUE);
        return $this->metaObject->exists() ? $this->metaObject->getData() : null;
    }

    public function retrieveParaList(array $reached)
    {
        $paras = array();
        foreach ($reached as $turn)
        {
            $obj = $this->paraBucket->get($this->genParaKey($turn), self::R_VALUE);
            if (!$obj->exists())
            {
                return false;
            }
            $data = $obj->getData();
            $paras[$turn] = $data['content'];
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

    public function isack()
    {
        return $this->curPara['turn'] <= 0;
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
{
    if (function_exists('imagecreate'))
    {
        header('Content-Type: image/png');
        header('Cache-Control: no-store, no-cache, must-revalidate');
        $image = imagecreate(1,1);
        imagepng($image);
        imagedestroy($image);
    }
    else
    {
        header('Content-Type: image/gif');
        $filesize = filesize('px.gif');
        header('Last-Modified: ' . gmdate('D, d M Y H:i:s') . ' GMT');
        header('Cache-Control: no-store, no-cache, must-revalidate');
        header('Content-length: '.$filesize);
        readfile('px.gif');
    }
}

function process()
{
    $pageHolder = new PageHolder();
    if ($pageHolder->pageExists())
    {
        // TODO: page already exists, increase count? currently nothing to do.
        responseError(0);
    }
    else
    {
        responseSuccess();
        if (!$pageHolder->isack())
        {
            fastcgi_finish_request();
            $pageHolder->process();
        }
    }
}

process();
