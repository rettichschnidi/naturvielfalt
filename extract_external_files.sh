#!/bin/bash
if [ ! -d webroot_drupal ]; then
	echo Please do not execute this script from an external place.
	exit 1
fi

echo -e "\nUntaring modules to webroot"
for next in `ls external/drupal/modules/*.tar.gz`;do
	echo "$next"
	tar -xzf $next -C webroot_drupal/sites/all/modules/
done

echo -e "\nUntaring themes to webroot"
for next in `ls external/drupal/themes/*.tar.gz`;do
	echo "$next"
	tar -xzf $next -C webroot_drupal/sites/all/themes/
done

#echo -e "\nUntaring files ~700MB"
#tar -xzf external/drupal/files/files.tar.gz -C webroot_drupal/sites/default/files/

for next in `ls external/drupal/core/*.tar.gz`;do
	echo "Untaring drupal - $next"
	tar -xzf $next -C external/drupal/core/
done

echo -e "\nCopy drupal files to webroot"
for next in `ls -d external/drupal/core/*/`;do
	echo "$next"
	cp -R $next* webroot_drupal/
	echo "Remove temp drupal files"
	rm -rf $next
done

echo -e "\nUntaring libraries to webroot"
for next in `ls external/drupal/libraries/*.tar.gz`;do
	echo "$next"
	tar -xzf $next -C webroot_drupal/sites/all/libraries/
done

cp external/drupal/translations-core/drupal-* webroot_drupal/profiles/standard/translations/
exit 0
