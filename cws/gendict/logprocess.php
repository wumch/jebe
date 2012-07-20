<?php

$log_file = '/tmp/texts.utf8.log';
//$log_file = '/tmp/texts.utf8.log';
$encoding_required = 'utf-8';
$encoding_list = array('utf-8', 'gbk');

$turn = intval($_GET['t']);
$text = urldecode($_GET['c']);

$encoding = mb_detect_encoding($text, $encoding_list, true);

if (strtolower($encoding) !== $encoding_required)
{
    $text = iconv($encoding, $encoding_required, $text);
}

$info = array(
    'turn' => $turn,
    'text' => $text,
);

file_put_contents($log_file, var_export($info, true) . PHP_EOL, FILE_APPEND | LOCK_EX);

/*
if ($text !== false)
{
    file_put_contents($log_file, var_export($text, true), FILE_APPEND | LOCK_EX);
}
*/

