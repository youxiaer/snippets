<?php
ini_set('date.timezone','Asia/Shanghai');
require_once("./config.php");

$_header = array(
    "Host:cl.yyq.im",
    "Referer:http://cl.yyq.im",
    "User-Agent:Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.11 (KHTML, like Gecko) Chrome/23.0.1271.64 Safari/537.11",
);

function getPageInfo($url) {
    $header = array(
        "Host:cl.yyq.im",
        "Referer:http://cl.yyq.im",
        "User-Agent:Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.11 (KHTML, like Gecko) Chrome/23.0.1271.64 Safari/537.11",
    );


    $ch = curl_init();
    $timeout = 60;
    curl_setopt ($ch, CURLOPT_URL, $url);
    curl_setopt ($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt ($ch, CURLOPT_CONNECTTIMEOUT, $timeout);
    curl_setopt ($ch, CURLOPT_HTTPHEADER, $header);
    $file_contents = curl_exec($ch);
    curl_close($ch);
    return $file_contents;
}

function loadImage($dir) {
    if (!file_exists($dir . 'url.txt')) {
echo $dir . 'url.txt';echo "\n";
        return false;
    }
    $urls = file_get_contents($dir . 'url.txt');
    $urls = explode("\n", $urls);
    array_pop($urls);
    $count = count($urls);
    for ($i = 0; $i < $count; $i++) {
        $rs = @ file_get_contents($urls[$i]);
        if ($rs) {
            file_put_contents($dir . ($i + 1) . '.jpg', $rs);
            echo ($i + 1) . "    success\n";
        }
    }
    return true;
}


if(defined("INIT_URL")) {
    $pageUrl = INIT_URL;
} elseif (defined("DOMAIN")) {
    $indexUrl = DOMAIN . 'index.php';
    echo  "凛冬将至: " .  $indexUrl. "\n";
    $index = file_get_contents($indexUrl);
    $index = mb_convert_encoding($index, 'UTF-8', 'UTF-8,GBK,GB2312,BIG5');

    if ($index) {
        $pattern = '/target=\"_blank\"><\/a><h2><a href=\"(.+?)\">新時代的我們<\/a>/';
        preg_match($pattern, $index, $indexMatches);
    }
    $pageUrl = DOMAIN . $indexMatches[1];
} else {
    exit("please define the seed url\n");
}
//echo $pageUrl . "\n"; 
$pidNums = 0;
for ($i = PAGE_START; $i < (PAGE_START + PAGE_COUNT); $i++) {
    $pageUrl = DOMAIN . $indexMatches[1] . '&search=&page=' . $i;
echo $pageUrl . "\n";
    $pageInfo = getPageInfo($pageUrl);
    if (!$pageInfo) {
        continue;
    }

    $pageInfo = mb_convert_encoding($pageInfo, 'UTF-8', 'UTF-8,GBK,GB2312,BIG5');
    $pattern = '/<a href=\"(.+?)\" target=\"_blank\" id=\"\">(.+?)\[(.+?)P\]<\/a>/';
    preg_match_all($pattern, $pageInfo, $pageMatches);
//var_dump($pageMatches);exit;
    $count = count($pageMatches[1]);
    $infoPattern = '/<input type=\'image\' src=\'(.+?)\' onclick=\"window.open/';
    for ($j = 0; $j < $count; $j++) {
        $url = DOMAIN . $pageMatches[1][$j];
        $info = @ file_get_contents($url);
        if (!$info) {
            continue;
        }
        $info = mb_convert_encoding($info, 'UTF-8', 'UTF-8,GBK,GB2312,BIG5');
        preg_match_all($infoPattern, $info, $infoMatches);
        //$urlDir = './image/' . $pageMatches[2][$j] . '[' . $pageMatches[3][$j] . 'P]';
        $urlDir = './image/' . $i . '_' . $j . '[' . $pageMatches[3][$j] . 'P]';
        if (!is_dir($urlDir)) {
            mkdir($urlDir);
        }
//var_dump($infoMatches);exit;
        foreach ($infoMatches[1] as $picUrl) {
            file_put_contents($urlDir . '/url.txt', $picUrl . "\n", FILE_APPEND);
        }
        echo $i . '    ' . $j . "\n";
        $pid = pcntl_fork();
        if ($pid == 0) {
            loadImage($urlDir . '/');
            exit;
        } elseif($pid > 0) {
            $pidNums += 1;
        }
    }
}

//父进程等待子进程结束
while ($pidNums) {
    $status = null;
    pcntl_wait($status);
    $pidNums--;
};
echo "done!\n";



