Connor Steele - Program 2 - CSC 474
_____________________________________________

-Build .sln using the cmake GUI
	- Direct source code to project directory where CMakeLists.txt is
	- Make a build folder in the project directoy, make the binaries build in that folder
	- Configure to build with your version of visual studio
	- click generate in cmake, once that's done you can close cmake
	- open lab.sln from build folder.
	- In visual studio right click on the lab project and set it as the startup project
	- After this the lab should run

-You can change the input to the face by going to ".../resources/textInput.txt" and editing its text.

- Pressing r will reset the talking animation and the output to the console.

-As letters are read off by the face they are printed to the console.

-Used Tyler's Base Code from Piazza.

-Change the ms variable in the updateFace() function to change the rate at which the face reads each 
character

