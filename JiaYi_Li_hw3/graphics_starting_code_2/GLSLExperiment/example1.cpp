// Draws colored cube  

#include "Angel.h"
#include <time.h>
using namespace std;

//----------------------------------------------------------------------------
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;
int width = 0;
int height = 0;
point4 *points;
color4 *colors;
float yshear;
float xrotate, yrotate, currentrotate;
float scale;
int state;
char *allplyfiles[45] = { "airplane.ply", "ant.ply", "apple.ply", "balance.ply", "beethoven.ply", "big_atc.ply", "big_dodge.ply", "big_porsche.ply",
	 "big_spider.ply", "canstick.ply", "chopper.ply", "cow.ply", "dolphins.ply", "egret.ply", "f16.ply", "footbones.ply", "fracttree.ply",
	 "galleon.ply", "hammerhead.ply", "helix.ply", "hind.ply", "kerolamp.ply", "ketchup.ply", "mug.ply", "part.ply", "pickup_big.ply",
	 "pump.ply", "pumpa_tb.ply", "sandal.ply", "saratoga.ply","scissors.ply", "shark.ply","steeringweel.ply","stratocaster.ply","street_lamp.ply",
	 "teapot.ply","tennis_shoe.ply","tommygun.ply","trashcan.ply","turbine.ply","urn2.ply","walkman.ply","weathervane.ply",
"cylinder.ply","sphere.ply" };
bool xtranslate, Xtranslate, ytranslate, Ytranslate, ztranslate, Ztranslate,xrotation,isnormalmode;
float breathrange = 0;
point4 *currentpoints;
int currentmodle = 0;
bool planetarget = false;

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

struct replacestr {
	char repold;
	char repnew;
};
struct rulestr {
	char ruleold;
	char rulenew[100];
};
struct Lsysfile {
	int  len;
	int	 iter;
	point4 rot;
	int	repcount;
	replacestr rep[10];
	char lstart;
	rulestr lrule[10];
	int rulecount;
}; 


int trunkcolor,leafcolor;
 struct CurrentState {
	mat4 cmatrix;
	point4 cpoints;
	point4 corientation;
}; 
int gstate = 0;
int drawnum ; 
int gmodelmatrix = 0;
CurrentState statestack[9999];
mat4 modlematrixstack[9999]; 
int  globalcuboidnumber;



void generateGeometry( void );
void display( void );
void keyboard( unsigned char key, int x, int y );
void DrawPly();

void TranslatePoint(point4 * point, int number);
Lsysfile ReadLsysfile(char *filename);
char* Grammar(Lsysfile);

int CountingDrawing(); //

void DrawPlane();
void DrawJoints(point4 , point4 );
void PushState(CurrentState);
CurrentState PopState();
void DrawTree(char);
void DrawThing();
void DrawPark();
void 	InitState();

// handle to program
GLuint program;

using namespace std;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
	

// RGBA olors
color4 vertex_colors[9] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(0.0, 1.0, 1.0, 1.0),   // cyan
	color4(1.0, 0.5, 0.0, 1.0), // brown
	color4(0.5, 0.5, 0.5, 1.0) // gray
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
	glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background'
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//----------------------------------------------------------------------------
// this is where the drawing should happen
void display( void )
{

	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0f, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);

	float projectionmatrixf[16];
	projectionmatrixf[0] = perspectiveMat[0][0];projectionmatrixf[4] = perspectiveMat[0][1];
	projectionmatrixf[1] = perspectiveMat[1][0];projectionmatrixf[5] = perspectiveMat[1][1];
	projectionmatrixf[2] = perspectiveMat[2][0];projectionmatrixf[6] = perspectiveMat[2][1];
	projectionmatrixf[3] = perspectiveMat[3][0];projectionmatrixf[7] = perspectiveMat[3][1];

	projectionmatrixf[8] = perspectiveMat[0][2];projectionmatrixf[12] = perspectiveMat[0][3];
	projectionmatrixf[9] = perspectiveMat[1][2];projectionmatrixf[13] = perspectiveMat[1][3];
	projectionmatrixf[10] = perspectiveMat[2][2];projectionmatrixf[14] = perspectiveMat[2][3];
	projectionmatrixf[11] = perspectiveMat[3][2];projectionmatrixf[15] = perspectiveMat[3][3];
	

	

	Angel::mat4 view = Angel::LookAt( 
		Angel::vec3(0, 0, 3),
		Angel::vec3(0, 0, 0), 
		Angel::vec3(0, 1, 0)  
	);
	float viewmatrixf[16];
	viewmatrixf[0] = view[0][0];viewmatrixf[4] = view[0][1];
	viewmatrixf[1] = view[1][0];viewmatrixf[5] = view[1][1];
	viewmatrixf[2] = view[2][0];viewmatrixf[6] = view[2][1];
	viewmatrixf[3] = view[3][0];viewmatrixf[7] = view[3][1];

	viewmatrixf[8] = view[0][2];viewmatrixf[12] = view[0][3];
	viewmatrixf[9] = view[1][2];viewmatrixf[13] = view[1][3];
	viewmatrixf[10] = view[2][2];viewmatrixf[14] = view[2][3];
	viewmatrixf[11] = view[3][2];viewmatrixf[15] = view[3][3];
	
	// set up projection matricies
	GLuint projectionMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv(projectionMatrix, 1, GL_FALSE, projectionmatrixf);


	GLuint viewMatrix = glGetUniformLocationARB(program, "view_matrix");
	glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, viewmatrixf);



}

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
Lsysfile ReadLsysfile(char *filename)
{
	char *name;
	FILE *fp = NULL;
	Lsysfile lsysf;
	int repcount, rulecount;
	char line[999];

	repcount = 0;
	rulecount = 0;

	name = (char *)malloc(sizeof(char) * (strlen(filename) + 5));
	strcpy(name, filename);


	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("can't open file.\n");
		exit(0);
	}

	while (!feof(fp)) {
		memset(line, 0, 999);
		fscanf(fp, "%s", &line);
		if (strcmp(line, "#") == 0) {
			fgets(line, 255, fp);
		}
		else if (strcmp(line, "len:") == 0) {
			fscanf(fp, "%d", &lsysf.len);
		}
		else if (strcmp(line, "iter:") == 0) {
			fscanf(fp, "%d", &lsysf.iter);
		}
		else if (strcmp(line, "start:") == 0) {
			fgetc(fp);
			fscanf(fp, "%c", &lsysf.lstart);
		}
		else if (strcmp(line, "rot:") == 0) {
			fscanf(fp, "%f %f %f", &lsysf.rot.x, &lsysf.rot.y, &lsysf.rot.z);
			lsysf.rot.w = 1.0f;
		}
		else if (strcmp(line, "rep:") == 0) {
			fgetc(fp);
			fscanf(fp, "%c", &lsysf.rep[repcount].repold);
			fgetc(fp);
			fscanf(fp, "%c", &lsysf.rep[repcount].repnew);
			if (lsysf.rep[repcount].repnew == '\n') {
				printf("The new rep is empty\n");
			}
			repcount++;
		}
		else {
			if (line[0] == '\n' || line[0] == '\t' || line[0] == '\0') {
				printf("Read finish\n");
				break;
			}
			lsysf.lrule[rulecount].ruleold = line[0];
			fgetc(fp);
			fscanf(fp, "%s", &lsysf.lrule[rulecount].rulenew);
			rulecount++;
		}
	}
	lsysf.repcount = repcount;
	lsysf.rulecount = rulecount;
	fclose(fp);
	return lsysf;
}
char* Grammar(Lsysfile lsysf) {
	int  k;
	bool loopmark;
	char tempstring[99999] = "\0";
	char finalstring[99999] = "\0";

	for (int i = 0; i < lsysf.iter; i++) {
		k = 0;
		finalstring[0] = lsysf.lstart;
		loopmark = FALSE;

		while (finalstring[k] != NULL) {
			for (int j = 0; j < lsysf.rulecount; j++) {
				if (finalstring[k] == lsysf.lrule[j].ruleold) {
					strcat(tempstring, lsysf.lrule[j].rulenew);
					loopmark = TRUE;
					break;
				}
			}
			if (!loopmark) {
				tempstring[strlen(tempstring)] = finalstring[k];
			}
			loopmark = FALSE;
			k++;
		}
		strcpy(finalstring, tempstring);
		memset(tempstring, 0, 99999);
	}
	if (lsysf.repcount > 0) {
		k = 0;
		while (finalstring[k] != NULL) {
			for (int j = 0; j < lsysf.repcount; j++) {
				if (finalstring[k] == lsysf.rep[j].repold) {
					/*if (lsysf.rep[j].repnew == '\n') {
						loopmark = TRUE;
						break;
					}*/
					
						tempstring[strlen(tempstring)] = lsysf.rep[j].repnew;
						loopmark = TRUE;
						break;
					
				}
			}
			if (!loopmark) { //if no change
				tempstring[strlen(tempstring)] = finalstring[k];
			}
			loopmark = FALSE;
			k++;
		}
		strcpy(finalstring, tempstring);
	}
	printf("End analyze\n");
	printf("$%s\n",&finalstring);

	return finalstring;
}
void DrawThing() {
	float x, y,z;
	drawnum = 3;

	//srand((int)time(0));
	int tnum;
	int randommodle = rand() % 6;
	switch (randommodle)
	{
	case 0:
		currentmodle = 1;  // ant
		break;
	case 1:
		currentmodle = 34; // street lamp
		break;
	case 2:
		currentmodle = 5; //big atc
		break;
	case 3:
		currentmodle = 6; //big doage
		break;
	case 4:
		currentmodle = 7; // BIG PORSCHE
		break;
	case 5:
		currentmodle = 25; // PICKUP BIG
		break;
	}


	x = float(rand() % 1800 -900)/1000 ; 
	y = float(rand() % 1800 - 1000)/1000; 
	z = float(rand() % 2000 - 1000) / 1000;


	Angel::mat4 modelMat = Angel::identity();
	modelMat = modelMat * Angel::Translate(x, y, z) * Angel::RotateY(0.0f) * Angel::Scale(0.1);
	float modelMatrixf[16];
	modelMatrixf[0] = modelMat[0][0]; modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0]; modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0]; modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0]; modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2]; modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2]; modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2]; modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2]; modelMatrixf[15] = modelMat[3][3];

	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf);

	
	tnum = CountingDrawing();
	TranslatePoint(points, tnum);

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
	glDrawArrays(GL_TRIANGLES, 0, tnum);
	glDisable(GL_DEPTH_TEST);

	printf("Draw car\n");

	glFlush();
}



void DrawPlane() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	planetarget = true;
	Angel::mat4 modelMat = Angel::identity();
	modelMat = modelMat * Angel::Translate(0, 0, 0) * Angel::RotateX(60.0f) * Angel::RotateY(0.0f) *Angel::RotateZ(0.0f);
	float modelMatrixf[16];
	modelMatrixf[0] = modelMat[0][0]; modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0]; modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0]; modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0]; modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2]; modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2]; modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2]; modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2]; modelMatrixf[15] = modelMat[3][3];

	//matrix
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMat);

	points = (point4 *)malloc(sizeof(point4)*4);
	colors = (color4 *)malloc(sizeof(color4)* 4);
	for (int icount = 0; icount < 4; icount++) {
		colors[icount] = vertex_colors[8];
	}
	points[0].x = -1.5f; points[0].y = -2.0f; points[0].z = 0.0f; points[0].w = 1.0f;
	points[1].x = -1.5f; points[1].y = 3.0f; points[1].z = 0.0f; points[1].w = 1.0f;
	points[2].x = 1.5f; points[2].y = 3.0f; points[2].z = 0.0f; points[2].w = 1.0f;
	points[3].x = 1.5f; points[3].y = -2.0f; points[3].z = 0.0f; points[3].w = 1.0f;


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point4)* 4 + sizeof(color4)* 4, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4)* 4, points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)* 4, sizeof(color4)* 4, colors);
	//reset buffer position
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4)* 4));
	glDrawArrays(GL_QUADS, 0, 4);
	glFlush(); // force output to graphics hardware

	printf("Draw Plane\n");

	free(points);
	free(colors);
}




void DrawTree(char treenum) {
	if (planetarget)
	{

	}
	else glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float treesize;
	char *finalstring;
	Lsysfile lsysf;
	if (treenum == 'a') 
	{
		lsysf = ReadLsysfile("lsys_files\\lsys1.txt");
		finalstring = Grammar(lsysf);
		treesize = 0.01f; 
	}
	else if (treenum == 'b') 
	{
		lsysf = ReadLsysfile("lsys_files\\lsys2.txt");
		finalstring = Grammar(lsysf);
		treesize = 0.04f;
	}
	else if (treenum == 'c') 
	{
		lsysf = ReadLsysfile("lsys_files\\lsys3.txt");
		finalstring = Grammar(lsysf);
		treesize = 0.1f;
	}
	else if (treenum == 'd') 
	{
		lsysf = ReadLsysfile("lsys_files\\lsys4.txt");
		finalstring = Grammar(lsysf);
		treesize = 0.1f;
	}
	else if (treenum == 'e') 
	{
		lsysf = ReadLsysfile("lsys_files\\lsys5.txt");
		finalstring = Grammar(lsysf);
		treesize = 0.02f;
	}

	//tree matrix
	Angel::mat4 treemodelMat = Angel::identity();
	treemodelMat = treemodelMat * Angel::RotateY(0.0f)* Angel::RotateX(0.0f) * Angel::RotateZ(0.0f); 


	point4 startpoint;
	startpoint.x = (float)(rand() % 2000 - 1000) / 1000;
	startpoint.y = (float)(rand() % 2000 - 1500) / 1000;   
	startpoint.z = (float)(rand() % 1000 - 1000) / 1000; ; 
	startpoint.w = 1.0f;

	vec4 orientation; //start vertex
	orientation.x = 0.0f;
	orientation.y = 1.0f;  //vector forward y axis
	orientation.z = 0.0f;
	orientation.w = 0.0f;

	int stringlength = strlen(finalstring);
	points = (point4 *)malloc(sizeof(point4)*stringlength);
	colors = (color4 *)malloc(sizeof(color4)*stringlength);
	while (true)
	{
		srand((unsigned)time(NULL));
		trunkcolor = rand() % 8;
		leafcolor = rand() % 3;
		if (leafcolor + 1 != trunkcolor)
		{
			break;
		}
	}
	int count = 0;
	point4 currentpoint = startpoint;
	point4 tpoint[2];
	CurrentState currentstate;
	while (finalstring[count] != NULL)   // grammar
	{ 
		if (finalstring[count] == 'F') 
		{ 
			//draw cylinder tree
			tpoint[0] = currentpoint;
			orientation = treemodelMat * orientation;
			tpoint[1].x = currentpoint.x + orientation.x /   lsysf.len*treesize; 
			tpoint[1].y = currentpoint.y + orientation.y /   lsysf.len*treesize;
			tpoint[1].z = currentpoint.z + orientation.z /   lsysf.len*treesize;
			tpoint[1].w = 1.0f;
			DrawJoints(tpoint[0], tpoint[1]);
			treemodelMat = Angel::identity();
			currentpoint = tpoint[1];
			currentstate.corientation = orientation;
			currentstate.cpoints = currentpoint;
			currentstate.cmatrix = treemodelMat;
		}
		else if (finalstring[count] == '+') 
		{
			//Apply a positive rotatation about the X-axis of xrot degrees.
			treemodelMat = treemodelMat * Angel::RotateZ(lsysf.rot.x); // x switch with z
			currentstate.cmatrix = treemodelMat;
		}
		else if (finalstring[count] == '-')
		{
			//Apply a negative rotatation about the X-axis of xrot degrees.
			treemodelMat = treemodelMat * Angel::RotateZ(-lsysf.rot.x);
			currentstate.cmatrix = treemodelMat;
		}
		else if (finalstring[count] == '&') 
		{
			//Apply a positive rotatation about the Y-axis of yrot degrees.
			treemodelMat = treemodelMat * Angel::RotateY(lsysf.rot.y);
			currentstate.cmatrix = treemodelMat;
		}
		else if (finalstring[count] == '^') 
		{
			//Apply a negative rotatation about the Y-axis of yrot degrees.
			treemodelMat = treemodelMat * Angel::RotateY(-lsysf.rot.y);
			currentstate.cmatrix = treemodelMat;
		}
		else if (finalstring[count] == '\\') 
		{
			//Apply a positive rotatation about the Z-axis of zrot degrees.
			treemodelMat = treemodelMat * Angel::RotateX(lsysf.rot.z);
			currentstate.cmatrix = treemodelMat;
		}
		else if (finalstring[count] == '/') 
		{
			//Apply a negative rotatation about the Z-axis of zrot degrees.
			treemodelMat = treemodelMat * Angel::RotateX(-lsysf.rot.z);
			currentstate.cmatrix = treemodelMat;
		}
		else if (finalstring[count] == '|') 
		{
			//Turn around 180 degrees.
			treemodelMat = treemodelMat * Angel::RotateX(180.0f);
			currentstate.cmatrix = treemodelMat;
		}
		else if (finalstring[count] == '[') 
		{
			//Push the current state of the turtle onto a pushdown stack.
			PushState(currentstate); 
		}
		else if (finalstring[count] == ']') 
		{
			//Pop the state from the top of the turtle stack, and make it the current turtle stack.
			currentstate = PopState();
			currentpoint = currentstate.cpoints;
			orientation = currentstate.corientation;
			treemodelMat = currentstate.cmatrix;
		}
		count++;
	}

	glFlush();
	free(points);
	free(colors);
}


void DrawJoints(point4 point1, point4 point2)
{
	float branchsize = 0.002;

	Angel::mat4 modelMat = Angel::identity();
	modelMat = modelMat * Angel::Translate(point1)* Angel::Scale(branchsize * 5);

	float modelMatrixf[16];
	modelMatrixf[0] = modelMat[0][0]; modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0]; modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0]; modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0]; modelMatrixf[7] = modelMat[3][1];
	modelMatrixf[8] = modelMat[0][2]; modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2]; modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2]; modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2]; modelMatrixf[15] = modelMat[3][3];
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf);

	currentmodle = 44; //sphere
	drawnum = 1;
	DrawPly(); 

	// vector connect two points
	vec3 vector;
	vector.x = point2.x - point1.x;
	vector.y = point2.y - point1.y;
	vector.z = point2.z - point1.z;

	vec3 axisZStart = vec3(0, 0, -1.0f);
	vec3 axisZEnd = vector;
	float movelength = Angel::dot(normalize(axisZStart), normalize(axisZEnd));
	double moveradius = acos(movelength);
	vec3 axisRotate = Angel::cross(axisZStart, axisZEnd);
	float veclength = Angel::length(vector);
	float veczoom = veclength / 2;
	float axislength = Angel::length(axisRotate);
	mat4 rotate_matrix; //rotationmatrix
	float c = cos(moveradius);
	float s = sin(moveradius);
	float x = axisRotate.x / axislength;
	float y = axisRotate.y / axislength;
	float z = axisRotate.z / axislength;
	//calculate
	rotate_matrix[0][0] = x * x*(1 - c) + c;
	rotate_matrix[0][1] = x * y*(1 - c) - z * s;
	rotate_matrix[0][2] = x * z*(1 - c) + y * s;
	rotate_matrix[0][3] = 0;
	rotate_matrix[1][0] = y * x*(1 - c) + z * s;
	rotate_matrix[1][1] = y * y*(1 - c) + c;
	rotate_matrix[1][2] = y * z*(1 - c) - x * s;
	rotate_matrix[1][3] = 0;
	rotate_matrix[2][0] = x * z*(1 - c) - y * s;
	rotate_matrix[2][1] = y * z*(1 - c) + x * s;
	rotate_matrix[2][2] = z * z*(1 - c) + c;
	rotate_matrix[2][3] = 0;
	rotate_matrix[3][0] = 0;
	rotate_matrix[3][1] = 0;
	rotate_matrix[3][2] = 0;
	rotate_matrix[3][3] = 1;
	
	modelMat = Angel::identity();
	modelMat = modelMat * Angel::Translate(point1) * rotate_matrix * Angel::Translate(0, 0, -1 * veczoom) * Angel::Scale(branchsize, branchsize, veczoom);
	modelMatrixf[0] = modelMat[0][0]; modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0]; modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0]; modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0]; modelMatrixf[7] = modelMat[3][1];
	modelMatrixf[8] = modelMat[0][2]; modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2]; modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2]; modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2]; modelMatrixf[15] = modelMat[3][3];
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf);

	currentmodle = 43; //cylinder
	drawnum = 2;
	DrawPly(); 
	modelMat = modelMat * Angel::Translate(point2)* Angel::Scale(branchsize * 5);
	modelMatrixf[0] = modelMat[0][0]; modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0]; modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0]; modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0]; modelMatrixf[7] = modelMat[3][1];
	modelMatrixf[8] = modelMat[0][2]; modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2]; modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2]; modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2]; modelMatrixf[15] = modelMat[3][3];
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf);
	currentmodle = 44; // sphere
	drawnum = 1;
	DrawPly(); 
	glFlush();

}
void DrawPark() {
	char treenum[5] = { 'a','b','c','d','e' };
	int t;
	DrawPlane();
		srand((unsigned)time(NULL));
		rand() % 5;

		DrawTree(treenum[rand() % 5]);
		DrawTree(treenum[rand() % 5]);
		DrawTree(treenum[rand() % 5]);
		DrawTree(treenum[rand() % 5]);
		DrawTree(treenum[rand() % 5]);

	for (int i = 0; i < 5; i++)
	{
		DrawThing();
	}
	glFlush();
}
void PushState(CurrentState currentstate) {
		statestack[gstate] = currentstate;
		gstate++;
}

CurrentState PopState() {
	CurrentState tstate;
	if (gstate <= 0) {
		printf("there are no state in stack\n");
		exit(0);
	}
	else {
		tstate = statestack[gstate - 1];
		gstate--;
	}
	return tstate;
}
int CountingDrawing() //
{
	char filename[100];

	int t=0,i=0;

	strcpy(filename, "ply_files\\");
	strcat(filename, allplyfiles[currentmodle]);
	PlyFile plyfile = ReadPlyFile(filename);
	int inumber = plyfile.facenum * 3;
	points = (point4 *)malloc(sizeof(point4)*inumber);
	colors = (color4 *)malloc(sizeof(color4)*inumber);
	color4 colornum;


	if (drawnum == 1) //leaf
	{ 
		if (leafcolor ==0)
			colornum = vertex_colors[1]; //red
		else if(leafcolor ==1)
			colornum = vertex_colors[2]; //yellow
		else if(leafcolor ==2 )
			colornum = vertex_colors[3]; //green
	}
	else if (drawnum == 2)  //tree trunk
	{ 
		colornum = vertex_colors[trunkcolor];
	}
	else if (drawnum == 3) //thing
	{ 
		colornum = vertex_colors[rand()%8];
	}

	while (i < plyfile.facenum) {
		points[t] = plyfile.flyvertex[plyfile.flyface[i].point1];
		points[t].w = 1.0;
		colors[t] = colornum;
		points[t + 1] = plyfile.flyvertex[plyfile.flyface[i].point2];
		points[t + 1].w = 1.0;
		colors[t + 1] = colornum;
		points[t + 2] = plyfile.flyvertex[plyfile.flyface[i].point3];
		points[t + 2].w = 1.0;
		colors[t + 2] = colornum;
		t = t + 3;
		i++;
	}
	return t;
}

int currentplyfacenum;
point4 * currentplypoint;
void DrawPly()
{
	int tnumber;
	tnumber = CountingDrawing();

	//currentplyfacenum = tnumber;
	//TranslatePoint(points, tnumber);

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
}


//translate point coordinate
void TranslatePoint(point4 *points, int pointnumber) {
	int tranrate = 2;
	float ratiox, ratioy, ratioz;
	float xmin, xmax, ymin, ymax, zmax, zmin;
	float xoffset, yoffset, zoffset;

	xmin = points[0].x;
	xmax = points[0].x;
	ymin = points[0].y;
	ymax = points[0].y;
	zmin = points[0].z;
	zmax = points[0].z;

	for (int i = 0; i < pointnumber; i++) {
		if (points[i].x > xmax) xmax = points[i].x;
		if (points[i].x < xmin) xmin = points[i].x;
		if (points[i].y > ymax) ymax = points[i].y;
		if (points[i].y < ymin) ymin = points[i].y;
		if (points[i].z > zmax) zmax = points[i].z;
		if (points[i].z < zmin) zmin = points[i].z;
	}

	ratiox = tranrate / (xmax - xmin);
	ratioy = tranrate / (ymax - ymin);
	ratioz = tranrate / (zmax - zmin);
	xoffset = xmax - (xmax - xmin) / tranrate;
	yoffset = ymax - (ymax - ymin) / tranrate;
	zoffset = zmax - (zmax - zmin) / tranrate;

	if (ratiox < ratioy) {
		for (int i = 0; i < pointnumber; i++) {
			points[i].x = (points[i].x - xoffset)*ratiox;
			points[i].y = (points[i].y - yoffset)*ratiox;
			points[i].z = (points[i].z)*ratiox;
			//ppoint[i].z = (ppoint[i].z-zoffset)*ratiox; 
		}
	}

	else {
		for (int i = 0; i < pointnumber; i++) {
			points[i].x = (points[i].x - xoffset)*ratioy;
			points[i].y = (points[i].y - yoffset)*ratioy;
			points[i].z = (points[i].z)*ratioy;
			//ppoint[i].z = (ppoint[i].z-zoffset)*ratioy;
		}
	}

}





// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
	switch ( key ) {
	case 033:
		exit( EXIT_SUCCESS );
		break;
	case 'a':
		DrawTree('a');
		break;

	case 'b':
		DrawTree('b');

		break;

	case 'c':
		DrawTree('c');

		break;

	case 'd':
		DrawTree('d');

		break;
	case 'e':
		DrawTree('e');

		break;
		
	case 'f':
		DrawPark();
		
		break;
	case 't':
		DrawThing();
		break;
	case 'p':
		DrawPlane();
		break;
	}
}


void	InitState()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}



//----------------------------------------------------------------------------
// entry point
int main( int argc, char **argv )
{
	// init glut
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_SINGLE | GLUT_DEPTH );
	width = 800;
	height = 600;
	glutInitWindowSize(width, height);
	
	//glutInitContextVersion(3, 1);
	//glutInitContextProfile(GLUT_CORE_PROFILE);
	// create window
	// opengl can be incorperated into other packages like wxwidgets, fltoolkit, etc.
	glutCreateWindow( "Color Cube" );

	// init glew
	glewInit();

	generateGeometry();
	//InitState();
	// assign handlers
	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	// should add menus
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	// frame rate can be controlled with 
	glutMainLoop();
	return 0;
}
