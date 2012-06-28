#!/bin/bash
if [ ! -d webroot_drupal ]; then
	echo Please do not execute this script from an external place.
	exit 1
fi

echo "Untaring modules to webroot"
for next in `ls external/drupal/modules/*.tar.gz`;do
	echo "$next"
	tar -xzf $next -C webroot_drupal/sites/all/modules/
done

echo "Untaring themes to webroot"
for next in `ls external/drupal/themes/*.tar.gz`;do
	echo "$next"
	tar -xzf $next -C webroot_drupal/sites/all/themes/
done

#echo "Untaring files ~700MB"
#tar -xzf external/drupal/files/files.tar.gz -C webroot_drupal/sites/default/files/

for next in `ls external/drupal/core/*.tar.gz`;do
	echo "Untaring drupal - $next"
	tar -xzf $next -C external/drupal/core/
done

for next in `ls -d external/drupal/core/*/`;do
	echo "Copy drupal files to webroot"
	cp -R $next* webroot_drupal/
	echo "Remove temp drupal files"
	rm -rf $next
done

mkdir webroot_drupal/sites/all/libraries
for next in `ls external/drupal/CKEditor/*.tar.gz`;do
	echo "Untaring CKEditor - $next"
	tar -xzf $next -C webroot_drupal/sites/all/libraries/
done

cp external/drupal/translations-core/drupal-* webroot_drupal/profiles/standard/translations/
exit 0
