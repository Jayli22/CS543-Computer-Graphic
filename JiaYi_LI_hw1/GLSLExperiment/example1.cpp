// Starter program for HW 1. 
// Program draws a triangle. Study the program first
// The function generateGeometry can be modified to draw many 2D drawings (e.g. 2D Sierpinski Gasket)
// Generated using randomly selected vertices and bisection

#include "Angel.h"  // Angel.h is homegrown include file, which also includes glew and freeglut
#include "mat.h"
#include "textfile.h"
#include <vector>
#include <iostream>
#include<fstream>
#include <time.h>
// Number of points in polyline
//const int NumPoints = 3;

// remember to prototype
void generateGeometry( void );
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void Keyboard( unsigned char key, int x, int y );
void Mouse(int button, int bState, int x , int y);

typedef vec2 point2;
point2 points[3000];
int  screenHeight = 600;
int screenWidth = 800;
double scaleratiox = 1, scaleratioy = 1;
char *filepath[] = {
		"Polyline\\birdhead.dat",
		"Polyline\\knight.dat",
		"Polyline\\dragon.dat",
		"Polyline\\house.dat",
		"Polyline\\knight.dat",
		"Polyline\\rex.dat",
		"Polyline\\scene.dat",
		"Polyline\\usa.dat",
		"Polyline\\vinci.dat",
		"Polyline\\dragon.dat"

};
using namespace std;

vector<point2> pointas;


// Array for polyline
//point2 points[NumPoints];
GLuint program;
int colorCount = 1;

//translate the float coord to coord we need
point2 translatepoint(float left, float top, float right, float bottom, float a, float b) {
	float xzoom = 2 / (right - left);
	float yzoom = 2 / (top - bottom);
	float xoffset = (right + left) / (right - left);
	float yoffset = (top + bottom) / (top - bottom);
	float newx = a * xzoom - xoffset;
	float newy = b * yzoom - yoffset;
	return point2(newx, newy);
}



void DrawFile(char* filepath)
{
	char *file;
	FILE *fp = fopen(filepath, "r");
	char line[300];
	float top, bottom, left, right;
	int lineNumber;
	int i, j;
	int * polygens = NULL;
	float x, y;
	while (!feof(fp)) {		
		fscanf(fp, "%s", line);
		if (line[0] == '*')	break;
	}
	fscanf(fp, "%f %f %f %f\n", &left, &top, &right, &bottom); 
	fscanf(fp, "%d", &lineNumber); 
	polygens = (int*)malloc(lineNumber * sizeof(int));

	for (i = 0; i < lineNumber; i++) 	
	{
		fscanf(fp, "%d", &polygens[i]);
		for (j = 0; j < polygens[i]; j++)
		{
			fscanf(fp, "%f %f", &x, &y);

			points[j] = translatepoint(left, top, right, bottom, x, y);
		}
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();                     
		glDrawArrays(GL_LINE_STRIP, 0, polygens[i]);
	}
	glFlush();


	free(polygens); 
	polygens = NULL;
	fclose(fp); 


}
void generateGeometry( void )
{
	// Specifiy the vertices for a triangle
	mat4 ortho = Ortho2D(0, 800, 0, 600);
	GLuint ProjLoc = glGetUniformLocation(program, "Proj");
	glUniformMatrix4fv(ProjLoc, 1, GL_TRUE, ortho);

}


void initGPUBuffers( void )
{
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers( 1, &buffer );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
}


char *vs = "vshader1.glsl";
char *fs = "fshader1.glsl";

void shaderSetup( void )
{
	// Load shaders and use the resulting shader program
	program = InitShader(vs, fs);
	glUseProgram( program );

	// Initialize the vertex position attribute from the vertex shader
	GLuint loc = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( loc );
	glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	glClearColor( 1.0, 1.0, 1.0, 1.0 );        // sets white as color used to clear screen
}


void Drawframe()
{

	point2 frame[] = { point2(-1,-1),point2(1,-1)};
	glBufferData(GL_ARRAY_BUFFER, sizeof(frame), frame, GL_STATIC_DRAW);
	glDrawArrays(GL_LINE_STRIP, 0,2 );

	point2	nframe[18];
	
	
		for (float i= - 0.8 ; i < 1;i = i + 0.2)
		{
			if (i != 0)
			{

				nframe[0] = point2(i, 1);
				nframe[1] = point2(i, -1);
				glBufferData(GL_ARRAY_BUFFER, sizeof(nframe), nframe, GL_STATIC_DRAW);
				glDrawArrays(GL_LINE_STRIP, 0, 2);
			}
		}
		
		

		
	glFlush();

}

void display( void )
{
	glViewport(0, (screenHeight - (screenHeight / 8)), screenWidth , screenHeight / 8  );
	Drawframe();

	for (int i = 0; i < 10; i++)
	{
		if (i == 0)
		{
			glViewport(0 , screenHeight - (screenHeight / 8), screenWidth / 10 , screenHeight / 12 );
			DrawFile(filepath[i]);
		}
		else
		{
			glViewport((screenWidth / 10 * i )  , screenHeight - (screenHeight / 8)  , screenWidth / 10  , screenHeight / 12  );

			DrawFile(filepath[i]);
		}

	}									
}

int mMode;
int thumbnailCount;

void Thumbnail(int x, int y)
{
	if (x >= 0   && x < screenWidth / 10  && y <= screenHeight / 8)
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, screenWidth, screenHeight - (screenHeight / 7) );
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glViewport(screenWidth / 12  , 0, screenWidth - screenWidth / 6  , screenHeight - screenHeight / 6  );
		DrawFile(filepath[0]);
		thumbnailCount = 0;
	}
	if (x >= screenWidth / 10   && x < screenWidth / 10   + screenWidth / 10 * 1   - 5 && y <= screenHeight / 8 )
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, screenWidth, screenHeight - (screenHeight / 7) );
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glViewport(screenWidth / 12  , 0, screenWidth - screenWidth / 6  , screenHeight - screenHeight / 6  );
		DrawFile(filepath[1]);
		thumbnailCount = 1;
	}
	if (x >=screenWidth / 10   + screenWidth / 10 * 1   && x < 5 + screenWidth / 13   + screenWidth / 10 * 2   - 5 && y <= screenHeight / 8)
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, screenWidth, screenHeight - (screenHeight / 7) );
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glViewport(screenWidth / 12  , 0, screenWidth - screenWidth / 6  , screenHeight - screenHeight / 6  );
		DrawFile(filepath[2]);
		thumbnailCount = 2;
	}if (x >= screenWidth / 10   + screenWidth / 10 * 2   && x < 5 + screenWidth / 13   + screenWidth / 10 * 3   - 5 && y <= screenHeight / 8 )
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, screenWidth, screenHeight - (screenHeight / 7) );
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glViewport(screenWidth / 12  , 0, screenWidth - screenWidth / 6  , screenHeight - screenHeight / 6  );
		DrawFile(filepath[3]);
		thumbnailCount = 3;
	}if (x >= screenWidth / 10   + screenWidth / 10 * 3   && x < 5 + screenWidth / 13   + screenWidth / 10 * 4   - 5 && y <= screenHeight / 8)
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, screenWidth, screenHeight - (screenHeight / 7) );
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glViewport(screenWidth / 12  , 0, screenWidth - screenWidth / 6  , screenHeight - screenHeight / 6  );
		DrawFile(filepath[4]);
		thumbnailCount = 4;
	}if (x >= screenWidth / 10   + screenWidth / 10 * 4   && x < 5 + screenWidth / 13   + screenWidth / 10 * 5   - 5 && y <= screenHeight / 8 )
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, screenWidth, screenHeight - (screenHeight / 7) );
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glViewport(screenWidth / 12  , 0, screenWidth - screenWidth / 6  , screenHeight - screenHeight / 6  );
		DrawFile(filepath[5]);
		thumbnailCount = 5;
	}if (x >= 5 + screenWidth / 10   + screenWidth / 10 * 5   && x < 5 + screenWidth / 13   + screenWidth / 10 * 6   - 5 && y <= screenHeight / 8 )
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, screenWidth, screenHeight - (screenHeight / 7) );
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glViewport(screenWidth / 12  , 0, screenWidth - screenWidth / 6  , screenHeight - screenHeight / 6  );
		DrawFile(filepath[6]);
		thumbnailCount = 6;
	}if (x >= screenWidth / 10   + screenWidth / 10 * 6  && x < 5 + screenWidth / 13   + screenWidth / 10 * 7   - 5 && y <= screenHeight / 8 )
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, screenWidth, screenHeight - (screenHeight / 7) );
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glViewport(screenWidth / 12  , 0, screenWidth - screenWidth / 6  , screenHeight - screenHeight / 6  );
		DrawFile(filepath[7]);
		thumbnailCount = 7;
	}if (x >= screenWidth / 10   + screenWidth / 10 * 7   && x < 5 + screenWidth / 13   + screenWidth / 10 * 8   - 5 && y <= screenHeight / 8 )
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, screenWidth, screenHeight - (screenHeight / 7) );
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glViewport(screenWidth / 12  , 0, screenWidth - screenWidth / 6  , screenHeight - screenHeight / 6  );
		DrawFile(filepath[8]);
		thumbnailCount = 8;
	}if (x >= screenWidth / 10   + screenWidth / 10 * 8    && x < 5 + screenWidth / 13   + screenWidth / 10 * 9   - 5 && y <= screenHeight / 8 )
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, screenWidth, screenHeight - (screenHeight / 7) );
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glViewport(screenWidth / 12  , 0, screenWidth - screenWidth / 6  , screenHeight - screenHeight / 6  );
		DrawFile(filepath[9]);
		thumbnailCount = 9;
	}

}
void DrawTile()
{
	glEnable(GL_SCISSOR_TEST);
	glScissor(0, 0, screenWidth, (screenHeight - screenHeight / 7) );
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
	float i;
	point2 frame[2];
	i = -0.75;
	while (i <= 0.9)
	{
		frame[0] = point2(0.75,i);
		frame[1] = point2(-0.75,i);
		glBufferData(GL_ARRAY_BUFFER, sizeof(frame), frame, GL_STATIC_DRAW);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		i += 0.3;
	}
	while (i>=-0.9)
	{
		frame[0] = point2(i, 0.75);
		frame[1] = point2(i, -0.75);
		glBufferData(GL_ARRAY_BUFFER, sizeof(frame), frame, GL_STATIC_DRAW);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		i -= 0.3;
	}

	srand((unsigned)time(NULL));
	int p;
	for (int i = 0;i<5;i++)
	{
		for (int j =0 ; j <5 ; j++)
		{
			p = rand() % 10;
			printf("%d\n", p);
			glViewport(screenWidth / 12   + (j+1)* (screenWidth - screenWidth / 40)  /8,
				screenWidth / 12   / 6 + (i+1)*(screenHeight - screenHeight / 40)   / 8,
				(screenWidth - screenWidth / 6  ) / 8, 
				(screenHeight - screenHeight / 6)   / 8);
			DrawFile(filepath[p]);

		}
	}
		glFlush();

}
bool newline = false;
point2 dPoint[100];
int dPointNum = 0;
void DrawMode(int x, int y)
{
	glViewport(0, 0, screenWidth, screenHeight);

	if (newline)
	{
		newline = false;
		for (int i=0;i<dPointNum;i++)
		{
			dPoint[i] = NULL;
		}
		dPointNum = 0;
	}

	dPoint[dPointNum] = translatepoint(0.0, 1.0, 1.0, 0.0, (float)x / (screenWidth), (float)(screenHeight - y) / (screenHeight));
	dPointNum++;
	if (dPointNum>1)
	{
		glBufferData(GL_ARRAY_BUFFER, sizeof(dPoint), dPoint, GL_STATIC_DRAW);
		glDrawArrays(GL_LINE_STRIP, 0, dPointNum);
	}
	

	glFlush();

}
void DrawBreadMan()
{
	glEnable(GL_SCISSOR_TEST);
	glScissor(0, 0, screenWidth, screenHeight - (screenHeight / 7));
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
	glViewport(screenWidth / 12, 0, screenWidth - screenWidth / 6, screenHeight - screenHeight / 6);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	scaleratiox = (double)screenWidth / 640;
	scaleratioy = (double)screenHeight / 480;
	int M = 40 ;
	int L = 3 ;
	point2 q[9999];
	q[0] = point2(115 , 121 );


	for (int i = 1; i < 9999; i++)
	{
		//q[i] = translatepoint(0.0, 1.0, 1.0, 0.0, M * (1 + 2L) - q[i-1].y + abs(q[i-1].x - L * M) / screenWidth, q[i - 1].x / screenHeight);
		q[i].x = M * (1 + 2 * L) - q[i - 1].y + fabs(q[i - 1].x - L * M); 
		q[i].y = q[i - 1].x;
	}
	for (int i =0;i<9999;i++)
	{
		q[i] = translatepoint(0.0, 1.0, 1.0, 0.0, q[i].x * scaleratiox / screenWidth, q[i].y * scaleratioy/ screenHeight);
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(q), q, GL_STATIC_DRAW);

	glDrawArrays(GL_POINTS, 0, 999);
	glFlush();

}

void DrawFern()
{
	srand((unsigned)time(NULL));
	glEnable(GL_SCISSOR_TEST);
	glScissor(0, 0, screenWidth, screenHeight - (screenHeight / 7));
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
	glViewport(screenWidth / 12, 0, screenWidth - screenWidth / 6, screenHeight - screenHeight / 6);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	scaleratiox = (double)screenWidth / 640;
	scaleratioy = (double)screenHeight / 480;
	
	float a[5], b[5], c[5], d[5], tx[5], ty[5];
	a[1] = 0.0; b[1] = 0.0; c[1] = 0.0; d[1]  = 0.16;
	tx[1]  = 0.0; ty[1]  = 0.0; 
		a[2] = 0.2; b[2]  = 0.23; c[2]  = -0.26; d[2]  = 0.22;
	tx[2] = 0.0; ty[2]  = 1.6; 
		a[3] = -0.15; b[3]  = 0.26; c[3]  = 0.28; d[3]  = 0.24;
	tx[3]  = 0.0; ty[3]  = 0.44;
		a[4] = 0.85; b[4]  = -0.04; c[4]  = 0.04; d[4]  = 0.85;
	tx[4]  = 0.0; ty[4] = 1.6; 
	
	point2 pNew[9999];
	pNew[0] = point2(0, 0);

	int index;

	for (int i=1;i<9999;i++)
	{
	double sign = rand() / double(RAND_MAX);
	if (sign<=0.01)
	{
		index = 1;
	}
	else if (sign>0.01&&sign<=0.08)
	{
		index = 2;
	}
	else if (sign>0.08&&sign<=0.15)
	{
		index = 3;
	}
	else if(sign>= 0.15)
	{
		index = 4;
	}
	pNew[i].x  = a[index] * pNew[i-1].x + c[index] * pNew[i-1].y + tx[index];
	pNew[i].y  = b[index] * pNew[i-1].x + d[index] * pNew[i-1].y + ty[index];
	}
	for (int i = 0; i < 9999; i++)
	{
		pNew[i] = translatepoint(0.0, 1.0, 1.0, 0.0, pNew[i].x * 30 * scaleratiox / screenWidth +0.5, pNew[i].y *30 * scaleratioy / screenHeight);
	}



	glBufferData(GL_ARRAY_BUFFER, sizeof(pNew), pNew, GL_STATIC_DRAW);

	glDrawArrays(GL_POINTS, 0, 9999);
	glFlush();
}
int xCount, yCount;

void Keyboard( unsigned char key, int x, int y )
{
	// keyboard handler

	switch ( key ) {
	case 'p':
		mMode = 1;
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, screenWidth, screenHeight - (screenHeight / 7) );
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glViewport(screenWidth / 12  , 0, screenWidth - screenWidth / 6  , screenHeight - screenHeight / 6  );
		DrawFile(filepath[0]);
		break;
	case 't':
		mMode = 2;
		glViewport(screenWidth / 12  , 0, (screenWidth - screenWidth / 6 ) , screenHeight - screenHeight / 6  );
		DrawTile();
		break;
	case 'e':
		mMode = 3;
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, screenWidth, screenHeight - (screenHeight / 7));
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glViewport(0, 0, screenWidth, screenHeight );

		break;
	case 'g':
		mMode = 4;
		
		DrawBreadMan();
		break;
	case 'f':
		mMode = 5;
		DrawFern();
		break;
	case 'c':
		
		if (colorCount == 1)
		{
			fs = "fshader2.glsl";
			colorCount = 2;
		}
		else if (colorCount == 2)
		{
			colorCount = 3;
			fs = "fshader3.glsl";


		}
		else if (colorCount == 3)
		{
			fs = "fshader1.glsl";

			colorCount = 1;
		}
		shaderSetup();
		display();
		switch (mMode)
		{
		case(1):
			Thumbnail(xCount, yCount);
			break;
		case(2):
			DrawTile();
			break;
		case (3):
			glViewport(0, 0, screenWidth - screenWidth / 6, screenHeight - screenHeight / 6);

			DrawMode(xCount, yCount);
			break;
		case(4):
			printf("1123123");
			DrawBreadMan();
			break;
		case (5):
			DrawFern();
			break;
		}
		break;
	case 'b':
		newline = true;
		break;
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
	}
}


void Mouse(int button, int bState, int x, int y)
{
	
	if (button == GLUT_LEFT_BUTTON && bState == GLUT_DOWN )
	{

		switch (mMode)
		{
		case(1):
			Thumbnail(x, y);
			xCount = x;
			yCount = y;
			break;
		case(2):
			DrawTile();
			break;
		case (3):
			glViewport(0, 0, screenWidth - screenWidth / 6, screenHeight - screenHeight / 6);
			xCount = x;
			yCount = y;
			DrawMode(x, y);
			
			break;
		case(4):
			printf("1123123");
			DrawBreadMan();
			break;
		case (5):
			DrawFern();
			break;
		}
	}

}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	screenHeight = h;
	screenWidth = w;
	switch (mMode)
	{
	case(1):
		Thumbnail(xCount,yCount);
		break;
	case(2):
		DrawTile();
		break;
	case (3):
		glViewport(0, 0, screenWidth - screenWidth / 6, screenHeight - screenHeight / 6);

		DrawMode(xCount, yCount);
		break;
	case(4):
		printf("1123123");
		DrawBreadMan();
		break;
	case (5):
		DrawFern();
		break;
	}
	
}
int main( int argc, char **argv )
{
	// main function: program starts here

	glutInit( &argc, argv );                       // intialize GLUT  
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB ); // single framebuffer, colors in RGB
	glutInitWindowSize( screenWidth, screenHeight );                // Window size: 640 wide X 480 high
	glutInitWindowPosition(100,150);               // Top left corner at (100, 150)
	glutCreateWindow( "CS543 Homework 1" );            // Create Window


	glewInit();										// init glew

	initGPUBuffers( );							   // Create GPU buffers
	shaderSetup( );                                // Connect this .cpp file to shader file
	generateGeometry();                           // Call function that generates points to draw

	glutDisplayFunc( display );                    // Register display callback function
	
	
	glutReshapeFunc(reshape);

	glutKeyboardFunc( Keyboard );                  // Register keyboard callback function
	glutMouseFunc( Mouse );
	// Can add minimalist menus here
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	glutMainLoop();
	return 0;
}
