/*Jeremy Galarza, Scott Weagley 10/09/2016
Warbird Simulation Phase 1
*/

// define your target operating system: __Windows__  __Linux__  __Mac__
#define __Windows__ 
#include "../includes465/include465.hpp"
#include "Shape3D.hpp"
#include "glm/gtx/rotate_vector.hpp"
//#include "MovableObj3D";

int n = 0, j  = 0;
const int X = 0, Y = 1, Z = 2, START = 0, STOP = 1;

//Constants for models:  file names, vertex count, model display size
const int nModels = 7, nCameras = 5; //Number of models in this scene & Number of cameras
int currentCamera = 0; //Current camera
char * modelFile[nModels] = { "spaceShip-bs100.tri","Ruber.tri","Unum.tri","Duo.tri","Primus.tri","Secundus.tri","obelisk-10-20-10.tri" };
float modelBR[nModels], scaleValue[nModels]; //Model's bounding radius & model's scaling "size" value
const int nVertices[nModels] = { 996 * 3 , 264 * 3,264 * 3,264 * 3,264 * 3,264 * 3,14 * 3 };
char * vertexShaderFile = "simpleVertex.glsl";
char * fragmentShaderFile = "simpleFragment.glsl";
GLuint shaderProgram, VAO[nModels], buffer[nModels]; //Vertex Array Objects & Vertex Buffer Objects

//Fixed timer interval settings (TQ Time Quantum)
int ace = 5, pilot = 40, trainee = 100, debug = 500; //5 ms is 200 U/S, 40 is 

int timerDelay = 5, frameCount = 0;
double currentTime, lastTime, timeInterval;
bool idleTimerFlag = true;  //Interval or idle timer ?

glm::mat4 identity(1.0f);
glm::mat4 rotation[nModels] = { glm::mat4(),glm::mat4() ,glm::mat4() ,glm::mat4() ,glm::mat4() ,glm::mat4() ,glm::mat4() };
float rotateRadian[nModels] = { 0.0f,0.0f,0.004f,0.002f,0.004f,0.002f,0.0f }; //rotation rates for the orbiting
float currentRadian[nModels] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f }; //the current radians meant for rotating

//Shader handles, matrices, etc
GLuint MVP;  //Model View Projection matrix's handle
GLuint vPosition[nModels], vColor[nModels], vNormal[nModels];   //vPosition, vColor, vNormal handles for models

//Model, view, projection matrices and values to create modelMatrix.
float modelSize[nModels] = { 100.0f,2000.0f,200.0f,400.0f,100.0f,150.0f,25.0f };   // size of model
glm::vec3 scale[nModels];       // set in init()
glm::vec3 translate[nModels] = { 
glm::vec3(5000.0f,1000.0f,5000.0f), 
glm::vec3(0, 0, 0), 
glm::vec3(4000, 0, 0), 
glm::vec3(9000, 0, 0), 
glm::vec3(8100, 0, 0), 
glm::vec3(7250, 0, 0),
glm::vec3(4900,1000,4850)
}; //initial positions of models
glm::vec3 unumPos = translate[2], duoPos = translate[3]; //Initialize unum and duo positions to original translate from vec3 translate array
glm::mat4 modelMatrix;          // set in display()
glm::mat4 viewMatrix;           // set in init()
glm::mat4 projectionMatrix;     // set in reshape()
glm::mat4 ModelViewProjectionMatrix; // set in display();


//array of look at matrices in order to make transitioning from easy
glm::mat4 camera[nCameras] = { 
glm::lookAt(glm::vec3(0.0f, 10000.0f, 20000.0f),glm::vec3(0), glm::vec3(0.0f, 1.0f, 0.0f)), //Front Camera
glm::lookAt(glm::vec3(0.0f, 20000.0f, 0.0f),glm::vec3(0),glm::vec3(0.0f, 0.0f, -1.0f)), //Top Camera 
glm::lookAt(glm::vec3(5000.0f, 1300.0f, 6000.0f), glm::vec3(5000.0f, 1300.0f, 5000.0f), glm::vec3(0.0f, 1.0f, 0.0f)), //Ship Camera
glm::lookAt(glm::vec3(4000.0f, 0.0f, -8000.0f), unumPos, glm::vec3(0.0f, 1.0f, 0.0f)), //Unum Camera
glm::lookAt(glm::vec3(9000.0f, 0.0f, -8000.0f), duoPos, glm::vec3(0.0f, 1.0f, 0.0f)) //Duo Camera
};

//Window title string variables
char viewStr[6] = "Front";
int warbirdMissleCount = 0, unumMissleCount = 0, secundusMissleCount = 0;
char titleStr[100], fpsStr[5] = { '0' }, timerStr[5] = { '0' };

//Load the shader programs, vertex data from model files, create the solids, set initial view
void init() {
	//Load the shader programs
	shaderProgram = loadShaders(vertexShaderFile, fragmentShaderFile);
	glUseProgram(shaderProgram);

	//Generate VAOs and VBOs
	glGenVertexArrays(nModels, VAO);
	glGenBuffers(nModels, buffer);

	//Load the buffers from the model files
	for (int i = 0; i < nModels; i++) {
		modelBR[i] = loadModelBuffer(modelFile[i], nVertices[i], VAO[i], buffer[i], shaderProgram,
			vPosition[i], vColor[i], vNormal[i], "vPosition", "vColor", "vNormal");

		//Set scale for models given bounding radius  
		scale[i] = glm::vec3(modelSize[i] * 1.0f / modelBR[i]);
	}

	MVP = glGetUniformLocation(shaderProgram, "ModelViewProjection");
	viewMatrix = camera[0];

	//Set render state values
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

	lastTime = glutGet(GLUT_ELAPSED_TIME);  // get elapsed system time
}

//Method to update objects to new screen size
void reshape(int width, int height) {
	float aspectRatio = (float)width / (float)height, FOVY = glm::radians(60.0f);
	glViewport(0, 0, width, height);
	//printf("reshape: FOVY = %5.2f, width = %4d height = %4d aspect = %5.2f \n", FOVY, width, height, aspectRatio);
	projectionMatrix = glm::perspective(FOVY, aspectRatio, 1.0f, 100000.0f);
}

//Method to update window title text
void updateTitle() {
	sprintf(titleStr, "Warbird %d  Unum %d  Secundus %d  U/S %s  F/S %s  View %s", warbirdMissleCount, unumMissleCount, secundusMissleCount, timerStr, fpsStr, viewStr);
	glutSetWindowTitle(titleStr);
}

//Method to display items in window
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Change background to black
	glClearColor(0, 0, 0, 0);

	//Update model matrix
	for (int m = 0; m < nModels; m++) {
		//for the moons to rotate around Duo equation is different than orbiting around the y axis
		if (m == 4 || m == 5) {
			glm::mat4 temp = rotation[3] * glm::translate(identity, translate[3]) * glm::scale(glm::mat4(), glm::vec3(scale[3]));
			glm::vec3 temppos = getPosition(temp);
			modelMatrix = glm::translate(identity, temppos) * rotation[m] * glm::translate(identity, translate[m]) * glm::translate(identity, -1.0f * translate[3]) * glm::scale(glm::mat4(), glm::vec3(scale[m]));
		}
		//Regular equation for rotating around the y-axis
		else {
			modelMatrix = rotation[m] * glm::translate(glm::mat4(), translate[m]) * glm::scale(glm::mat4(), glm::vec3(scale[m]));
		}

		//Set the view matrix here so cameras can be dynamic
		viewMatrix = camera[currentCamera];

		//glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr( modelMatrix)); 
		ModelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix; //Ask Jeremy
		glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
		glBindVertexArray(VAO[m]);
		glDrawArrays(GL_TRIANGLES, 0, nVertices[m]);
	}
	glutSwapBuffers();
	frameCount++;

	//See if a second has passed to set estimated fps information
	currentTime = glutGet(GLUT_ELAPSED_TIME);  // get elapsed system time
	timeInterval = currentTime - lastTime;

	if (timeInterval >= 1000) {
		sprintf(fpsStr, "%4d", (int)(frameCount / (timeInterval / 1000.0f)));
		if (idleTimerFlag)
			sprintf(timerStr,"%s",fpsStr);
		lastTime = currentTime;
		frameCount = 0;
		updateTitle();
	}
	
}

void update(void) {

	//Create rotation matrices for each model
	for (int m = 0; m < nModels; m++) {
		currentRadian[m] += rotateRadian[m];
		if (currentRadian[m] > 2 * PI) currentRadian[m] = 0.0f;
		rotation[m] = glm::rotate(identity, currentRadian[m], glm::vec3(0, 1, 0));
	}

	//Using rotations to calculate the position and look at of the camera
	glm::vec3 temp2 = glm::rotate(unumPos, currentRadian[2], glm::vec3(0, 1, 0)); 
	glm::vec3 temp = glm::rotate(glm::vec3(4000.0f, 0.0f, -8000.f), currentRadian[2], glm::vec3(0, 1, 0));
	camera[3] = glm::lookAt(temp, temp2, glm::vec3(0.0f, 1.0f, 0.0f));
	temp2 = glm::rotate(duoPos, currentRadian[3], glm::vec3(0, 1, 0));
	temp = glm::rotate(glm::vec3(9000.0f, 0.0f, -8000.0f), currentRadian[3], glm::vec3(0, 1, 0));
	camera[4] = glm::lookAt(temp, temp2, glm::vec3(0.0f, 1.0f, 0.0f));
	glutPostRedisplay();
}

//Estimate FPS, use for fixed interval timer driven animation
void intervalTimer(int i) { 
	glutTimerFunc(timerDelay, intervalTimer, 1);
	if (!idleTimerFlag) update();  // fixed interval timer
}

//pressing v adds one to the index 
//pressing x subtracts from the index
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 033: case 'q':  case 'Q': exit(EXIT_SUCCESS); break;
	case 'a': case 'A':  //Change animation timer for ace mode
		timerDelay = 5;
		glutIdleFunc(display);
		sprintf(timerStr, "%4d", 1000 / timerDelay);
		if(idleTimerFlag) idleTimerFlag = false;
		break;
	case 'd': case 'D':  //Change animation timer for debug mode
		timerDelay = 500;
		glutIdleFunc(display);
		sprintf(timerStr, "%4d", 1000 / timerDelay);
		if(idleTimerFlag) idleTimerFlag = false;
		break;
	case 'p': case 'P':  //Change animation timer for pilot mode
		timerDelay = 40;
		glutIdleFunc(display);
		sprintf(timerStr, "%4d", 1000 / timerDelay);
		if (idleTimerFlag) idleTimerFlag = false;
		break;
	case 't': case 'T':  //Change animation timer for pilot mode
		timerDelay = 100;
		glutIdleFunc(display);
		sprintf(timerStr, "%4d", 1000 / timerDelay);
		if (idleTimerFlag) idleTimerFlag = false;
		break;
	case 'i': case 'I':  //Change animation timer for trainee mode
		glutIdleFunc(update);
		idleTimerFlag = true;
		break;
	case 'v': case 'V':  //Front view
		currentCamera++;
		break;
	case 'x': case 'X':  //Bottom view
		currentCamera--;
		break;
	}

	//If currentCamera index is less than 0 set it to the last index if larger than last index go to zero
	if (currentCamera < 0) {currentCamera = nCameras - 1;}
	else if (currentCamera >= nCameras) {currentCamera = 0;}
		if (currentCamera == 0)	{strcpy(viewStr, "Front");}
		else if (currentCamera == 1){ strcpy(viewStr, "Top");}
		else if (currentCamera == 2){ strcpy(viewStr, "Ship");}
		else if (currentCamera == 3){ strcpy(viewStr, "Unum");}
		else if (currentCamera == 4){ strcpy(viewStr, "Duo");}

	updateTitle();
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);

	# ifdef __Mac__
		// Can't change the version in the GLUT_3_2_CORE_PROFILE
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
	# endif

	# ifndef __Mac__
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	# endif

	glutInitWindowSize(800, 600);

	//Set OpenGL and GLSL versions to 3.3 for Comp 465/L, comment to see highest versions
	# ifndef __Mac__
		glutInitContextVersion(3, 3);
		glutInitContextProfile(GLUT_CORE_PROFILE);
	# endif

	sprintf(titleStr, "Warbird %d  Unum %d  Secundus %d  U/S %s  F/S %s  View %s", warbirdMissleCount, unumMissleCount, secundusMissleCount, timerStr, fpsStr, viewStr);
	glutCreateWindow(titleStr);
	
	//Initialize and verify glew
	glewExperimental = GL_TRUE;  // needed my home system 
	GLenum err = glewInit();

	if (GLEW_OK != err)
		printf("GLEW Error: %s \n", glewGetErrorString(err));
	else {
		printf("Using GLEW %s \n", glewGetString(GLEW_VERSION));
		printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
	}

	//Initialize scene
	init();

	//Set glut callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	
	//I still need to see exactly what we need to update here
	glutIdleFunc(update);
	glutTimerFunc(timerDelay, intervalTimer, 1);
	glutMainLoop();
	printf("done\n");
	return 0;
}

//void specialKeyEvent(int key, int x, int y)
//{
//if (key = GLUT_KEY_UP && glutGetModifiers() != GLUT_ACTIVE_CTRL)
//player->setMove(1);
//else if (key = GLUT_KEY_DOWN && glutGetModifiers() != GLUT_ACTIVE_CTRL)
//player->setMove(-1);
//else if (key = GLUT_KEY_RIGHT && glutGetModifiers() != GLUT_ACTIVE_CTRL)
//player->setYaw(1);
//else if (key = GLUT_KEY_LEFT && glutGetModifiers() != GLUT_ACTIVE_CTRL)
//player->setYaw(-1);

//if (key = GLUT_KEY_UP && glutGetModifiers() == GLUT_ACTIVE_CTRL)
//player->setPitch(1);
//else if (key = GLUT_KEY_DOWN && glutGetModifiers() == GLUT_ACTIVE_CTRL)
//player->setPitch(-1);
//else if (key = GLUT_KEY_RIGHT && glutGetModifiers() == GLUT_ACTIVE_CTRL)
//player->setRoll(1);
//else if (key = GLUT_KEY_LEFT && glutGetModifiers() == GLUT_ACTIVE_CTRL)
//player->setRoll(-1);

//}

