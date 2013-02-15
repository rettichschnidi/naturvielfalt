@ECHO OFF
ECHO Running extract_external_files.bat

@IF NOT "%cd:~-14%"=="\naturvielfalt" (
	ECHO This script must be executed in the "naturvielfalt" folder.
	EXIT /b 1
)


ECHO.
ECHO *** Un-taring modules to webroot ***
@FOR %%f IN (external/drupal/modules/*.tar.gz) DO (
 ECHO %%f
 tar -xzf external/drupal/modules/%%f -C webroot_drupal/sites/all/modules/
)


ECHO.
ECHO *** Un-taring themes to webroot ***
@FOR %%f IN (external/drupal/themes/*.tar.gz) DO (
	ECHO %%f
	tar -xzf external/drupal/themes/%%f -C webroot_drupal/sites/all/themes/
)


ECHO.
ECHO *** Un-taring Core ***
@FOR %%f IN (external/drupal/core/*.tar.gz) DO (
	ECHO Untaring drupal - %%f
	tar -xzf external/drupal/core/%%f -C external/drupal/core/
)


ECHO.
ECHO *** Copy drupal files to webroot ***
REM We need to copy all files under the version drupal folder (e.g. drupal-7.14).
REM Since windows does not support wildchars for folders in the DIR command, and the drupal version could change in the future,$
REM we need the following to dynamically get the folder to the versioned drupal folder.
FOR /f "tokens=*" %%d in ( 
'DIR "external/drupal/core" /A:D /B' 
) do ( 
set drupalVersionedFolder=%%d
) 
XCOPY "external/drupal/core/%drupalVersionedFolder%" "webroot_drupal" /S /Y /Q


ECHO.
ECHO *** Removing temporary drupal folder ***
RMDIR "external/drupal/core/%drupalVersionedFolder%" /S /Q


ECHO.
ECHO *** Un-taring libraries to webroot ***
FOR %%f IN (external/drupal/libraries/*.tar.gz) DO (
	ECHO %%f
	tar -xzf external/drupal/libraries/%%f -C webroot_drupal/sites/all/libraries/
)

ECHO *** Copying translations to webroot ***
XCOPY "external/drupal/translations-core" "webroot_drupal/profiles/standard/translations" /Y /Q

EXIT /B 0
