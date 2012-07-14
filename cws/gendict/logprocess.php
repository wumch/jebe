<?php

$log_file = '/home/david/texts.utf8.log';
//$log_file = '/tmp/texts.utf8.log';
$encoding_required = 'utf-8';
$encoding_list = array('utf-8', 'gbk');

$text = urldecode($_REQUEST['text']);

$encoding = mb_detect_encoding($text, $encoding_list, true);

if (strtolower($encoding) !== $encoding_required)
{
    $text = iconv($encoding, $encoding_required, $text);
}

if ($text !== false)
{
    file_put_contents($log_file, $text, FILE_APPEND | LOCK_EX);
}
