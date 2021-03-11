////////////////////////////////////////////////////          
// Modelviewer for .OBJ files
//

#include "Model.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>

#include "GL/glut.h"

using namespace std;

// initiate a Model object and a display list
GLuint model_list;		// display list
GLuint theTorus;
Model model_object;		// an instance of Model

// global variables
static int isWire = 1; // wireframe?


void Model::loadFile(int argc, char **argv){
	ifstream fileObject("icaso.obj");		// object of the file being read
	string fileObjectLine;				// string to save the lines of the file that is being read

	model_list = glGenLists(1);				// init one display list
	glNewList(model_list, GL_COMPILE);		// start of display list
	

	if (fileObject.is_open()) {
		while (! fileObject.eof() ){
			getline(fileObject, fileObjectLine);

			if (fileObjectLine.c_str()[0] == 'v'){				// if the line starts with 'v', it's declaring a vertice
				//cout << fileObjectLine << endl;
				float x, y, z;
				fileObjectLine[0] = ' ';						// get rid of 'v'
				sscanf(fileObjectLine.c_str(),"%f %f %f ", &x, &y, &z);		// assign the vertice's values to x,y,z
				model_object.vertex_list.push_back(x);						// save the values into the vector vertex_list of
				model_object.vertex_list.push_back(y);						// object model_object
				model_object.vertex_list.push_back(z);
				cout << "line is " << fileObjectLine << endl;
				cout << model_object.vertex_list.size() << endl;
				continue;													// skip to next iteration
			}
		}

		// NOTE: this will only run once, after all the vertices have been added and right before polygons/faces are added

		// go back to beginning of the file
		fileObject.clear();
		fileObject.seekg(0, ios::beg);

		// read file again and process the lines starting with 's'
		while (! fileObject.eof()){
			getline(fileObject, fileObjectLine);
			
			// scanning for 'f' and adding polygons to display list
			if (fileObjectLine.c_str()[0] == 'f'){				// if the line starts with 'f', it's declaring a face/polygon

				s_list.push_back(fileObjectLine);
				fileObjectLine[0] = ' ';			// get rid of 'f' from the line string

				istringstream iss(fileObjectLine);

				glBegin(GL_POLYGON);				// start of polygon
				cout << "poly is : " << fileObjectLine << endl;
				while(iss){
					int value;
					iss >> value;					// get values one by one
					if (iss.fail()){break;}			// if it fails to get a value, then break out of loop
					glVertex3f(model_object.vertex_list.at(3*(value-1)), model_object.vertex_list.at(3*(value-1) + 1), model_object.vertex_list.at(3*(value-1) + 2));
						// add vertex to the object model_object in vector vertex_list.
				}
				glEnd();							// end of polygon
			}			
		}		
	}
	glEndList();				// end of display list
}


// Drawing (display) routine.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	// wireframe or not?
	if (isWire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glCallList(model_list);
	glFlush();
}


// Initialization routine.
void setup(void) 
{  
   // Set background (or clearing) color.
   glClearColor(0.0, 0.0, 0.0, 0.0); 
}



// OpenGL window reshape routine.
void resize(int w, int h)
{
   // Set viewport size to be entire OpenGL window.
   glViewport(0, 0, (GLsizei)w, (GLsizei)h);
   // Set matrix mode to projection.
   glMatrixMode(GL_PROJECTION);
   // Clear current projection matrix to identity.
   glLoadIdentity();

   // Set matrix mode to modelview.
   glMatrixMode(GL_MODELVIEW);
   // Clear current modelview matrix to identity.
   glLoadIdentity();
   
}


// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
	// start of Translating Camera
	if(key == GLUT_KEY_UP){
		glTranslatef(0.0f, 0.1f, 0.0f);
		glutPostRedisplay();
		cout << "Mooving" << endl;
	} 
   
	if(key == GLUT_KEY_DOWN) { //GLUT_KEY_UP
		glTranslatef(0.0f, -0.1f, 0.0f);
		glutPostRedisplay();
		cout << "Mooving" << endl;
	}

	if(key == GLUT_KEY_LEFT){
		glTranslatef(-0.1f, 0.0f, 0.0f);
		glutPostRedisplay();
		cout << "Mooving" << endl;
	}
	if(key == GLUT_KEY_RIGHT){
		glTranslatef(0.1f, 0.0f, 0.0f);
		glutPostRedisplay();
		cout << "Mooving" << endl;
	}
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 'x':
		case 'X':
			glRotatef(10., 1.0, 0.0, 0.0);
			glutPostRedisplay();
			cout<<"Rotating"<<endl;
			break;
		case 'y':
		case 'Y':
			glRotatef(10., 0.0, 1.0, 0.0);
			glutPostRedisplay();
			cout << "Rotating" << endl;
			break;
		case 'z':
		case 'Z':
			glRotatef(10., 0.0, 0.0, 1.0);
			glutPostRedisplay();
			cout << "Rotating" << endl;
			break;
		case 'i':
		case 'I':
			glLoadIdentity();
			glutPostRedisplay();
			break;
		case '=':
			glScalef(1.1f, 1.1f, 1.1f);
			glutPostRedisplay();
			break;
		case '-':
			glScalef(0.9f, 0.9f, 0.9f);
			glutPostRedisplay();
			break;
		case 'q':
		case 'Q':
			exit(0);
			break;
	}
}



int main(int argc, char **argv) 
{  
   // Initialize GLUT.
   glutInit(&argc, argv);
   // Set display mode as single-buffered and RGB color.
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
   // Set OpenGL window size.
   glutInitWindowSize(500, 500);	
   // Set position of OpenGL window upper-left corner.
   glutInitWindowPosition(100, 100); 
   // Create OpenGL window with title.
   glutCreateWindow("Model.cpp");

   // load the model
   model_object.loadFile(argc, argv);
   
   // Initialize.
   setup(); 
   // Register display routine.
   glutDisplayFunc(display); 
   // Register reshape routine.
   glutReshapeFunc(resize);  
   // Register keyboard routine.
   glutKeyboardFunc(keyboard);
   // Register the callback function for non-ASCII key entry.
   glutSpecialFunc(specialKeyInput);

   // Begin processing.
   glutMainLoop(); 
   

   return 0;  
}