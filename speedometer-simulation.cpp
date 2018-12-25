#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800

#define  TIMER_PERIOD  30 // Period for the timer.
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer

#define D2R 0.0174532

typedef struct
{
	float x, y;
}point_t;
typedef struct
{
	point_t a, b, c, d;
	int angle;
}rect_t;

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

rect_t rectangle = { 0 };

//
// to draw circle, center at (x,y)
//  radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void print(int x, int y, char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
void vprint(int x, int y, void *font, char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

void vprint2(int x, int y, float size, char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void myCircle_wire(float x, float y, float r) {
	glColor3f(1, 1, 1);
	glLineWidth(1);
	glBegin(GL_LINE_LOOP);
	for (float angle = 0; angle < 360; angle += 5) {
		glVertex2f(r * cos(angle*D2R) + x, r * sin(angle*D2R) + y);
	}
	glEnd();
}
void myInnerCircle_wire(float x, float y, float r) {
	glColor3f(1, 1, 1);
	glLineWidth(1);
	glBegin(GL_LINE_STRIP);
	glVertex2f((r + 2) * cos(-70 * D2R) + x, (r + 2) * sin(-70 * D2R) + y);
	glVertex2f((r + 24) * cos(-70 * D2R) + x, (r + 24) * sin(-70 * D2R) + y);
	for (float angle = 0 - 70; angle <= 320 - 70; angle += 5) {
		glVertex2f(r * cos(angle*D2R) + x, r * sin(angle*D2R) + y);
	}
	glVertex2f((r + 2) * cos(250 * D2R) + x, (r + 2) * sin(250 * D2R) + y);
	glVertex2f((r + 24) * cos(250 * D2R) + x, (r + 24) * sin(250 * D2R) + y);
	glEnd();
}
void draw_lines(float x, float y, float r) {
	glColor3f(1, 1, 1);
	glLineWidth(1);
	glBegin(GL_LINES);
	for (float angle = 0 - 70; angle <= 320 - 70; angle += 20) {
		glVertex2f(r * cos(angle*D2R) + x, r * sin(angle*D2R) + y);
		glVertex2f((r - 15) * cos(angle*D2R) + x, (r - 15) * sin(angle*D2R) + y);
	}
	glEnd();
}
void draw_nums(float x, float y, float r) {
	glLineWidth(10);
	glColor3f(1, 1, 1);
	for (float angle = 0 - 70; angle <= 320 - 70; angle += 20) {
		vprint(r * cos(angle*D2R) + x, r * sin(angle*D2R) + y, GLUT_BITMAP_HELVETICA_18, "%d", -1 * ((int)angle - 250));
	}
}
void calculate_indicator(float x, float y, float r){
	rectangle.a.x = r * cos((250 + -rectangle.angle) * D2R) + x;
	rectangle.a.y = r * sin((250 + -rectangle.angle) * D2R) + y;

	rectangle.b.x = 15 * cos((150 + -rectangle.angle) * D2R) + x;
	rectangle.b.y = 15 * sin((150 + -rectangle.angle) * D2R) + y;

	rectangle.c.x = 20 * cos((70 + -rectangle.angle) * D2R) + x;
	rectangle.c.y = 20 * sin((70 + -rectangle.angle) * D2R) + y;

	rectangle.d.x = 15 * cos((-10 + -rectangle.angle) * D2R) + x;
	rectangle.d.y = 15 * sin((-10 + -rectangle.angle) * D2R) + y;
}
void draw_indicator(float x, float y, float r) {
	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(rectangle.a.x, rectangle.a.y);
	glVertex2f(rectangle.b.x, rectangle.b.y);
	glVertex2f(rectangle.c.x, rectangle.c.y);
	glVertex2f(rectangle.d.x, rectangle.d.y);
	glEnd();

	glLineWidth(3);
	glColor3f(0.5, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(rectangle.a.x, rectangle.a.y);
	glVertex2f(rectangle.b.x, rectangle.b.y);
	glVertex2f(rectangle.c.x, rectangle.c.y);
	glVertex2f(rectangle.d.x, rectangle.d.y);
	glEnd();

	glColor3f(0, 0, 0);
	circle(x, y, 8);
}
void draw_speedInfo(float x, float y) {
	glColor3f(1, 1, 0);
	glLineWidth(3);
	vprint(x + 10, y, GLUT_BITMAP_TIMES_ROMAN_24, "%d", rectangle.angle);
	vprint(x, y - 30, GLUT_BITMAP_HELVETICA_18, "KM/h");
}
void draw_speedArc(float x, float y, float r){
	glColor3f(0, 1, 0);
	if (rectangle.angle > 260)
		glColor3f(1, 0, 0);
	glLineWidth(4);
	glBegin(GL_LINE_STRIP);
	glVertex2f((r + 16) * cos(250 * D2R) + x, (r + 16) * sin(250 * D2R) + y);
	glVertex2f((r + 4) * cos(250 * D2R) + x, (r + 4) * sin(250 * D2R) + y);
	for (float a = 0; a <= rectangle.angle; a += 5) {
		glVertex2f((r + 4) * cos((250 + -a)*D2R) + x, (r + 4) * sin((250 + -a)*D2R) + y);
	}
	glVertex2f((r + 4) * cos((250 + -rectangle.angle) * D2R) + x, (r + 4) * sin((250 + -rectangle.angle) * D2R) + y);
	glVertex2f((r + 16) * cos((250 + -rectangle.angle) * D2R) + x, (r + 16) * sin((250 + -rectangle.angle) * D2R) + y);
	glEnd();
	glBegin(GL_LINE_STRIP);
	for (float a = 0; a <= rectangle.angle; a++) {
		glVertex2f((r + 16) * cos((250 + -a)*D2R) + x, (r + 16) * sin((250 + -a)*D2R) + y);
	}
	glEnd();
}

void display_label() {
	glColor3f(1, 1, 1);
	vprint(-390, 360, GLUT_BITMAP_8_BY_13, "FARUK ERYILMAZ");
	vprint(-390, 340, GLUT_BITMAP_8_BY_13, "#3");
	vprint(-100, -340, GLUT_BITMAP_8_BY_13, "UP arrow key to accelerate");
}
void display_outline() {
	myCircle_wire(0, 0, 272);
	myInnerCircle_wire(0, 0, 248);
}
void display_dangerZone() {
	glColor3f(1, 0, 0);
	glLineWidth(4);
	glBegin(GL_LINE_STRIP);
	for (float angle = 320 - 30; angle <= 320 + 30; angle += 5) {
		glVertex2f(240 * cos(angle*D2R) + 0, 240 * sin(angle*D2R) + 0);
	}
	glEnd();
}
void display_speedText() {
	draw_lines(0, 0, 248);
	draw_nums(-10, 0, 200);
}
void display_indicator() {
	calculate_indicator(0, 0, 248);
	draw_indicator(0, 0, 248);
	draw_speedInfo(-20, -160);
}
void display_speedArc() {
	draw_speedArc(0, 0, 250);
}

void display()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	display_label();
	display_outline();
	display_speedText();
	display_dangerZone();
	display_indicator();
	display_speedArc();

	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;

	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;

	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// to refresh the window it calls display() function	
	glutPostRedisplay();
}

void onMove(int x, int y) {
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	
	if (up == true && rectangle.angle >= 0 && (rectangle.angle <= 312 || rectangle.angle <= 316)){
		if (rectangle.angle == 316)
			rectangle.angle = rectangle.angle + 4;
		else
			rectangle.angle = rectangle.angle + 8;
	}
	else if (up == false && rectangle.angle >= 4 && rectangle.angle <= 320)
		rectangle.angle = rectangle.angle - 4;

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

int main(int argc, char *argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("GRADING");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);
	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}
