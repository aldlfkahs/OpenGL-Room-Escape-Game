/* include */
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <windows.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include "glm/glm.hpp"
using namespace std;
using namespace glm;


/* define */
#define screenSize  800
#define screenOffx  700
#define screenOffy  100
#define bufferSize  512
#define pickSize    5.0
#define scaleInit   1.0
#define humanHeight 11.0
#define maxDistance 1.0
#define nLine(n)    n*(-25)


/* struct */
struct mouse_t {
	/* button number             */
	/* | 0x0  | 0x1    | 0x2   | */
	/* | left | middle | right | */
	int button[3];
	int x, y;
	int modifier;
};

struct obj_t {
	/* object attribute */
	GLuint ID = 0;
	string fname = "";
	const char* fpath = "";
	bool selection = false;

	/* load object */
	vector<unsigned int> indices;
	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;

	/* generate buffer object */
	GLuint vao = 0;		/* vertex  array  object */
	GLuint vbo = 0;		/* vertex  buffer object */
	GLuint ubo = 0;		/* uv      buffer object */
	GLuint nbo = 0;		/* normal  buffer object */
	GLuint ebo = 0;		/* element buffer object */

	/* attribute pointer */
	GLuint vap = 0;		/* vertex attribute pointer */
	GLuint uap = 1;		/* uv     attribute pointer */
	GLuint nap = 2;		/* normal attribute pointer */
};

/* struct for laodBMP */
#pragma pack(push, 1)
struct BMPHeader {
	short type;
	int size;
	short reserved1;
	short reserved2;
	int offset;
};

struct BMPInfoHeader {
	int size;
	int width;
	int height;
	short planes;
	short bitsPerPixel;
	unsigned compression;
	unsigned imageSize;
	int xPelsPerMeter;
	int yPelsPerMeter;
	int clrUsed;
	int clrImportant;
};

struct uchar4 {
	unsigned char x, y, z, w;
};
#pragma pack(pop)


/* define view vector and mouse */
vec3 eyePosition = { 0.0, humanHeight, 0.0 };
vec3 at = { 0.0, 0.0, -1.0 };
vec3 up = { 0.0, 1.0, 0.0 };
vec3 rightVec = { 1.0, 0.0, 0.0 };
vec3 rot = { 0.0f, 0.0f, 1.0f };
int mousePos[2];
float theta = 0.0, pi = 0.0;

/* object variables */
obj_t* obj_house = new obj_t;
obj_t* obj_door = new obj_t;
obj_t* obj_book = new obj_t;
obj_t* obj_book_with_7 = new obj_t;
obj_t* obj_cushion = new obj_t;
obj_t* obj_frame_with_3 = new obj_t;
obj_t* obj_lamp_with_5 = new obj_t;
obj_t* obj_laptop = new obj_t;
obj_t* obj_pottery = new obj_t;

/* password variables */
int selectionID, pwChar;
int pwCount = 3;
int saveCount = 0;
bool pwSolved = false;
bool pwTrial = false;
string pwBefore;
string pwAnswer = "  7  5  3";
float doorAngle = 0.0;

/* image variables */
/* 0 : password | 1 : placard */
int imageWidth[2], imageHeight[2];
GLuint imageTex[2];
uchar4* dst[2];

/* global variables */
GLuint mouseMode = GLUT_CURSOR_CROSSHAIR;
GLfloat shapeRatio;
bool validSelection = false;
bool moveLock = false;
float depthBuf[bufferSize];


/* openGL common function */
void display(void);
void drawAxis(float w, float l);
void idle();
void keyboard(unsigned char key, int x, int y);
void mouseWheel(int wheel, int direction, int x, int y);
bool movePossible();
void passiveMouseMove(int x, int y);
void reshape(int w, int h);

/* OBJ control function */
void constructOBJ(string fname, obj_t* object, GLuint ID);
void destructOBJ(obj_t* object);
void drawAllObjects(GLenum mode);
void drawOBJ(obj_t* object);
void drawValidObjects(void);
void generateVAO(obj_t* object);
void glConstructOBJs(void);
void glDestructOBJs(void);
void loadOBJ(obj_t* object);

/* selection control function */
void pickObject(int button, int state, int x, int y);
void processHits(GLuint hits, GLuint buffer[]);
bool selectionCheck(void);
void selectionView(vec3 pos, vec3 offset);
void updateDeselection(void);
void updateSelection(GLuint ID);

/* password control function */
string insertPassword(int chiper);
void loadPassword(void);
void printPassword(int x, int y);
void printText(int x, int y, vec3 color, char mode, const char* text);

/* image control function */
void loadAllBMPs(void);
void loadBMP(uchar4** dst, int* width, int* height, const char* name);

/* light function */
void glLighting0(void);
void glLighting1(void);
void glLighting2(void);
void glLighting3(void);
void glLightingOff(void);
void glLightingOpt(bool is_valid);


/* define function */
void drawAllObjects(GLenum mode)
{
	if (mode == GL_SELECT) glLoadName(obj_house->ID);
	glPushMatrix();
	glPushName(obj_house->ID);
	{
		glColor3f(1.0, 1.0, 1.0);
		drawOBJ(obj_house);
	}
	glPopName();
	glPopMatrix();

	if (mode == GL_SELECT) glLoadName(obj_door->ID);
	glPushMatrix();
	glPushName(obj_door->ID);
	{
		if (pwSolved) {
			if (doorAngle > -90)
				doorAngle -= 3.0;

			glTranslatef(33.46, 0, -18.80);
			glRotatef(doorAngle, 0, 1, 0);
			glTranslatef(-33.46, 0, 18.80);
		}

		glColor3f(0.5, 0.5, 0.6);
		drawOBJ(obj_door);
	}
	glPopName();
	glPopMatrix();

	if (mode == GL_SELECT) glLoadName(obj_book->ID);
	glPushMatrix();
	glPushName(obj_book->ID);
	{
		glColor3f(0.8, 0.7, 1.0);
		drawOBJ(obj_book);
	}
	glPopName();
	glPopMatrix();

	if (mode == GL_SELECT) glLoadName(obj_book_with_7->ID);
	glPushMatrix();
	glPushName(obj_book_with_7->ID);
	{
		glColor3f(1.0, 0.4, 0.5);
		drawOBJ(obj_book_with_7);
	}
	glPopName();
	glPopMatrix();

	if (mode == GL_SELECT) glLoadName(obj_cushion->ID);
	glPushMatrix();
	glPushName(obj_cushion->ID);
	{
		glColor3f(0.3, 0.5, 0.6);
		drawOBJ(obj_cushion);
	}
	glPopName();
	glPopMatrix();

	if (mode == GL_SELECT) glLoadName(obj_frame_with_3->ID);
	glPushMatrix();
	glPushName(obj_frame_with_3->ID);
	{
		glColor3f(0.6, 1.0, 0.8);
		drawOBJ(obj_frame_with_3);
	}
	glPopName();
	glPopMatrix();

	if (mode == GL_SELECT) glLoadName(obj_lamp_with_5->ID);
	glPushMatrix();
	glPushName(obj_lamp_with_5->ID);
	{
		glColor3f(1.0, 1.0, 0.6);
		drawOBJ(obj_lamp_with_5);
	}
	glPopName();
	glPopMatrix();

	if (mode == GL_SELECT) glLoadName(obj_laptop->ID);
	glPushMatrix();
	glPushName(obj_laptop->ID);
	{
		glColor3f(0.7, 0.7, 1.0);
		drawOBJ(obj_laptop);
	}
	glPopName();
	glPopMatrix();

	if (mode == GL_SELECT) glLoadName(obj_pottery->ID);
	glPushMatrix();
	glPushName(obj_pottery->ID);
	{
		glColor3f(0.6, 0.6, 0.6);
		drawOBJ(obj_pottery);
	}
	glPopName();
	glPopMatrix();
}

void drawValidObjects(void)
{
	/* set attribute */
	mouseMode = GLUT_CURSOR_NONE;
	vec3 targetPosition = vec3(eyePosition.x + 10 * at.x, eyePosition.y + 10 * at.y, eyePosition.z + 10 * at.z);

	/* draw only valid object as drawAllObjects() */
	if (obj_door->selection) {
		glPushMatrix();
		{
			glLightingOff();
			glLightingOpt(false);
			drawAllObjects(GL_RENDER);
		}
		glPopMatrix();

		/* active password system */
		printPassword(screenSize/2 - 115, screenSize/2 + 150);
	}
	else if (obj_book->selection) {
		glPushMatrix();
		{
			selectionView(targetPosition, vec3(-21.27, -3.74, -8.45));
			glColor3f(0.8, 0.7, 1.0);
			drawOBJ(obj_book);
		}
		glPopMatrix();
	}
	else if (obj_book_with_7->selection) {
		glPushMatrix();
		{
			selectionView(targetPosition, vec3(-2.50, -3.85, 14.93));
			glColor3f(1.0, 0.4, 0.5);
			drawOBJ(obj_book_with_7);
		}
		glPopMatrix();
	}
	else if (obj_cushion->selection) {
		glPushMatrix();
		{
			selectionView(targetPosition, vec3(-30.97, -3.08, -17.35));
			glColor3f(0.3, 0.5, 0.6);
			drawOBJ(obj_cushion);
		}
		glPopMatrix();
	}
	else if (obj_frame_with_3->selection) {
		glPushMatrix();
		{
			selectionView(targetPosition, vec3(-6.67, -10.34, -19.13));
			glColor3f(0.6, 1.0, 0.8);
			drawOBJ(obj_frame_with_3);
		}
		glPopMatrix();
	}
	else if (obj_lamp_with_5->selection) {
		glPushMatrix();
		{
			selectionView(targetPosition, vec3(29.94, -6.57, -16.15));
			glColor3f(1.0, 1.0, 0.6);
			drawOBJ(obj_lamp_with_5);
		}
		glPopMatrix();
	}
	else if (obj_laptop->selection) {
		glPushMatrix();
		{
			selectionView(targetPosition, vec3(28.12, -7.70, 8.82));
			glColor3f(0.7, 0.7, 1.0);
			drawOBJ(obj_laptop);
		}
		glPopMatrix();
	}
	else if (obj_pottery->selection) {
		glPushMatrix();
		{
			selectionView(targetPosition, vec3(26.86, -8.35, 16.11));
			glColor3f(0.6, 0.6, 0.6);
			drawOBJ(obj_pottery);
		}
		glPopMatrix();
	}
}

void display(void)
{
	/* common settings */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glScalef(scaleInit, scaleInit, scaleInit);

	glInitNames();

	glutInitDisplayMode(GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	/* view point */
	gluLookAt(eyePosition.x, eyePosition.y, eyePosition.z,
		      eyePosition.x + at.x, eyePosition.y + at.y, eyePosition.z + at.z,
		      0.0, 1.0, 0.0);

	/* draw all objects */
	if (validSelection == false) {
		glLightingOpt(false);
		drawAllObjects(GL_RENDER);
	}

	/* draw valid selected objects */
	else {
		glLightingOpt(true);
		drawValidObjects();
	}
	glLightingOff();

	/* print manual text */
	if (!selectionCheck()) {
		printText(10, 770 + nLine(0), vec3(1.0, 1.0, 1.0), 'b', "Move: WASD");
		printText(10, 770 + nLine(1), vec3(1.0, 1.0, 1.0), 'b', "Pick: Mouse Left");
		printText(10, 770 + nLine(2), vec3(1.0, 1.0, 1.0), 'b', "Exit: ESC");
	}
	
	else if (selectionCheck()) {
		if (obj_door->selection) {
			printText(10, 770 + nLine(0), vec3(1.0, 1.0, 1.0), 'b', "Number: Keypad");
			printText(10, 770 + nLine(1), vec3(1.0, 1.0, 1.0), 'b', "Reset: R");
			printText(10, 770 + nLine(2), vec3(1.0, 1.0, 1.0), 'b', "Quit: Q");
		}
		else {
			printText(10, 770 + nLine(0), vec3(1.0, 1.0, 1.0), 'b', "Rotate: Mouse Rotate");
			printText(10, 770 + nLine(1), vec3(1.0, 1.0, 1.0), 'b', "Scale: Mouse Wheel");
			printText(10, 770 + nLine(2), vec3(1.0, 1.0, 1.0), 'b', "Quit: Q");
		}
	}

	/* draw placard texture */
	if (pwSolved) {
		glBindTexture(GL_TEXTURE_2D, imageTex[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth[1], imageHeight[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, dst[1]);

		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
			glTexCoord2d(1.0, 0.0); glVertex3d(33.5, humanHeight - 1.0, -30);
			glTexCoord2d(0.0, 0.0); glVertex3d(24.5, humanHeight - 1.0, -30);
			glTexCoord2d(0.0, 1.0); glVertex3d(24.5, humanHeight + 1.0, -30);
			glTexCoord2d(1.0, 1.0); glVertex3d(33.5, humanHeight + 1.0, -30);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

	glFlush();
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(screenSize, screenSize);
	glutInitWindowPosition(screenOffx, screenOffy);
	glutCreateWindow("Team 2 - Final");

	/* initialization */
	glewInit();
	glConstructOBJs();
	loadAllBMPs();
	
	/* register callback function */
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMouseWheelFunc(mouseWheel);
	glutMouseFunc(pickObject);
	glutPassiveMotionFunc(passiveMouseMove);

	/* register callback loop */
	glutMainLoop();

	/* destruct function */
	glDestructOBJs();
	return 0;
}

/* openGL common function */
void drawAxis(float w, float l)
{
	glLineWidth(w);
	glBegin(GL_LINES);
	/* axis +x */
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(l, 0, 0);

	/* axis +y */
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, l, 0);

	/* axis +z */
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, l);
	glEnd();
	glLineWidth(scaleInit);
}

void idle()
{
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	float moveRate = 2.0;
	vec3 temp_at = normalize(vec3(at.x, 0, at.z));
	vec3 temp_right = normalize(vec3(rightVec.x, 0, rightVec.z));

	/* exit program */
	if (key == 27) exit(0);

	/* object deselection */
	else if (key == 'q' || key == 'Q')
		updateDeselection();

	/* moving direction */
	else if (movePossible() && !moveLock) {
		vec3 before_eye = eyePosition;

		if (key == 'w' || key == 'W') {
			eyePosition.x += temp_at.x / moveRate;
			eyePosition.y += temp_at.y / moveRate;
			eyePosition.z += temp_at.z / moveRate;

			if (!movePossible())
				eyePosition = before_eye;
		}
		else if (key == 's' || key == 'S') {
			eyePosition.x -= temp_at.x / moveRate;
			eyePosition.y -= temp_at.y / moveRate;
			eyePosition.z -= temp_at.z / moveRate;

			if (!movePossible())
				eyePosition = before_eye;
		}
		else if (key == 'a' || key == 'A') {
			eyePosition.x -= temp_right.x / moveRate;
			eyePosition.y -= temp_right.y / moveRate;
			eyePosition.z -= temp_right.z / moveRate;

			if (!movePossible())
				eyePosition = before_eye;
		}
		else if (key == 'd' || key == 'D') {
			eyePosition.x += temp_right.x / moveRate;
			eyePosition.y += temp_right.y / moveRate;
			eyePosition.z += temp_right.z / moveRate;

			if (!movePossible())
				eyePosition = before_eye;
		}
	}

	/* password input */
	if (key == 'r' || key == 'R') {
		pwSolved = false;
		pwTrial = false;
		pwCount = 3;
		pwBefore = "";
		doorAngle = 0.0;
	}
	else if (key == '1') { pwChar = 1; pwCount--; saveCount = 1; }
	else if (key == '2') { pwChar = 2; pwCount--; saveCount = 1; }
	else if (key == '3') { pwChar = 3; pwCount--; saveCount = 1; }
	else if (key == '4') { pwChar = 4; pwCount--; saveCount = 1; }
	else if (key == '5') { pwChar = 5; pwCount--; saveCount = 1; }
	else if (key == '6') { pwChar = 6; pwCount--; saveCount = 1; }
	else if (key == '7') { pwChar = 7; pwCount--; saveCount = 1; }
	else if (key == '8') { pwChar = 8; pwCount--; saveCount = 1; }
	else if (key == '9') { pwChar = 9; pwCount--; saveCount = 1; }
	else if (key == '0') { pwChar = 0; pwCount--; saveCount = 1; }

	glutPostRedisplay();
}

void passiveMouseMove(int x, int y)
{
	float mouseMove_x = (float)(mousePos[0] - x);
	float mouseMove_y = (float)(mousePos[1] - y);

	glutSetCursor(mouseMode);
	glutWarpPointer(screenSize / 2, screenSize / 2);
	mousePos[0] = screenSize / 2;
	mousePos[1] = screenSize / 2;

	if (validSelection == false) {

		float rotateRate = 2400.0f;
		float tempAt[3];

		theta += (float)(6.28318 * mouseMove_x / rotateRate);
		if (theta >= 6.28318)		theta -= 6.28318f;
		else if (theta <= -6.28318)	theta += 6.28318f;

		pi += (float)(1.570795 * mouseMove_y / rotateRate);
		if (pi >= 1.570795)			pi = 1.570795f;
		else if (pi <= -1.570795)	pi = -1.570795f;

		rightVec = normalize(cross(at, up));

		at.x = 0;
		at.y = sin(pi);
		at.z = -cos(pi);

		tempAt[0] = cos(theta) * at.x + sin(theta) * at.z;
		tempAt[1] = at[1];
		tempAt[2] = -sin(theta) * at.x + cos(theta) * at.z;

		at.x = tempAt[0];
		at.y = tempAt[1];
		at.z = tempAt[2];
	}
	else {
		float rotateRate = 10.0f;
		rot.x -= (float)(mouseMove_y / rotateRate);
		rot.y -= (float)(mouseMove_x / rotateRate);
	}

	glutPostRedisplay();
}

void mouseWheel(int wheel, int direction, int x, int y)
{
	if (direction > 0)
		rot.z += 0.03f;

	else if (direction < 0) {
		rot.z -= 0.03f;
		if (rot.z < 0.5f) rot.z = 0.5f;
	}

	glutPostRedisplay();
}

bool movePossible()
{
	bool result;
	bool inRoom = (eyePosition.x <  34   && eyePosition.x >  15  && eyePosition.z > -17 && eyePosition.z <  4)  ||
				  (eyePosition.x <  28.3 && eyePosition.x >  23  && eyePosition.z >  4  && eyePosition.z <  18) ||
				  (eyePosition.x <  15   && eyePosition.x > -8.5 && eyePosition.z > -3  && eyePosition.z <  18) ||
				  (eyePosition.x < -1.6  && eyePosition.x > -14  && eyePosition.z > -17 && eyePosition.z < -3)  ||
				  (eyePosition.x < -8.5  && eyePosition.x > -26  && eyePosition.z >  14 && eyePosition.z <  18) ||
				  (eyePosition.x <  15   && eyePosition.x > -1.6 && eyePosition.z > -8  && eyePosition.z < -3);

	if (pwSolved) 
		result = inRoom || (eyePosition.x < 32 && eyePosition.x > 26 && eyePosition.z < -15) || 
				 eyePosition.x > 37 || eyePosition.x < -33 || eyePosition.z > 21 || eyePosition.z < -22;
	else 
		result = inRoom;

	return result;
}

void reshape(int w, int h)
{
	shapeRatio = (GLfloat)w / (GLfloat)h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, shapeRatio, 1.0, 300.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -20.0);
}

/* OBJ control function */
void constructOBJ(string fname, obj_t* object, GLuint ID)
{
	string fpath = "../object/" + fname + ".obj";
	object->fname = fname;
	object->fpath = fpath.c_str();
	object->ID = ID;

	printf("object path : %s\n", object->fpath);
	loadOBJ(object);
	generateVAO(object);
}

void destructOBJ(obj_t* object)
{
	glDisableVertexAttribArray(object->vap);
	glDisableVertexAttribArray(object->uap);
	glDisableVertexAttribArray(object->nap);

	glDeleteBuffers(1, &object->vbo);
	glDeleteBuffers(1, &object->ubo);
	glDeleteBuffers(1, &object->nbo);
	glDeleteBuffers(1, &object->ebo);
	glDeleteVertexArrays(1, &object->vao);

	delete object;
}

void drawOBJ(obj_t* object)
{
	/* bind VAO & EBO */
	glBindVertexArray(object->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->ebo);
	glDrawElements(GL_TRIANGLES, (GLsizei)object->indices.size(), GL_UNSIGNED_INT, (void*)0);

	/* unbind EBO & reset VAO */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glColor3f(1.0, 1.0, 1.0);
}

void generateVAO(obj_t* object)
{
	/* generate VAO */
	glGenVertexArrays(1, &object->vao);
	glBindVertexArray(object->vao);

	/* generate VBO */
	glGenBuffers(1, &object->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	glBufferData(GL_ARRAY_BUFFER, object->vertices.size() * sizeof(vec3), &object->vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* generate UBO */
	glGenBuffers(1, &object->ubo);
	glBindBuffer(GL_ARRAY_BUFFER, object->ubo);
	glBufferData(GL_ARRAY_BUFFER, object->uvs.size() * sizeof(vec2), &object->uvs[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* generate NBO */
	glGenBuffers(1, &object->nbo);
	glBindBuffer(GL_ARRAY_BUFFER, object->nbo);
	glBufferData(GL_ARRAY_BUFFER, object->normals.size() * sizeof(vec3), &object->normals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* generate EBO */
	glGenBuffers(1, &object->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object->indices.size() * sizeof(unsigned int), &object->indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/* enable each attribute array */
	glEnableVertexAttribArray(object->vap);
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	glVertexAttribPointer(object->vap, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(object->uap);
	glBindBuffer(GL_ARRAY_BUFFER, object->ubo);
	glVertexAttribPointer(object->uap, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(object->nap);
	glBindBuffer(GL_ARRAY_BUFFER, object->nbo);
	glVertexAttribPointer(object->nap, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);
}

void glConstructOBJs(void)
{
	/* object update if import new object file */
	constructOBJ("house", obj_house, 1);
	constructOBJ("door", obj_door, 2);
	constructOBJ("book", obj_book, 3);
	constructOBJ("book_with_7", obj_book_with_7, 4);
	constructOBJ("cushion", obj_cushion, 5);
	constructOBJ("frame_with_3", obj_frame_with_3, 6);
	constructOBJ("lamp_with_5", obj_lamp_with_5, 7);
	constructOBJ("laptop", obj_laptop, 8);
	constructOBJ("pottery", obj_pottery, 9);

	printf("Objects load end.\n\n");
}

void glDestructOBJs(void)
{
	/* object update if import new object file */
	destructOBJ(obj_house);
	destructOBJ(obj_door);
	destructOBJ(obj_book);
	destructOBJ(obj_book_with_7);
	destructOBJ(obj_cushion);
	destructOBJ(obj_frame_with_3);
	destructOBJ(obj_lamp_with_5);
	destructOBJ(obj_laptop);
	destructOBJ(obj_pottery);
}

void loadOBJ(obj_t* object)
{
	vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	vector<vec3> temp_vertices;
	vector<vec2> temp_uvs;
	vector<vec3> temp_normals;

	FILE* file = fopen(object->fpath, "r");

	if (file == NULL) {
		printf("Impossible to open the file.\n");
		exit(1);
	}

	while (1) {
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0) {
			vec3 vertex;
			int lineData = fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}

		else if (strcmp(lineHeader, "vt") == 0) {
			vec2 uv;
			int lineData = fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}

		else if (strcmp(lineHeader, "vn") == 0) {
			vec3 normal;
			int lineData = fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}

		else if (strcmp(lineHeader, "f") == 0) {
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
			                     &vertexIndex[0], &uvIndex[0], &normalIndex[0],
				                 &vertexIndex[1], &uvIndex[1], &normalIndex[1],
				                 &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

			if (matches != 9) {
				printf("File can't be read by loader.\nmatches : %d", matches);
				exit(1);
			}

			for (int i = 0; i < 3; i++) {
				vertexIndices.push_back(vertexIndex[i]);
				uvIndices    .push_back(uvIndex[i]);
				normalIndices.push_back(normalIndex[i]);
			}
		}

		else continue;
	}

	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertexIndex  = vertexIndices[i];
		unsigned int uvIndex      = uvIndices[i];
		unsigned int normalIndex  = normalIndices[i];
		unsigned int elementIndex = i;

		vec3 vertex = temp_vertices[vertexIndex - 1];
		vec2 uv     = temp_uvs[uvIndex - 1];
		vec3 normal = temp_normals[normalIndex - 1];

		object->vertices.push_back(vertex);
		object->uvs     .push_back(uv);
		object->normals .push_back(normal);
		object->indices .push_back(elementIndex);
	}

	fclose(file);
}

/* selection control function */
void pickObject(int button, int state, int x, int y)
{
	GLuint selectBuf[bufferSize];
	GLint hits;
	GLint viewport[4];

	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
		return;

	glGetIntegerv(GL_VIEWPORT, viewport);

	glSelectBuffer(bufferSize, selectBuf);
	(void)glRenderMode(GL_SELECT);

	glInitNames();
	glPushName(0);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	{
		glLoadIdentity();
		gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), pickSize, pickSize, viewport);
		gluPerspective(40.0, shapeRatio, 1.0, 300.0);
		drawAllObjects(GL_SELECT);

		glMatrixMode(GL_PROJECTION);
	}
	glPopMatrix();
	glFlush();

	hits = glRenderMode(GL_RENDER);
	processHits(hits, selectBuf);

	glutPostRedisplay();
}

void processHits(GLuint hits, GLuint buffer[])
{
	float zFront, zBack;
	float zMin = maxDistance;
	GLuint objectID, selectionID, hitCounts;
	GLuint* ptr = (GLuint*)buffer;

	/* initialization */
	selectionID = 0;
	for (int i = 0; i < bufferSize; i++)
		depthBuf[i] = maxDistance;

	/* selection detect */
	if (hits != 0)
	{
		for (unsigned int i = 0; i < hits; i++)
		{
			/* save object hit counts */
			hitCounts = *ptr;
			ptr++;

			/* object z-front depth value */
			zFront = (float)*ptr / 0x7fffffff;
			ptr++;

			/* object z-back depth value */
			zBack = (float)*ptr / 0x7fffffff;
			ptr++;

			/* save object ID */
			objectID = *ptr;

			/* skip buffer pointer */
			for (unsigned int j = 0; j < hitCounts; j++)
				ptr++;

			/* save z-front depth value */
			depthBuf[objectID] = zFront - 1.0f;
		}


		for (int i = 0; i < bufferSize; i++)
		{
			if (zMin >= depthBuf[i]) {
				zMin = depthBuf[i];
				selectionID = i;
			}
		}
	}

	/* print selection result */
	printf(" ==================================\n");
	printf(" =\n");
	printf(" =    hits          : %d\n", hits);

	if (hits == 0)
		printf(" =    Object        : None\n");

	else
	{
		printf(" =    Object ID     : %d\n", selectionID);
		updateSelection(selectionID);
		printf(" =    z-front depth : %f\n", depthBuf[selectionID]);
	}

	printf(" =\n");
	printf(" ==================================\n\n");
}

bool selectionCheck(void)
{
	if (obj_door->selection)              return true;
	else if (obj_book->selection)         return true;
	else if (obj_book_with_7->selection)  return true;
	else if (obj_cushion->selection)      return true;
	else if (obj_frame_with_3->selection) return true;
	else if (obj_lamp_with_5->selection)  return true;
	else if (obj_laptop->selection)       return true;
	else if (obj_pottery->selection)      return true;
}

void selectionView(vec3 pos, vec3 offset)
{
	glTranslatef(pos.x, pos.y, pos.z);
	glRotated(rot.y, 0.0f, 1.0f, 0.0f);
	glRotatef(rot.x, 1.0f, 0.0f, 0.0f);
	glScalef(rot.z, rot.z, rot.z);
	glTranslatef(offset.x, offset.y, offset.z);
}

void updateSelection(GLuint ID)
{
	/* selection update with selected object */
	if (ID == obj_house->ID) {
		obj_house->selection = true;
		printf(" =    Object Name   : %s\n", obj_house->fname.c_str());
	}
	else if (ID == obj_door->ID) {
		obj_door->selection = true;
		printf(" =    Object Name   : %s\n", obj_door->fname.c_str());
	}
	else if (ID == obj_book->ID) {
		obj_book->selection = true;
		printf(" =    Object Name   : %s\n", obj_book->fname.c_str());
	}
	else if (ID == obj_book_with_7->ID) {
		obj_book_with_7->selection = true;
		printf(" =    Object Name   : %s\n", obj_book_with_7->fname.c_str());
	}
	else if (ID == obj_cushion->ID) {
		obj_cushion->selection = true;
		printf(" =    Object Name   : %s\n", obj_cushion->fname.c_str());
	}
	else if (ID == obj_frame_with_3->ID) {
		obj_frame_with_3->selection = true;
		printf(" =    Object Name   : %s\n", obj_frame_with_3->fname.c_str());
	}
	else if (ID == obj_lamp_with_5->ID) {
		obj_lamp_with_5->selection = true;
		printf(" =    Object Name   : %s\n", obj_lamp_with_5->fname.c_str());
	}
	else if (ID == obj_laptop->ID) {
		obj_laptop->selection = true;
		printf(" =    Object Name   : %s\n", obj_laptop->fname.c_str());
	}
	else if (ID == obj_pottery->ID) {
		obj_pottery->selection = true;
		printf(" =    Object Name   : %s\n", obj_pottery->fname.c_str());
	}

	/* define valid selected objects */
	if (selectionCheck()) {
		validSelection = true;
		moveLock = true;
	}
}

void updateDeselection(void)
{
	/* reset attribute */
	rot = vec3(0.0, 0.0, 1.0);
	validSelection = false;
	moveLock = false;
	mouseMode = GLUT_CURSOR_CROSSHAIR;

	/* all objects deselection */
	obj_house->selection = false;
	obj_door->selection = false;
	obj_book->selection = false;
	obj_book_with_7->selection = false;
	obj_cushion->selection = false;
	obj_frame_with_3->selection = false;
	obj_lamp_with_5->selection = false;
	obj_laptop->selection = false;
	obj_pottery->selection = false;
}

/* password control function */
string insertPassword(int chiper)
{
	string pwAdd;

	if (chiper == 2)
		pwAdd = pwBefore + "  _  _";

	else if (chiper == 1)
		pwAdd = pwBefore + "  _";

	else if (chiper == 0)
	{
		pwAdd = pwBefore;

		printf(" ==================================\n");
		printf(" =\n");
		printf(" = Input Password :%s\n", pwBefore.c_str());

		if (pwBefore.compare(pwAnswer) == 0)
		{
			printf(" = Password Correct!\n");
			pwSolved = true;
		}
		else
		{
			printf(" = Password Incorret!\n");
			pwCount--;
		}

		printf(" =\n");
		printf(" ==================================\n\n");
	}

	return pwAdd;
}

void loadPassword(void)
{
	vec3 targetPosition = vec3(eyePosition.x + 3.5 * at.x, eyePosition.y + 3.5 * at.y, eyePosition.z + 3.5 * at.z);

	glPushMatrix();
	{
		glTranslatef(targetPosition.x, targetPosition.y, targetPosition.z);
		glRotated(theta * 180.0 / 3.14159, 0.0, 1.0, 0.0);
		glRotated(pi * 180.0 / 3.14159, 1.0, 0.0, 0.0);

		glBindTexture(GL_TEXTURE_2D, imageTex[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth[0], imageHeight[0], 0, GL_RGBA, GL_UNSIGNED_BYTE, dst[0]);

		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
			glTexCoord2d(0.0, 0.0); glVertex2d(-0.5, -0.7);
			glTexCoord2d(1.0, 0.0); glVertex2d( 0.5, -0.7);
			glTexCoord2d(1.0, 1.0); glVertex2d( 0.5,  0.7);
			glTexCoord2d(0.0, 1.0); glVertex2d(-0.5,  0.7);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	glPopMatrix();
}

void printPassword(int x, int y)
{
	char temp[5];
	string pwString;

	/* load password image */
	glPushMatrix();
	glDisable(GL_LIGHTING);
	loadPassword();
	glPopMatrix();

	/* password correct */
	if (pwSolved) {
		if (pwTrial == false) {
			for (int i = 0; i < 5 * pow(10,6); i++);
			pwTrial = true;
		}

		printText(x + 55, y, vec3(1.0, 1.0, 1.0), 's', "Correct !");
		return;
	}

	/* password incorrect */
	else if (pwCount >= 0) {
		pwString = "   Biggest...?";

		if (pwCount < 3) {
			if (saveCount) {
				pwBefore = pwBefore + "  " + itoa(pwChar, temp, 10);
				saveCount = 0;
			}

			pwString = insertPassword(pwCount);
		}

		printText(x, y, vec3(1.0, 1.0, 1.0), 's', pwString.c_str());
	}

	/* password reset */
	else {
		Sleep(300);
		pwCount = 3;
		pwBefore = "";
	}
}

void printText(int x, int y, vec3 color, char mode, const char* text)
{
	const unsigned char* str = (const unsigned char*)text;

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);

	glPushMatrix();
	{
		glLoadIdentity();
		gluOrtho2D(0.0, (double)screenSize, 0.0, (double)screenSize);
		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();
		{
			glLoadIdentity();
			glColor3f(color.x, color.y, color.z);

			/* bitmap mode */
			if (mode == 'b') {
				string s = text;
				glRasterPos2i(x, y);
				glutBitmapString(GLUT_BITMAP_HELVETICA_18, str);
			}

			/* stroke mode */
			else if (mode == 's') {
				glLineWidth(2.0);
				glTranslated(x, y, 0);
				glScaled(0.2, 0.2, 0);
				glutStrokeString(GLUT_STROKE_ROMAN, str);
			}

			glLineWidth(scaleInit);
			glScaled(scaleInit, scaleInit, scaleInit);
			glMatrixMode(GL_PROJECTION);
		}
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
	glPopMatrix();
}

/* image control function */
void loadBMP(uchar4 **dst, int *width, int *height, const char *name)
{
	BMPHeader hdr;
	BMPInfoHeader infoHdr;
	int x, y;

	FILE *fd;


	printf("Loading %s...\n", name);
	if(sizeof(uchar4) != 4){
		printf("***Bad uchar4 size***\n");
		exit(0);
	}

	if( !(fd = fopen(name,"rb")) ){
		printf("***BMP load error: file access denied***\n");
		exit(0);
	}

	fread(&hdr, sizeof(hdr), 1, fd);
	if(hdr.type != 0x4D42){
		printf("***BMP load error: bad file format***\n");
		exit(0);
	}
	fread(&infoHdr, sizeof(infoHdr), 1, fd);

	if(infoHdr.bitsPerPixel != 24){
		printf("***BMP load error: invalid color depth***\n");
		exit(0);
	}

	if(infoHdr.compression){
		printf("***BMP load error: compressed image***\n");
		exit(0);
	}

	*width  = infoHdr.width;
	*height = infoHdr.height;
	*dst    = (uchar4 *)malloc(*width * *height * 4);

	printf("BMP width: %u\n", infoHdr.width);
	printf("BMP height: %u\n", infoHdr.height);

	fseek(fd, hdr.offset - sizeof(hdr) - sizeof(infoHdr), SEEK_CUR);

	for(y = 0; y < infoHdr.height; y++){
		for(x = 0; x < infoHdr.width; x++){
			(*dst)[(y * infoHdr.width + x)].z = fgetc(fd);
			(*dst)[(y * infoHdr.width + x)].y = fgetc(fd);
			(*dst)[(y * infoHdr.width + x)].x = fgetc(fd);
		}

		for(x = 0; x < (4 - (3 * infoHdr.width) % 4) % 4; x++)
			fgetc(fd);
	}


	if(ferror(fd)){
		printf("***Unknown BMP load error.***\n");
		free(*dst);
		exit(0);
	}else
		printf("BMP file loaded successfully!\n");

	fclose(fd);
}

void loadAllBMPs(void)
{
	loadBMP(&dst[0], &imageWidth[0], &imageHeight[0], "../numeric_keypad.bmp");
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &imageTex[0]);

	loadBMP(&dst[1], &imageWidth[1], &imageHeight[1], "../placard.bmp");
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &imageTex[1]);
}

/* light function */
void glLighting0(void)
{
	/* default lighting value */
	glEnable(GL_LIGHT0);
	GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_position[] = {0.0, 0.0, 1.0, 0.0};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void glLighting1(void)
{
	/* lighting for all objects */
	glEnable(GL_LIGHT1);
	GLfloat light_ambient[] = {0.1, 0.1, 0.1, 1.0};
	GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_position[] = {-10.0, -3.0, 3.0, 0.5};

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
}

void glLighting2(void)
{
	/* lighting for all objects */
	glEnable(GL_LIGHT2);
	GLfloat light_ambient[] = {0.1, 0.1, 0.1, 1.0};
	GLfloat light_diffuse[] = {1.0, 1.0, 0.7, 1.0};
	GLfloat light_specular[] = {1.0, 1.0, 1.0, 0.1};
	GLfloat light_position[] = {-19.0, 17.0, 13.0, 0.5};

	glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT2, GL_POSITION, light_position);
}

void glLighting3(void)
{
	/* lighting for select objects */
	glEnable(GL_LIGHT3);
	vec3 targetPosition = vec3(eyePosition.x - 3 * at.x, eyePosition.y - 25 * at.y, eyePosition.z + 2 * at.z);
	GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_position[] = {targetPosition.x, targetPosition.y, targetPosition.z, 0.5};

	glLightfv(GL_LIGHT3, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT3, GL_POSITION, light_position);
}

void glLightingOff(void)
{
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
}

void glLightingOpt(bool is_valid)
{
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);

	if (is_valid == false) {
		glLighting1();
		glLighting2();
	}

	else
		glLighting3();
}