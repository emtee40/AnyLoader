<?php
require_once("AnyLoader.php");
$loader = new AnyLoader();

if ($_GET["value"] == "videoTrack")
	$loader->setVideoTrack($_GET["movie"], $_GET["track"]);
elseif ($_GET["value"] == "audioTrack")
	$loader->setAudioTrack($_GET["movie"], $_GET["track"]);
elseif ($_GET["value"] == "terminateRip")
	$loader->terminateRip();
elseif ($_GET["value"] == "terminateEncode")
	$loader->terminateEncode();
elseif ($_GET["value"] == "terminateUpload")
	$loader->terminateUpload();
elseif ($_GET["value"] == "removeMovie")
	$loader->removeMovie($_GET["movie"]);
elseif ($_GET["value"] == "startTerminatedTasks")
	$loader->startTerminatedTasks();
elseif ($_GET["value"] == "loadISOs")
	$loader->addRecursiveISOs("/home/zx2c4/AnyLoader");

header("Location: index.php");
?>