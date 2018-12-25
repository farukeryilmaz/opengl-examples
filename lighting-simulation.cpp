
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 700

#define  TIMER_PERIOD  16 // Period for the timer.
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define V_D2R 0.0174532
#define V_R2D 57.29608323

typedef struct {
	double x, y;
} vec_t;

typedef struct {
	double magnitude, angle;
} polar_t;

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

double magV(vec_t v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

double angleV(vec_t v) {
	double angle = atan2(v.y, v.x) * V_R2D;
	return angle < 0 ? angle + 360 : angle;
}

vec_t addV(vec_t v1, vec_t v2) {
	return{ v1.x + v2.x, v1.y + v2.y };
}

vec_t subV(vec_t v1, vec_t v2) {
	return{ v1.x - v2.x, v1.y - v2.y };
}

vec_t mulV(double k, vec_t v) {
	return{ k * v.x, k * v.y };
}

double dotP(vec_t v1, vec_t v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

vec_t unitV(vec_t v) {
	return mulV(1.0 / magV(v), v);
}

// convert from polar representation to rectangular representation
vec_t pol2rec(polar_t p) {
	return{ p.magnitude * cos(p.angle * V_D2R), p.magnitude * sin(p.angle * V_D2R) };
}

polar_t rec2pol(vec_t v) {
	return{ magV(v), angleV(v) };
}

double angleBetween2V(vec_t v1, vec_t v2) {
	double magV1 = magV(v1);
	double magV2 = magV(v2);
	double dot = dotP(v1, v2);
	double angle = acos(dot / (magV1 * magV2)) * V_R2D; // in degree
	return angle;
}


typedef struct {
	float r, g, b;
} color_t;
typedef struct {
	vec_t pos;
	color_t color;
} light_t;
typedef struct {
	vec_t pos;
	vec_t N;
} vertex_t;
typedef struct {
	vec_t p1;
	vec_t p2;
	vec_t p3;
	vec_t p4;
	bool visibility;
} arrow_t;

/* Lighting Calculations */
color_t mulColor(float k, color_t c) {
	color_t tmp = { k * c.r, k * c.g, k * c.b };
	return tmp;
}
color_t addColor(color_t c1, color_t c2) {
	color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
	return tmp;
}
// To add distance into calculation
// when distance is 0  => its impact is 1.0
// when distance is 350 => impact is 0.0
// Linear impact of distance on light calculation.
double distanceImpact(double d) {
	return (-1.0 / 350.0) * d + 1.0;
}
color_t calculateColor(light_t source, vertex_t v) {
	vec_t L = subV(source.pos, v.pos);
	vec_t uL = unitV(L);
	float factor = dotP(uL, v.N) * distanceImpact(magV(L));
	return mulColor(factor, source.color);
}

//global variables of items
light_t light = { { 0, 0 }, { 1, 1, 1 } };
arrow_t redArrow = { 0 };
arrow_t whiteArrows[50] = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, true };
int i = 0;
float triangY1 = 0, triangY2 = 0;
bool spot = false;

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
void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
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

void display_info(){
	glColor3f(1, 1, 0);
	vprint(-450, 325, GLUT_BITMAP_8_BY_13, "Faruk Eryilmaz - #4");

	glColor3f(1, 1, 1);
	vprint(-70, -325, GLUT_BITMAP_8_BY_13, "F1 to switch light source type");

	if (spot == false)
		vprint(-70, -300, GLUT_BITMAP_HELVETICA_18, "POINT LIGHT SOURCE");
	else
		vprint(-70, -300, GLUT_BITMAP_HELVETICA_18, "SPOTLIGHT SOURCE");
}
void display_ceiling(){
	glLineWidth(4);
	glColor3f(0.5, 0.5, 0.5);
	glBegin(GL_LINES);
	for (int i = 0; i < 50; i++)
	{
		glVertex2f(-500 + i * 20, 300);
		glVertex2f(-490 + i * 20, 310);
	}
	glEnd();

	glLineWidth(7);
	glColor3f(0, 0, 1);
	glBegin(GL_LINES);
	glVertex2f(-500, 300);
	glVertex2f(500, 300);
	glEnd();

	glLineWidth(4);
	glColor3f(0.5, 0.5, 0.5);
	glBegin(GL_LINE_STRIP);
	for (float angle = 180; angle <= 360; angle += 5) {
		glVertex2f(35 * cos(angle*D2R), 35 * sin(angle*D2R) + 300);
	}
	glEnd();

	circle(0, 300, 5);
}
void display_point_pendulum(light_t ls){
	glColor3f(ls.color.r, ls.color.g, ls.color.b);
	circle(ls.pos.x, ls.pos.y, 20);
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex2f(0, 300);
	glVertex2f(ls.pos.x, ls.pos.y);
	glEnd();
}
void display_spotlight_pendulum(light_t ls){
	int radiusY = 0;
	triangY1 = triangY2 = 0;

	glColor3f(ls.color.r, ls.color.g, ls.color.b);
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex2f(0, 300);
	glVertex2f(ls.pos.x, ls.pos.y);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex2f(light.pos.x, light.pos.y);
	glVertex2f(50 * cos((sin((i - 1) * D2R) * 60 + 240)*D2R) + light.pos.x, 50 * sin((sin((i - 1) * D2R) * 60 + 240)*D2R) + light.pos.y);
	glVertex2f(50 * cos((sin((i - 1) * D2R) * 60 + 300)*D2R) + light.pos.x, 50 * sin((sin((i - 1) * D2R) * 60 + 300)*D2R) + light.pos.y);
	glEnd();

	do
	{
		triangY1 += 5;
		radiusY = triangY1 * sin((sin((i - 1) * D2R) * 60 + 240)*D2R) + light.pos.y;
	} while (radiusY >= -245 && triangY1 < 2000);

	glBegin(GL_LINES);
	glVertex2f(light.pos.x, light.pos.y);
	glVertex2f(triangY1 * cos((sin((i - 1) * D2R) * 60 + 240)*D2R) + light.pos.x, triangY1 * sin((sin((i - 1) * D2R) * 60 + 240)*D2R) + light.pos.y);

	do
	{
		triangY2 += 5;
		radiusY = triangY2 * sin((sin((i - 1) * D2R) * 60 + 300)*D2R) + light.pos.y;
	} while (radiusY >= -245 && triangY2 < 2000);

	glVertex2f(light.pos.x, light.pos.y);
	glVertex2f(triangY2 * cos((sin((i - 1) * D2R) * 60 + 300)*D2R) + light.pos.x, triangY2 * sin((sin((i - 1) * D2R) * 60 + 300)*D2R) + light.pos.y);
	glEnd();
}
void red_arrow(arrow_t v){
	glColor3f(1, 0, 0);
	circle(redArrow.p1.x, redArrow.p1.y, 3);
	glLineWidth(3);

	glBegin(GL_LINES);
	//vector
	glVertex2f(redArrow.p1.x, redArrow.p1.y);
	glVertex2f(redArrow.p2.x, redArrow.p2.y);

	// arrow tip 1
	glVertex2f(v.p2.x, v.p2.y);
	glVertex2f(v.p3.x, v.p3.y);

	// arrow tip 2
	glVertex2f(v.p2.x, v.p2.y);
	glVertex2f(v.p4.x, v.p4.y);

	glEnd();
}
void white_arrows(){
	glLineWidth(2);
	glColor3f(1, 1, 1);
	int j = 0;
	for (int x = -500; x <= 500; x += 20) {
		vertex_t P = { { x, -248 }, { 0, 0.8 } };
		color_t res = { 0.5, 0.5, 0.5 };
		res = addColor(res, calculateColor(light, P));

		whiteArrows[j].p1 = { x, -248 };
		whiteArrows[j].p2 = { x, -248 + 30 * (res.r + res.g + res.b) };
		whiteArrows[j].p3 = { x - 6 * (res.r + res.g + res.b) / 3, -258 + 30 * (res.r + res.g + res.b) };
		whiteArrows[j].p4 = { x + 6 * (res.r + res.g + res.b) / 3, -258 + 30 * (res.r + res.g + res.b) };

		j++;
	}
	for (j = 0; j <= 50; j ++)
	{
		if (spot == false)
			whiteArrows[j].visibility = true;
		else
		{
			if (whiteArrows[j].p1.x >= (triangY1 * cos((sin((i - 1) * D2R) * 60 + 240)*D2R) + light.pos.x) && whiteArrows[j].p1.x <= (triangY2 * cos((sin((i - 1) * D2R) * 60 + 300)*D2R) + light.pos.x))
				whiteArrows[j].visibility = true;
			else
				whiteArrows[j].visibility = false;
			//printf("%f - %f ---- %f x %f\n", triangY1, triangY2, (triangY1 * cos((sin((i - 1) * D2R) * 60 + 240)*D2R) + light.pos.x), (triangY2 * cos((sin((i - 1) * D2R) * 60 + 300)*D2R) + light.pos.x));
		}

		if (whiteArrows[j].visibility == true)
		{
			glBegin(GL_LINES);
			glVertex2f(whiteArrows[j].p1.x, whiteArrows[j].p1.y);
			glVertex2f(whiteArrows[j].p2.x, whiteArrows[j].p2.y);

			glVertex2f(whiteArrows[j].p2.x, whiteArrows[j].p2.y);
			glVertex2f(whiteArrows[j].p3.x, whiteArrows[j].p3.y);

			glVertex2f(whiteArrows[j].p2.x, whiteArrows[j].p2.y);
			glVertex2f(whiteArrows[j].p4.x, whiteArrows[j].p4.y);
			glEnd();
		}
	}
}
void colored_circles(){
	int j = 0;
	for (int x = -500; x <= 500; x += 20) {
		vertex_t P = { { x, -250 }, { 0, 0.8 } };


		color_t res = { 0.5, 0.5, 0.5 };
		res = addColor(res, calculateColor(light, P));

		glColor3f(res.r, res.g, res.b);
		if (whiteArrows[j].visibility == true)
			circle(x, -250, 10);

		glColor3f(1, 1, 1);
		circle_wire(x, -250, 10);
		j++;
	}
}
void display_surface() {
	white_arrows();
	colored_circles();
}

void display()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	display_info();
	display_ceiling();
	if (spot == false)
		display_point_pendulum(light);
	else
		display_spotlight_pendulum(light);
	red_arrow(redArrow);
	display_surface();

	glutSwapBuffers();
}

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
void onSpecialKeyDown(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;

	}

	if (key == GLUT_KEY_F1)
		spot = !spot;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}
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
void onClick(int button, int stat, int x, int y)
{
	// to refresh the window it calls display() function
	//glutPostRedisplay();
}
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
	//glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);

	light.pos = { 380 * cos((sin(i * D2R) * 60 + 270)*D2R), 380 * sin((sin(i * D2R) * 60 + 270)*D2R) + 300 };

	redArrow = { { 380 * cos((sin(i * D2R) * 60 + 270)*D2R), 380 * sin((sin(i * D2R) * 60 + 270)*D2R) + 300 },
	{ 450 * cos((sin(i * D2R) * 60 + 270)*D2R), 450 * sin((sin(i * D2R) * 60 + 270)*D2R) + 300 },
	{ 440 * cos((sin(i * D2R) * 60 + 269)*D2R), 440 * sin((sin(i * D2R) * 60 + 269)*D2R) + 300 },
	{ 440 * cos((sin(i  * D2R) * 60 + 271)*D2R), 440 * sin((sin(i * D2R) * 60 + 271)*D2R) + 300 } };
	
	i++;
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
	glutCreateWindow("CTIS 164 - HW #4 - SPR 2017");

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
