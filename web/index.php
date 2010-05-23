<html>
<head>
<title>AnyLoader</title>
<script language="JavaScript">
function setVideoTrack(movie)
{
	var track = parseInt(prompt("After looking at the list of video titles below, choose one track by entering the title number here.", "1"));
	if (!isNaN(track) && track >= 0)
		location.href = "setValue.php?value=videoTrack&movie=" + encodeURIComponent(movie) + "&track=" + track;
}
function setAudioTrack(movie)
{
	var track = parseInt(prompt("After looking at the list of audio tracks for your desired video track below, choose one audio track by entering the track number here.", "1"));
	if (!isNaN(track) && track >= 0)
		location.href = "setValue.php?value=audioTrack&movie=" + encodeURIComponent(movie) + "&track=" + track;
}
</script>
<style>
a { text-decoration: none; color: blue; }
td, th { border: 1px gray solid; padding: 2px; }
table { border-collapse: collapse; }
pre { margin: 0; }
@font-face { font-family: 'Myriad Pro'; src: local('Myriad Pro Regular'), local('Myriad Pro'), url('http://static2.anyclipimg.com/fonts/MyriadPro-Regular.otf') format('opentype'); }
@font-face { font-family: 'Myriad Pro'; font-weight: bold; src: local('Myriad Pro Bold'), url('http://static2.anyclipimg.com/fonts/MyriadPro-Bold.otf') format('opentype'); }
@font-face { font-family: 'Myriad Pro'; font-style: italic; src: local('Myriad Pro Italic'), url('http://static2.anyclipimg.com/fonts/MyriadPro-It.otf') format('opentype'); }
@font-face { font-family: 'Myriad Pro'; font-weight: bold; font-style: italic; src: local('Myriad Pro Bold Italic'), url('http://static2.anyclipimg.com/fonts/MyriadPro-BoldIt.otf') format('opentype'); }
body { font-family: "Myriad Pro", Myriad, "Myriad Web Pro", Helvetica, Arial, sans-serif; }
</style>
</head>
<body>
<img src="http://labs.anyclip.com/labs-logo.png" style="float:right">
<h2>AnyLoader<br><font size="-1"><i>(testing phase)</i></font></h2>
<?php
function yesno($bool)
{
	return $bool ? "yes" : "no";
}
require_once("AnyLoader.php");
$loader = new AnyLoader();
$rippingEnabled = $loader->isRippingEnabled();
echo "<table>";
if ($rippingEnabled) {
	echo "<tr><th>Rip Status</th><td>";
	$ripStatus = $loader->ripStatus();
	if ($ripStatus === false)
		echo "(idle)";
	else
		echo "<i>".$ripStatus[0]."</i>: ".$ripStatus[1]." (<a href=\"setValue.php?value=terminateRip\">terminate</a>)";
	echo "</td></tr>";
}
echo "<tr><th>Encode Status</th><td>";
$encodeStatus = $loader->encodeStatus();
if ($encodeStatus === false)
	echo "(idle)";
else
	echo "<i>".$encodeStatus[0]."</i>: ".$encodeStatus[1]." (<a href=\"setValue.php?value=terminateEncode\">terminate</a>)";
echo "</td></tr>";
echo "<tr><th>Upload Status</th><td>";
$uploadStatus = $loader->uploadStatus();
if ($uploadStatus === false)
	echo "(idle)";
else
	echo "<i>".$uploadStatus[0]."</i>: ".$uploadStatus[1]." (<a href=\"setValue.php?value=terminateUpload\">terminate</a>)";
echo "</td></tr>";
echo "<tr><td align=center colspan=2><a href=\"setValue.php?value=startTerminatedTasks\">Start Terminated Tasks</a></td></tr>";
echo "</table>";
echo "<br>";
echo "<table>";
echo "<tr><th>Title</th>";
if ($rippingEnabled)
	echo "<th>Ripped</th>";
echo "<th>Encoded</th><th>Uploaded</th><th>Video Title</th><th>Audio Track</th></tr>";
foreach ($loader->getTitles() as $title) {
	echo "<tr><td>".$title->title." (<a href=\"setValue.php?value=removeMovie&movie=".$title->title."\">x</a>)</td>";
	if ($rippingEnabled)
		echo "<td>".yesno($title->hasRipped)."</td>";
	echo "<td>".yesno($title->hasEncoded)."</td><td>".yesno($title->hasUploaded)."</td><td>";
	if (!$title->hasEncoded && $encodeStatus[0] != $title->title)
		echo "<a href=\"javascript:setVideoTrack('".$title->title."');\">";
	if ($title->videoTrack == 0)
		echo "(not yet selected)";
	else
		echo "Video Title ".$title->videoTrack;
	if (!$title->hasEncoded && $encodeStatus[0] != $title->title) {
		echo "</a>";
		if ($title->videoTrack != 0)
			echo " <i>(<a href='javascript:void(0);' onclick='this.parentNode.parentNode.parentNode.nextSibling.style.display = \"table-row\";this.parentNode.parentNode.removeChild(this.parentNode);'>Show Titles &darr;</a>)</i>";
	}
	echo "</td><td>";
	if (!$title->hasEncoded && $encodeStatus[0] != $title->title)
		echo "<a href=\"javascript:setAudioTrack('".$title->title."');\">";
	if ($title->audioTrack == 0)
		echo "(not yet selected)";
	else
		echo "Audio Track ".$title->audioTrack;
	if (!$title->hasEncoded && $encodeStatus[0] != $title->title)
		echo "</a>";
	echo "</td></tr>";
	if (!$title->hasEncoded && $encodeStatus[0] != $title->title) {
		echo "<tr";
		if ($title->videoTrack != 0)
			echo " style='display:none;'";
		echo "><td></td><td colspan=".($rippingEnabled ? "5" : "4")."><div style='height:300px;width:700px;overflow:auto'><pre>";
		echo $loader->getTitleInformation($title->title);
		echo "</pre></div></td></tr>";
	}
}
echo "<tr><th colspan=".($rippingEnabled ? "6" : "5")."><a href=\"setValue.php?value=loadISOs\">Load ISOs From Attached Hard Drive</a></th></tr>";
echo "</table>";
?>
</body>
</html>
