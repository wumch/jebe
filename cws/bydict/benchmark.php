#!/usr/bin/env php
<?php

$counts = 5000;
$url = "http://localhost:10087";

function getContent()
{
    return file_get_contents(dirname(realpath(__FILE__)) . DIRECTORY_SEPARATOR . 'tests' . DIRECTORY_SEPARATOR . 'content.txt');
}

function request($post)
{
    global $url;
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_POST, 1);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $post);
    $response = curl_exec($ch);
    $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
    curl_close($ch);
    return $httpCode;
}

function bench($counts, $post)
{
    $begin = microtime(true);
    for ($i = 0; $i < $counts; ++$i)
    {
        if (request($post) !== 200)
        {
            die('error occured' . PHP_EOL);
        }
    }
    $end = microtime(true);
    return $end - $begin;
}

$post = getContent();
$post = mb_substr($post, 0, intval(1024 / 3));
$time = bench($counts, $post);
echo "post: [{$post}]", PHP_EOL,
     "{$counts} finished in {$time}", PHP_EOL,
     "QPS: ", $counts / $time, PHP_EOL,
     "time per request: ", $time / $counts, PHP_EOL;

