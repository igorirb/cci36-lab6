#ifdef WIN32
    #include <windows.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>

#ifdef __APPLE__
    #include <GLUT/glut.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/glut.h>
    #include <GL/glu.h>
    #include <GL/gl.h>
#endif

using std::cerr;
using std::endl;

int SCREEN_HEIGHT = 480;

// Keep track of times clicked, on 3 clicks draw.
int NUMPOINTS = 0;
int ROW = 0;
int COLUMN = 0;

long factorial (int n)
{
    if (n == 1 || n == 0)
        return 1;
    else
        return n * factorial(n - 1);
}

// Point class to keep it a little cleaner.
class Point {
public:
    float x, y, z;
    void setxy(float x2, float y2) { x = x2; y = y2; }
    const Point & operator=(const Point &rPoint) {
        x = rPoint.x;
        y = rPoint.y;
        z = rPoint.z;
        return *this;
    }
};

Point points[100];
Point surface[100][100];
int rows[100];

void myInit () {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glPointSize(4.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 640.0, 0.0, 480.0);
}

void drawDot (int x, int y) {
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
    glFlush();
}

void clearDot (int x, int y) {
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
    glFlush();
}

void drawLine (Point p1, Point p2) {
    glBegin(GL_LINES);
    glVertex3f(p1.x, p1.y, p1.z);
    glVertex3f(p2.x, p2.y, p2.z);
    glEnd();
    glFlush();
}

Point drawBezierSurface (Point list[], double t) {
    Point P;
    P.x = P.y = P.z = 0;
    int m;
    int n = COLUMN - 1;
    for (int j = 0; j <= n; j++) {
        m = rows[j];
        for (int i = 0; i <= m; i++) {
            P.x += surface[j][i].x * (pow(t, i) * pow(1 - t, m - i) * factorial(m) / (factorial(m - i) * factorial(i)));
            P.y += surface[j][i].y * (pow(t, i) * pow(1 - t, m - i) * factorial(m) / (factorial(m - i) * factorial(i)));
            P.z += surface[j][i].z * (pow(t, i) * pow(1 - t, m - i) * factorial(m) / (factorial(m - i) * factorial(i)));
        }
        P.x += P.x * pow(t, j) * pow(1 - t, n - j) * factorial(n) / (factorial(n - j) * factorial(j));
        P.y += P.y * pow(t, j) * pow(1 - t, n - j) * factorial(n) / (factorial(n - j) * factorial(j));
        P.z += P.z * pow(t, j) * pow(1 - t, n - j) * factorial(n) / (factorial(n - j) * factorial(j));
    }
    return P;
}

Point drawBezier (Point list[], double t) {
    Point P;
    P.x = P.y = P.z = 0;
    int n = NUMPOINTS - 1;
    for (int i = 0; i <= n; i++) {
        P.x += points[i].x * pow(t, i) * pow(1 - t, n - i) * factorial(n) / (factorial(n - i) * factorial(i));
        P.y += points[i].y * pow(t, i) * pow(1 - t, n - i) * factorial(n) / (factorial(n - i) * factorial(i));
        P.z += points[i].z * pow(t, i) * pow(1 - t, n - i) * factorial(n) / (factorial(n - i) * factorial(i));
    }
    return P;
}

void myMouse(int button, int state, int x, int y) {
    // If left button was clicked
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Store where the user clicked, note Y is backwards.
        points[NUMPOINTS].setxy((float)x, (float)(SCREEN_HEIGHT - y));
        surface[COLUMN][ROW].setxy((float)x, (float)(SCREEN_HEIGHT - y));
        
        NUMPOINTS++;
        ROW++;
        rows[COLUMN] += 1;
        
        drawDot(x, SCREEN_HEIGHT - y);
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        for (int i = 0; i < NUMPOINTS - 1; i++) {
            glColor3f(1.0, 1.0, 1.0);
            drawLine(points[i], points[i + 1]);
            printf("%d - %d\n", i, i+1);
        }
        
        Point POld = points[0];
        
        /* Draw each segment of the curve. Make t increment in
            smaller amounts for a more detailed curve. */
        for(double t = 0.0;t <= 1.0; t += 0.1) {
            Point P;
            P = drawBezier(points, t);
            glColor3f(0.0, 1.0, 0.0);
            drawLine(POld, P);
            POld = P;
        }
        
        glColor3f(1.0, 0.0, 0.0);
        
        NUMPOINTS = 0;
    }
}

void myKeyboard(unsigned char key, int x, int y) {
    if (key == GLUT_KEY_UP) {
        COLUMN++;
        ROW = 0;
        printf("column: %d\n", COLUMN);
    } else if (key == GLUT_KEY_DOWN) {
        printf("bezier surface.\n");
        
        if (COLUMN == 0) COLUMN++;
        
        for (int j = 0; j <= COLUMN; j++) {
            printf("%d\n", rows[j]);
            for (int i = 0; i < rows[j] - 1; i++) {
                printf("%f, %f - %d, %d - %d\n", surface[j][i].x, surface[j][i+1].x, i, i+1, j);
                drawLine(surface[j][i], surface[j][i + 1]);
            }
            
            Point POld = surface[j][0];
            
            for(double t = 0.0;t <= 1.0; t += 0.1) {
                Point P;
                P = drawBezierSurface(surface[j], t);
                drawLine(POld, P);
                POld = P;
            }
            
            glColor3f(1.0, 0.0, 0.0);
        }
    }
}

void myDisplay () {
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

int main (int argc, char *argv[]) {
    printf("e = incrementa coluna.\n");
    printf("g = bezier surface.\n");
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(100,150);
    glutCreateWindow("Bezier Curve");
    glutMouseFunc(myMouse);
    glutKeyboardFunc(myKeyboard);
    glutDisplayFunc(myDisplay);
    myInit();
    glutMainLoop();
    return 0;
}