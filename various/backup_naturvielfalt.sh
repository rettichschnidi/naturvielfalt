#!/bin/sh
EXITVALUE=0
TABLES=`psql -U drupal -h localhost -d naturvielfalt -t -c "SELECT table_name FROM information_schema.tables WHERE table_schema='public';"`;
BASEDIR="/home/naturvielfalt"
OUTPUTDIR="`date +$BASEDIR/naturvielfalt_\%Y_\%m_\%d_\%H_\%M_\%S`/"
mkdir "$OUTPUTDIR"
if [ ! !? ]; then
	echo Could not create directory "$OUTPUTDIR"
	exit 1
fi

for TABLE in $TABLES; do
	OUTPUTSCHEMA="$OUTPUTDIR/dbdump_naturvielfalt_$TABLE-schema.sql"
	pg_dump -U drupal --column-inserts -h localhost --schema-only -f $OUTPUTSCHEMA -t $TABLE naturvielfalt
	if [ ! $? ]; then
		echo Failure while dumping schema of table $TABLE to $OUTPUTSCHEMA
		EXITVALUE=1
	fi

	OUTPUTDATA="$OUTPUTDIR/dbdump_naturvielfalt_$TABLE-data.sql"
	pg_dump -U drupal --column-inserts -h localhost --data-only -f $OUTPUTDATA -t $TABLE naturvielfalt
	if [ ! $? ]; then
		echo Failure while dumping data of table $TABLE to $OUTPUTDATA
		EXITVALUE=1
	fi
done

OUTPUTFILEDUMP="$OUTPUTDIR/filedump_naturvielfalt.tar"
cd /var/www/naturvielfalt/; tar cf "$OUTPUTFILEDUMP" webroot_drupal/

exit $EXITVALUE
