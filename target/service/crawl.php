<?php

class Config
{
    public $text;

    private static $_instance;
    private function __construct() {}
    public static function instance()
    {
        if (!self::$_instance instanceof self)
        {
            self::$_instance = new self;
        }
        return self::$_instance;
    }
}

class Crawler
{
    public function __construct()
    {

    }

    public function process($data)
    {
        $info = json_decode(gzdeflate($data));

    }

    protected function processInfo($info)
    {
        $mongodb = new MongoDB();
        $mongodb->selectCollection();
    }
}

file_put_contents("/tmp/crawled", file_get_contents("php://input") . PHP_EOL, FILE_APPEND | FILE_BINARY);

