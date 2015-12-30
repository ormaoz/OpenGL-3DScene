/*
Assigment 6
Or Maoz 029983111
Aviv Florian 038064556
*/
#include "glut.h"
#include "Vector3f.h"
#include <stdio.h>

#define maxFaces 131072
#define maxSize 65535

class Element
{

public:
	GLfloat vertices[maxSize * 3 / 2];
	GLuint faces[maxFaces * 2 * 3];
	GLuint faceElements[maxFaces];

	GLfloat mat_a[4];
	GLfloat mat_d[4];
	GLfloat mat_s[4];
	GLfloat low_sh[1];

	Vector3f COM;
	Vector3f elementTranslate;
	Vector3f elementRotation;
	GLfloat  elementMat[16];

	bool isIncludeTexture;

	float colorID[3];
	int id;

	int numOfVertices;

	Element(float i_id);
	void findCOM();
	~Element(void);
};