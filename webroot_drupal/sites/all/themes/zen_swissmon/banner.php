<?php
$PATH = $_SERVER['DOCUMENT_ROOT'];  //Webroot holen
$imageDirPath = $PATH.'/sites/all/themes/zen_swissmon/images/banner/'; // Zum Ordner mit den Bannern wechseln

$files=array();
$imageDir = opendir($imageDirPath); // Ordner mit Bannern öffnen

while ($f = readdir($imageDir)){
	if (preg_match("/\.gif/i", $f) || preg_match("/\.jpeg/i", $f) || preg_match("/\.jpg/i", $f) || preg_match("/\.png/i", $f)){
		array_push($files, "$f");
	}
}

// Jeden Tag ein anderes Bild
$bannerNr = date("z") % count($files);

if (empty($bannerNr)){	
	$bannerNr = rand(0, count($files)-1);
	setcookie($cookieName, $bannerNr);  // Bildnummer
	echo $a;
}

if (preg_match("/\.gif/i", $files[$bannerNr])){
	header("Content-type: image/gif");
	$image = imagecreatefromgif($imageDirPath.$files[$bannerNr]);
	imagegif($image);
	imagedestroy($image);
}

if (preg_match("/\.jpeg/i", $files[$bannerNr])){
	header("Content-type: image/jpg");
	$image = imagecreatefromjpeg($imageDirPath.$files[$bannerNr]);
	imagejpeg($image);
	imagedestroy($image);
}

if (preg_match("/\.jpg/i", $files[$bannerNr])){
	header("Content-type: image/jpg");
	$image = imagecreatefromjpeg($imageDirPath.$files[$bannerNr]);
	imagejpeg($image);
	imagedestroy($image);
}

if (preg_match("/\.png/i", $files[$bannerNr])){
	header("Content-type: image/png");
	$image = imagecreatefrompng($imageDirPath.$files[$bannerNr]);
	imagepng($image);
	imagedestroy($image);
}
?>
