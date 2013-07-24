#include <stdio.h> // Header File For Standard
#include <stdlib.h>
#include <string.h>
#include <math.h> // Math Library Header File
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/glu.h> // Header File For The GLu32 Library
#include <GL/gl.h> // Header File For The OpenGL32 Library
#include <time.h>
#include <sys/timeb.h>
#include "imageloader.h"
#include "vec3f.h"
#endif

static GLfloat spin, spin2 = 0.0;
float angle = 0;
float sudutnya = 30;
using namespace std;

float lastx, lasty;
GLint stencilBits;
static int viewx = 50;
static int viewy = 150;
static int viewz = 450;

float rot = 0;

//train 2D
//class untuk terain 2D
class Terrain {
private:
	int w; //Width
	int l; //Length
	float** hs; //Heights
	Vec3f** normals;
	bool computedNormals; //Whether normals is up-to-date
public:
	Terrain(int w2, int l2) {
		w = w2;
		l = l2;

		hs = new float*[l];
		for (int i = 0; i < l; i++) {
			hs[i] = new float[w];
		}

		normals = new Vec3f*[l];
		for (int i = 0; i < l; i++) {
			normals[i] = new Vec3f[w];
		}

		computedNormals = false;
	}

	~Terrain() {
		for (int i = 0; i < l; i++) {
			delete[] hs[i];
		}
		delete[] hs;

		for (int i = 0; i < l; i++) {
			delete[] normals[i];
		}
		delete[] normals;
	}

	int width() {
		return w;
	}

	int length() {
		return l;
	}

	//Sets the height at (x, z) to y
	void setHeight(int x, int z, float y) {
		hs[z][x] = y;
		computedNormals = false;
	}

	//Returns the height at (x, z)
	float getHeight(int x, int z) {
		return hs[z][x];
	}

	//Computes the normals, if they haven't been computed yet
	void computeNormals() {
		if (computedNormals) {
			return;
		}

		//Compute the rough version of the normals
		Vec3f** normals2 = new Vec3f*[l];
		for (int i = 0; i < l; i++) {
			normals2[i] = new Vec3f[w];
		}

		for (int z = 0; z < l; z++) {
			for (int x = 0; x < w; x++) {
				Vec3f sum(0.0f, 0.0f, 0.0f);

				Vec3f out;
				if (z > 0) {
					out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
				}
				Vec3f in;
				if (z < l - 1) {
					in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
				}
				Vec3f left;
				if (x > 0) {
					left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
				}
				Vec3f right;
				if (x < w - 1) {
					right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
				}

				if (x > 0 && z > 0) {
					sum += out.cross(left).normalize();
				}
				if (x > 0 && z < l - 1) {
					sum += left.cross(in).normalize();
				}
				if (x < w - 1 && z < l - 1) {
					sum += in.cross(right).normalize();
				}
				if (x < w - 1 && z > 0) {
					sum += right.cross(out).normalize();
				}

				normals2[z][x] = sum;
			}
		}

		//Smooth out the normals
		const float FALLOUT_RATIO = 0.5f;
		for (int z = 0; z < l; z++) {
			for (int x = 0; x < w; x++) {
				Vec3f sum = normals2[z][x];

				if (x > 0) {
					sum += normals2[z][x - 1] * FALLOUT_RATIO;
				}
				if (x < w - 1) {
					sum += normals2[z][x + 1] * FALLOUT_RATIO;
				}
				if (z > 0) {
					sum += normals2[z - 1][x] * FALLOUT_RATIO;
				}
				if (z < l - 1) {
					sum += normals2[z + 1][x] * FALLOUT_RATIO;
				}

				if (sum.magnitude() == 0) {
					sum = Vec3f(0.0f, 1.0f, 0.0f);
				}
				normals[z][x] = sum;
			}
		}

		for (int i = 0; i < l; i++) {
			delete[] normals2[i];
		}
		delete[] normals2;

		computedNormals = true;
	}

	//Returns the normal at (x, z)
	Vec3f getNormal(int x, int z) {
		if (!computedNormals) {
			computeNormals();
		}
		return normals[z][x];
	}
};
//end class

float pindahx =0.0, pindahy=0.0, pindahz=0.0;
float xpos = 0, ypos = 1.2, zpos = -2.0, xrot = 7, yrot = 0, putary=0; //Rotasi Sudut
float inner, outer; int garisv,garish; //Variabel Torus

const float clockR    = 0.5f,
            clockVol  = 1.0f,

            angle1min = M_PI / 30.0f,

            minStart  = 0.9f,
            minEnd    = 1.0f,

            stepStart = 0.8f,
            stepEnd   = 1.0f;

float angleHour = 0,
      angleMin  = 0,
      angleSec  = 0;

      
//FUNGSI JAM      
void newLine(float rStart, float rEnd, float angle){
  float c = cos(angle), s = sin(angle);
  glVertex2f( clockR*rStart*c, clockR*rStart*s);
  glVertex2f( clockR*rEnd*c, clockR*rEnd*s);
}

void jam (void)
{
  glColor3f(0.0f, 0.0f, 0.0f);
  glLineWidth(1.0f);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);

  glBegin(GL_LINES);
    for(int i=0; i<60; i++){
      if(i%5){ // normal minute
        if(i%5 == 1)
          glColor3f(0.1f, 0.1f, 0.2f);
        newLine(minStart, minEnd, i*angle1min);
      }else{
        glColor3f(1.0f, 0.0f, 0.0f);
        newLine(stepStart, stepEnd, i*angle1min);
      }
    }
  glEnd();

  glLineWidth(3.0f);
  glBegin(GL_LINES);
    newLine(0.0f, 0.5f, -angleHour+M_PI/2);
    newLine(0.0f, 0.8f, -angleMin+M_PI/2);
  glEnd();

  glLineWidth(1.0f);
  glColor3f(0.9f, 0.0f, 1.0f);
  glBegin(GL_LINES);
    newLine(0.0f, 0.8f, -angleSec+M_PI/2);
  glEnd();
}

void TimerFunction(int value){
  struct timeb tb;
  time_t tim=time(0);
  struct tm* t;
  t=localtime(&tim);
  ftime(&tb);

  angleSec = (float)(t->tm_sec+ (float)tb.millitm/1000.0f)/30.0f * M_PI;
  angleMin = (float)(t->tm_min)/30.0f * M_PI + angleSec/60.0f;
  angleHour = (float)(t->tm_hour > 12 ? t->tm_hour-12 : t->tm_hour)/6.0f * M_PI+
              angleMin/12.0f;
  
  // called if timer event
  // ...advance the state of animation incrementally...
  //rot+=1;
  glutPostRedisplay(); // request redisplay
  glutTimerFunc(33,TimerFunction, 1); // request next timer event
}
//AKHIR FUNGSI JAM


void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
}

//Loads a terrain from a heightmap.  The heights of the terrain range from
//-height / 2 to height / 2.
//load terain di procedure inisialisasi
Terrain* loadTerrain(const char* filename, float height) {
	Image* image = loadBMP(filename);
	Terrain* t = new Terrain(image->width, image->height);
	for (int y = 0; y < image->height; y++) {
		for (int x = 0; x < image->width; x++) {
			unsigned char color = (unsigned char) image->pixels[3 * (y
					* image->width + x)];
			float h = height * ((color / 255.0f) - 0.5f);
			t->setHeight(x, y, h);
		}
	}

	delete image;
	t->computeNormals();
	return t;
}

float _angle = 60.0f;
//buat tipe data terain
Terrain* _terrain;
Terrain* _terrainTanah;
Terrain* _terrainAir;

const GLfloat light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
const GLfloat light_diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 1.0f };

const GLfloat light_ambient2[] = { 0.3f, 0.3f, 0.3f, 0.0f };
const GLfloat light_diffuse2[] = { 0.3f, 0.3f, 0.3f, 0.0f };

const GLfloat mat_ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

void cleanup() {
	delete _terrain;
	delete _terrainTanah;
}

//untuk di display
void drawSceneTanah(Terrain *terrain, GLfloat r, GLfloat g, GLfloat b) {
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();
	 glTranslatef(0.0f, 0.0f, -10.0f);
	 glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	 glRotatef(-_angle, 0.0f, 1.0f, 0.0f);

	 GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	 glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	 GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
	 GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	 glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	 glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	 */
	float scale = 500.0f / max(terrain->width() - 1, terrain->length() - 1);
	glScalef(scale, scale, scale);
	glTranslatef(-(float) (terrain->width() - 1) / 2, 0.0f,
			-(float) (terrain->length() - 1) / 2);

	glColor3f(r, g, b);
	for (int z = 0; z < terrain->length() - 1; z++) {
        // Process Each Triangle
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for (int x = 0; x < terrain->width(); x++) {
			Vec3f normal = terrain->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, terrain->getHeight(x, z), z);
			normal = terrain->getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, terrain->getHeight(x, z + 1), z + 1);
		}
		glEnd();
	}

}

unsigned int LoadTextureFromBmpFile(char *filename);

void menara()
{      
    //kotak yang tinggi
    glPushMatrix();
    glTranslatef(50.0, 75.0, 35.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glColor3f(1,0.49,0);
    glScalef(40.0, 155.0, 40.0);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();
    
    //kotak anu gede na men
    glPushMatrix();
    glTranslatef(50.0, 125.0, 36.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glColor3f(1, 0.49, 0);
    glScalef(43.0, 38.0, 48.0);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();

    //limas runving
    glPushMatrix();
    glTranslatef(50.9,200.5, 36.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(45.0,0.0,1.0,0.0);
    glRotatef(-90.0,1.0,0.0,0.0);
    glColor3f(1,0.49,0);
    glutSolidCone(1.5, 31.86,4,2); //kerucut
    glPopMatrix(); 
    
    //limas1
    glPushMatrix();
    glTranslatef(50.0, 153.5, 35.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(45.0,0.0,1.0,0.0);
    glRotatef(-90.0,1.0,0.0,0.0);
    glColor3f(1,0.49,0);
    glutSolidCone(28.5, 50.8, 4, 2); //kerucut
    glPopMatrix();


 for(int n=0;n<2;n++)
 {
         for(int t=0;t<8;t++)

 {   glPushMatrix();
     glTranslatef(33+(t*5), 80.5-(n*45),55); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glScalef(1.0, 20.0, 1.0);
    glColor3f(0.1f, 0.1f, 0.1f);
    glutSolidCube(2); //kerucut
    glPopMatrix();
 }
}
//atas
         for(int t=0;t<7;t++)

 {   glPushMatrix();
     glTranslatef(34+(t*5), 148.5,85); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glScalef(1.0, 2.0, 1.0);
    glColor3f(0.0f, 0.1f, 0.1f);
    glutSolidCube(2); //kerucut
    glPopMatrix();
 }

//atas pisan
         for(int t=0;t<7;t++)

 {   glPushMatrix();
     glTranslatef(39+(t*3.5), 173.5,85); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glScalef(1.0, 5.0, 1.0);
    glColor3f(0.0f, 0.0f, 0.0f);
    glutSolidCube(2); //kerucut
    glPopMatrix();
 }


 for(int n=0;n<2;n++)
 {
         for(int t=0;t<8;t++)

 {   glPushMatrix();
     glTranslatef(33+(t*5), 80.5-(n*45),16); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glScalef(1.0, 20.0, 1.0);
    glColor3f(0.4f, 0.1f, 0.1f);
    glutSolidCube(2); //kerucut
    glPopMatrix();
 }
}
 
 //sekat gedung tengah
    glPushMatrix();
     glTranslatef(50,58,55); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glScalef(20.0, 1.0, 1.0);
    glColor3f(0.1f, 0.1f, 0.1f);
    glutSolidCube(2); //kerucut
    glPopMatrix();

 //sekat gedung bawah
    glPushMatrix();
     glTranslatef(50,12,55); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glScalef(20.0, 1.0, 1.0);
    glColor3f(0.1f, 0.1f, 0.1f);
    glutSolidCube(2); //kerucut
    glPopMatrix();
 //sekat gedung bawah
    glPushMatrix();
     glTranslatef(50,7,55); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glScalef(20.0, 1.0, 1.0);
    glColor3f(0.1f, 0.1f, 0.1f);
    glutSolidCube(2); //kerucut
    glPopMatrix();
 //sekat gedung bawah
    glPushMatrix();
     glTranslatef(50,2,55); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glScalef(20.0, 1.0, 1.0);
    glColor3f(0.1f, 0.1f, 0.1f);
    glutSolidCube(2); //kerucut
    glPopMatrix();
     //sekat gedung atas
    glPushMatrix();
     glTranslatef(50,103,55); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glScalef(20.0, 1.0, 1.0);
    glColor3f(0.1f, 0.1f, 0.1f);
    glutSolidCube(2); //kerucut
    glPopMatrix();

    //jendela
    for(int z=0; z<3;z++)
{   
    int y=0;
        for(int o=0; o<2;o++)
   {
    glPushMatrix();
    
    glTranslatef(40+(z*10), 160.5-(o*6),47+y); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(45.0,0.0,1.0,0.0);
    glRotatef(-90.0,1.0,0.0,0.0);
    glColor3f(0.9f, 0.9f, 0.9f);
    glutSolidTorus(3,4,5,100); //kerucut
    glPopMatrix();
  y=y+2;
  }  
} 
    //kotak anu luhur pisan
    glPushMatrix();
    glTranslatef(50.0, 170.0, 35.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glColor3f(1,0.49,0);
    glScalef(30.0, 30.0, 30.0);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();
    
    //limas2
    glPushMatrix();
    glTranslatef(50.0, 185.0, 40.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(45.0,0.0,1.0,0.0);
    glRotatef(-90.0,1.0,0.0,0.0);
    glColor3f(1,0.49,0);
    glutSolidCone(20.5, 27.86,4,2); //kerucut
    glPopMatrix();
}

void ngepek(){
     //JAM BIGBANG
     glPushMatrix();
     glColor3f(0.9,0.9,0.9);
     glTranslatef(50.35, 125, 54.26); // We move the object forward (the model matrix is multiplied by the translation matrix)
     glScalef(38.78, 38.78, 19.12);
     glutSolidSphere(0.5,20,20); //bola 
     glPopMatrix();
     
     //JAM
     glPushMatrix();
     glTranslatef(50.35, 125, 64.26); // We move the object forward (the model matrix is multiplied by the translation matrix)
     glScalef(30, 30, 30);
     jam();
     glPopMatrix();
     //JAM    
}

void gedungsebelah(int y)
{
    //gedungpersegipanjangmen
    glPushMatrix();
    glColor3f(1,0.49,0);
    glTranslatef(-57.9, 20.0, 35.7); // We move the object forward (the model matrix is multiplied by the translation matrix)
    //glTranslatef(pindahx,pindahy,pindahz);
    glScalef(328.8,91.8,81.8);
    glutSolidCube(0.5); //kubus solid  
    glPopMatrix();

    //jendela depan
    for(int n=0;n<2;n++)
    {
         for(int t=0;t<8;t++)
         {   
             glPushMatrix();
             glTranslatef(-56+(t*10), 110.5-(n*45),-46); // We move the object forward (the model matrix is multiplied by the translation matrix)
             glScalef(3.0, 20.0, 1.0);
             glColor3f(0.7f, 0.7f, 0.7f);
             glutSolidCube(2); //kerucut
             glPopMatrix();
         }
    }

    //jendela belakang
    for(int n=0;n<2;n++)
    {
         for(int t=0;t<8;t++)
         {   
             glPushMatrix();
             glTranslatef(-56+(t*10), 110.5-(n*45),-136); // We move the object forward (the model matrix is multiplied by the translation matrix)
             glScalef(3.0, 20.0, 1.0);
             glColor3f(0.7f, 0.7f, 0.7f);
             glutSolidCube(2); //kerucut
             glPopMatrix();
         }
    }

    glPushMatrix();
    glTranslatef(-21, 138.5,-46); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glScalef(23.0, 4.0, 1.0);
    glColor3f(0.7f, 0.7f, 0.7f);
    glutSolidCube(2); //kerucut
    glPopMatrix();

    //gedungbelakang
    glPushMatrix();
    glColor3f(1,0.49,0);
    glTranslatef(-30, 18.0, -90.7); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(45.0,0.0,1.0,0.0);
    glRotatef(-90.0,1.0,0.0,0.0);
    glutSolidTorus(50,80,20,4); //donat  
    glPopMatrix();
    
    //limas runving
    glPushMatrix();
    glTranslatef(-20.9,140.5, -56.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(45.0,0.0,1.0,0.0);
    glRotatef(-90.0,1.0,0.0,0.0);
    glColor3f(1,0.49,0);
    glutSolidCone(1.5, 31.86,4,2); //kerucut
    glPopMatrix(); 

    
    //menara tengah
    glPushMatrix();
    glColor3f(1,0.49,0);
    glTranslatef(-20.9, 80.0, -90.7); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glScalef(158.8,220.8,181.8);
    glutSolidCube(0.5); //kubus solid
    glPopMatrix();

    //menara tengah  yang atas
    glPushMatrix();
    glColor3f(1,0.49,0);
    glTranslatef(-20.9, 140.0, -90.7); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glScalef(138.8,20.8,181.8);
    glutSolidCube(0.5); //kubus solid
    glPopMatrix();

    //gedungpersegimen
    glPushMatrix();
    glColor3f(1,0.49,0);
    glTranslatef(-95.9+(y*80), 60.0, 35.7); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glScalef(58.8,71.8,81.8);
    glutSolidCube(0.5); //kubus solid
    glPopMatrix();
    
    //limasnaieumah
    glPushMatrix();
    glTranslatef(-97.9+(y*80), 77.5, 35.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(45.0,0.0,1.0,0.0);
    glRotatef(-90.0,1.0,0.0,0.0);
    glColor3f(1,0.49,0);
    glutSolidCone(23.5, 31.86,4,2); //kerucut
    glPopMatrix(); 
    
    //limas runving
    glPushMatrix();
    glTranslatef(-97.9+(y*80), 97.5, 35.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(45.0,0.0,1.0,0.0);
    glRotatef(-90.0,1.0,0.0,0.0);
    glColor3f(1,0.49,0);
    glutSolidCone(1.5, 31.86,4,2); //kerucut
    glPopMatrix(); 
    
    //jendela atas kiri
    glPushMatrix();
    glTranslatef(-100.9+(y*80), 70.5, 55.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(0.9f, 0.9f, 0.9f);
    glScalef(7.0, 7.0, 1.7);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();

    //jendela atas kiri
    glPushMatrix();
    glTranslatef(-90.9+(y*80), 70.5, 55.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(0.9f, 0.9f, 0.9f);
    glScalef(7.0, 7.0, 1.7);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();

    //jendela bawah kiri
    glPushMatrix();
    glTranslatef(-100.9+(y*80), 60.5, 55.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(0.9f, 0.9f, 0.9f);
    glScalef(7.0, 7.0, 1.7);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();

    //jendela bawah kiri
    glPushMatrix();
    glTranslatef(-90.9+(y*80), 60.5, 55.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(0.9f, 0.9f, 0.9f);
    glScalef(7.0, 7.0, 1.7);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-93.5, 40, 18.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(0.7f, 0.3f, 0.1f);
    glScalef(9.0, 9.0, 1.5);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-93.5, 44.0, 18.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glRotatef(45,0,0,1);
    glColor3f(0.9f, 0.9f, 0.9f);
    glScalef(5.0, 5.0, 1.7);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-93.5, 44.0, 18.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glRotatef(45,0,0,1);
    glColor3f(0.7f, 0.3f, 0.1f);
    glScalef(7.0, 7.0, 1.5);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-93.5, 40.0, 18.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(0.7f, 0.3f, 0.1f);
    glScalef(1.0, 9.0, 2.1);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-93.5, 40, 18.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(0.7f, 0.3f, 0.1f);
    glScalef(9.0, 1.0, 2.1);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-93.5, 40, 18.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(0.7f, 0.3f, 0.1f);
    glScalef(9.0, 1.0, 2.1);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix(); 
}

void telepon(){
    //wartel 
    glPushMatrix();
    glTranslatef(128.5, 10.0, 78.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(1.0, 0.0, 0.0);
    glScalef(14.0, 25.0, 15.1);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix(); 
    
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glTranslatef(128.8, 20.0, 78.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glScaled(16, 16, 16);
	glutSolidSphere(0.5, 8, 5); //bola
	glPopMatrix();
    
    //jendela hareupna
    glPushMatrix();
    glTranslatef(128.5, 11.2, 88.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(1.0, 1.0, 1.0);
    glScalef(12.0, 18.0, 1.0);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();   
    
    //jendela hareupna tapi anu garis beureum anu ka hiji
    glPushMatrix();
    glTranslatef(128.5, 5.8, 89.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(1.0, 0.0, 0.0);
    glScalef(12.0, 2.0, 1.0);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();
    
    //jendela hareupna tapi anu garis beureum anu ka dua
    glPushMatrix();
    glTranslatef(128.5, 10.8, 89.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(1.0, 0.0, 0.0);
    glScalef(12.0, 2.0, 1.0);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();
    
    //jendela hareupna tapi anu garis beureum anu ka tilu
    glPushMatrix();
    glTranslatef(128.5, 16.5, 89.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(1.0, 0.0, 0.0);
    glScalef(12.0, 2.0, 1.0);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();
    
    //jendela hareupna tapi anu garis bodas anu ka hiji
    glPushMatrix();
    glTranslatef(125.5, 11.5, 90.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(1.0, 0.0, 0.0);
    glScalef(1.0, 16.0, 1.0);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();
    
    //jendela hareupna tapi anu garis bodas anu ka dua
    glPushMatrix();
    glTranslatef(130.5, 11.5, 90.5);// We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(1.0, 0.0, 0.0);
    glScalef(1.0, 16.0, 1.0);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();
}

void jalan()
{
    glPushMatrix();
    glTranslatef(195.0, -2.5, 170.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glColor3f(0.9f, 0.9f, 0.9f);
    glScalef(5.0, 5.0, 50.5);
    glutSolidCube(0.5f); //kubus solid
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(195.0, -2.5, 120.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glColor3f(0.9f, 0.9f, 0.9f);
    glScalef(5.0, 5.0, 50.5);
    glutSolidCube(0.5f); //kubus solid
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(195.0, -2.5, 70.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glColor3f(0.9f, 0.9f, 0.9f);
    glScalef(5.0, 5.0, 50.5);
    glutSolidCube(0.5f); //kubus solid
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(195.0, -2.5, 20.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glColor3f(0.9f, 0.9f, 0.9f);
    glScalef(5.0, 5.0, 50.5);
    glutSolidCube(0.5f); //kubus solid
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(195.0, -2.5, -30.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glColor3f(0.9f, 0.9f, 0.9f);
    glScalef(5.0, 5.0, 50.5);
    glutSolidCube(0.5f); //kubus solid
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(195.0, -2.5, -80.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glColor3f(0.9f, 0.9f, 0.9f);
    glScalef(5.0, 5.0, 50.5);
    glutSolidCube(0.5f); //kubus solid
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(195.0, -2.5, -130.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glColor3f(0.9f, 0.9f, 0.9f);
    glScalef(5.0, 5.0, 50.5);
    glutSolidCube(0.5f); //kubus solid
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(195.0, -2.5, -180.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glColor3f(0.9f, 0.9f, 0.9f);
    glScalef(5.0, 5.0, 50.5);
    glutSolidCube(0.5f); //kubus solid
    glPopMatrix();
}

void bus(){
    //bus bawah
  
    glPushMatrix();
    glTranslatef(178.5, 10.0, 68.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(1.0, 0.0, 0.0);
    glScalef(12.0, 15.0, 55.1);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix(); 

    //bus atas
    glPushMatrix();
    glTranslatef(178.5, 23.0, 68.5); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(1.0, 0.0, 0.0);
    glScalef(12.0, 8.0, 55.1);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix(); 
    
    //sekat
    glPushMatrix();
    glTranslatef(178.5, 18, 68.5);// We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(0.0, 0.0, 0.0);
    glScalef(12.0, 1.0, 55.1);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix(); 
    
    //roda    depan kiri     
    glPushMatrix();
    glColor3f(0.9f, 0.9f, 0.9f);
    glTranslatef(161.0, 18.5, 112.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(0.0,0.0,1.0,1.0);
    glRotatef(90.0,0.0,1.0,1.0);
    glColor3f(0.0f, 0.0f, 0.f);
    glutSolidTorus(1,3,100,200); //kerucut
    glPopMatrix();


    //roda belakang kiri
    glPushMatrix();
    glColor3f(0.9f, 0.9f, 0.9f);
    glTranslatef(161.0, 18.5, 75); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(0.0,0.0,1.0,1.0);
    glRotatef(90.0,0.0,1.0,1.0);
    glColor3f(0.0f, 0.0f, 0.f);
    glutSolidTorus(1,3,100,200); //kerucut
    glPopMatrix();

    //kaca depan bawah
    glPushMatrix();
    glTranslatef(178.9, 12.8, 99); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(1.0, 1.0, 1.0);
    glScalef(12.0, 8.0, 1.0);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix(); 
    

    //kaca depan atas
    glPushMatrix();
    glTranslatef(178.9, 24.8, 99); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(1.0, 1.0, 1.0);
    glScalef(12.0, 6.0, 1.0);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix(); 
    
    //kaca belakang
    glPushMatrix();
    glTranslatef(177.9, 11.8, 40.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(3,0,1,0);
    glColor3f(1.0, 1.0, 1.0);
    glScalef(14.0, 8.0, 1.0);
    glutSolidCube(1.0f); //kubus solid
    glPopMatrix();    
}

void pohon()
{
     //pohon deket jalan
	glPushMatrix();
    glColor3f(0.4f, 0.2f, 0.0f);
    glTranslatef(235.0, -2.5, 170.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(270, 1, 0, 0);
	glScaled(4,4,35);
	glutSolidCone(0.5,1,5,3); //kerucut
	glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.5f, 0.7f, 0.1f);
    glTranslatef(235.0, 25.5, 170.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glScaled(20,20,20);
	glutSolidSphere(0.5,8,5); //bola
	glPopMatrix();
	
	glPushMatrix();
    glColor3f(0.4f, 0.2f, 0.0f);
    glTranslatef(235.0, -2.5, 130.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(270, 1, 0, 0);
	glScaled(4,4,35);
	glutSolidCone(0.5,1,5,3); //kerucut
	glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.5f, 0.7f, 0.1f);
    glTranslatef(235.0, 25.5, 130.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glScaled(20,20,20);
	glutSolidSphere(0.5,8,5); //bola
	glPopMatrix();
	
	glPushMatrix();
    glColor3f(0.4f, 0.2f, 0.0f);
    glTranslatef(235.0, -2.5, 90.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(270, 1, 0, 0);
	glScaled(4,4,35);
	glutSolidCone(0.5,1,5,3); //kerucut
	glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.5f, 0.7f, 0.1f);
    glTranslatef(235.0, 25.5, 90.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glScaled(20,20,20);
	glutSolidSphere(0.5,8,5); //bola
	glPopMatrix();
	
	glPushMatrix();
    glColor3f(0.4f, 0.2f, 0.0f);
    glTranslatef(235.0, -2.5, 50.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(270, 1, 0, 0);
	glScaled(4,4,35);
	glutSolidCone(0.5,1,5,3); //kerucut
	glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.5f, 0.7f, 0.1f);
    glTranslatef(235.0, 25.5, 50.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glScaled(20,20,20);
	glutSolidSphere(0.5,8,5); //bola
	glPopMatrix();
	
	glPushMatrix();
    glColor3f(0.4f, 0.2f, 0.0f);
    glTranslatef(235.0, -2.5, 10.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(270, 1, 0, 0);
	glScaled(4,4,35);
	glutSolidCone(0.5,1,5,3); //kerucut
	glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.5f, 0.7f, 0.1f);
    glTranslatef(235.0, 25.5, 10.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glScaled(20,20,20);
	glutSolidSphere(0.5,8,5); //bola
	glPopMatrix();
	
	glPushMatrix();
    glColor3f(0.4f, 0.2f, 0.0f);
    glTranslatef(235.0, -2.5, -30.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(270, 1, 0, 0);
	glScaled(4,4,35);
	glutSolidCone(0.5,1,5,3); //kerucut
	glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.5f, 0.7f, 0.1f);
    glTranslatef(235.0, 25.5, -30.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glScaled(20,20,20);
	glutSolidSphere(0.5,8,5); //kerucut
	glPopMatrix();
	
	glPushMatrix();
    glColor3f(0.4f, 0.2f, 0.0f);
    glTranslatef(235.0, -2.5, -70.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(270, 1, 0, 0);
	glScaled(4,4,35);
	glutSolidCone(0.5,1,5,3); //kerucut
	glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.5f, 0.7f, 0.1f);
    glTranslatef(235.0, 25.5, -70.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glScaled(20,20,20);
	glutSolidSphere(0.5,8,5); //bola
	glPopMatrix();
	
	glPushMatrix();
    glColor3f(0.4f, 0.2f, 0.0f);
    glTranslatef(235.0, -2.5, -110.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(270, 1, 0, 0);
	glScaled(4,4,35);
	glutSolidCone(0.5,1,5,3); //kerucut
	glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.5f, 0.7f, 0.1f);
    glTranslatef(235.0, 25.5, -110.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glScaled(20,20,20);
	glutSolidSphere(0.5,8,5); //bola
	glPopMatrix();
	
	glPushMatrix();
    glColor3f(0.4f, 0.2f, 0.0f);
    glTranslatef(235.0, -2.5, -150.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(270, 1, 0, 0);
	glScaled(4,4,35);
	glutSolidCone(0.5,1,5,3); //kerucut
	glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.5f, 0.7f, 0.1f);
    glTranslatef(235.0, 25.5, -150.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glScaled(20,20,20);
	glutSolidSphere(0.5,8,5); //bola
	glPopMatrix();	
}

void lampujalan(int z)
{
	glPushMatrix();
    glColor3f(1.1f, 1.2f, 0.0f);
    glTranslatef(161.0, -12.5, -150.0+(z*80)); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(270, 1, 0, 0);
	glScaled(4,4,35);
	glutSolidCone(0.5,1,5,3);
	glPopMatrix();

    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(161.0, 19.5, -150.0+(z*80)); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glScaled(20,20,20);
	glutSolidSphere(0.12,80,50); //bola
	glPopMatrix();	
     
    glPushMatrix();
    glColor3f(0.9f, 0.9f, 0.9f);
    glTranslatef(161.0, 18.5, -150.0+(z*80)); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glRotatef(45.0,0.0,1.0,0.0);
    glRotatef(-90.0,1.0,0.0,0.0);
    glColor3f(0.9f, 0.9f, 0.9f);
    glutSolidTorus(2,2,15,100); //kerucut
    glPopMatrix();
	    
    glPushMatrix();
    glTranslatef(161.0, -2.5, -150.0+(z*80)); // We move the object forward (the model matrix is multiplied by the translation matrix)
    glRotatef(45.0,0.0,1.0,0.0);
    glRotatef(-90.0,1.0,0.0,0.0);
    glColor3f(0.9f, 0.9f, 0.9f);
    glutSolidTorus(2,2,5,100); //kerucut
    glPopMatrix();
}
void batu()
{
    glPushMatrix();
    glColor3f(0.5f, 0.5f, 0.5f);
    glTranslatef(125.0, -4.5, 100.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glScaled(10,10,10);
	glutSolidSphere(0.5,8,5); //bola
	glPopMatrix();
	
	glPushMatrix();
    glColor3f(0.5f, 0.5f, 0.5f);
    glTranslatef(135.0, -2.5, 105.0); // We move the object forward (the model matrix is multiplied by the translation matrix)
	glScaled(5,5,5);
	glutSolidSphere(0.5,8,5); //bola
	glPopMatrix();
}

// fungsi yang digunakan untuk menampilkan setiap objek yang kita buat
void display(void) {
	glClearStencil(0); //clear the stencil buffer
	glClearDepth(1.0f);
	glClearColor(0.0, 0.6, 0.8, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //clear the buffers
	glLoadIdentity();
	gluLookAt(viewx-1, viewy, viewz, 0.0, 0.0, 5.0, 0.0, 1.0, 0.0);

	glPushMatrix();
	//glBindTexture(GL_TEXTURE_3D, texture[0]);
	drawSceneTanah(_terrain, 0.3f, 0.9f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	//glBindTexture(GL_TEXTURE_3D, texture[0]);
	drawSceneTanah(_terrainTanah, 0.4f, 0.4f, 0.4f);
	glPopMatrix();

	glPushMatrix();
	//glBindTexture(GL_TEXTURE_3D, texture[0]);
	drawSceneTanah(_terrainAir, 0.0f, 0.2f, 0.5f);
	glPopMatrix();
	
	
	ngepek();
	menara();
	for(int y=0;y<2;y++)
    {	
        gedungsebelah(y);
    }
	telepon();
    jalan();


           glPushMatrix();
     glTranslatef(0.8,-0.8,1.3);
     glPushMatrix();
     glTranslatef(pindahx,pindahz,pindahy);
    bus();
glPopMatrix();
glPopMatrix();	


	pohon();
	batu();
	for(int y=0;y<5;y++)
{	
    lampujalan(y);
}
	glutSwapBuffers();
	glFlush();
	rot++;
	angle++;

}

//fungsi yang digunakan sebagai tempat menggambar
void init(void) {
	glEnable(GL_DEPTH_TEST); // Enables Depth Testing
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDepthFunc(GL_LESS); // The Type Of Depth Testing To Do
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do
	glShadeModel(GL_SMOOTH); // Enable Smooth Shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);

	_terrain = loadTerrain("heightmap.bmp", 20);
	_terrainTanah = loadTerrain("heightmapTanah.bmp", 20);
	_terrainAir = loadTerrain("heightmapAir.bmp", 20);

	//binding texture

}

static void kibor(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_HOME:
		viewy+=10;
		break;
	case GLUT_KEY_END:
		viewy-=10;
		break;
	case GLUT_KEY_UP:
		viewz-=10;
		break;
	case GLUT_KEY_DOWN:
		viewz+=10;
		break;

	case GLUT_KEY_RIGHT:
		viewx+=10;
		break;
	case GLUT_KEY_LEFT:
		viewx-=10;
		break;

	case GLUT_KEY_F1: {
        // Setup The Ambient Light
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	}
		;
		break;
	case GLUT_KEY_F2: {
        // Setup The Ambient Light
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient2);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse2);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	}
		;
		break;
	default:
		break;
	}
}
void mouse(int klik, int state, int xx, int yy)
{
     
     switch(klik){
                  case GLUT_LEFT_BUTTON:
                       
                       if((putary==-270)||(putary==90))
                       {
                       pindahx  +=10.1f;
                       }
                       if((putary==180)||(putary==-180))
                       {
                       pindahy  -=10.1f;
                       }
                       if((putary==-90)||(putary==270))
                       {
                       pindahx  -=10.1f;
                       }
                       if((putary==0) || (putary==360) || (putary==-360))
                       {
                       pindahy  +=10.1f;
                       }
                  break;
                  case GLUT_RIGHT_BUTTON:
                       
                       if((putary==-270)||(putary==90))
                       {
                       pindahx  -=10.1f;
                       }
                       if((putary==180)||(putary==-180))
                       {
                       pindahy  +=10.1f;
                       }
                       if((putary==-90)||(putary==270))
                       {
                       pindahx  +=10.1f;
                       }
                       if((putary==0) || (putary==360) || (putary==-360))
                       {
                       pindahy  -=10.1f;
                       }
                  }
}


void keyboard(unsigned char key, int x, int y) {
	if (key == 'd') {

		spin = spin - 1;
		if (spin > 360.0)
			spin = spin - 360.0;
	}
	if (key == 'a') {
		spin = spin + 1;
		if (spin > 360.0)
			spin = spin - 360.0;
	}
	if (key == 'q') {
		viewz += 10;
	}
	if (key == 'e') {
		viewz-=10;
	}
	if (key == 's') {
		viewy-=10;
	}
	if (key == 'w') {
		viewy+=10;
	}
}

void reshape(int w, int h) {
    // Viewport transformation
	glViewport(0, 0, (GLsizei) w, (GLsizei) h); // Reset The Current Viewport
	glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
	// We initialize the projection matrix as identity
	glLoadIdentity(); // Reset The Modelview Matrix
	// We define the "viewing volume"
	gluPerspective(60, (GLfloat) w / (GLfloat) h, 0.1, 1000.0); // Calculate The Aspect Ratio Of The Window
	glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
}

int main(int argc, char **argv) {
	// Konfigurasi dan Menampilkan Window
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_DEPTH); //add a stencil buffer to the window
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Big Ben");
	
	// Fungsi untuk melakukan initialisasi
	init();
    
    glutTimerFunc(33, TimerFunction, 1);
    
    // Registrasi Callback Function
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

    glutMouseFunc(mouse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
    
    // Event Processing Loop
	glutMainLoop();
	return 0;
}
