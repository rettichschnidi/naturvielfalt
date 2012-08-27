<?php



$PATH = $_SERVER['DOCUMENT_ROOT'];  //Webroot holen
$imageDirPath = $PATH.'/sites/all/themes/zen_swissmon/images/banner/'; // Zum Ordner mit den Bannern wechseln

$files=array();
$imageDir = opendir($imageDirPath); // Ordner mit Bannern öffnen

while ($f = readdir($imageDir)){
	if (eregi("\.gif", $f) || eregi("\.jpeg", $f) || eregi("\.jpg", $f) || eregi("\.png", $f)){
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

if (eregi("\.gif", $files[$bannerNr])){
	header("Content-type: image/gif");
	$image = imagecreatefromgif($imageDirPath.$files[$bannerNr]);
	imagegif($image);
	imagedestroy($image);
}

if (eregi("\.jpeg", $files[$bannerNr])){
	header("Content-type: image/jpg");
	$image = imagecreatefromjpeg($imageDirPath.$files[$bannerNr]);
	imagejpeg($image);
	imagedestroy($image);
}

if (eregi("\.jpg", $files[$bannerNr])){
	header("Content-type: image/jpg");
	$image = imagecreatefromjpeg($imageDirPath.$files[$bannerNr]);
	imagejpeg($image);
	imagedestroy($image);
}

if (eregi("\.png", $files[$bannerNr])){
	header("Content-type: image/png");
	$image = imagecreatefrompng($imageDirPath.$files[$bannerNr]);
	imagepng($image);
	imagedestroy($image);
}
?>
