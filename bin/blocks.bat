@echo off
echo Super Advanced Game Launcher©
echo Singleplayer: 		1 
echo Multiplayer Host: 	2 
echo Dedicated server: 	3 
echo Join Multiplayer:	4
set /p poop="What do you want to do? Enter a number:" %=%

IF "%poop%" == "1" (
	start blocks
	exit
)ELSE IF "%poop%" == "2" (
	start blocks host
	exit
)ELSE IF "%poop%" == "3" (
	start blocks dedicated
	exit
)ELSE IF "%poop%" == "4" (
	set /p addr="Enter server address:" %=%
	start blocks join %addr%
	exit
)

