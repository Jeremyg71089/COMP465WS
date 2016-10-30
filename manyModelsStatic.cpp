/*Jeremy Galarza, Scott Weagley 10/09/2016
Warbird Simulation Phase 1
*/

// define your target operating system: __Windows__  __Linux__  __Mac__
#define __Windows__ 
#include "../includes465/include465.hpp"
#include "Shape3D.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "player.hpp"
#include "missile.hpp"

int n = 0, j  = 0;
const int X = 0, Y = 1, Z = 2, START = 0, STOP = 1;
Player *player;
glm::vec3 L;
glm::vec3 T;
glm::vec3 axis;
float angle;
glm::mat4 tempMat;
glm::vec3 tempVec;
float AORdirection;
//Constants for models:  file names, vertex count, model display size
const int nModels = 11, nCameras = 5; //Number of models in this scene & Number of cameras
int currentCamera = 0; //Current camera
char * modelFile[nModels] = { "spaceShip-bs100.tri","Ruber.tri","Unum.tri","Duo.tri","Primus.tri","Secundus.tri","obelisk-10-20-10.tri", "unum-missile-r25.tri", "secundus-missile-r25.tri", "unum-missile-site-r30.tri", "secundus-missile-site-r30.tri" };
float modelBR[nModels], scaleValue[nModels]; //Model's bounding radius & model's scaling "size" value
const int nVertices[nModels] = { 996 * 3 , 264 * 3,264 * 3,264 * 3,264 * 3,264 * 3,14 * 3, 14 * 3, 14 * 3, 12 * 3, 12 * 3 };
char * vertexShaderFile = "simpleVertex.glsl";
char * fragmentShaderFile = "simpleFragment.glsl";
GLuint shaderProgram, VAO[nModels], buffer[nModels]; //Vertex Array Objects & Vertex Buffer Objects
int nextWarp = 2;
int timerDelay = 5, frameCount = 0;
int timerDelayCounter = 1; //Counter for timer delay speed
double currentTime, lastTime, timeInterval;
bool idleTimerFlag = false;  //Interval or idle timer ?
bool gravity = false; //Boolean for Gravity
glm::mat4 identity(1.0f);
glm::mat4 rotation[nModels] = { glm::mat4(),glm::mat4() ,glm::mat4() ,glm::mat4() ,glm::mat4() ,glm::mat4() ,glm::mat4(), glm::mat4(), glm::mat4(), glm::mat4(), glm::mat4() };
float rotateRadian[nModels] = { 0.0f,0.0f,0.004f,0.002f,0.004f,0.002f, 0.0f,0.004f, 0.002f, 0.004f, 0.002f }; //rotation rates for the orbiting
float currentRadian[nModels] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; //the current radians meant for rotating
glm::vec3 currPCL[2];
//Shader handles, matrices, etc
GLuint MVP;  //Model View Projection matrix's handle
GLuint vPosition[nModels], vColor[nModels], vNormal[nModels];   //vPosition, vColor, vNormal handles for models

//Model, view, projection matrices and values to create modelMatrix.
float modelSize[nModels] = { 100.0f,2000.0f,200.0f,400.0f,100.0f, 150.0f,80.0f, 80.0f, 80.0f, 30.0f, 30.0f };   // size of model
glm::vec3 scale[nModels];       // set in init() 
glm::vec3 translate[nModels] = { 
glm::vec3(5000.0f,1000.0f,5000.0f), //Spaceship
glm::vec3(0, 0, 0), //Ruber
glm::vec3(4000, 0, 0), //Unum
glm::vec3(9000, 0, 0), //Duo
glm::vec3(8100, 0, 0), //Primus
glm::vec3(7250, 0, 0), //Secundus
glm::vec3(5000.0f,1000.0f,5000.0f), //Missile position should be same as ship position

glm::vec3(4000, 210, 0), //Unum missile 
glm::vec3(7250, 160, 0), //Secundus missile

glm::vec3(4000, 195, 0), //Unum missile site
glm::vec3(7250, 145, 0) //Secundus missile site
}; //initial positions of models

glm::vec3 unumPos = translate[2], duoPos = translate[3]; //Initialize unum and duo positions to original translate from vec3 translate array
glm::vec3 curShipPos = translate[0]; //Initialize ship position to the ship model's translation vector

//glm::mat4 modelMatrix;          // set in display()
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

glm::mat4 modelMatrix[nModels] = {identity, identity,identity,identity,identity,identity,identity,identity,identity,identity,identity };
glm::mat4 lastOMShip;
glm::mat4 lastOMDuo;
glm::mat4 lastOMUnum;

//Window title string variables
char viewStr[6] = "Front";
int warbirdMissileCount = 9, unumMissileCount = 5, secundusMissileCount = 5;
bool warbirdMissileFired = false, unumMissileFired = false, secundusMissileFired = false;
bool unumSiteDestroyed = false, secundusSiteDestroyed = false;
char titleStr[100], fpsStr[5] = { '0' }, timerStr[5] = { '0' };

//Missile variables
const int warbirdMissilesTotal = 9, unumMissilesTotal = 5, secundusMissilesTotal = 5;
int missileUpdates = 0, numUpdates = 0;
int currentWarbirdMissile = 0, currentUnumMissile = 0, currentSecundusMissile = 0;
Missile *warbirdMissiles[warbirdMissilesTotal], *unumMissiles[unumMissilesTotal], *secundusMissiles[secundusMissilesTotal];
glm::mat4 tempMissileRM, tempMissileTM, tempMissileOM;

//Calculate the distance between two points
float getDistance(float x, float y, float z) {
	return glm::sqrt(x*x + y*y + z*z);	
}

void specialKeyEvent(int key, int x, int y)
{
if (key == GLUT_KEY_UP && glutGetModifiers() != GLUT_ACTIVE_CTRL)
	player->setMove(1);
else if (key == GLUT_KEY_DOWN && glutGetModifiers() != GLUT_ACTIVE_CTRL)
	player->setMove(-1);
else if (key == GLUT_KEY_RIGHT && glutGetModifiers() != GLUT_ACTIVE_CTRL)
	player->setYaw(1);
else if (key == GLUT_KEY_LEFT && glutGetModifiers() != GLUT_ACTIVE_CTRL)
	player->setYaw(-1);

if (key == GLUT_KEY_UP && glutGetModifiers() == GLUT_ACTIVE_CTRL)
	player->setPitch(-1);
else if (key == GLUT_KEY_DOWN && glutGetModifiers() == GLUT_ACTIVE_CTRL)
	player->setPitch(1);
else if (key == GLUT_KEY_RIGHT && glutGetModifiers() == GLUT_ACTIVE_CTRL)
	player->setRoll(1);
else if (key == GLUT_KEY_LEFT && glutGetModifiers() == GLUT_ACTIVE_CTRL)
	player->setRoll(-1);

}

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
	player = new Player(curShipPos, scale[0]);


	//Create the missile objects for warbird
	for (int i = 0; i < warbirdMissilesTotal; i++) {
		warbirdMissiles[i] = new Missile(translate[6], scale[6], false);
	}

	//Create the missile objects for unum missile site
	for (int i = 0; i < unumMissilesTotal; i++) {
		unumMissiles[i] = new Missile(getPosition(modelMatrix[9]), scale[9], true);
	}

	//Create the missile objects for secundus missile site
	for (int i = 0; i < secundusMissilesTotal; i++) {
		secundusMissiles[i] = new Missile(getPosition(modelMatrix[10]), scale[10], true);
	}

	//Start missles with the original position
	modelMatrix[6] = warbirdMissiles[0]->getOM();
	modelMatrix[7] = unumMissiles[0]->getOM();
	modelMatrix[8] = secundusMissiles[0]->getOM();

	viewMatrix = camera[0];
	lastOMShip = player->getOM();
	lastOMUnum = rotation[2] * glm::translate(glm::mat4(), translate[2]) * glm::scale(glm::mat4(), glm::vec3(scale[2]));
	lastOMDuo= rotation[3] * glm::translate(glm::mat4(), translate[3]) * glm::scale(glm::mat4(), glm::vec3(scale[3]));
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
	if (unumSiteDestroyed && secundusSiteDestroyed) {
		sprintf(titleStr, "Cadet passes flight training!");
		//Add code to stop the program
	}
	else if (warbirdMissileCount > 0) {
		sprintf(titleStr, "Warbird %d  Unum %d  Secundus %d  U/S %s  F/S %s  View %s", warbirdMissileCount, unumMissileCount, secundusMissileCount, timerStr, fpsStr, viewStr);
	}
	else {
		sprintf(titleStr, "Cadet resigns from War College!");
		//Add code to stop the movement of the program
	}
	glutSetWindowTitle(titleStr);
}

//Method to display items in window
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Change background to black
	glClearColor(0, 0, 0, 0);

	//Update model matrix
	for (int m = 0; m < nModels; m++) {
		
		//Set the view matrix here so cameras can be dynamic
		viewMatrix = camera[currentCamera];

		//glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr( modelMatrix)); 
		ModelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix[m]; 
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
	player->update();

	//Create rotation matrices for each model
	for (int m = 0; m < nModels; m++) { 
		rotation[m] = glm::rotate(rotation[m], rotateRadian[m], glm::vec3(0, 1, 0));
		
		if (m == 0) {

			modelMatrix[m] = player->getOM(); //Get player's OM matrix and update			
		
		} else if (m == 4 || m == 5) { //for the moons to rotate around Duo equation is different than orbiting around the y axis

			glm::vec3 temp = getPosition(modelMatrix[3]);
			modelMatrix[m] = glm::translate(identity, temp) * rotation[m] * glm::translate(identity, translate[m]) * glm::translate(identity, -1.0f * translate[3]) * glm::scale(glm::mat4(), glm::vec3(scale[m]));
		
		} else if (m == 6) { //Warbird missle object index
			
			//Check if warbird missile fired, if so then keep it updated until it collides or detonates
			if (warbirdMissiles[currentWarbirdMissile]->getFired()) {
			
				//Need to add code here for the missile to find face it's target each time before updating
				//*************************************************************************************
				//*************************************************************************************
				warbirdMissiles[currentWarbirdMissile]->faceTarget(modelMatrix[10]);
				warbirdMissiles[currentWarbirdMissile]->update();
				
				//If missile detonated and there are more missles available, then put the next missile in the ship's position
				if (warbirdMissiles[currentWarbirdMissile]->detonated() && currentWarbirdMissile < warbirdMissilesTotal) {

					//Testing purposes
					printf("Warbird Missile %d detonated\n", currentWarbirdMissile);
					
					warbirdMissiles[currentWarbirdMissile+1]->setRM(player->getRM());
					warbirdMissiles[currentWarbirdMissile+1]->setTM(player->getTM());
					glm::mat4 tempOM = player->getTM() * player->getRM() * warbirdMissiles[currentWarbirdMissile + 1]->getSM();
					warbirdMissiles[currentWarbirdMissile+1]->setOM(tempOM);
					currentWarbirdMissile++;
				}
				
				modelMatrix[6] = warbirdMissiles[currentWarbirdMissile]->getOM();				

			} else if (!warbirdMissiles[currentWarbirdMissile]->getFired()) {

				//Get the forward amount from the player that changed and apply it to the current missile
				warbirdMissiles[currentWarbirdMissile]->setTM(glm::translate(warbirdMissiles[currentWarbirdMissile]->getTM(), player->getForward()));
				warbirdMissiles[currentWarbirdMissile]->setRM(player->getRM());
				modelMatrix[6] = warbirdMissiles[currentWarbirdMissile]->getTM() * warbirdMissiles[currentWarbirdMissile]->getRM() * warbirdMissiles[currentWarbirdMissile]->getSM();
				warbirdMissiles[currentWarbirdMissile]->setOM(modelMatrix[6]);

			} else if (warbirdMissiles[currentWarbirdMissile]->collided()) {

				//Put code here to account for destroyed missile sites
				//If both sites, destroyed then player wins and update title and stop game

			} 

		} else if (m == 7) { //Unum missile object index

			//Activate missile defense sites after 200 updates
			if (numUpdates > 200 || currentUnumMissile < unumMissilesTotal) {
				//Check to see if the spaceship is within detection for the two missile sites
				glm::vec3 target = getPosition(modelMatrix[0]) - getPosition(modelMatrix[9]); //Spaceship pos - Unum missile site position

				//If so then fire a missile at the spaceship from unum's missile base
				if ( (getDistance(target.x, target.y, target.z) - modelSize[9] - modelSize[0]) <= 5000.0f ) {

					//If missile isn't fired yet, then fire it
					if (!unumMissiles[currentUnumMissile]->getFired()) {
						unumMissiles[currentUnumMissile]->fireMissile();
						unumMissiles[currentUnumMissile]->faceTarget(modelMatrix[0]);
						unumMissiles[currentUnumMissile]->update();
						
						//Add code to update missile
						//Call chasetarget
						//Call update

					}
					else { //Otherwise, keep track of it
						//If it's not detonated or collided, then update it **Need to add code for collided with warbird
						if (unumMissiles[currentUnumMissile]->collided()) {
							//put code here to stop the program and to update the window title
						}
						else if (unumMissiles[currentUnumMissile]->detonated()) {
							//Testing
							printf("Unum Missile %d detonated\n", currentUnumMissile);
							unumMissileCount--;
							currentUnumMissile++; //Either it detonated or collided so move to the next missile
						}
					}

					//Testing
					//printf("Spaceship deteted by Unum\n");
				}
			}

			modelMatrix[m] = rotation[m] * glm::translate(glm::mat4(), translate[m]) * glm::scale(glm::mat4(), glm::vec3(scale[m]));

		} else if (m == 8) { //Secundus missile object

			modelMatrix[m] = rotation[m] * glm::translate(glm::mat4(), translate[m]) * glm::scale(glm::mat4(), glm::vec3(scale[m]));

				//Activate missile defense sites after 200 updates
				if (numUpdates > 200 || currentSecundusMissile < secundusMissilesTotal) {

					//If so then fire a missile at the spaceship from secundus' missile base
					glm::vec3 target = getPosition(modelMatrix[0]) - getPosition(modelMatrix[10]); //Spaceship pos - Secundus missile site position
					if ( (getDistance(target.x, target.y, target.z)-modelSize[10]-modelSize[0]) <= 5000.0f ) {

						//If missile isn't fired yet, then fire it
						if (!secundusMissiles[currentSecundusMissile]->getFired()) {
							secundusMissiles[currentSecundusMissile]->fireMissile();
							//Add code to update missile
							//Call chasetarget
							//Call update
						}
						else { //Otherwise, keep track of it
							//If it's not detonated or collided, then update it **Need to add code for collided with warbird
							if (secundusMissiles[currentSecundusMissile]->collided()) {
								//put code here to stop the program and to update the window title
							}
							else if (secundusMissiles[currentSecundusMissile]->detonated()) {
								//Testing
								printf("Secundus Missile %d detonated\n", currentSecundusMissile);
								secundusMissileCount--;
								currentSecundusMissile++; //Either it detonated or collided so move to the next missile
							}
							else {
								secundusMissiles[currentSecundusMissile]->update();
							}
							//Testing
							//printf("Spaceship deteted by Secundus\n");
						}
					}
				}
		} else {//Regular equation for rotating around the y-axis
			modelMatrix[m] = rotation[m] * glm::translate(glm::mat4(), translate[m]) * glm::scale(glm::mat4(), glm::vec3(scale[m]));
		}

	}

	//matrix subtraction: find the difference between the last orientation matrix and the current one of warship
	//then take that difference and multiply it with the camera matrix so the camera follows it 
	camera[2] = camera[2] * (lastOMShip * glm::inverse(player->getOM()));
	lastOMShip = player->getOM();
	//Using rotations to calculate the position and look at of the camera
	camera[3] = camera[3] * (lastOMUnum * glm::inverse(modelMatrix[2]));
	lastOMUnum = modelMatrix[2];
	camera[4] = camera[4] * (lastOMDuo * glm::inverse(modelMatrix[3]));
	lastOMDuo = modelMatrix[3];
	
	numUpdates++; //Keep track of updates
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
	if (timerDelayCounter == 4) {
		timerDelayCounter = 0;
	}

	switch (key) {
	case 033: case 'q':  case 'Q': exit(EXIT_SUCCESS); break;
	case 'f': case 'F': //Launch ship missile
		
		//If in cadet mode, the current fired missile must detonate so check for it
		if (!warbirdMissiles[currentWarbirdMissile]->getFired() && currentWarbirdMissile < warbirdMissilesTotal) {
			 
			warbirdMissiles[currentWarbirdMissile]->fireMissile();
			warbirdMissileCount--;				
		} 
		break;

	case 'g': case 'G': //Toggle gravity
		if (gravity == true) {
			gravity = false;
		}
		else {
			gravity = true;
		}
		break;

	case 's': case 'S': //For next ship speed % nSpeeds
		player->changeSpeed();
		break;

	case 't': case 'T':  //Change animation timer for pilot mode
		switch (timerDelayCounter) {
		case 0: //Ace Mode
			timerDelay = 5;
			sprintf(timerStr, "%4d", 1000 / timerDelay);
			break;
		case 1: //Pilot Mode
			timerDelay = 40;
			sprintf(timerStr, "%4d", 1000 / timerDelay);
			break;
		case 2: //Trainee Mode
			timerDelay = 100;
			sprintf(timerStr, "%4d", 1000 / timerDelay);
			break;
		case 3: //Debug Mode
			timerDelay = 500;			
			sprintf(timerStr, "%4d", 1000 / timerDelay);			
			break;
		}
		glutIdleFunc(display);
		timerDelayCounter++;
		if (idleTimerFlag) idleTimerFlag = false;
		break;

	case 'w': case 'W': //Warp ship % nPlanets
		//Put code here to warp planets
		tempMat = rotation[nextWarp] * glm::translate(identity, translate[nextWarp] + glm::vec3(0.0f, 0.0f, -8000.0f)) * glm::scale(glm::mat4(), glm::vec3(scale[0]));
		tempVec = getPosition(tempMat);
		player->warp(modelMatrix[nextWarp],rotation[nextWarp], tempVec);
		nextWarp++;
		if (nextWarp >= 4) {
			nextWarp = 2;
		}
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
	//glutPostRedisplay();
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

	sprintf(titleStr, "Warbird %d  Unum %d  Secundus %d  U/S %s  F/S %s  View %s", warbirdMissileCount, unumMissileCount, secundusMissileCount, timerStr, fpsStr, viewStr);
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
	glutSpecialFunc(specialKeyEvent);

	
	//I still need to see exactly what we need to update here
	glutTimerFunc(timerDelay, intervalTimer, 1);
	glutIdleFunc(display);
	glutMainLoop();
	printf("done\n");
	return 0;
}





