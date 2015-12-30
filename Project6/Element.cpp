/*
Assigment 6
Or Maoz 029983111
Aviv Florian 038064556
*/
#include "Element.h"

Element::Element(float element_id)
{
	id = element_id;
	numOfVertices = 0;

	mat_a[0] = 0.3;
	mat_a[1] = 0.4;
	mat_a[2] = 0.5;
	mat_a[3] = 0;

	mat_d[0] = 0.0;
	mat_d[1] = 0.6;
	mat_d[2] = 0.7;

	// Since we do not use the alpha of defuse in the display we'll use it as a uniqe parameter for the element for the picking mode
	mat_d[3] = (float)element_id / (float)100;

	mat_s[0] = 0.0;
	mat_s[1] = 0.0;
	mat_s[2] = 0.8;
	mat_s[3] = 0;

	low_sh[0] = 5.0;

	// Initializes the current element matrix
	for (int i = 0; i < 16; i++) {
		elementMat[i] = 0;
	}

	elementMat[0] = 1;
	elementMat[5] = 1;
	elementMat[10] = 1;
	elementMat[15] = 1;

	// The rotation and translation changes 
	elementTranslate = Vector3f(0, 0, 0);
	elementRotation = Vector3f(0, 0, 0);

	for (int i = 0; i < (maxSize * 3 / 2); i++) {
		vertices[i] = 0;
	}

	for (int i = 0; i < (maxFaces * 2 * 3); i++) {
		faces[i] = 0;
	}

	for (int i = 0; i < (maxFaces); i++) {
		faceElements[i] = 0;
	}


}


//finding the center of the element
void Element::findCOM()
{
	Vector3f comCalc = Vector3f(0, 0, 0);

	for (int i = 0; i < maxSize; i = i + 3) {

		comCalc.x += vertices[i];
		comCalc.y += vertices[i + 1];
		comCalc.z += vertices[i + 2];
	}

	COM = Vector3f(comCalc.x / numOfVertices, comCalc.y / numOfVertices, comCalc.z / numOfVertices);
}