#!/bin/bash
echo "Untaring modules to webroot"
for next in `ls external/drupal/modules/*.tar.gz`
    do
        echo "$next"
        tar -xzf $next -C webroot_drupal/sites/all/modules/
    done
    
for next in `ls external/drupal/core/*.tar.gz`
	do
		echo "Untaring drupal - $next"
		tar -xzf $next -C external/drupal/core/
	done

for next in `ls -d external/drupal/core/*/`
	do
		echo "Copy drupal files to webroot"
		cp -R $next* webroot_drupal/
		echo "Remove temp drupal files"
		rm -rf $next
	done
exit 0