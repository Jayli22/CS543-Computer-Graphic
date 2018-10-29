// Draws colored cube  

#include "Angel.h"


//----------------------------------------------------------------------------
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;
int width = 0;
int height = 0;
point4 *points;
color4 *colors;
float xcoord, ycoord, zcoord;
float yshear;
float xrotate, yrotate, currentrotate;
float scale;
int state;
char *allplyfiles[43] = { "airplane.ply", "ant.ply", "apple.ply", "balance.ply", "beethoven.ply", "big_atc.ply", "big_dodge.ply", "big_porsche.ply",
	 "big_spider.ply", "canstick.ply", "chopper.ply", "cow.ply", "dolphins.ply", "egret.ply", "f16.ply", "footbones.ply", "fracttree.ply",
	 "galleon.ply", "hammerhead.ply", "helix.ply", "hind.ply", "kerolamp.ply", "ketchup.ply", "mug.ply", "part.ply", "pickup_big.ply",
	 "pump.ply", "pumpa_tb.ply", "sandal.ply", "saratoga.ply","scissors.ply", "shark.ply","steeringweel.ply","stratocaster.ply","street_lamp.ply",
	 "teapot.ply","tennis_shoe.ply","tommygun.ply","trashcan.ply","turbine.ply","urn2.ply","walkman.ply","weathervane.ply" };
bool xtranslate, Xtranslate, ytranslate, Ytranslate, ztranslate, Ztranslate,xrotation,isnormalmode;
float breathrange = 0;
point4 *currentpoints;


// remember to prototype
void generateGeometry( void );
void display( void );
void keyboard( unsigned char key, int x, int y );
void DrawPly();
void ClearTranslate();
void DrawNormal();
void DrawBreathing();
void IncrementShearingY();
void DecreaseShearingY();
void TranslatePoint(point4 * point, int number);
mat4 ShearMatrix(GLfloat yshear);
// handle to program
GLuint program;

using namespace std;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)



// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4( -0.5, -0.5,  0.5, 1.0 ),
	point4( -0.5,  0.5,  0.5, 1.0 ),
	point4(  0.5,  0.5,  0.5, 1.0 ),
	point4(  0.5, -0.5,  0.5, 1.0 ),
	point4( -0.5, -0.5, -0.5, 1.0 ),
	point4( -0.5,  0.5, -0.5, 1.0 ),
	point4(  0.5,  0.5, -0.5, 1.0 ),
	point4(  0.5, -0.5, -0.5, 1.0 )
};
// RGBA olors
color4 vertex_colors[8] = {
	color4( 0.0, 0.0, 0.0, 1.0 ),  // black
	color4( 1.0, 0.0, 0.0, 1.0 ),  // red
	color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
	color4( 0.0, 1.0, 0.0, 1.0 ),  // green
	color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
	color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
	color4( 1.0, 1.0, 1.0, 1.0 ),  // white
	color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};



void generateGeometry( void )
{	

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers( 1, &buffer );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );



	// Load shaders and use the resulting shader program
	program = InitShader( "vshader1.glsl", "fshader1.glsl" );
	glUseProgram( program );

	// sets the default color to clear screen
	glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

bool isbreath;
//----------------------------------------------------------------------------
// this is where the drawing should happen
void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window

	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);

	float viewMatrixf[16];
	viewMatrixf[0] = perspectiveMat[0][0];viewMatrixf[4] = perspectiveMat[0][1];
	viewMatrixf[1] = perspectiveMat[1][0];viewMatrixf[5] = perspectiveMat[1][1];
	viewMatrixf[2] = perspectiveMat[2][0];viewMatrixf[6] = perspectiveMat[2][1];
	viewMatrixf[3] = perspectiveMat[3][0];viewMatrixf[7] = perspectiveMat[3][1];

	viewMatrixf[8] = perspectiveMat[0][2];viewMatrixf[12] = perspectiveMat[0][3];
	viewMatrixf[9] = perspectiveMat[1][2];viewMatrixf[13] = perspectiveMat[1][3];
	viewMatrixf[10] = perspectiveMat[2][2];viewMatrixf[14] = perspectiveMat[2][3];
	viewMatrixf[11] = perspectiveMat[3][2];viewMatrixf[15] = perspectiveMat[3][3];
	
	Angel::mat4 modelMat = Angel::identity();
	modelMat = modelMat * ShearMatrix(yshear) * Angel::Translate(xcoord, ycoord, zcoord) * Angel::RotateY(yrotate) * Angel::RotateX(xrotate);
	float modelMatrixf[16];
	modelMatrixf[0] = modelMat[0][0];modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0];modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0];modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0];modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2];modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2];modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2];modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2];modelMatrixf[15] = modelMat[3][3];
	
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, viewMatrixf);

	if (isnormalmode)
	{
		DrawNormal();

	}
	if (isbreath)
	{
		DrawBreathing();
	}
	else
	{
		DrawPly(); 
	}
	glFlush(); // force output to graphics hardware
	free(points);
	free(colors);
	// use this call to double buffer
	glutSwapBuffers();
	// you can implement your own buffers with textures
}
struct face3 {
	int number;
	int point1;
	int point2;
	int point3;
};
struct PlyFile
{
	int vertexnum;
	int facenum;
	point4 *flyvertex;
	face3 *flyface;

};

//----------------------------------------------------------------------------
PlyFile ReadPlyFile(char *filename)
{
	FILE *fp = NULL;
	char filetype[5];
	char temp1[20],temp2[20],temp3[20],temp4[20],temp5[20];
	PlyFile tplyfile;

	fp = fopen(filename,"r");
	if (fp == NULL) {
		printf("can not find file\n");
		exit(0);
	}
	fscanf(fp, "%s",&filetype); 

	if(strcmp(filetype,"ply")!=0){
		printf("This is not ply file\n");
		exit(0);
	}

	fscanf(fp, "%s %s %f",&temp1,&temp2,&temp3);
	fscanf(fp, "%s %s %d",&temp1,&temp2,&tplyfile.vertexnum); 

	fscanf(fp, "%s %s %s",&temp1,&temp2,&temp3);
	fscanf(fp, "%s %s %s",&temp1,&temp2,&temp3);
	fscanf(fp, "%s %s %s",&temp1,&temp2,&temp3);
	fscanf(fp, "%s %s %d",&temp1,&temp2,&tplyfile.facenum);
	fscanf(fp, "%s %s %s %s %s\n",&temp1,&temp2,&temp3,&temp4,&temp5);
	fscanf(fp, "%s",&temp1);

	tplyfile.flyvertex = (point4 *)malloc(sizeof(point4)*tplyfile.vertexnum);
	for(int i=0;i< tplyfile.vertexnum;i++){
		fscanf(fp, "%f %f %f", &tplyfile.flyvertex[i].x, &tplyfile.flyvertex[i].y, &tplyfile.flyvertex[i].z);

	}
	tplyfile.flyface = (face3 *)malloc(sizeof(face3)*tplyfile.facenum);
	for(int i=0;i< tplyfile.facenum;i++){
		fscanf(fp, "%d %d %d %d", &tplyfile.flyface[i].number, &tplyfile.flyface[i].point1, &tplyfile.flyface[i].point2, &tplyfile.flyface[i].point3);

	}
	
	fclose(fp);
	return tplyfile;
}

int currentmodle = 0;

int filldrawing() //
{
	char filename[100];

	int t=0;
	strcpy(filename, "ply_files\\");
	strcat(filename, allplyfiles[currentmodle]);
	PlyFile plyfile = ReadPlyFile(filename);
	int inumber = plyfile.facenum * 3;
	points = (point4 *)malloc(sizeof(point4)*inumber);
	colors = (color4 *)malloc(sizeof(color4)*inumber);

	for (int i = 0; i < plyfile.facenum; i++)
	{
		points[t] = plyfile.flyvertex[plyfile.flyface[i].point1];
		points[t].w = 1.0;
		colors[t] = vertex_colors[4]; 
		points[t + 1] = plyfile.flyvertex[plyfile.flyface[i].point2];
		points[t + 1].w = 1.0;
		colors[t + 1] = vertex_colors[4]; 
		points[t + 2] = plyfile.flyvertex[plyfile.flyface[i].point3];
		points[t + 2].w = 1.0;
		colors[t + 2] = vertex_colors[4]; 
		t += 3;
		i++;
	}

	return t;
}

int currentplyfacenum;
point4 * currentplypoint;
void DrawPly()
{
	int tnumber;
	tnumber = filldrawing();
	currentplyfacenum = tnumber;
	TranslatePoint(points, tnumber);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBufferData(GL_ARRAY_BUFFER, sizeof(point4)*tnumber + sizeof(color4)*tnumber, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4)*tnumber, points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*tnumber, sizeof(color4)*tnumber, colors);
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4)*tnumber));
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, tnumber);
	glDisable(GL_DEPTH_TEST);

	free(currentpoints);
	currentpoints = (point4 *)malloc(sizeof(point4)*currentplyfacenum);
	memcpy(currentpoints, points, sizeof(point4)*currentplyfacenum);
}


//translate point coordinate
void TranslatePoint(point4 *point, int pointnumber) {
	float xratio, yratio, zratio;
	float xmax, xmin, ymax, ymin, zmax, zmin;
	float xoffset, yoffset,zoffset;
	xmax = point[0].x;
	xmin = point[0].x;
	ymax = point[0].y;
	ymin = point[0].y;
	zmax = point[0].z;
	zmin = point[0].z;

	//printf("%f", xmax);
	for (int i = 0; i < pointnumber; i++) {
		if (point[i].x > xmax) {
			xmax = point[i].x;
		}
		if (point[i].x < xmin) {
			xmin = point[i].x;
		}
		if (point[i].y > ymax) {
			ymax = point[i].y;
		}
		if (point[i].y < ymin) {
		ymin = point[i].y;
	}
		if (point[i].z > zmax) {
			zmax = point[i].z;
		}
		if (point[i].z < zmin) {
			zmin = point[i].z;
		}
	}
	xratio = 0.5/ (xmax - xmin);
	yratio =  0.5/(ymax - ymin);
	zratio =  0.5/(zmax - zmin);

	xoffset = xmax - (xmax - xmin) / 2;
	yoffset = ymax - (ymax - ymin) / 2;
	zoffset = zmax - (zmax - zmin) / 2;

	if (xratio < yratio) {
		for (int i = 0; i < pointnumber; i++) {
			point[i].x = (point[i].x - xoffset)*xratio;
			point[i].y = (point[i].y - yoffset)*xratio;
			point[i].z = (point[i].z)*xratio;
		}
	}

	else {
		for (int i = 0; i < pointnumber; i++) {
			point[i].x = (point[i].x - xoffset)*yratio;
			point[i].y = (point[i].y - yoffset)*yratio;
			point[i].z = (point[i].z)*yratio;
		}
	}
}
void DrawNextPly()
{
	if (currentmodle == 42)
	{
		currentmodle = 0;
	}
	else
	{
		currentmodle++;

	}
}
void DrawPrePly()
{
	if (currentmodle == 0)
	{
		currentmodle = 42;
	}
	else
	{
		currentmodle--;

	}
}
void InitState()
{
	xcoord = 0.0f;
	ycoord = 0.0f;
	zcoord = 0.0f;
	yrotate = 0.0f;
	xrotate = 0.0f;
	scale = 0.0f;
	yshear = 0.0f;
	currentmodle = 0;
}
void Translate()
{
	if (state == 1) {
		if (Xtranslate == TRUE)
		{
			Xtranslate = FALSE;
		}
		else
		{
			Xtranslate = TRUE;

		}
	}
	if (state == 2) {
		if (xtranslate == TRUE) 
		{
			xtranslate = FALSE;
		}
		else
		{
			xtranslate = TRUE;
		}
	}
	if (state == 3) {
		if (Ytranslate == TRUE)
		{
			Ytranslate = FALSE;
		}
		else {
			Ytranslate = TRUE;
		}
	}
	if (state == 4) {
		if (ytranslate == TRUE)
		{
			ytranslate = FALSE;
		}
		else {
			ytranslate = TRUE;
		}
	}
	if (state == 5) {
		if (Ztranslate == TRUE)
		{
			Ztranslate = FALSE;
		}
		else
		{
			Ztranslate = TRUE;
		}
	}
	if (state == 6) {
		if (ztranslate == TRUE)
		{
			ztranslate = FALSE;
		}
		else
		{
			ztranslate = TRUE;
		}
	}
	if (state == 7) {
		if (xrotation == TRUE)
		{
			xrotation = FALSE;
		}
		else {
			xrotation = TRUE;
		}
	}
}



void DrawNormal() {
	point4  *normals;
	int i = 0, t = 0;
	int normalpointnumber;
	color4 *normalcolors;
	normalpointnumber = currentplyfacenum / 3 * 2;
	normals = (point4 *)malloc(sizeof(point4)*normalpointnumber);
	normalcolors = (color4 *)malloc(sizeof(color4)*normalpointnumber);

	while (t < currentplyfacenum) {
		normals[i].x = (currentpoints[t].x + currentpoints[t + 1].x + currentpoints[t + 2].x) / 3;
		normals[i].y = (currentpoints[t].y + currentpoints[t + 1].y + currentpoints[t + 2].y) / 3;
		normals[i].z = (currentpoints[t].z + currentpoints[t + 1].z + currentpoints[t + 2].z) / 3;
		normals[i].w = 1.0f;
		normals[i + 1].x = (currentpoints[t].y - currentpoints[t + 1].y)*(currentpoints[t].z + currentpoints[t + 1].z) +
			(currentpoints[t + 1].y - currentpoints[t + 2].y)*(currentpoints[t + 1].z + currentpoints[t + 2].z) +
			(currentpoints[t + 2].y - currentpoints[t].y)*(currentpoints[t + 2].z + currentpoints[t].z);
		normals[i + 1].y = (currentpoints[t].z - currentpoints[t + 1].z)*(currentpoints[t].x + currentpoints[t + 1].x) +
			(currentpoints[t + 1].z - currentpoints[t + 2].z)*(currentpoints[t + 1].x + currentpoints[t + 2].x) +
			(currentpoints[t + 2].z - currentpoints[t].z)*(currentpoints[t + 2].x + currentpoints[t].x);
		normals[i + 1].z = (currentpoints[t].x - currentpoints[t + 1].x)*(currentpoints[t].y + currentpoints[t + 1].y) +
			(currentpoints[t + 1].x - currentpoints[t + 2].x)*(currentpoints[t + 1].y + currentpoints[t + 2].y) +
			(currentpoints[t + 2].x - currentpoints[t].x)*(currentpoints[t + 2].y + currentpoints[t].y);
		float normalunit = sqrt(normals[i + 1].x*normals[i + 1].x +
			normals[i + 1].y*normals[i + 1].y + normals[i + 1].z*normals[i + 1].z);
		normals[i + 1].x = normals[i + 1].x / normalunit;
		normals[i + 1].y = normals[i + 1].y / normalunit;
		normals[i + 1].z = normals[i + 1].z / normalunit;
		normals[i + 1].x = float(normals[i].x + normals[i + 1].x*0.1);
		normals[i + 1].y = float(normals[i].y + normals[i + 1].y*0.1);
		normals[i + 1].z = float(normals[i].z + normals[i + 1].z*0.1);
		normals[i + 1].w = 1.0f;

		i = i + 2;
		t = t + 3;
	}

	for (int j = 0; j < normalpointnumber; j++) {
		normalcolors[j] = vertex_colors[1]; //red
	}
	

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point4)*normalpointnumber + sizeof(color4)*normalpointnumber, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4)*normalpointnumber, normals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*normalpointnumber, sizeof(color4)*normalpointnumber, normalcolors);
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4)*normalpointnumber));
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_LINES, 0, normalpointnumber);
	glDisable(GL_DEPTH_TEST);

	//recycle
	free(normals);
	free(normalcolors);

}
void DrawBreathing()
{
	int tnum;
	tnum = filldrawing();
	currentplyfacenum = tnum;
	TranslatePoint(points, tnum);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	point4  facenormal[1];
	int index = 0;
	while (index < tnum) {
		facenormal[0].x = (points[index].y - points[index + 1].y)*(points[index].z + points[index + 1].z) +
			(points[index + 1].y - points[index + 2].y)*(points[index + 1].z + points[index + 2].z) +
			(points[index + 2].y - points[index].y)*(points[index + 2].z + points[index].z);
		facenormal[0].y = (points[index].z - points[index + 1].z)*(points[index].x + points[index + 1].x) +
			(points[index + 1].z - points[index + 2].z)*(points[index + 1].x + points[index + 2].x) +
			(points[index + 2].z - points[index].z)*(points[index + 2].x + points[index].x);
		facenormal[0].z = (points[index].x - points[index + 1].x)*(points[index].y + points[index + 1].y) +
			(points[index + 1].x - points[index + 2].x)*(points[index + 1].y + points[index + 2].y) +
			(points[index + 2].x - points[index].x)*(points[index + 2].y + points[index].y);
		facenormal[0].w = 1.0f;

		float normalunit = sqrt(facenormal[0].x*facenormal[0].x + facenormal[0].y*facenormal[0].y + facenormal[0].z*facenormal[0].z);
		facenormal[0].x = facenormal[0].x / normalunit;
		facenormal[0].y = facenormal[0].y / normalunit;
		facenormal[0].z = facenormal[0].z / normalunit;



		points[index].x = points[index].x + facenormal[0].x*breathrange;
		points[index].y = points[index].y + facenormal[0].y*breathrange;
		points[index].z = points[index].z + facenormal[0].z*breathrange;
		points[index].w = 1.0f;
		colors[index] = vertex_colors[4];
		points[index + 1].x = points[index + 1].x + facenormal[0].x*breathrange;
		points[index + 1].y = points[index + 1].y + facenormal[0].y*breathrange;
		points[index + 1].z = points[index + 1].z + facenormal[0].z*breathrange;
		points[index + 1].w = 1.0f;
		colors[index + 1] = vertex_colors[4]; 
		points[index + 2].x = points[index + 2].x + facenormal[0].x*breathrange;
		points[index + 2].y = points[index + 2].y + facenormal[0].y*breathrange;
		points[index + 2].z = points[index + 2].z + facenormal[0].z*breathrange;
		points[index + 2].w = 1.0f;
		colors[index + 2] = vertex_colors[4]; 

		index += 3;
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point4)*tnum + sizeof(color4)*tnum, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4)*tnum, points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*tnum, sizeof(color4)*tnum, colors);
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4)*tnum));
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, index);
	glDisable(GL_DEPTH_TEST);

	free(currentplypoint);
	currentpoints = (point4 *)malloc(sizeof(point4)*currentplyfacenum);
	memcpy(currentpoints, points, sizeof(point4)*currentplyfacenum);
}
void IncrementShearingY() {
	yshear += 0.1;
}
void DecreaseShearingY() {
	yshear -= 0.1;
}
mat4 ShearMatrix(GLfloat yshear) {
	mat4 y;
	y = Angel::identity();
	GLfloat angle = DegreesToRadians * (90 - yshear);
	y[1][0] = cos(angle) / sin(angle);
	//y[1][2] = cos(angle) / sin(angle);
	return y;
}

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
	switch ( key ) {
	case 033:
		exit( EXIT_SUCCESS );
		break;
	case 'W':
		state = 0;
		InitState();
		break;
	case 'P':
		DrawNextPly();
		break;
	case 'N':
		DrawPrePly();
		break;
	case 'X':
		state = 1;
		ClearTranslate();

		Translate();
		break;
	case 'x':
		state = 2;
		ClearTranslate();
		Translate();
		break;
	case 'Y':
		state = 3;
		ClearTranslate();

		Translate();
		break;
	case 'y':
		state = 4;
		ClearTranslate();

		Translate();
		break;
	case'Z':
		state = 5;
		ClearTranslate();

		Translate();

		break;
	case 'z':
		state = 6;
		ClearTranslate();

		Translate();
		break;
	case'R':
		state = 7;
		ClearTranslate();

		Translate();
		break;
	case 'B':
		if (isbreath)
		{
			isbreath = false;
		}
		else
		{
			isbreath = true;
		}
		break;
	case 'm':
		if (isnormalmode)
		{
			isnormalmode = false;
		}
		else
		{
			isnormalmode = true;
		}
		break;
	case'h':
		IncrementShearingY();
		break;
	case 'H':
		DecreaseShearingY();
		break;
	}
}
bool isexpire = true;
void IdleFunction()
{
	
	if (state == 1)
	{
		if (Xtranslate)
		{
			xcoord += 0.001f;
		}
		else
		{

		}
	}
	if (state == 2)
	{
		if (xtranslate)
		{
			xcoord -= 0.001f;
		}
		else
		{

		}
	}
	if (state == 5)
	{
		if (Ztranslate)
		{
			zcoord += 0.001f;
		}
		else
		{

		}
	}if (state == 6)
	{
		if (ztranslate)
		{
			zcoord -= 0.001f;
		}
		else
		{

		}
	}if (state == 3)
	{
		if (Ytranslate)
		{
			ycoord += 0.001f;
		}
		else
		{

		}
	}if (state == 4)
	{
		if (ytranslate)
		{
			ycoord -= 0.001f;
		}
		else
		{

		}
	}
	if (state ==7)
	{
		if (xrotation)
		{
			xrotate += 0.1;
		}
		else
		{

		}
	}
	if (isbreath)
	{
		if (isexpire)
		{
			breathrange += 0.00002;

			if (breathrange > 0.006)
			{
				isexpire = false;
			}
		}
		else
		{
			breathrange -= 0.00002;
			if (breathrange<0.0006)
			{
				isexpire = true;
			}
		}

	}
	glutPostRedisplay();

}
void ClearTranslate()
{
	if (state == 1)
	{
		
		xtranslate = false;
		Ytranslate = false;
		Ytranslate = false;
		Ztranslate = false;
		ztranslate = false;
		xrotation = false;
	}
	if (state == 2)
	{
		Xtranslate = false;
		
		Ytranslate = false;
		ytranslate = false;
		Ztranslate = false;
		ztranslate = false;
		xrotation = false;
	}
	if (state == 3)
	{
		Xtranslate = false;
		xtranslate = false;
		
		ytranslate = false;
		Ztranslate = false;
		ztranslate = false;
		xrotation = false;
	}if (state == 4)
	{
		Xtranslate = false;
		xtranslate = false;
		Ytranslate = false;

		Ztranslate = false;
		ztranslate = false;
		xrotation = false;
	}if (state == 5)
	{
		Xtranslate = false;
		xtranslate = false;
		Ytranslate = false;
		Ytranslate = false;
		
		ztranslate = false;
		xrotation = false;
	}if (state == 6)
	{
		Xtranslate = false;
		xtranslate = false;
		Ytranslate = false;
		Ytranslate = false;
		Ztranslate = false;
		xrotation = false;
	}
	if (state == 7)
	{
		Xtranslate = false;
		xtranslate = false;
		Ytranslate = false;
		Ytranslate = false;
		Ztranslate = false;
		ztranslate = false;
	}
}
//----------------------------------------------------------------------------
// entry point
int main( int argc, char **argv )
{
	// init glut
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( 512, 512 );
	width = 512;
	height = 512;

	// create window
	// opengl can be incorperated into other packages like wxwidgets, fltoolkit, etc.
	glutCreateWindow( "Color Cube" );

	// init glew
	glewInit();

	generateGeometry();

	// assign handlers
	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutIdleFunc(IdleFunction);
	// should add menus
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	// frame rate can be controlled with 
	glutMainLoop();
	return 0;
}
