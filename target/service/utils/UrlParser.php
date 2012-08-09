<?php
namespace utils;

class UrlParser
{
    const SPLIT = 1;
    const NO_QUERY = 2;

    protected $type = array('com', 'mobi', 'gov', 'edu', 'so', 'net', 'org', 'name', 'me', 'co', 'com', 'net', 'tel', 'info', 'biz', 'cc', 'tv');
    protected $area = array('cn', 'hk', 'jp', 'ko', 'ra');

    protected $mode;

    public function __construct()
    {
    }

    public function parse($url, $mode = self::SPLIT)
    {
        $this->mode = $mode;
        return $this->splitUrl($url);
    }

    public function splitUrl($url)
    {
        $info = parse_url($url);
        if (!isset($info['scheme']) or !isset($info['host']))
        {
            return false;
        }
        $scheme = $this->parseScheme($info['scheme']);
        $domain = $this->parseDomain($info['host']);
        if ($domain === false)
        {
            //return false;
        }
        $domain = implode(' ', $this->parseDomain($info['host']));
        if ($scheme === false or $domain === false)
        {
            return false;
        }
        $res = $scheme . ' ' . $domain;
        if (isset($info['path']))
        {
            if (($path = $this->parsePath($info['path'])) === false)
            {
                return false;
            }
            $res .= ' ' . implode(' ', $path);
        }
        if (!($this->mode & self::NO_QUERY) and isset($info['query']))
        {
            if (($query = $this->parseQuery($info['query'])) === false)
            {
                return false;
            }
            $res .= ' ' . implode(' ', $query);
        }
        return trim($res);
    }

    public function parsePath($path)
    {
        $info = array_slice(array_filter(array_map('trim', explode('/', $path)), 'strlen'), 0);
        $res = array();
        for ($i = 0, $end = count($info); $i < $end; ++$i)
        {
            $res[$i] = str_repeat('/', $i + 1) . $info[$i];
        }
        return $res;
    }

    public function parseDomain($domain)
    {
        // strip the 'port'
        $colonPos = strpos($domain, ':');
        if ($colonPos !== false)
        {
            $domain = substr($domain, 0, $colonPos);
        }
        $info = explode('.', $domain);
        $len = count($info);
        switch ($len)
        {
            case 1:
                return false;
            case 2:
                return array($info[0]);
            default:
                $last = end($info);
                $last2 = prev($info);
                $subLen = 0;
                if (in_array($last, $this->type))
                {
                    $subLen = $len - 2;
                }
                else if (in_array($last, $this->area))
                {
                    $subLen = $len - 3;
                }
                else
                {
                    // TODO: should solve un-normal domains.
                    $subLen = $len - 2;
                }
                $sub = implode('.', array_slice($info, 0, $subLen));
                $main = implode('.', array_slice($info, $subLen));
                return array($sub, $main);
        }
    }

    public function parseQuery($query)
    {
        return array_filter(explode('&', $query));
    }

    public function parseScheme($scheme)
    {
        return ($scheme === 'http' or $scheme === 'https' or $scheme === 'ftp') ?
            $scheme : false;
    }
}

function test($url)
{
    $parser = new UrlParser();
    echo 'url:  ', $url, PHP_EOL,
        'info: ', $parser->parse($url), PHP_EOL, PHP_EOL;
}

test('http://www.baidu.com');
test('http:/www.baidu.com');
test('http://www.baidu.com?fsdf=8998&fsdaf=fsdf');
test('http://www.baidu.com/fasdfd/afsdfadf/fasdf?fsdf=8998&fsdaf=fsdf');
test('http://yundao.hi.baidu.com.cn/fasdfd/afsdfadf/fasdf?fsdf=8998&fsdaf=fsdf');
test('http://yundao.hi.baidu.com.xo/fasdfd/afsdfadf/fasdf?fsdf=8998&fsdaf=fsdf');
test('http://yundao.hi.baidu.cxx.xo/fasdfd/afsdfadf/fasdf?fsdf=8998&fsdaf=fsdf');
