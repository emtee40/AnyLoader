<?php
function pretty($obj)
{
	echo "<pre>";
	print_r($obj);
	echo "</pre>";
}
require_once("AnyLoader.php");
$loader = new AnyLoader();
pretty($loader->addRecursiveISOs("/home/zx2c4/AnyLoader"));
pretty($loader->getTitleInformation("The Sound Of Music"));
pretty($loader->getTitles());
pretty($loader->encodeStatus());
pretty($loader->setVideoTrack("The Sound Of Music", 1));
sleep(1);
pretty($loader->encodeStatus());
pretty($loader->removeMovie("The Sound Of Music"));
sleep(1);
pretty($loader->encodeStatus());
pretty($loader->getTitles());
pretty($loader->getTitleInformation("The Sound Of Music"));
?>