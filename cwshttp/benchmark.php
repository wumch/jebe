#!/usr/bin/env php
<?php

$counts = 10;
$url = "http://localhost:10086/count";

function getContent()
{
    return file_get_contents(dirname(realpath(__FILE__)) . DIRECTORY_SEPARATOR . 'tests' . DIRECTORY_SEPARATOR . 'content.txt');
}

function request($post)
{
    global $url;
    static $echoed_flag = false;
    static $first_resp = false;
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_POST, 1);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $post);
    $response = curl_exec($ch);
    if ($first_resp === false)
    {
        $first_resp = $response;
    }
    else
    {
        if ($first_resp !== $response)
        {
            echo 'response not equal to the first one', PHP_EOL;
        }
    }
    if (!$echoed_flag)
    {
        $echoed_flag = true;
        echo 'response: ', $response, PHP_EOL;
    }
    $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
    curl_close($ch);
    return $httpCode;
}

function bench($counts, $post)
{
    $begin = microtime(true);
    for ($i = 0; $i < $counts; ++$i)
    {
        $respCode = request($post);
        if ($respCode !== 200)
        {
            die("{$i} response code: {$respCode}" . PHP_EOL);
        }
    }
    $end = microtime(true);
    return $end - $begin;
}

$post = getContent();
$post = mb_substr($post, 0, intval(1500));
$time = bench($counts, $post);
echo "post: [{$post}]", PHP_EOL,
     "{$counts} finished in {$time}", PHP_EOL,
     "QPS: ", $counts / $time, PHP_EOL,
     "time per request: ", $time / $counts, PHP_EOL;

