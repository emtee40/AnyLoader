<?php
class AnyLoader {
	function __construct($host = "localhost", $port = 8964)
	{
		$this->socket = pfsockopen($host, $port);
	}
	private function request() {
		$args = func_get_args();
		$request = implode("|", $args);
		fwrite($this->socket, $request."\n");
		fflush($this->socket);
		$response = trim(fgets($this->socket));
		if ($response == "||||||||") {
			$fullText = "";
			while (($response = fgets($this->socket)) != "||||||||\n")
				$fullText .= $response;
			return $fullText;
		} else if ($response == "|||||||||||||") {
			$entries = array();
			while (($response = trim(fgets($this->socket))) != "|||||||||||||")
				$entries[] = $this->parseLine($response);
			return $entries;
		} else
			return $this->parseLine($response);
	}
	private function parseLine($line)
	{
		if ($line == "error")
			return false;
		elseif ($line == "success")
			return true;
		elseif (strpos($line, "|") !== false)
			return explode("|", $line);
		else
			return $line;
	}
	public function getTitleInformation($movieTitle) {
		return $this->request("getTitleInformation", $movieTitle);
	}
	public function getTitles() {
		$ret = $this->request("getTitles");
		if ($ret === false)
			return array();
		$all = array();
		foreach ($ret as $structure) {
			$all[] = (object)array(
				"title" => $structure[0],
				"hasRipped" => ($structure[1] == "1" ? true : false),
				"hasEncoded" => ($structure[2] == "1" ? true : false),
				"hasUploaded" => ($structure[3] == "1" ? true : false),
				"videoTrack" => $structure[4],
				"audioTracks" => ($structure[5] == "auto" ? "auto" : explode(",", $structure[5]))
			);
		}
		return $all;
	}
	public function addISO($fileName) {
		return $this->request("addISO", $fileName);
	}
	public function addRecursiveISOs($dirName) {
		return $this->request("addRecursiveISOs", $dirName);
	}
	public function setVideoTrack($movieTitle, $videoTrack) {
		return $this->request("setVideoTrack", $movieTitle, $videoTrack);
	}
	public function setAudioTracks($movieTitle, $audioTracks) {
		return $this->request("setAudioTracks", $movieTitle, implode(",", $audioTracks));
	}
	public function ripStatus() {
		return $this->request("ripStatus");
	}
	public function encodeStatus() {
		return $this->request("encodeStatus");
	}
	public function uploadStatus() {
		return $this->request("uploadStatus");
	}
	public function terminateRip() {
		return $this->request("terminateRip");
	}
	public function terminateEncode() {
		return $this->request("terminateEncode");
	}
	public function terminateUpload() {
		return $this->request("terminateUpload");
	}
	public function startTerminatedTasks() {
		return $this->request("startTerminatedTasks");
	}
}
?>