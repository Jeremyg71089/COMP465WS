/*

manyModelsStatic.cpp

465 utility include files:  shader465.hpp, triModel465.hpp  

Shaders:  simpleVertex.glsl and simpleFragment.glsl
  provide flat shading with a fixed light position

C OpenGL Core 3.3 example that loads "nModels" *.tri model files 
and displays them in at static locations with a static view.
Demonstrates use of triModel465.hpp functions for loading *.tri models.

display() updated to comment out unneeded statements like:
glEnableVertexAttribArray( vPosition[m] );

These lines are not needed even when the models are moving.

Mike Barnes
10/23/2014
*/

// define your target operating system: __Windows__  __Linux__  __Mac__
# define __Windows__ 
# include "../includes465/include465.hpp"
#include "Shape3D.hpp"
#include "glm/gtx/rotate_vector.hpp"

const int X = 0, Y = 1, Z = 2, START = 0, STOP = 1;
// constants for models:  file names, vertex count, model display size
const int nModels = 7; // number of models in this scene
const int nCameras = 5; //number of cameras
int currentCamera = 0; //current camera
char * modelFile [nModels] = {"spaceShip-bs100.tri","Ruber.tri","Unum.tri","Duo.tri","Primus.tri","Secundus.tri","obelisk-10-20-10.tri"};
float modelBR[nModels];       // model's bounding radius
float scaleValue[nModels];    // model's scaling "size" value
const int nVertices[nModels] = { 996 * 3 , 264 * 3,264 * 3,264 * 3,264 * 3,264 * 3,14*3 };
char * vertexShaderFile   = "simpleVertex.glsl";     
char * fragmentShaderFile = "simpleFragment.glsl";    
GLuint shaderProgram; 
GLuint VAO[nModels];      // Vertex Array Objects
GLuint buffer[nModels];   // Vertex Buffer Objects
int timerDelay = 40;
glm::mat4 rotation[nModels] = {glm::mat4(),glm::mat4() ,glm::mat4() ,glm::mat4() ,glm::mat4() ,glm::mat4() ,glm::mat4()};
float rotateRadian[nModels] = { 0.0f,0.0f,0.004f,0.002f,0.004f,0.002f,0.0f }; //rotation rates for the orbiting
float currentRadian[nModels] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f }; //the current radians meant for rotating

// Shader handles, matrices, etc
GLuint MVP ;  // Model View Projection matrix's handle
GLuint vPosition[nModels], vColor[nModels], vNormal[nModels];   // vPosition, vColor, vNormal handles for models
// model, view, projection matrices and values to create modelMatrix.
float modelSize[nModels] = { 100.0f,2000.0f,200.0f,400.0f,100.0f,150.0f ,25.0f};   // size of model
glm::vec3 scale[nModels];       // set in init()
glm::vec3 translate[nModels] = {glm::vec3(5000.0f,1000.0f,5000.0f), glm::vec3(0, 0, 0), glm::vec3(4000, 0, 0) , glm::vec3(9000, 0, 0), glm::vec3(8100, 0, 0), glm::vec3(7250, 0, 0),glm::vec3(4900,1000,4850) }; //initial positions of models
glm::mat4 modelMatrix;          // set in display()
glm::mat4 viewMatrix;           // set in init()
glm::mat4 projectionMatrix;     // set in reshape()
glm::mat4 ModelViewProjectionMatrix; // set in display();
glm::vec3 unumPos = translate[2];
glm::vec3 duoPos = translate[3];
//array of look at matrices in order to make transitioning from easy
glm::mat4 camera[nCameras] = { glm::lookAt(glm::vec3(0.0f, 10000.0f, 20000.0f),glm::vec3(0), glm::vec3(0.0f, 1.0f, 0.0f)),
glm::lookAt(glm::vec3(0.0f, 20000.0f, 0.0f),glm::vec3(0),glm::vec3(0.0f, 0.0f, -1.0f)),
glm::lookAt(glm::vec3(5000.0f, 1300.0f, 6000.0f), glm::vec3(5000.0f,1300.0f,5000.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
glm::lookAt(glm::vec3(4000.0f, 0.0f, -8000.0f), unumPos, glm::vec3(0.0f, 1.0f, 0.0f)),
glm::lookAt(glm::vec3(9000.0f, 0.0f, -8000.0f), duoPos, glm::vec3(0.0f, 1.0f, 0.0f)) };

// window title string
char baseStr [50]= "465 Warbird Simulation: ";
char viewStr[20] = "Front View";
char titleStr[50];
void reshape(int width, int height) {
  float aspectRatio = (float) width / (float) height;
  float FOVY = glm::radians(60.0f);
  glViewport(0, 0, width, height);
  printf("reshape: FOVY = %5.2f, width = %4d height = %4d aspect = %5.2f \n", 
    FOVY, width, height, aspectRatio);
  projectionMatrix = glm::perspective(FOVY, aspectRatio, 1.0f, 100000.0f); 
  }
//update the title text
void updateTitle() {
	strcpy(titleStr, baseStr);
	strcat(titleStr, viewStr);
	glutSetWindowTitle(titleStr);
}
//pressing v adds one to the index 
//pressing x subtracts from the index

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 033: case 'q':  case 'Q': exit(EXIT_SUCCESS); break;
		case 'v': case 'V':  // front view
			currentCamera++;
			break;
			
		case 'x': case 'X':  // bottom view
			currentCamera--;
			break;
		

		}
	//if currentCamera index is less than 0 set it to the last index
	//if larger than last index go to zero
	if (currentCamera < 0) {
		currentCamera = nCameras - 1;
	}
	else if (currentCamera >= nCameras) {
		currentCamera = 0;
	}
	if (currentCamera == 0)
	{
		strcpy(viewStr, "Front View");
	}
	else if (currentCamera == 1)
	{
		strcpy(viewStr, "Top View");
	}
	else if (currentCamera == 2)
	{
		strcpy(viewStr, "Ship View");
	}
	else if (currentCamera == 3)
	{
		strcpy(viewStr, "Unum View");
	}
	else if (currentCamera == 4)
	{
		strcpy(viewStr, "Duo View");
	}
	
	
	updateTitle();
	glutPostRedisplay();
	
	}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // update model matrix
  for(int m = 0; m < nModels; m++) {
	  //for the moons to rotate around Duo equation is different than orbiting around the y axis
	  if(m == 4 || m == 5 ){
		  glm::vec3 temp = glm::rotate(translate[3],currentRadian[3], glm::vec3(0, 1, 0));
		  modelMatrix = glm::translate(glm::mat4(), temp) *rotation[m] * glm::translate(glm::mat4(), translate[m]) *
			  glm::scale(glm::mat4(), glm::vec3(scale[m]));
		  
	  }
	 //the regualar equation for rotating around the y axia
	 else {

		 modelMatrix = rotation[m] * glm::translate(glm::mat4(), translate[m]) *
			 glm::scale(glm::mat4(), glm::vec3(scale[m]));

	 }
	 //set the view matrix here so cameras can be dynamic
	 viewMatrix = camera[currentCamera];
    // glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr( modelMatrix)); 
    ModelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix; 
    glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
	glBindVertexArray(VAO[m]);
	/*  The following 3 lines are not needed !
    glEnableVertexAttribArray( vPosition[m] );
    glEnableVertexAttribArray( vColor[m] );
    glEnableVertexAttribArray( vNormal[m] );
	*/
    glDrawArrays(GL_TRIANGLES, 0, nVertices[m] ); 
    }
  glutSwapBuffers();
  }

// load the shader programs, vertex data from model files, create the solids, set initial view
void init() {
  // load the shader programs
  shaderProgram = loadShaders(vertexShaderFile,fragmentShaderFile);
  glUseProgram(shaderProgram);
  
  // generate VAOs and VBOs
  glGenVertexArrays( nModels, VAO );
  glGenBuffers( nModels, buffer );
 
  // load the buffers from the model files
  for (int i = 0; i < nModels; i++) {

    modelBR[i] = loadModelBuffer(modelFile[i], nVertices[i], VAO[i], buffer[i], shaderProgram,
      vPosition[i], vColor[i], vNormal[i], "vPosition", "vColor", "vNormal"); 
    // set scale for models given bounding radius  
    scale[i] = glm::vec3( modelSize[i] * 1.0f/modelBR[i]);
    }
  
  MVP = glGetUniformLocation(shaderProgram, "ModelViewProjection");

 /*
  printf("Shader program variable locations:\n");
  printf("  vPosition = %d  vColor = %d  vNormal = %d MVP = %d\n",
    glGetAttribLocation( shaderProgram, "vPosition" ),
    glGetAttribLocation( shaderProgram, "vColor" ),
    glGetAttribLocation( shaderProgram, "vNormal" ), MVP);
 */

  viewMatrix = camera[0];
  // set render state values
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
  }

void update(void) {
	//creating rotation matrices for each model
	for (int m = 0; m < nModels; m++) {
		currentRadian[m] += rotateRadian[m];
		if (currentRadian[m] >  2 * PI) currentRadian[m] = 0.0f;
		rotation[m] = glm::rotate(glm::mat4(), currentRadian[m], glm::vec3(0, 1, 0));
	}
	glm::vec3 temp2 = glm::rotate(unumPos, currentRadian[2], glm::vec3(0, 1, 0));
	glm::vec3 temp = glm::rotate(glm::vec3(4000.0f, 0.0f, -8000.0f), currentRadian[2], glm::vec3(0, 1, 0));
	camera[3] = glm::lookAt(temp, temp2, glm::vec3(0.0f, 1.0f, 0.0f));
	temp2 = glm::rotate(duoPos, currentRadian[3], glm::vec3(0, 1, 0));
	temp = glm::rotate(glm::vec3(9000.0f, 0.0f, -8000.0f), currentRadian[3], glm::vec3(0, 1, 0));
	camera[4] = glm::lookAt(temp, temp2, glm::vec3(0.0f, 1.0f, 0.0f));
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
	// set OpenGL and GLSL versions to 3.3 for Comp 465/L, comment to see highest versions
# ifndef __Mac__
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
# endif
	strcpy(titleStr, baseStr);
	strcat(titleStr, viewStr);
  glutCreateWindow(titleStr);
  // initialize and verify glew
  glewExperimental = GL_TRUE;  // needed my home system 
  GLenum err = glewInit();  
  if (GLEW_OK != err) 
      printf("GLEW Error: %s \n", glewGetErrorString(err));      
    else {
      printf("Using GLEW %s \n", glewGetString(GLEW_VERSION));
      printf("OpenGL %s, GLSL %s\n", 
        glGetString(GL_VERSION),
        glGetString(GL_SHADING_LANGUAGE_VERSION));
      }
  // initialize scene
  init();
  // set glut callback functions
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(update);
  glutMainLoop();
  printf("done\n");
  return 0;
  }
  

