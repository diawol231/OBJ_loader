#include "Model.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>

#include "GL/glut.h"


// initiate a Model object and a display list
GLuint model_list;		// display list
GLuint theTorus;
Model model_object;		// an instance of Model

// global variables
static int isWire = 1; // wireframe?
float angle = 0.0;    //we rotate or object with angle degrees


struct coordinate {
	float x, y, z;
	coordinate(float a, float b, float c) : x(a), y(b), z(c) {};

};

struct face {
	int facenum; 
	bool four;
	int faces[4];
	face(int facen, int f1, int f2, int f3) : facenum(facen) {  //constructor for triangle
		faces[0] = f1;
		faces[1] = f2;
		faces[2] = f3;
		four = false;
	}
	face(int facen, int f1, int f2, int f3, int f4) : facenum(facen) { //overloaded constructor for quad
		faces[0] = f1;
		faces[1] = f2;
		faces[2] = f3;
		faces[3] = f4;
		four = true;
	}
};


void Model::loadFile(std::string strOfFile) {
	std::vector<std::string*> coord;    //read every single line of the obj file as a string
	std::vector<coordinate*> vertex;
	std::vector<face*> faces;
	std::string fileObjStr;
	std::vector<coordinate*> normals;   //normal vectors for every face
	std::ifstream fileObject(strOfFile); //open the .obj file
	if (!fileObject.is_open())   //if not opened, exit with -1
	{
		std::cout << "Nor oepened" << std::endl;
		return;
	}
	while (!fileObject.eof())
	{
		getline(fileObject, fileObjStr);
		coord.push_back(new std::string(fileObjStr));
	}
	//go through all of the elements of coord, and decide what kind of element is that
	for (int i = 0; i < int(coord.size()); i++)
	{
		if (coord[i]->c_str()[0] == '#')   //if it is a comment (the first character is #)
			continue;   //we don't care about that
		else if (coord[i]->c_str()[0] == 'v' && coord[i]->c_str()[1] == ' ') //if vector
		{
			float tmpx, tmpy, tmpz;
			sscanf(coord[i]->c_str(), "v %f %f %f", &tmpx, &tmpy, &tmpz);   //read in the 3 float coordinate to tmpx,tmpy,tmpz
			vertex.push_back(new coordinate(tmpx, tmpy, tmpz));   //and then add it to the end of our vertex list
		}
		else if (coord[i]->c_str()[0] == 'v' && coord[i]->c_str()[1] == 'n')    //if normal vector
		{
			float tmpx, tmpy, tmpz;   //do the same thing
			sscanf(coord[i]->c_str(), "vn %f %f %f", &tmpx, &tmpy, &tmpz);
			normals.push_back(new coordinate(tmpx, tmpy, tmpz));
		}
		else if (coord[i]->c_str()[0] == 'f') //if face
		{
			int a, b, c, d, e;
			if (count(coord[i]->begin(), coord[i]->end(), ' ') == 3) //if it is a triangle (it has 3 space in it)
			{
				sscanf(coord[i]->c_str(), "f %d//%d %d//%d %d//%d", &a, &b, &c, &b, &d, &b);
				faces.push_back(new face(b, a, c, d)); //read in, and add to the end of the face list
			}
			else {
				sscanf(coord[i]->c_str(), "f %d//%d %d//%d %d//%d %d//%d", &a, &b, &c, &b, &d, &b, &e, &b);
				faces.push_back(new face(b, a, c, d, e));   //do the same, except we call another constructor, and we use different pattern
			}
		}
	}
	//raw
	int num;    //the id for the list
	num = glGenLists(1);  //generate a uniqe
	glNewList(num, GL_COMPILE);  //and create it
	for (int i = 0; i < int(faces.size()); i++)
	{
		if (faces[i]->four)  //if it's a quad draw a quad
		{
			glBegin(GL_QUADS);
			//basically all I do here, is use the facenum (so the number of the face) as an index for the normal, so the 1st normal owe to the first face
			//I subtract 1 because the index start from 0 in C++
			glNormal3f(normals[faces[i]->facenum - 1]->x, normals[faces[i]->facenum - 1]->y, normals[faces[i]->facenum - 1]->z);
			//draw the faces
			glVertex3f(vertex[faces[i]->faces[0] - 1]->x, vertex[faces[i]->faces[0] - 1]->y, vertex[faces[i]->faces[0] - 1]->z);
			glVertex3f(vertex[faces[i]->faces[1] - 1]->x, vertex[faces[i]->faces[1] - 1]->y, vertex[faces[i]->faces[1] - 1]->z);
			glVertex3f(vertex[faces[i]->faces[2] - 1]->x, vertex[faces[i]->faces[2] - 1]->y, vertex[faces[i]->faces[2] - 1]->z);
			glVertex3f(vertex[faces[i]->faces[3] - 1]->x, vertex[faces[i]->faces[3] - 1]->y, vertex[faces[i]->faces[3] - 1]->z);
			glEnd();
		}
		else {
			glBegin(GL_TRIANGLES);
			glNormal3f(normals[faces[i]->facenum - 1]->x, normals[faces[i]->facenum - 1]->y, normals[faces[i]->facenum - 1]->z);
			glVertex3f(vertex[faces[i]->faces[0] - 1]->x, vertex[faces[i]->faces[0] - 1]->y, vertex[faces[i]->faces[0] - 1]->z);
			glVertex3f(vertex[faces[i]->faces[1] - 1]->x, vertex[faces[i]->faces[1] - 1]->y, vertex[faces[i]->faces[1] - 1]->z);
			glVertex3f(vertex[faces[i]->faces[2] - 1]->x, vertex[faces[i]->faces[2] - 1]->y, vertex[faces[i]->faces[2] - 1]->z);
			glEnd();
		}
	}
	glEndList();
	//delete everything to avoid memory leaks
	for (int i = 0; i < int(coord.size()); i++)
		delete coord[i];
	for (int i = 0; i < int(faces.size()); i++)
		delete faces[i];
	for (int i = 0; i < int(normals.size()); i++)
		delete normals[i];
	for (int i = 0; i < int(vertex.size()); i++)
		delete vertex[i];
	//return num;
}

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

void specialKeyInput(int key, int x, int y)
{
	// start of Translating Camera
	if (key == GLUT_KEY_UP) {
		glTranslatef(0.0f, 0.1f, 0.0f);
		glutPostRedisplay();
		std::cout << "Mooving" << std::endl;
	}

	if (key == GLUT_KEY_DOWN) { //GLUT_KEY_UP
		glTranslatef(0.0f, -0.1f, 0.0f);
		glutPostRedisplay();
		std::cout << "Mooving" << std::endl;
	}

	if (key == GLUT_KEY_LEFT) {
		glTranslatef(-0.1f, 0.0f, 0.0f);
		glutPostRedisplay();
		std::cout << "Mooving" << std::endl;
	}
	if (key == GLUT_KEY_RIGHT) {
		glTranslatef(0.1f, 0.0f, 0.0f);
		glutPostRedisplay();
		std::cout << "Mooving" << std::endl;
	}
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'x':
	case 'X':
		glRotatef(10., 1.0, 0.0, 0.0);
		glutPostRedisplay();
		std::cout << "Rotating" << std::endl;
		break;
	case 'y':
	case 'Y':
		glRotatef(10., 0.0, 1.0, 0.0);
		glutPostRedisplay();
		std::cout << "Rotating" << std::endl;
		break;
	case 'z':
	case 'Z':
		glRotatef(10., 0.0, 0.0, 1.0);
		glutPostRedisplay();
		std::cout << "Rotating" << std::endl;
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


int main(int argc, char** argv) {
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
	model_object.loadFile("C:\\Users\\ÄÍÑ\\Desktop\\model_viewer\\Debug\\cube.obj");
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

}