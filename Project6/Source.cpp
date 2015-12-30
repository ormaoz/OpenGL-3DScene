/*
Assigment 6
Or Maoz 029983111
Aviv Florian 038064556
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "glut.h"
#include "Element.h"
#include "Vector3f.h"
#include "loadImage.h"
#define maxSize 65535
#define maxFaces 131072
#define bufSize 512
GLfloat vertices[maxSize * 3];
GLfloat normals[maxSize * 3];
GLfloat textures[maxSize * 3];
int vIndex, vnIndex, fIndex, eIndex, vtIndex;
FILE *f;
int xx;
int yy;
int ii;
int jj;
int numOfElements;
bool mouseLeft;
bool mouseRight;
bool mouseMid;
bool cameraMode = true;
bool pickingMode = false;
GLint hits;
GLfloat pixels[512 * 512];
float modelMatrix[16];
Vector3f curPosition;
int currentObject;
Element **elementsArray;
GLuint textureId1;
GLuint textureId2;
GLuint textureId3;
GLuint textureId4;

GLubyte *texturePixels;
GLubyte textureHeader[54];
GLubyte textureColorTable[1024];
int textureSize;
GLint textureWidth;
GLint textureHeight;
int rd;
GLfloat zBuffer[512 * 512];

/* Initiate the textures*/
void textureReader() {
	
	Image* texture = loadBMP("tex1.bmp");
	glGenTextures(1, &textureId1);
	glBindTexture(GL_TEXTURE_2D, textureId1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->pixels);

	Image* texture2 = loadBMP("tex2.bmp");
	glGenTextures(2, &textureId2);
	glBindTexture(GL_TEXTURE_2D, textureId2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture2->width, texture2->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture2->pixels);

	Image* texture3 = loadBMP("tex3.bmp");
	glGenTextures(3, &textureId3);
	glBindTexture(GL_TEXTURE_2D, textureId3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture3->width, texture3->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture3->pixels);

	Image* texture4 = loadBMP("tex4.bmp");
	glGenTextures(4, &textureId4);
	glBindTexture(GL_TEXTURE_2D, textureId4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture4->width, texture4->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture4->pixels);

}

void picking(int x, int y) {
	GLfloat pixel[4];
	glReadBuffer(GL_FRONT_AND_BACK);

	// Check the uniqe diffuse alpha value of the element 
	glReadPixels(x, 600 - y, 1, 1, GL_RGBA, GL_FLOAT, &pixel);

	// Get the objects index
	currentObject = (int)floorf(pixel[3] * 100 + 0.5);
	currentObject--;

	if (!(currentObject >= 0 && currentObject < 20)) {
		printf("%s\n", "No element was picked.");
		currentObject = -1;
	}
}

void motion(int x, int y){

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	GLfloat dy = ((GLfloat)(yy - y) / viewport[3]);

	if (pickingMode){
		picking(x, y);
		printf("picking mode\n");
		if (mouseLeft == true){  //Picking mode LeftMouse

			float AngelY = ((xx - x) * 180) / 800;
			float AngelX = ((yy - y) * 180) / 600;


			// makes sure that the an element was picked
			if (currentObject != -1) {
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glTranslatef(-elementsArray[currentObject]->COM.x, -elementsArray[currentObject]->COM.y, -elementsArray[currentObject]->COM.z);
				elementsArray[currentObject]->elementRotation.x += AngelX;
				elementsArray[currentObject]->elementRotation.y += AngelY;
				glTranslatef(elementsArray[currentObject]->COM.x, elementsArray[currentObject]->COM.y, elementsArray[currentObject]->COM.z);
				glPopMatrix();

			}
		}
		else if (mouseRight == true){ //Picking mode RightMouse


			float moveOnZ = ((yy - y) * 100) / 600;

			// makes sure that the an element was picked
			if (currentObject != -1) {
				elementsArray[currentObject]->elementMat[14] += moveOnZ;
				elementsArray[currentObject]->COM.z += moveOnZ;
			}
		}
		else if (mouseMid == true){ //Picking mode MiddleMouse

			// calculates the mouse movements
			float xMove = (x - xx) / 25;
			float yMove = (yy - y) / 25;

			curPosition.x += xMove;
			curPosition.y += yMove;

			// makes sure that the an element was picked
			if (currentObject != -1) {
				elementsArray[currentObject]->elementMat[12] += xMove;
				elementsArray[currentObject]->COM.x += xMove;
				elementsArray[currentObject]->elementMat[13] += yMove;
				elementsArray[currentObject]->COM.y += yMove;
			}
		}
	}
	else {
		if (cameraMode){ //Camera mode
			printf("camera mode\n");
			if (mouseLeft == true){ //Camera mode LeftMouse

				GLint theViewPort[4];
				GLfloat theProjMat[16];

				glGetIntegerv(GL_VIEWPORT, theViewPort);
				glMatrixMode(GL_PROJECTION);

				glTranslatef(curPosition.x, curPosition.y, curPosition.z);

				glGetFloatv(GL_PROJECTION_MATRIX, theProjMat);
				glRotatef((x - xx) / (theViewPort[2] / 180.0), 0, 1, 0);
				glGetFloatv(GL_PROJECTION_MATRIX, theProjMat);
				glRotatef((y - yy) / (theViewPort[3] / 180.0), theProjMat[0], theProjMat[4], theProjMat[8]);

				//moving the scene back
				glTranslatef(-curPosition.x, -curPosition.y, -curPosition.z);
				xx = x;
				yy = y;
				glMatrixMode(GL_MODELVIEW);

			}
			else if (mouseRight == true){ //Camera mode RightMouse


				GLfloat theProjMat[16];
				glGetFloatv(GL_PROJECTION_MATRIX, theProjMat);

				glMatrixMode(GL_PROJECTION);


				glTranslatef((y - yy) * (theProjMat[2] / 5.0), (y - yy) * (theProjMat[6] / 5.0), (y - yy) * (theProjMat[10] / 5.0));
				curPosition -= Vector3f((y - yy) * (theProjMat[2] / 5.0), (y - yy) * (theProjMat[6] / 5.0), (y - yy) * (theProjMat[10] / 5.0));

				// Update x, y values
				xx = x;
				yy = y;
			}
			else if (mouseMid == true){ //Camera mode MiddleMouse

				GLfloat theProjMat[16];
				glGetFloatv(GL_PROJECTION_MATRIX, theProjMat);
				glMatrixMode(GL_MODELVIEW);
				glTranslatef((xx - x) * (theProjMat[0] / 5.0), (xx - x) * (theProjMat[4] / 5.0), (xx - x) * (theProjMat[8] / 5.0));
				glTranslatef((y - yy) * (theProjMat[1] / 5.0), (y - yy) * (theProjMat[5] / 5.0), (y - yy) * (theProjMat[9] / 5.0));

				xx = x;
				yy = y;
			}
		}
		else { //Global mode
			printf("global mode\n");
			if (mouseLeft == true){ //Global mode LeftMouse

				glRotatef(((x - xx) / 3.5), 0, 1, 0);
				glRotatef(((y - yy) / 3.5), 1, 0, 0);

				xx = x;
				yy = y;
				glMatrixMode(GL_MODELVIEW);

			}
			else if (mouseRight == true){ //Global mode RightMouse
				glMatrixMode(GL_MODELVIEW);
				glScalef(1 + dy, 1 + dy, 1 + dy);

				xx = x;
				yy = y;

			}
			else if (mouseMid == true){ //Global mode MiddleMouse
				GLfloat theProjMat[16];

				glGetFloatv(GL_PROJECTION_MATRIX, theProjMat);

				glMatrixMode(GL_MODELVIEW);


				glTranslatef((x - xx) * (theProjMat[0] / 5.0), (x - xx) * (theProjMat[4] / 5.0), (x - xx) * (theProjMat[8] / 5.0));
				glTranslatef((yy - y) * (theProjMat[1] / 5.0), (yy - y) * (theProjMat[5] / 5.0), (yy - y) * (theProjMat[9] / 5.0));

				xx = x;
				yy = y;
			}
		}
	}
}

void reader()
{
	Element *currentElement = NULL;
	elementsArray = new Element*[20];

	char c;
	f = fopen("scene1.obj", "r");
	int b = 0;
	int b1 = 0;
	int b2 = 0;
	vIndex = 0;
	vnIndex = 0;
	vtIndex = 0;
	fIndex = 0;
	eIndex = 0;
	int line = 0;
	c = fgetc(f);
	while (c != EOF && vIndex<maxSize * 3 / 2 && vnIndex<maxSize * 3 && fIndex<maxSize * 2 * 3 && vtIndex<maxSize * 3)
	{
		line++;
		switch (c)
		{
		case 'o':
			if (numOfElements == 0) {
				numOfElements++;
				currentElement = new Element(numOfElements);

			}
			else {

				fIndex = 0;
				eIndex = 0;
				if (currentElement != NULL) {
					currentElement->findCOM();
					elementsArray[numOfElements - 1] = currentElement;
				}
				numOfElements++;
				currentElement = new Element(numOfElements);

			}
			break;

		case 'v':
			c = fgetc(f);
			if (currentElement == NULL) {
				numOfElements++;
				currentElement = new Element(numOfElements);
				currentElement->findCOM();
				elementsArray[numOfElements - 1] = currentElement;
			}
			if (c == 'n')
			{
				fscanf(f, " %f %f %f\n", &normals[vnIndex], &normals[vnIndex + 1], &normals[vnIndex + 2]);
				printf(" normals[%d]: %f\tnormals[%d]: %f\tnormals[%d]: %f\n", vnIndex, normals[vnIndex], vnIndex + 1, normals[vnIndex + 1], vnIndex+2, normals[vnIndex + 2]);
				vnIndex += 3;
			}
			else if (c == 't') {
				fscanf(f, " %f %f\n", &textures[vtIndex], &textures[vtIndex + 1]);
				printf(" %f %f\n", textures[vtIndex], textures[vtIndex + 1]);
				vtIndex += 2;

			}
			else
			{
				fscanf(f, "%f %f %f\n", &vertices[vIndex], &vertices[vIndex + 1], &vertices[vIndex + 2]);
				currentElement->vertices[vIndex] = vertices[vIndex];
				currentElement->vertices[vIndex + 1] = vertices[vIndex + 1];
				currentElement->vertices[vIndex + 2] = vertices[vIndex + 2];
				currentElement->numOfVertices++;
				vIndex += 3;
			}
			break;
		case 'f':
			c = fgetc(f);
			b = 2;
			if (currentElement == NULL) {
				numOfElements++;
				currentElement = new Element(numOfElements);
				currentElement->findCOM();
				elementsArray[numOfElements - 1] = currentElement;
			}
			while (c != '\n' && b>0)
			{
				if (currentElement != NULL) {
					b = fscanf(f, "%d/", &currentElement->faces[fIndex]);
					b1 = fscanf(f, "%d/", &currentElement->faces[fIndex + 1]);
					
					// If the obj file has no texture "f XX//XX"
					if (b == 1 && b1 == 0) {
						currentElement->isIncludeTexture = false;
						b1 = fscanf(f, "/", &currentElement->faces[fIndex + 1]);
						b2 = fscanf(f, "%d", &currentElement->faces[fIndex + 1]);
					}
					// If the obj file has texture "f XX/XX/XX"
					else {
						currentElement->isIncludeTexture = true;
						b2 = fscanf(f, "%d", &currentElement->faces[fIndex + 2]);
					}
				}
				if (!currentElement->isIncludeTexture)
				{
					fIndex += 2;
					c = fgetc(f);
					if (currentElement != NULL) {
						currentElement->faceElements[eIndex]++;
					}
				}
				else {
					fIndex += 3;
					c = fgetc(f);
					if (currentElement != NULL) {
						currentElement->faceElements[eIndex]++;
					}
				}
			}
			eIndex++;
			break;
		default:
			if (currentElement == NULL) {
				numOfElements++;
				currentElement = new Element(numOfElements);
				currentElement->findCOM();
				elementsArray[numOfElements - 1] = currentElement;
			}
			while (c != '\n' && c != EOF)
				c = fgetc(f);
			break;
		}
		if (c != EOF)
			c = fgetc(f);
	}
	currentElement->findCOM();
	elementsArray[numOfElements - 1] = currentElement;
	fclose(f);
}

void draw(int elementIndex)
{

	// Making an object 15% lighter when picked
	if (currentObject > -1 && elementIndex == currentObject && pickingMode) {
		float mat_lighter_a[3];
		mat_lighter_a[0] = elementsArray[elementIndex]->mat_a[0] * 1.15;
		mat_lighter_a[1] = elementsArray[elementIndex]->mat_a[1] * 1.15;
		mat_lighter_a[2] = elementsArray[elementIndex]->mat_a[2] * 1.15;

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_lighter_a);
	}
	else {
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, elementsArray[elementIndex]->mat_a);
	}

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, elementsArray[elementIndex]->mat_d);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, elementsArray[elementIndex]->mat_s);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, elementsArray[elementIndex]->low_sh);

	// Makes it so we'll still be able to see OBJ files without VT
	int numOfVars;
	if (elementsArray[elementIndex]->isIncludeTexture) {
		numOfVars = 3;
	}
	else {
		numOfVars = 2;
	}

	glEnable(GL_TEXTURE_2D);

	// Check what is the index of the element and bind the right texture to it
	if (elementIndex == 1) {
		glBindTexture(GL_TEXTURE_2D, textureId1);
	}
	else if (elementIndex == 2) {
		glBindTexture(GL_TEXTURE_2D, textureId2);
	}
	else if (elementIndex == 3) {
		glBindTexture(GL_TEXTURE_2D, textureId3);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, textureId4);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor3f(1.0f, 1.0f, 1.0f);

	for (int i = 0, k = 0; i < maxFaces && k < maxFaces; i += elementsArray[elementIndex]->faceElements[k] * numOfVars, k++) {
		
		glBegin(GL_POLYGON);
		for (int j = 0; j<elementsArray[elementIndex]->faceElements[k] * numOfVars; j += numOfVars)
		{

			glNormal3f(normals[(elementsArray[elementIndex]->faces[i + j + 2] - 1) * 3],
				normals[(elementsArray[elementIndex]->faces[i + j + 2] - 1) * 3 + 1],
				normals[(elementsArray[elementIndex]->faces[i + j + 2] - 1) * 3 + 2]);

			glTexCoord2f(textures[(elementsArray[elementIndex]->faces[i + j + 1] - 1) * 2], 
				textures[(elementsArray[elementIndex]->faces[i + j + 1] - 1) * 2 + 1]);
				
			glVertex3f(vertices[(elementsArray[elementIndex]->faces[i + j] - 1) * 3],
				vertices[(elementsArray[elementIndex]->faces[i + j] - 1) * 3 + 1],
				vertices[(elementsArray[elementIndex]->faces[i + j] - 1) * 3 + 2]);

		}
		glEnd();
		
	}

}

void init()
{
	gluPerspective(60, 1, 2, 200);
	glTranslatef(0, 100, 100);
	curPosition = Vector3f(0, 50, 100);
	glClearColor(0, 0, 0, 1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1); // This light was not requested, but the light Tamir told us to do kept the scene in the dark.

	GLfloat light_direction[] = { 0, -1, -1 };
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 }; //color
	GLfloat light_diffuse[] = { 0.0, 0.5, 0.5, 1.0 }; //color
	GLfloat light_specular[] = { 0.0, 0.0, 0.5, 1.0 };

	// !!!!!!!!!!!!!!!!!!!!!!!! This light is was not requested but the light Tamir told us to do kept the scene in the dark. !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	GLfloat light1_direction[] = { 0, 0, -1 };
	GLfloat light1_ambient[] = { 0.9, 0.9, 0.9, 1.0 }; //color
	GLfloat light1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 }; //color
	GLfloat light1_specular[] = { 0.0, 0.0, 0.5, 1.0 };

	glMatrixMode(GL_MODELVIEW);


	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_direction);

	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_direction);

	reader();
	glLoadIdentity();
}

void display(void)
{

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	for (int i = 0; i < numOfElements; i++) {
		glPushMatrix();

		//moving to the (0,0,0)
		glTranslatef(elementsArray[i]->COM.x, elementsArray[i]->COM.y, elementsArray[i]->COM.z);

		//Rotating according to the mouse move
		glRotatef(elementsArray[i]->elementRotation.x, 1, 0, 0);
		glRotatef(elementsArray[i]->elementRotation.y, 0, 1, 0);

		//moving back to the origin center of the element
		glTranslatef(-elementsArray[i]->COM.x, -elementsArray[i]->COM.y, -elementsArray[i]->COM.z);

		//multiplying all the actions we did on the current object matrix
		glMultMatrixf(elementsArray[i]->elementMat);
		draw(i);
		glPopMatrix();
	}
	glFlush();
}

void reshape(int width, int height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 400.0);
	glTranslatef(0.0, -50, -100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y){
	mouseLeft = false;
	mouseRight = false;
	mouseMid = false;
	xx = x;
	yy = y;

	switch (button){
	case GLUT_LEFT_BUTTON:
		mouseLeft = true;
		break;
	case GLUT_RIGHT_BUTTON:
		mouseRight = true;
		break;
	case GLUT_MIDDLE_BUTTON:
		mouseMid = true;
		break;
	}
	glutMotionFunc(motion);
}

void readKey(unsigned char key, int xmouse, int ymouse)
{

	switch (key)
	{
	case '\b':
		//moving from camera mode to global mode
		cameraMode = !cameraMode;
		break;
	case 244: //moving to picking mode (Some computer just 'p' won't work) 
		pickingMode = !pickingMode;
		break;
	case 'p': //moving to picking mode 
		pickingMode = !pickingMode;
		break;
	}

}

int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(150, 150);
	glutCreateWindow("Object Renderer");
	textureReader();
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(readKey);
	glutIdleFunc(display);
	glutMainLoop();

	return 0;
}