
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <math.h>
#include <vector>

using namespace std;

int WINDOW_WIDTH = 720;
int WINDOW_HEIGHT = 480;

float sq_pos[] = { 0.0f, 0.0f };
float sq_rot = 0.0f;
float sq_scl = 1.0f;

const float pi = 3.14;

int rad;

struct Point {
	float x, y;

	Point(float _x, float _y) : x(_x), y(_y) {}
};

vector<Point> pixels; 

void pixel(float u, float v) {
	pixels.push_back(Point(u, v));
}

void calcFirstOctantMidpoint(int radius) {
	pixels.clear();
	int x = 0;
	int y = radius;
	int decisionParameter = 1 - radius; 
	pixel(x, y);
	while (y > x) {
		if (decisionParameter < 0) {
			decisionParameter += 2 * x + 3;
		}
		else {
			decisionParameter += 2 * (x - y) + 5;
			y--;
		}
		x++;
		pixel(x, y);
	}
}

void calcFullCircleMidpoint(int centerX, int centerY, int radius) {
	// Draw the first octant using Midpoint Circle Drawing Algorithm
	calcFirstOctantMidpoint(radius);
	int size = pixels.size();
	for (int i = 0; i < size; i++) {
		int x = pixels[i].x;
		int y = pixels[i].y;

		// Draw points in all eight octants
		pixel(centerX + x, centerY + y);
		pixel(centerX + y, centerY + x);
		pixel(centerX - x, centerY + y);
		pixel(centerX - y, centerY + x);
		pixel(centerX + x, centerY - y);
		pixel(centerX + y, centerY - x);
		pixel(centerX - x, centerY - y);
		pixel(centerX - y, centerY - x);
	}
}



void drawCirclePixels() {
	glColor3f(1.0f, 1.0f, 1.0f);

	for (const auto& p : pixels) {
		glBegin(GL_QUADS);
		glVertex2f(p.x - 0.5f, p.y - 0.5f);
		glVertex2f(p.x + 0.5f, p.y - 0.5f);
		glVertex2f(p.x + 0.5f, p.y + 0.5f);
		glVertex2f(p.x - 0.5f, p.y + 0.5f);
		glEnd();
	}
}


void drawCircle(int centerX, int centerY, int radius, float thickness) {
	glColor3f(0.0f, 1.0f, 0.0f);

	glLineWidth(thickness);

	// Draw the circle
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 360; ++i) {
		float angle = i * 3.14159 / 180.0;
		float x = centerX + radius * cos(angle);
		float y = centerY + radius * sin(angle);
		glVertex2f(x, y);
	}
	glEnd();

	// Reset the line thickness to default
	glLineWidth(1.0f);
}


void square() {
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f); // green
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glEnd();
}

void axes() {
	// x axis
	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f); 
	glVertex2f(-10.0f, 0.0f);
	glVertex2f(10.0f, 0.0f);
	glEnd();

	// y axis
	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f); 
	glVertex2f(0.0f, 10.0f);
	glVertex2f(0.0f, -10.0f);
	glEnd();
}

void cell(int u, int v) {
	glBegin(GL_LINE_LOOP);
	glVertex2f(u - 0.5f, v - 0.5f);
	glVertex2f(u + 0.5f, v - 0.5f);
	glVertex2f(u + 0.5f, v + 0.5f);
	glVertex2f(u - 0.5f, v + 0.5f);
	glEnd();
}

void grid() {
	int u = 0, v = 0, m = 0, n = 0;

	glPushMatrix();
	glColor3f(0.3f, 0.3f, 0.3f);

	for (int i = 0; i < 10; i++) {
		u = 0, v = 0;
		for (int j = 0; j < 10; j++) {
			cell(u, m);
			if (v != u) {
				cell(v, m);
			}
			if (m != n) {
				cell(u, n);
				if (v != u) {
					cell(v, n);
				}
			}
			u++;
			v--;
		}
		m++;
		n--;
	}
	glPopMatrix();
}

void drawCartesianCoordinates() {
	// Set the line color
	glColor3f(0.3f, 0.3f, 0.3f);

	// Display tick marks on x-axis
	for (int i = -10; i <= 10; ++i) {
		glBegin(GL_LINES);
		glVertex2f(i, -0.2);
		glVertex2f(i, 0.2);
		glEnd();
	}

	// Display tick marks on y-axis
	for (int i = -10; i <= 10; ++i) {
		glBegin(GL_LINES);
		glVertex2f(-0.2, i);
		glVertex2f(0.2, i);
		glEnd();
	}

	// Display the axis labels
	glColor3f(0.5f, 0.5f, 0.5f);
	glRasterPos2f(10.2, 0.0);
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'X');
	glRasterPos2f(0.0, 10.2);
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'Y');
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	axes();

	grid();
	drawCartesianCoordinates();

	drawCirclePixels();
	drawCircle(0, 0, rad, 3);

	glFlush();
}

void reshape(int w, int h) {
	glViewport(0, 0, w, h);

	GLfloat aspect_ratio = h == 0 ? w / 1 : (GLfloat)w / (GLfloat)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h) {
		gluOrtho2D(-11, 11, -11 / aspect_ratio, 11 / aspect_ratio);
	}
	else {
		gluOrtho2D(-11 * aspect_ratio, 11 * aspect_ratio, -11, 11);
	}
}

void keyboardSpecial(int key, int x, int y) {

	if (key == GLUT_KEY_UP) {
		sq_scl += 0.5f;
	}
	if (key == GLUT_KEY_DOWN) {
		sq_scl -= 0.5f;
	}

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {

	// movement

	if (key == 'w') { // up
		sq_pos[1] += 0.5f;
	}
	if (key == 'a') { // left
		sq_pos[0] -= 0.5f;
	}
	if (key == 's') { // down
		sq_pos[1] -= 0.5f;
	}
	if (key == 'd') { // right
		sq_pos[0] += 0.5f;
	}

	glutPostRedisplay();

}

void init() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void getUserInput() {
	cout << "Enter radius: ";
	cin >> rad;
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(500, 300);
	glutCreateWindow("2D Graphics Template");

	getUserInput();

	calcFirstOctantMidpoint(rad);
	calcFullCircleMidpoint(0, 0, rad);
	/*printPixels();*/

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutSpecialFunc(keyboardSpecial);
	glutKeyboardFunc(keyboard);

	init();
	glutMainLoop();

	return 0;
}