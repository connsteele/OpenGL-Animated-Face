/*Connor Steele - Program 2 - CSC 474 Spring 2018*/


/*
Utilzed Tyler's base code from Piazza
musicvisualizer base code
by Christian Eckhardt 2018
with some code snippets from Ian Thomas Dunn and Zoe Wood, based on their CPE CSC 471 base code
On Windows, it whould capture "what you here" automatically, as long as you have the Stereo Mix turned on!! (Recording devices -> activate)
*/

// Core libraries
#include <cmath>
//3x below are for files
#include <iostream>
#include <iomanip>
#include <fstream>

// Third party libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <time.h>

// Local headers
#include "GLSL.h"
#include "Program.h"
#include "WindowManager.h"
#include "Shape.h"
#include "Camera.h"
#include "tiny_obj_loader.h"

//timeing stuff
#include <ctime>
#include <ratio>
#include <chrono>

using namespace std;
using namespace glm;
using namespace std::chrono;

std::shared_ptr<Program> phongShader; //changed to global
high_resolution_clock::time_point t1, t2; //use global timers
int charindex; //global charater index
std::vector<char> txtarray;
float interpol = 0.0; //goes from 0 to 1, send to shader as t

double get_last_elapsed_time() {
	static double lasttime = glfwGetTime();
	double actualtime = glfwGetTime();
	double difference = actualtime - lasttime;
	lasttime = actualtime;
	return difference;
}

//
double frametime;
double gametime = 0; //initial is at 0
double facetime = 0;

class Application : public EventCallbacks {
public:
	WindowManager *windowManager = nullptr;
    Camera *camera = nullptr;

    
	//std::shared_ptr<Program> phongShader; //changed to global
	vector<tinyobj::shape_t> shapesAAh, shapesE, shapesFV, shapesLD, shapesO, shapesMBsilent, shapesSTCh, shapesUR, shapesT800; //For Mouth Faces
    vector<tinyobj::material_t> objMaterial1, objMaterial2, objMaterialFV, objMaterialLD, objMaterialO, objMaterialMB, objMaterialSTCH, objMaterialUR, objMaterial9;
	vector<tinyobj::material_t> objMaterialAAh, objMaterialE; //For Mouth Faces
    
    string curItem;
    
    bool mousePressed = false;
    bool mouseCaptured = false;
    glm::vec2 mouseMoveOrigin = glm::vec2(0);
    glm::vec3 mouseMoveInitialCameraRot;
    
	GLuint VertexArrayID;
	GLuint VertexBufferID1, VertexBufferID2, VertexBufferID3, VertexBufferID4, VertexBufferID5, VertexBufferID6, VertexBufferID7, VertexBufferID8, VertexBufferID9;
	GLuint VertexNormsID1, VertexNormsID2, VertexNormsID3, VertexNormsID4, VertexNormsID5, VertexNormsID6, VertexNormsID7, VertexNormsID8, VertexNormsID9;
    GLuint VAO;
    
    Application() {
        camera = new Camera();
    }
    
    ~Application() {
        delete camera;
    }

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
		// Movement
        if (key == GLFW_KEY_W && action != GLFW_REPEAT) camera->vel.z = (action == GLFW_PRESS) * -0.2f;
        if (key == GLFW_KEY_S && action != GLFW_REPEAT) camera->vel.z = (action == GLFW_PRESS) * 0.2f;
        if (key == GLFW_KEY_A && action != GLFW_REPEAT) camera->vel.x = (action == GLFW_PRESS) * -0.2f;
        if (key == GLFW_KEY_D && action != GLFW_REPEAT) camera->vel.x = (action == GLFW_PRESS) * 0.2f;
        // Rotation
        if (key == GLFW_KEY_I && action != GLFW_REPEAT) camera->rotVel.x = (action == GLFW_PRESS) * 0.02f;
        if (key == GLFW_KEY_K && action != GLFW_REPEAT) camera->rotVel.x = (action == GLFW_PRESS) * -0.02f;
        if (key == GLFW_KEY_J && action != GLFW_REPEAT) camera->rotVel.y = (action == GLFW_PRESS) * 0.02f;
        if (key == GLFW_KEY_L && action != GLFW_REPEAT) camera->rotVel.y = (action == GLFW_PRESS) * -0.02f;
        if (key == GLFW_KEY_U && action != GLFW_REPEAT) camera->rotVel.z = (action == GLFW_PRESS) * 0.02f;
        if (key == GLFW_KEY_O && action != GLFW_REPEAT) camera->rotVel.z = (action == GLFW_PRESS) * -0.02f;
        // Disable cursor (allows unlimited scrolling)
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
            mouseCaptured = !mouseCaptured;
            glfwSetInputMode(window, GLFW_CURSOR, mouseCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            resetMouseMoveInitialValues(window);
        }
		//Mine, reset charindex
		if (key == GLFW_KEY_R && action == GLFW_PRESS)
		{
			charindex = 1;
			cout << "\n" << txtarray[0]; ;
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods) {
        mousePressed = (action != GLFW_RELEASE);
        if (action == GLFW_PRESS) {
            resetMouseMoveInitialValues(window);
        }
    }
    
    void mouseMoveCallback(GLFWwindow *window, double xpos, double ypos) {
        if (mousePressed || mouseCaptured) {
            float yAngle = (xpos - mouseMoveOrigin.x) / windowManager->getWidth() * 3.14159f;
            float xAngle = (ypos - mouseMoveOrigin.y) / windowManager->getHeight() * 3.14159f;
            camera->setRotation(mouseMoveInitialCameraRot + glm::vec3(-xAngle, -yAngle, 0));
        }
        //cout << "X: " << xpos / windowManager->getWidth() << "\n";
        //cout << "Y: " << ypos / windowManager->getHeight() << "\n";
    }

	void resizeCallback(GLFWwindow *window, int in_width, int in_height) { }
    
    // Reset mouse move initial position and rotation
    void resetMouseMoveInitialValues(GLFWwindow *window) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        mouseMoveOrigin = glm::vec2(mouseX, mouseY);
        mouseMoveInitialCameraRot = camera->rot;
    }

	void initGeom(const std::string& resourceDirectory) {
		//Get the faces objs for the different sounds
		string errStrAAh;
		bool rcAAh = false;
		rcAAh = tinyobj::LoadObj(shapesAAh, objMaterialAAh, errStrAAh, "../resources/faces/face_AAh.obj");
		if (shapesAAh.size() <= 0) {
			cout << "shapesAAh size < 0";
			return;
		}

		string errStrE;
		bool rcE = false;
		rcE = tinyobj::LoadObj(shapesE, objMaterialE, errStrE, "../resources/faces/face_E.obj");
		if (shapesE.size() <= 0) {
			cout << "shapesE size < 0";
			return;
		}

		string errStrFV;
		bool rc3 = false;
		rc3 = tinyobj::LoadObj(shapesFV, objMaterialFV, errStrFV, "../resources/faces/face_FV.obj");
		if (shapesFV.size() <= 0) {
			cout << "shapesFV size < 0";
			return;
		}

		string errStrLD;
		bool rcLD = false;
		rcLD = tinyobj::LoadObj(shapesLD, objMaterialLD, errStrLD, "../resources/faces/face_LD.obj");
		if (shapesLD.size() <= 0) {
			cout << "shapesLD size < 0";
			return;
		}

		string errStrO;
		bool rcO = false;
		rcO = tinyobj::LoadObj(shapesO, objMaterialO, errStrO, "../resources/faces/face_O.obj");
		if (shapesO.size() <= 0) {
		cout << "shapes5 size < 0";
		return;
		}

		string errStrMBsilent;
		bool rcMB = false;
		rcMB = tinyobj::LoadObj(shapesMBsilent, objMaterialMB, errStrMBsilent, "../resources/faces/face_slientMB.obj");
		if (shapesMBsilent.size() <= 0) {
			cout << "shapesMBsilent size < 0";
			return;
		}

		string errStrSTCH;
		bool rcSTCH = false;
		rcSTCH = tinyobj::LoadObj(shapesSTCh, objMaterialSTCH, errStrSTCH, "../resources/faces/face_STCh.obj");
		if (shapesSTCh.size() <= 0) {
			cout << "shapesSTCh size < 0";
			return;
		}

		string errStrUR;
		bool rcUR = false;
		rcLD = tinyobj::LoadObj(shapesUR, objMaterialUR, errStrUR, "../resources/faces/face_UR.obj");
		if (shapesUR.size() <= 0) {
			cout << "shapesUR size < 0";
			return;
		}

		//Get the terminator model
		/*string errStrT800;
		bool rcT = false;
		rcT = tinyobj::LoadObj(shapesT800, objMaterial9, errStrT800, "../resources/faces/t800default.obj");
		if (shapesT800.size() <= 0) {
			cout << "shapesT800 size < 0";
			return;
		}*/


		//BIND BUFFERS
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

		//Bind faces for mouth sounds
		//A & Ah face
		//Bind AAh Vert Pos
		glGenBuffers(1, &VertexBufferID1);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID1);
		glBufferData(GL_ARRAY_BUFFER, shapesAAh[0].mesh.positions.size() * sizeof(float), &shapesAAh[0].mesh.positions[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//Bind AAh Vert Norms
		glGenBuffers(1, &VertexNormsID1);
		glBindBuffer(GL_ARRAY_BUFFER, VertexNormsID1);
		glBufferData(GL_ARRAY_BUFFER, shapesAAh[0].mesh.normals.size() * sizeof(float), &shapesAAh[0].mesh.normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		////E Face
		glGenBuffers(1, &VertexBufferID2);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID2);
		glBufferData(GL_ARRAY_BUFFER, shapesE[0].mesh.positions.size() * sizeof(float), &shapesE[0].mesh.positions[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &VertexNormsID2);
		glBindBuffer(GL_ARRAY_BUFFER, VertexNormsID2);
		glBufferData(GL_ARRAY_BUFFER, shapesE[0].mesh.normals.size() * sizeof(float), &shapesE[0].mesh.normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//FV Face
		//VBO
		glGenBuffers(1, &VertexBufferID3);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID3);
		glBufferData(GL_ARRAY_BUFFER, shapesFV[0].mesh.positions.size() * sizeof(float), &shapesFV[0].mesh.positions[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(4); //Set up position 4 in the shader
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); //send to position 4 in shader
		//Norms
		glGenBuffers(1, &VertexNormsID3);
		glBindBuffer(GL_ARRAY_BUFFER, VertexNormsID3);
		glBufferData(GL_ARRAY_BUFFER, shapesFV[0].mesh.normals.size() * sizeof(float), &shapesFV[0].mesh.normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); //send to position 5 in shader

		//LD
		glGenBuffers(1, &VertexBufferID4);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID4);
		glBufferData(GL_ARRAY_BUFFER, shapesLD[0].mesh.positions.size() * sizeof(float), &shapesLD[0].mesh.positions[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &VertexNormsID4);
		glBindBuffer(GL_ARRAY_BUFFER, VertexNormsID4);
		glBufferData(GL_ARRAY_BUFFER, shapesLD[0].mesh.normals.size() * sizeof(float), &shapesLD[0].mesh.normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(7); //needs to match 1st arg in func below to pass to shader in this position
		glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//O
		glGenBuffers(1, &VertexBufferID5);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID5);
		glBufferData(GL_ARRAY_BUFFER, shapesO[0].mesh.positions.size() * sizeof(float), &shapesO[0].mesh.positions[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &VertexNormsID5);
		glBindBuffer(GL_ARRAY_BUFFER, VertexNormsID5);
		glBufferData(GL_ARRAY_BUFFER, shapesO[0].mesh.normals.size() * sizeof(float), &shapesO[0].mesh.normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(9);
		glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//MBsilent
		glGenBuffers(1, &VertexBufferID6);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID6);
		glBufferData(GL_ARRAY_BUFFER, shapesMBsilent[0].mesh.positions.size() * sizeof(float), &shapesMBsilent[0].mesh.positions[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(10);
		glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &VertexNormsID6);
		glBindBuffer(GL_ARRAY_BUFFER, VertexNormsID6);
		glBufferData(GL_ARRAY_BUFFER, shapesMBsilent[0].mesh.normals.size() * sizeof(float), &shapesMBsilent[0].mesh.normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(11);
		glVertexAttribPointer(11, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//STCh
		glGenBuffers(1, &VertexBufferID7);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID7);
		glBufferData(GL_ARRAY_BUFFER, shapesSTCh[0].mesh.positions.size() * sizeof(float), &shapesSTCh[0].mesh.positions[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(12);
		glVertexAttribPointer(12, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &VertexNormsID7);
		glBindBuffer(GL_ARRAY_BUFFER, VertexNormsID7);
		glBufferData(GL_ARRAY_BUFFER, shapesSTCh[0].mesh.normals.size() * sizeof(float), &shapesSTCh[0].mesh.normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(13);
		glVertexAttribPointer(13, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//UR
		glGenBuffers(1, &VertexBufferID8);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID8);
		glBufferData(GL_ARRAY_BUFFER, shapesUR[0].mesh.positions.size() * sizeof(float), &shapesUR[0].mesh.positions[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(14);
		glVertexAttribPointer(14, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &VertexNormsID8);
		glBindBuffer(GL_ARRAY_BUFFER, VertexNormsID8);
		glBufferData(GL_ARRAY_BUFFER, shapesUR[0].mesh.normals.size() * sizeof(float), &shapesUR[0].mesh.normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(15);
		glVertexAttribPointer(15, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); 

		//T800 default
		/*glGenBuffers(1, &VertexBufferID9);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID9);
		glBufferData(GL_ARRAY_BUFFER, shapesT800[0].mesh.positions.size() * sizeof(float), &shapesT800[0].mesh.positions[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(16);
		glVertexAttribPointer(16, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &VertexNormsID9);
		glBindBuffer(GL_ARRAY_BUFFER, VertexNormsID9);
		glBufferData(GL_ARRAY_BUFFER, shapesT800[0].mesh.normals.size() * sizeof(float), &shapesT800[0].mesh.normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(17);
		glVertexAttribPointer(17, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);*/

	}
	
	void init(const std::string& resourceDirectory) {
		GLSL::checkVersion();

		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
        
		// Initialize the GLSL programs
        phongShader = std::make_shared<Program>();
        phongShader->setShaderNames(resourceDirectory + "/phong.vert", resourceDirectory + "/phong.frag");
        phongShader->init();
        phongShader->addAttribute("vertPos1");
        phongShader->addAttribute("vertNor1");
        phongShader->addAttribute("vertPos2");
        phongShader->addAttribute("vertNor2");
        phongShader->addAttribute("vertPos3");
        phongShader->addAttribute("vertNor3");
        phongShader->addAttribute("vertPos4");
        phongShader->addAttribute("vertNor4");
        phongShader->addAttribute("vertPos5");
        phongShader->addAttribute("vertNor5");
        phongShader->addAttribute("vertPos6");
        phongShader->addAttribute("vertNor6");
        phongShader->addAttribute("vertPos7");
        phongShader->addAttribute("vertNor7");
        phongShader->addAttribute("vertPos8");
        phongShader->addAttribute("vertNor8");
        phongShader->addAttribute("vertPos9");
        phongShader->addAttribute("vertNor9");
        phongShader->addUniform("t");
		//my uniform for interpolation between faces
		phongShader->addUniform("facet");
		phongShader->addUniform("facetPrev");
	}
    
    glm::mat4 getPerspectiveMatrix() {
        float fov = 3.14159f / 4.0f;
        float aspect = windowManager->getAspect();
        return glm::perspective(fov, aspect, 0.01f, 10000.0f);
    }

	void updateFace() //My function to update the face, called in render(), add a button to reset charindex so you can restart the animation
	{ 
		//FACE KEY: Face AAh is 1, Face E is 2, Face FV is 3, Face LD is 4, Face O is 5, face slient/m/b is 6, face STCh is 7, face UR is 8
		static int count = 0;
		char ch, chprev;
		int ms = 80; //number of miliseconds to read one character, around 80 looks the best, around 40 for human like speed, use 1000 for debug
					 //Eckhardt said that 80 is fine for testing speed.

		//Number that relates to what faces to use
		//int face1 = 6, face2 = 6; //default to silent face, //Maybe get the initial face
		static int face1 = 6, face2 = 6;

		
		t2 = high_resolution_clock::now(); //current time timer
		duration<double, std::milli> time_span = t2 - t1;
		interpol = time_span.count() / ms; //Get the interpolation amount based on the time interval
		if (interpol < 0.02) //stay in range 0.0 to 1.0
		{
			interpol = 0.0; //Stay at the old face 100%
		}
		if (interpol > 1.0) //stay in range 0.0 to 1.0
		{
			interpol = 1.0; //Go to the next face 100%
		}
		glUniform1f(phongShader->getUniform("t"), interpol); //send the interpolation amount to the shader
		//cout << "interpolation is: " << interpol << "\n"; //print interpolation float to console 

		
		if (count == 0) 
		{
			cout << txtarray[0]; //print off the first letter of the input string only once
		}
		count += 1;
		//cout << "face1 is: " << face1 << " face2 is :" << face2 << "\n" ;
		if ((charindex < txtarray.size() - 1) && (time_span.count() >= ms)) //count() give time in milliseconds, so read a char off every 100ms
		{
			t1 = high_resolution_clock::now(); //update t1 to move to the next animation interval

			ch = txtarray.at(charindex); //get current char
			chprev = txtarray.at(charindex - 1); //get previous character


			face1 = face2;//Previous iteration current face should be face1
			glUniform1i(phongShader->getUniform("facetPrev"), face1); //send the previous face to the shader

			
			//All letters of the alphabet have been implemented
			if (ch == 'A' || ch == 'a' || ch == 'H' || ch == 'h')
			{
				face2 = 1;
				glUniform1i(phongShader->getUniform("facet"), face2); //send new face to the shader
			}
			else if (ch == 'E' || ch == 'e' || ch == 'I' || ch == 'i')
			{
				face2 = 2;
				glUniform1i(phongShader->getUniform("facet"), face2); //send new face to the shader
			}
			else if (ch == 'F' || ch == 'f' || ch == 'V' || ch == 'v' || ch == 'Z' || ch == 'z')
			{
				face2 = 3;
				glUniform1i(phongShader->getUniform("facet"), face2); //send new face to the shader
			}
			else if (ch == 'L' || ch == 'l' || ch == 'D' || ch == 'd' || ch == 'N' || ch == 'n' || ch == 'Y' || ch == 'y')
			{
				face2 = 4;
				glUniform1i(phongShader->getUniform("facet"), face2); //send new face to the shader
			}
			else if (ch == 'O' || ch == 'o' || ch == 'W' || ch == 'w')
			{
				face2 = 5;
				glUniform1i(phongShader->getUniform("facet"), face2); //send new face to the shader
			}
			else if (ch == ' '|| ch == 'M' || ch == 'm' || ch == 'B' || ch == 'b' || ch == 'P' || ch == 'p')
			{
				face2 = 6;
				glUniform1i(phongShader->getUniform("facet"), face2); //send new face to the shader
			}
			else if (ch == 'S' || ch == 's' || ch == 'T' || ch == 't' || ch == 'C' || ch == 'c' 
					  || ch == 'K' || ch == 'k' || ch == 'Q' || ch == 'q' || ch == 'X' || ch == 'x')
			{
				face2 = 7;
				glUniform1i(phongShader->getUniform("facet"), face2); //send new face to the shader
			}
			else if (ch == 'U' || ch == 'u' || ch == 'R' || ch == 'r' || ch == 'G' || ch == 'g'
					 || ch == 'J' || ch == 'j')
			{
				face2 = 8;
				glUniform1i(phongShader->getUniform("facet"), face2); //send new face to the shader
			}


			cout << txtarray[charindex]; //Output the current letter being read
			//cout << txtarray[charindex - 1] << txtarray[charindex]; //output what the current and prev letter is

			charindex += 1; //move onto the next character in the array
		}
		else if (charindex >= txtarray.size())
		{
			cout << "\nFINISHED READING\n";
		}
	}

	void render() {
		frametime = get_last_elapsed_time();
		gametime += frametime;

		// Clear framebuffer.
		glClearColor(0.3f, 0.7f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks.
		glm::mat4 V, M, P;
        P = getPerspectiveMatrix();
        V = camera->getViewMatrix();
        M = glm::mat4(1);
        
        /**************/
        /* DRAW SHAPE */
        /**************/

		//Draw the face
		M = glm::translate(glm::mat4(1), glm::vec3(0, -1.0, -10));
		phongShader->bind();
		phongShader->setMVP(&M[0][0], &V[0][0], &P[0][0]);
		//shape->draw(phongShader, false);  //Draw the face_AAh.obj w/o going thru verts, DONT USE for final only use to check
		glBindVertexArray(VertexArrayID);
		glDrawArrays(GL_TRIANGLES, 0, shapesAAh[0].mesh.positions.size() / 3); //Go thru the verts and draw them
		updateFace(); //bind the correct face to the shader based on input string
		phongShader->unbind(); //Finish

	}
};

std::vector<char> getTxtFile() //Read the TXT input
{
	ifstream inFile;
	std::vector<char> txtarray; //store a char array of all characters in the file
	std::string resourceDir = "../resources";
	inFile.open(resourceDir + "/textInput.txt");
	if (!inFile) {
		cout << "Unable to open file";
		exit(1); // terminate with error
	}
	//Read off to an array
	char c;
	while (inFile.get(c))
	{
		txtarray.push_back(c);
	}

	inFile.close(); //Close the text file

	return txtarray;
	
}


int main(int argc, char **argv) {


	std::string resourceDir = "../resources";
	if (argc >= 2) {
		resourceDir = argv[1];
	}

	Application *application = new Application();

    // Initialize window.
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1280, 720);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// Initialize scene.
	application->init(resourceDir);
	application->initGeom(resourceDir);



	//My stuff up to while loop
	txtarray = getTxtFile(); //Read all of the text time the file into an global array
	//bool inpRead = FALSE; //FALSE if not done reading, TRUE if done reading
    
	//Timing for my stuff
	t1 = high_resolution_clock::now(); // Get the current time before the loop starts
	charindex = 1; //set initial
	
	// Loop until the user closes the window.
	while (!glfwWindowShouldClose(windowManager->getHandle())) {
		// Update camera position.
		application->camera->update();
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();


	}
	
	// Quit program. Outside main while loop
	windowManager->shutdown();
	return 0;

}
