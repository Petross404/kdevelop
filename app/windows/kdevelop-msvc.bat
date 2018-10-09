setlocal EnableDelayedExpansion
setlocal enableextensions

Rem Check for newest VS (2017) too. These variables are defined if the script is run from Developer Command Prompt
echo off
if DEFINED VS150COMNTOOLS (
    SET "base=%VS150COMNTOOLS%"
	SET script="!base!..\..\VC\Auxiliary\Build\vcvarsall.bat" x64
	REM Avoid all the other if - else and go directly to fire up the script.
	goto call_script
) else ( IF NOT DEFINED VS150COMNTOOLS (
			for /f "usebackq tokens=3*" %%a in (`reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7" /s`) do (
			set _vs15_path=%%a %%b
		) ))
		if /i "!_vs15_path:2017=!"=="!_vs15_path!" (
			echo "It seems you haven't installed Visual Studio 2017"
		) else (
			if exist "!_vs15_path!Common7\Tools\VsDevCmd.bat" (
				REM Choosing architecture interactively and fire up.
				echo "Possible architectures are:"
				echo "x86_amd64 or x64"
				echo "Refer to !_vs15_path!VC\Auxiliary\Build\vcvarsall.bat for more"
				set /p "arch= Type an arch and press enter...: "
				call "!_vs15_path!Common7\Tools\VsDevCmd.bat"
				call "!_vs15_path!VC\Auxiliary\Build\vcvarsall.bat" !arch!
			) else (
				echo "Oh no! %~n0%~x0 couldn't locate !_vs15_path!Common7\Tools\VsDevCmd.bat"
			)
		)
) else ( IF DEFINED VS140COMNTOOLS (
    SET "base=%VS140COMNTOOLS%"
) else ( IF DEFINED VS120COMNTOOLS (
    SET "base=%VS120COMNTOOLS%"
) else ( IF DEFINED VS110COMNTOOLS (
    SET "base=%VS110COMNTOOLS%"
) else ( IF DEFINED VS100COMNTOOLS (
    SET "base=%VS100COMNTOOLS%"
) else (
    START CMD /C "The Microsoft Visual C++ compiler was not found on your system, you might not be able to compile programs. && PAUSE"
) ))))

:call_script
echo on
REM Code for versions older than 2017 or if kdevelop-msvc.bat for VS2017 was ran from Developer Command Prompt
if NOT DEFINED _vs15_path (
	if NOT DEFINED VS150COMNTOOLS (
		SET script="!base!\..\..\VC\vcvarsall.bat"
	) 
	CALL !script! 
)

FOR /F "usebackq tokens=3*" %%A IN (`REG QUERY "HKEY_LOCAL_MACHINE\Software\KDE\KDevelop" /v Install_Dir`) DO (
    set appdir=%%A %%B
)

if NOT DEFINED appdir (
    FOR /F "usebackq tokens=3*" %%A IN (`REG QUERY "HKEY_LOCAL_MACHINE\Software\Wow6432Node\KDE\KDevelop" /v Install_Dir`) DO (
        set appdir=%%A %%B
    )
)

START "" "%appdir%\bin\kdevelop.exe"