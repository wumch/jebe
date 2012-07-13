#!/usr/bin/env php
<?php

error_reporting(E_ALL);
set_time_limit(86400);

class Decoder
{
    protected $charset = 'utf-8';
    protected $charlen = 3;

    protected $infile, $outfile;
    protected $in, $out;
    protected $insize;
    protected $maxchars;
    protected $limit;
    protected $step = 3145728;  // 3M(bytes)
    protected $reserve = 2;

    public function __construct($infile, $outfile, $maxchars = 0)
    {
        $this->infile = $infile;
        $this->outfile = $outfile;
        $this->maxchars = $maxchars;
    }

    public function process()
    {
        $this->prepare();
        $this->decode();
    }

    protected function decode()
    {
        $processed = 0;
        $remains = '';
        $decoded_remains = '';
        while ($processed < $this->limit)
        {
            $content = fread($this->in, min($this->step, $this->limit - $processed));
            if ($content === false)
            {
                throw new Exception("failed on read content", 10086);
            }
            if ($remains !== '')
            {
                $content = $remains . $content;
                $remains = '';
            }

            $len = strlen($content);
            if ($len < 3)
            {
                throw new Exception('strlen($content) === ' . $len);
            }

            $tailpos = 0;
            if ($content[$len - 3] != '%')
            {
                if ($content[$len - 1] === '%')
                {
                    $tailpos = $len - 1;
                }
                else if ($content[$len - 2] === '%')
                {
                    $tailpos = $len - 2;
                }
            }

            if ($tailpos !== 0)
            {
                $remains = substr($content, $tailpos);
                $content = substr($content, 0, $tailpos);
            }
            $processed += strlen($content);

            // ------ decoded contents ------
            if ($decoded_remains === '')
            {
                $decoded = urldecode($content);
            }
            else
            {
                $decoded = $decoded_remains . urldecode($content);
//                $decoded_remains = '';
            }
//            $decoded_len = strlen($decoded);
            $decoded_mblen = mb_strlen($decoded, $this->charset);
            if ($decoded_mblen > 3)
            {
                $decoded_remains = mb_substr($decoded, $decoded_mblen - 3, 3, $this->charset);
                $decoded = mb_substr($decoded, 0, $decoded_mblen - 3, $this->charset);
            }


//            $tail = substr($decoded, $decoded_len - $this->charlen);
//
//            if (mb_strlen($tail, $this->charset) !== 1)
//            {
//                $broken_len = $this->charlen;
//
//                $has_ascii = false;
//                for ($i = $this->charlen; $i > 0; ++$i)
//                {
//                    if (ord($tail[$i]) < 128)
//                    {
//                        $broken_len -= $i;
//                        $has_ascii = true;
//                        break;
//                    }
//                }
//
//                if ($has_ascii)
//                {
//                    $decoded_remains = substr($tail, $broken_len);
//                    $decoded = substr($decoded, 0, $decoded_len - $broken_len);
//                }
//                else
//                {
//                    $append = '';
//                    for ($i = 0; $i < $this->charlen; ++$i)
//                    {
//                        $append .= $tail[$i];
//                        if (mb_strlen())
//                    }
//                }
//            }
            fwrite($this->out, $decoded);
        }
        if ($decoded_remains)
        {
            fwrite($this->out, $decoded_remains);
        }
    }

    protected function prepare()
    {
        $this->in = fopen($this->infile, 'r');
        $this->out = fopen($this->outfile, 'a+');

        if (!$this->in or !$this->out)
        {
            throw new ErrorException("open file failed.", 10086);
        }

        $stats = fstat($this->in);
        $this->insize = $stats['size'];

        $this->limit = ($this->maxchars != 0 and $this->insize > $this->maxchars) ?
                $this->maxchars : $this->insize;
    }
}

function main()
{
    list($argc, $argv) = array($_SERVER['argc'], $_SERVER['argv']);
    if ($argc < 3)
    {
        die("usage: {$argv[0]} content-file words-file [max-characters-to-process]" . PHP_EOL);
    }
    $infile = $argv[1];
    $outfile = $argv[2];
    $maxchars = ($argc >= 4) ? intval($argv[3]) : 0;

    $decoder = new Decoder($infile, $outfile, $maxchars);
    $decoder->process();
}

main();
