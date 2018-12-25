
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define  TIMER_PERIOD  15 // Period for the timer.
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer

#define D2R 0.0174532

typedef struct
{
	float x, y;
}point_t;
typedef struct
{
	point_t start, end;
	float A, B, C, t;
}line_t;
typedef struct
{
	point_t center;
	int radius, radius_in;
}circle_t; 
typedef struct {
	bool hit;
	float dist;
	point_t nearest;
	point_t ixect;
}result_t;

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

#define P_DIFF 50 // distance between two point: the value can be changed, program calculates and creates points according to the value again
#define L_DIFF 20 // distance between two vertical line: the value can be changed, program calculates and creates lines according to the value again
int pSize = WINDOW_WIDTH / P_DIFF + 1; // point structure array size
int lSize = WINDOW_WIDTH / L_DIFF + 1; // line structure array size
point_t *point = (point_t *)malloc(pSize * sizeof(point_t)); // because the distance is modifiable, count of points changes so the structure array is created properly 
line_t *line = (line_t *)malloc(lSize * sizeof(line_t)); // because the distance is modifiable, count of lines changes so the structure array is created properly 

circle_t circ = { { 0, 0 }, 25 , 23};
bool fall = false, show = false;
float V = 0;
int loc;

line_t perp;
result_t res;

float testPoint(point_t p1, point_t p2, point_t ixect) {
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	if (dx != 0) {
		return (ixect.x - p1.x) / dx;
	}
	return (ixect.y - p1.y) / dy;
}
float findDistance(point_t p, point_t q) {
	return sqrt((p.x - q.x) * (p.x - q.x) + (p.y - q.y)*(p.y - q.y));
}
void testLineSegmentCircle(point_t p1, point_t p2) {
	float A, B, C;
	// Step #1 : Find the line equation passing thru line segment
	A = p1.y - p2.y;  // y1 - y2
	B = p2.x - p1.x;  // x2 - x1
	C = A * p1.x + B * p1.y;
	// simplify A, B, and C
	if (B != 0) {
		A = A / B;
		C = C / B;
		B = 1;
	}

	// Step #2 : Find perpendicular line equation
	perp.A = -B;
	perp.B = A;
	perp.C = perp.A * circ.center.x + perp.B * circ.center.y;

	// Step #3 : Find the intersection point of two lines represented by general line equations
	float denom = (B * perp.A - perp.B * A);
	res.ixect.x = (B * perp.C - perp.B * C) / denom;
	res.ixect.y = (perp.A * C - A * perp.C) / denom;


	// Step #4 : Find the nearest point on line segment to the center of the circle
	float t = testPoint(p1, p2, res.ixect);
	if (t < 0) res.nearest = p1;
	else if (t > 1.0) res.nearest = p2;
	else res.nearest = res.ixect;

	// Step #5 : Find the distance between nearest point and the circle center.
	res.dist = findDistance(res.nearest, circ.center);
}

// finds parameter "t" : parametric line equation
double calculate_t(line_t *ln1, point_t *start2, point_t *end2){
	double a, b;

	a = (end2->y - start2->y) * (start2->x - ln1->start.x) - (start2->y - ln1->start.y)*(end2->x - start2->x);
	b = (end2->y - start2->y) * (ln1->end.x - ln1->start.x) - (ln1->end.y - ln1->start.y)*(end2->x - start2->x);

	return(a / b);
}

// finds intersection points of the lines and updates the starting points of the lines
void intercept(line_t *ln1, point_t *start2, point_t *end2) {

	ln1->start.x = ln1->start.x + (ln1->end.x - ln1->start.x) * ln1->t;
	ln1->start.y = ln1->start.y + (ln1->end.y - ln1->start.y) * ln1->t;
}

void Init_Globals(){
	int i;
	// creates points which have random y coordinates.
	for (i = 0; i < pSize; i++)
	{
		(point + i)->x = i * P_DIFF - WINDOW_WIDTH / 2;
		(point + i)->y = rand() % 201 * -1;
	}
	// creates lines: y1 = 0, y2 = -300
	for (i = 0; i < lSize; i++)
	{
		(line + i)->start.x = i * L_DIFF - WINDOW_WIDTH / 2;
		(line + i)->start.y = 0;
		(line + i)->end.x = (line + i)->start.x;
		(line + i)->end.y = -WINDOW_HEIGHT / 2;
	}
	int k = 0;
	// finds parameter "t" : parametric line equation
	for (i = 0; i < lSize; i++){
		(line + i)->t = calculate_t(line + i, point + k, point + k + 1);
		while (k < (pSize - 1) && (line + i)->start.x > (point + k + 1)->x)
		{
			k++;
			(line + i)->t = calculate_t(line + i, point + k, point + k + 1);
		}
		// finds intersection points of the lines and updates the starting points of the lines
		intercept(line + i, point + k, point + k + 1);
	}
}

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
	glColor3f(1, 1, 1);
	vprint(-390, 280, GLUT_BITMAP_8_BY_13, "FARUK ERYILMAZ");
	vprint(-390, 260, GLUT_BITMAP_8_BY_13, "#2");
	vprint(-390, 240, GLUT_BITMAP_8_BY_13, "PRESS F1 TO CHANGE THE SURFACE");
}
void draw_connected_lines(){
	glColor3f(1, 0, 0);
	glLineWidth(3);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < pSize; i++)
		glVertex2f((point + i)->x, (point + i)->y);
	glEnd();
}
void draw_points(){
	glColor3f(0, 1, 0);
	for (int i = 0; i < pSize; i++)
		circle((point + i)->x, (point + i)->y, 5);
}
void draw_lines(){
	glColor3f(1, 1, 1);
	glLineWidth(1);
	for (int i = 0; i < lSize; i++){
		circle((line + i)->start.x, (line + i)->start.y, 2);
		glBegin(GL_LINES);
		glVertex2f((line + i)->start.x, (line + i)->start.y);
		glVertex2f((line + i)->end.x, (line + i)->end.y);
		glEnd();
	}
}
void show_ball(){
	glColor3f(1, 1, 1);
	circle(circ.center.x, circ.center.y, circ.radius);
	glColor3f(0.3, 0.3, 1);
	circle(circ.center.x, circ.center.y, circ.radius_in);
}

void display()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	display_info();
	draw_connected_lines();
	draw_points();
	draw_lines();

	if (show == true)
	{
		show_ball();
	}

	glutSwapBuffers();
}

void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
	{
		free(line);
		free(point);
		exit(0);
	}

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
	{
		show = fall = false;
		Init_Globals();
	}

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
	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN)
	{
		V = 0;
		show = fall = true;
		x = x - winWidth / 2;
		y = winHeight / 2 - y;

		circ.center.x = x;
		circ.center.y = y;

		loc = ((WINDOW_WIDTH / 2) + circ.center.x) / P_DIFF;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
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
	glutPostRedisplay();
}
#if TIMER_ON == 1
		float t;
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);

	if (show == true && fall == true)
	{
		circ.center.y -= V;
		V += 0.12;

		// finds parameter "t" : parametric line equation
		for (int i = loc - 1; i <= loc + 1; i++){
			testLineSegmentCircle(*(point + i), *(point + i + 1));

			//Test if the circle intersects with the line segment
			res.hit = res.dist <= circ.radius;

			if (res.hit)
				fall = false;
		}

		if (circ.center.y <= -WINDOW_HEIGHT / 2 - circ.radius)
			fall = false;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	Init_Globals();
}

int main(int argc, char *argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("CTIS164 HW #2 - SPR 2017");

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
