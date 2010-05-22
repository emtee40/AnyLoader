<html>
<head>
<title>AnyLoader</title>
<script language="JavaScript">
function setVideoTrack(movie)
{
	var track = parseInt(prompt("After looking at the list of video tracks below, choose ONE track by entering the track number here.\n\nNote: Setting the video track will start the encoding, so if you plan to choose a custom audio track, you should choose the audio track(s) first.", "1"));
	if (!isNaN(track))
		location.href = "setValue.php?value=videoTrack&movie=" + encodeURIComponent(movie) + "&track=" + track;
}
</script>
<style>
a { text-decoration: none; color: blue; }
</style>
</head>
<body>
<img src="http://labs.anyclip.com/labs-banner.png" style="float:right"> 
<h2>AnyLoader</h2>
<?php
function yesno($bool)
{
	return $bool ? "yes" : "no";
}
require_once("AnyLoader.php");
$loader = new AnyLoader();
echo "<table border=1>";
echo "<tr><th>Rip Status:</th><td>";
$ripStatus = $loader->ripStatus();
if ($ripStatus === false)
	echo "(idle)";
else
	echo "<i>".$ripStatus[0]."</i>: ".$ripStatus[1]." (<a href=\"setValue.php?value=terminateRip\">terminate</a>)";
echo "</td></tr>";
echo "<tr><th>Encode Status:</th><td>";
$encodeStatus = $loader->encodeStatus();
if ($encodeStatus === false)
	echo "(idle)";
else
	echo "<i>".$encodeStatus[0]."</i>: ".$encodeStatus[1]." (<a href=\"setValue.php?value=terminateEncode\">terminate</a>)";
echo "</td></tr>";
echo "<tr><th>Upload Status:</th><td>";
$uploadStatus = $loader->uploadStatus();
if ($uploadStatus === false)
	echo "(idle)";
else
	echo "<i>".$uploadStatus[0]."</i>: ".$uploadStatus[1]." (<a href=\"setValue.php?value=terminateUpload\">terminate</a>)";
echo "</td></tr>";
echo "<tr><td colspan=2><a href=\"setValue.php?value=startTerminatedTasks\">Start Terminated Tasks</a></td></tr>";
echo "</table>";
echo "<br>";
echo "<table border=1>";
echo "<tr><th>Title</th><th>Ripped</th><th>Encoded</th><th>Uploaded</th><th>Video Track</th></tr>";
foreach ($loader->getTitles() as $title) {
	echo "<tr><td>".$title->title." (<a href=\"setValue.php?value=removeMovie&movie=".$title->title."\">x</a>)</td><td>".yesno($title->hasRipped)."</td><td>".yesno($title->hasEncoded)."</td><td>".yesno($title->hasUploaded)."</td><td>";
	if (!$title->hasEncoded && $encodeStatus[0] != $title->title)
		echo "<a href=\"javascript:setVideoTrack('".$title->title."');\">";
	if ($title->videoTrack == 0)
		echo "(not yet selected)";
	else
		echo $title->videoTrack;
	if (!$title->hasEncoded && $encodeStatus[0] != $title->title)
		echo "</a>";
	echo "</td></tr>";
	if (!$title->hasEncoded && $encodeStatus[0] != $title->title) {
		echo "<tr><td></td><td colspan=4><div style='height:300px;width:700px;overflow:auto'><pre>";
		echo $loader->getTitleInformation($title->title);
		echo "</pre></div></td></tr>";
	}
}
echo "<tr><th colspan=5><a href=\"setValue.php?value=loadISOs\">Load ISOs From Attached Hard Drive</a></th></tr>";
echo "</table>";
?>
</body>
</html>