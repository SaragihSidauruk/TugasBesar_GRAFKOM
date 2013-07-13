#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>

float sumbux=0.0, sumbuy=0.0, sumbuz=0.0;
float sudut=0.0f;
float vektorx=0.0f, vektorz=-1.0f;
float x=0.0f, z=9.00f;
float pindahx =0.0, pindahy=0.0, pindahz=0.0;
float xpos = 0, ypos = 1.2, zpos = -2.0, xrot = 7, yrot = 0, putary=0, angle=0.0; //Rotasi Sudut
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

  glutPostRedisplay();
  glutTimerFunc(33,TimerFunction, 1);
}

//AKHIR FUNGSI JAM

void bangun (int w, int h)
{
     if (h == 0)
		h = 1;
	 float rasio =  w * 1.0 / h;
     
     glMatrixMode (GL_PROJECTION);
     glLoadIdentity();
     glViewport (0,0, w, h);
     gluPerspective(70.0f, rasio, 0.3f, 100.0f);
     glMatrixMode(GL_MODELVIEW);
     glOrtho(0.0,2.0,0.0,2.0,-2.0,2.0);
}

void init (void)
{
     GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
     GLfloat mat_shininess[] = {50.0};
     GLfloat light_position[] = {1.0, 1.0, 1.0, 1.0};
     GLfloat white_light[] = {1.0, 1.0, 1.0, 1.0};
     GLfloat lmodel_ambient[] = {0.1, 0.1, 0.1, 1.0}; 
     
     glClearColor (0.0, 0.0, 0.0, 0.0);
     glShadeModel (GL_SMOOTH);
     
     //SHADING
     glMaterialfv(GL_FRONT,GL_SPECULAR, mat_specular);
     glMaterialfv(GL_FRONT,GL_SHININESS, mat_shininess);
     glLightfv(GL_LIGHT0, GL_POSITION, light_position);
     glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
     
     
     glEnable(GL_DEPTH_TEST);
      
}

//Garis Koordinat
void koordinatx(void)
{
     glBegin(GL_LINE_STRIP);
     glVertex2f(-4,0);
     glVertex2f(-2.6,0);
     glEnd();
}

void koordinaty(void)
{
     glBegin(GL_LINE_STRIP);
     glVertex2f(0,3);
     glVertex2f(0,-3);
     glEnd();
}

void koordinatz(void)
{
     glBegin(GL_LINE_STRIP);
     glVertex2f(0,0);
     glVertex2f(5.4,3);
     glEnd();
}
// Akhir Garis Koordinat

//kerucut
void cone (float diameter, float tinggi, int garisv, int garish)
{
     glutSolidCone(diameter,tinggi,garisv,garish);
}

void wirecone (float diameter, float tinggi, int garisv, int garish)
{
     glutSolidCone(diameter,tinggi,garisv,garish);
}

//BANGUN
void kubus (void)
{
     glutSolidCube(0.5);
}
void kubussolid (void)
{
     glutSolidCube(0.5);
}

void bola ()
{
     glutSolidSphere(0.5,10,10);
}
void bolasolid (float diameter, int garisv, int garish)
{
     glutSolidSphere(diameter,garisv,garish);
}

void torus(float inner, float outer, int garisv, int garish)
{
     glutSolidTorus(inner,outer,garisv,garish);
}

void torussolid(float inner, float outer, int garisv, int garish)
{
     glutSolidTorus(inner,outer,garisv,garish);
}

void icosahedron()
{
     glutSolidIcosahedron();
}

void dodecahedron()
{
     glutSolidDodecahedron();
}

void dodecahedronsolid()
{
     glutSolidDodecahedron();
}

void kotak ()
{
     glBegin(GL_QUADS);
     glVertex2f(0.1,0.2);
     glVertex2f(0.2,0.2);
     glVertex2f(0.2,0.1);
     glVertex2f(0.1,0.1);
     glEnd();
}

//CAMERA
void camera (void) {
    glRotatef(xrot,1.0,0.0,0.0);
    glRotatef(yrot,0.0,1.0,0.0);
    glTranslated(-xpos,-ypos,-zpos);
}

void display (void)
{
         const double waktu = glutGet (GLUT_ELAPSED_TIME) / 1000.0;
     const double lama1 = 50*waktu, lama2 = 10*waktu, lama3 = waktu/10, lama4=10*waktu;
     GLdouble eqn[4] = {0.0, 0.0, 1.0, 0.0};
     GLdouble eqn2[4] = {1.0, 0.0, 0.0, 0.0};  
     GLdouble eqn3[4] = {0.0, 1.0, 0.0, 0.0};   
     GLfloat mat_transparent[] = { 0.1, 0.8, 0.8, 0.6 };
     GLfloat mat_emission[] = { 0.0, 0.3, 0.3, 0.6 };
     GLfloat mat_solid[] = { 0.75, 0.75, 0.0, 1.0 };
     GLfloat mat_zero[] = { 0.0, 0.0, 0.0, 1.0 };
     //GLfloat light_position[] = {0.0, 0.0, 1.0, 1.0};
          
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glClearColor(0.7,0.7,1.0,0.5);
     
     glLoadIdentity();
     
     //Setting Kamera
     gluLookAt(x, 0.0, z, x+vektorx, 0.0, z+vektorz, 0.0, 1.0, 0.0);
     camera();
     glEnable(GL_TEXTURE_2D);
     glEnable(GL_COLOR_MATERIAL);
     //glColorMaterial(GL_FRONT,GL_SPECULAR);
     glPushMatrix();
      //CLIP untuk Y < 0
     glClipPlane(GL_CLIP_PLANE0,eqn);
     glEnable(GL_CLIP_PLANE0);
     
     //CLIP untuk X < 0
     glClipPlane(GL_CLIP_PLANE1,eqn2);
     glEnable(GL_CLIP_PLANE1);
     glPopMatrix();
     glDisable(GL_CLIP_PLANE0);
     glDisable(GL_CLIP_PLANE1);
     
     glPushMatrix();
     
     glColor3f(1,0.49,0);
     glTranslatef(-0.36,0.80,-1.76);
     glScalef(0.05,0.5,0.05);
     glRotatef(-90.0,1.0,0.0,0.0);
     cone (0.5,0.7,30,6);
     glPopMatrix();
  
 //TANAH
     glPushMatrix();
     glColor3f(0.0,0.0,0.0);
     glTranslatef(1.5,-1.32,-1);
     glScalef(5.0,0.05,20.0); 
     kubussolid();
     glPopMatrix();

 //jalan
     glPushMatrix();
     glColor3f(0.122,0.668,0.53);
     glTranslatef(-4.76,-1.32,-1);
     glScalef(20.0,0.05,20.0); 
     kubussolid();
     glPopMatrix();

 //TANAH

     glPushMatrix();
     glColor3f(0.12,1.8,0.53);
     glTranslatef(4.76,-1.32,-1);
     glScalef(8.0,0.05,20.0); 
     kubussolid();
     
     glPopMatrix();

     
/*     
     
     //DONAT-DONAT
//DEPAN KIRI
     for(int i=0; i<=100; i++)
     {
     glEnable(GL_LIGHTING);
    
     glPushMatrix();
     glColor3f(1.2,1.6,0.1);
     glTranslatef(-0.79,-1.3+(i/10.0)*0.2,-1.38);
     glScalef(0.17,0.17,0.17);
     glRotatef(90.0,1.0,0.0,0.0);
     torussolid(0.1,0.3,50,50);
     glDisable(GL_LIGHTING); 
     glPopMatrix();
     }


//BELAKANG KIRI
     for(int i=0; i<=100; i++)
     {
     glEnable(GL_LIGHTING);
    
     glPushMatrix();
     glColor3f(1.2,1.6,0.1);
     glTranslatef(-0.79,-1.3+(i/10.0)*0.2,-2.2);
     glScalef(0.17,0.17,0.17);
     glRotatef(90.0,1.0,0.0,0.0);
     torussolid(0.1,0.3,50,50);
     glDisable(GL_LIGHTING); 
     glPopMatrix();
     }


//DEPAM KANAN
     for(int i=0; i<=100; i++)
     {
     glEnable(GL_LIGHTING);
    
     glPushMatrix();
     glColor3f(1.2,1.6,0.1);
     glTranslatef(0.05,-1.3+(i/10.0)*0.2,-1.38);
     glScalef(0.17,0.17,0.17);
     glRotatef(90.0,1.0,0.0,0.0);
     torussolid(0.1,0.3,50,50);
     glDisable(GL_LIGHTING); 
     glPopMatrix();
     }


//BELAKANG KANAN
     for(int i=0; i<=100; i++)
     {
     glEnable(GL_LIGHTING);
    
     glPushMatrix();
     glColor3f(1.2,1.6,0.1);
     glTranslatef(0.05,-1.3+(i/10.0)*0.2,-2.2);
     glScalef(0.17,0.17,0.17);
     glRotatef(90.0,1.0,0.0,0.0);
     torussolid(0.1,0.3,50,50);
     glDisable(GL_LIGHTING); 
     glPopMatrix();
     }
  */   
     //GEDUNG 5
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glColor3f(1,0.49,0);
     glTranslatef(-0.36,1.28,-1.79);
     //glTranslatef(pindahx,pindahy,pindahz);
     glScalef(1.9,1.9,1.9);
     kubussolid();
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();

     
     //GEDUNG 5.1
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glColor3f(1,0.49,0);
     glTranslatef(-0.36,0.38,-1.79);
     //glTranslatef(pindahx,pindahy,pindahz);
     glScalef(1.8,1.8,1.8);
     kubussolid();         
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();

     //GEDUNG 5.2
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glColor3f(1,0.49,0);
     glTranslatef(-0.36,-0.77,-1.79);
     glRotatef(0.0,0.0,0.0,0.0);
     glScalef(1.8,2.8,1.8);
     kubussolid();
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();

     //GEDUNG 5.3
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glColor3f(1,0.49,0);
     glTranslatef(-0.36,1.58,-1.79);
     //glTranslatef(pindahx,pindahy,pindahz);
     glScalef(1.8,1.8,1.8);
     kubussolid();         
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();
     
     //KUBUS PANGLUHURNA
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glColor3f(1,0.49,0);
     glTranslatef(-0.36,2.5,-1.79);
     //glTranslatef(pindahx,pindahy,pindahz);
     glScalef(1.2,0.9,1.2);
     kubussolid();         
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();
     
     //GEDUNG GIGIREUN MENARA PANJANGGGG
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glColor3f(1,0.49,0);
     glTranslatef(-2.9,-0.88,-1.7);
     //glTranslatef(pindahx,pindahy,pindahz);
     glScalef(7.8,1.8,1.8);
     kubussolid();         
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();

     for(int i=0; i<=1; i++)
     {     
     //LIMAS gedung sebelah
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
          glTranslatef(-2.0+(i+(i-1.5)),0.18,-1.6);
     glRotatef(45.0,0.0,1.0,0.0);
     glRotatef(-90.0,1.0,0.0,0.0);
     glColor3f(1,0.49,0);
     cone(0.40,0.46,4,2);
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();

     //GEDUNG GIGIREUN MENARA2 LUHUR KANAN
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glColor3f(1,0.49,0);
          glTranslatef(-2.0+(i+(i-1.5)),-0.1,-1.6);

     //glTranslatef(pindahx,pindahy,pindahz);
     glScalef(1.3,1.2,1.3);
     kubussolid();         
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();
     
     for(int h=0;h<2;h++)
     {

     glEnable(GL_LIGHTING);
     //LIMAS diawal menara kecil
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glTranslatef(-1.77+(h/1.8)+(i+(i-2.0)),0.21,-1.33);
     glRotatef(45.0,0.0,1.0,0.0);
     glRotatef(-90.0,1.0,0.0,0.0);
     glColor3f(1,0.49,0);
     cone(0.08,0.08,4,2);
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();
     //LIMAS diawal menara kecil
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glTranslatef(-1.77+(h/1.8)+(i+(i-2.0)),0.21,-1.89);
     glRotatef(45.0,0.0,1.0,0.0);
     glRotatef(-90.0,1.0,0.0,0.0);
     glColor3f(1,0.49,0);
     cone(0.08,0.8,4,2);
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();

     //LIMAS diatas menara kecil
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glTranslatef(-1.77+(h/1.8)+(i+(i-2.0)),0.61,-1.30);
     glRotatef(45.0,0.0,1.0,0.0);
     glRotatef(-90.0,1.0,0.0,0.0);
     glColor3f(1,0.49,0);
     cone(0.04,0.04,4,2);
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();
          
     //LIMAS diatas menara kecil
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glTranslatef(-1.77+(h/1.8)+(i+(i-2.0)),0.61,-1.89);
     glRotatef(45.0,0.0,1.0,0.0);
     glRotatef(-90.0,1.0,0.0,0.0);
     glColor3f(1,0.49,0);
     cone(0.04,0.04,4,2);
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();
          
     //menara kecil kecil
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glColor3f(1,0.49,0);
     glTranslatef(-1.77+(h/1.8)+(i+(i-2.0)),0.3,-1.3);
     glScalef(0.09,1.2,0.09);
     kubussolid();         
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();
     }
     for(int h=0;h<2;h++)
     {
     //menara kecil kecil
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glColor3f(1,0.49,0);
     glTranslatef(-1.77+(h/1.8)+(i+(i-2.0)),0.3,-1.89);
     glScalef(0.09,1.2,0.09);
     kubussolid();         
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();
     
     }

                    
}    
//Atap digedung sebelah tengah
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glTranslatef(-2.5,-0.4,-1.80);
     glRotatef(45.9,0,1,7);
     glColor3f(1,0.49,0);
     kubus();
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();

     //diatas limas tengah
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glTranslatef(-2.5,-0.04,-1.80);
     glColor3f(1,0.49,0);
     glScalef(0.03,0.1,1.09);
          kubus();
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();


     
     //LIMAS
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glTranslatef(-0.36,2.03,-1.79);
     glRotatef(45.0,0.0,1.0,0.0);
     glRotatef(-90.0,1.0,0.0,0.0);
     glColor3f(1,0.49,0);
     cone(0.60,0.86,4,2);
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();
          
     //LIMAS ANU LUHUR
     glPushMatrix();
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glTranslatef(-0.36,2.72,-1.79);
     glRotatef(45.0,0.0,1.0,0.0);
     glRotatef(-90.0,1.0,0.0,0.0);
     glColor3f(1,0.49,0);
     cone(0.43,0.86,4,2);
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     glPopMatrix();
    
     //JAM BIGBANG
     glPushMatrix();
     glColor3f(0.9,0.9,0.9);
     glTranslatef(-0.35,1.25,-1.26);
     glScalef(0.78,0.78,-0.12);
     bolasolid(0.5,20,20);
     glPopMatrix();
     
     //JAM
     glPushMatrix();
     glTranslatef(-0.35,1.25,-1.20);
     glScalef(0.71,0.71,0.71);
     jam();
     glPopMatrix();
     //JAM
     
     //AKHIR JAM BIGBEN
     glPushMatrix();
     glTranslatef(0.8,-0.8,1.3);
     glPushMatrix();
     glTranslatef(pindahx,pindahz,pindahy);
     glRotatef(putary,0.0,1.0,0.0);
     glPushMatrix();
     glScalef(0.2,0.2,0.2);
     glRotatef(90.0,0.0,1.0,0.0);
         
     //BADAN MOBIL
     glPushMatrix();
     glTranslatef(0.9,-0.135,0.0);
     glScalef(10.0,2.47,2.0);
     glColor3f(1.0,0.0,0.0);
     kubussolid();
     glColor3f(0.8,0.8,0.8);
     kubus();
     glPopMatrix();
     //AKHIR BADAN MOBIL

     //BADAN MOBIL tingkat 2
     glPushMatrix();
     glTranslatef(0.9,0.95,0.0);
     glScalef(10.0,1.47,2.0);
     glColor3f(1.0,0.0,0.0);
     kubussolid();
     glColor3f(0.8,0.8,0.8);
     kubus();
     glPopMatrix();
     //AKHIR BADAN MOBIL
     


     //sekat badan mobil
     glPushMatrix();
     glTranslatef(0.9,0.55,0.0);
     glScalef(10.0,0.27,2.0);
     glColor3f(0.0,0.0,0.0);
     kubussolid();
     glColor3f(0.8,0.8,0.8);
     kubus();
     glPopMatrix();
     //AKHIR BADAN MOBIL
     

     //KAP MOBIL
     glPushMatrix();
     glTranslatef(-1.6,-0.45,0.0);
     glScalef(0.2,0.7,2.0);
     glColor3f(0.0,0.0,0.0);
     kubussolid();
     glPopMatrix();
     //AKHIR KAP MOBIL
     
     //JENDELA DEPAN
     glMaterialfv(GL_FRONT, GL_EMISSION, mat_zero);
     glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_solid);
     glPushMatrix();glColor3f(0.2,0.0,1.0);
     glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
     glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_transparent);
     glEnable(GL_BLEND);
     glDepthMask(GL_TRUE);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE);
     glTranslatef(-1.5,0.0,0.0);
     glScalef(0.8,0.7,2.0);
     glRotatef(45,0.0,0.0,1.0);
     
     kubussolid(); 
     glDisable(GL_BLEND);
     glPopMatrix();
     //AKHIR JENDELA

     //JENDELA DEPAN2
     glMaterialfv(GL_FRONT, GL_EMISSION, mat_zero);
     glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_solid);
     glPushMatrix();glColor3f(0.2,0.0,1.0);
     glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
     glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_transparent);
     glEnable(GL_BLEND);
     glDepthMask(GL_TRUE);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE);
     glTranslatef(-1.5,1.0,0.0);
     glScalef(0.8,0.7,2.0);
     glRotatef(45,0.0,0.0,1.0);
     
     kubussolid(); 
     glDisable(GL_BLEND);
     glPopMatrix();
     //AKHIR JENDELA
     
         
     //RODA KIRI DEPAN
     glPushMatrix();
     glScalef(0.8,0.8,0.8);
     glTranslatef(-1.4,-0.78,0.65); 
     glColor3f(0.0,0.0,0.0);
     torussolid(0.1,0.2,20,20);
     glPopMatrix();
     //AKHIR RODA KIRI DEPAN
     
     //RODA KANAN DEPAN
     glPushMatrix();
     glScalef(0.8,0.8,0.8);
     glTranslatef(-1.4,-0.78,-0.65); 
     torussolid(0.1,0.2,20,20);
     glPopMatrix();
     //AKHIR RODA KANAN DEPAN
     
     //RODA KIRI BELAKANG
     glPushMatrix();
     glScalef(0.8,0.8,0.8);
     glTranslatef(3.0,-0.78,0.65); 
     torussolid(0.1,0.2,20,20);
     glPopMatrix();
     //AKHIR RODA KIRI BELAKANG
     
     //RODA KANAN BELAKANG
     glPushMatrix();
     glScalef(0.8,0.8,0.8);
     glTranslatef(3.0,-0.78,-0.65); 
     torussolid(0.1,0.2,20,20);
     glPopMatrix();
     //AKHIR RODA KANAN BELAKANG
     
     
     //LAMPU KANAN
     glPushMatrix();
     glTranslatef(-1.7,-0.45,0.4);
     glScalef(0.1,0.2,0.2);
     glColor3f(0.8,0.8,0.8);
     kubus();
     glPopMatrix();

//grid bus
     glPushMatrix();
     glTranslatef(-1.7,-0.45,0.05);
     glScalef(0.1,0.01,0.9);
     glColor3f(0.8,0.8,0.8);
     kubus();
     glPopMatrix();

     glPushMatrix();
     glTranslatef(-1.7,-0.55,0.05);
     glScalef(0.1,0.01,0.9);
     glColor3f(0.8,0.8,0.8);
     kubus();
     glPopMatrix();

    //LAMPU kiri
     glPushMatrix();
     glTranslatef(-1.7,-0.45,-0.33);
     glScalef(0.1,0.2,0.2);
     glColor3f(0.8,0.8,0.8);
     kubus();
     glPopMatrix();




     //AKHIR LAMPU KANAN
     glPopMatrix();
     glPopMatrix();
     glPopMatrix();
     glFlush();
     glutSwapBuffers();
     angle++;
}

//FUNGSI KEYBOARD
void keyboard (unsigned char key, int x, int y)
{
     float detik = 1000.0;
     const double time = glutGet(GLUT_ELAPSED_TIME) / detik;
     
     switch (key){
            case 'w':
            case 'W':
                 xrot += 1;
              if (xrot >360) 
              xrot -= 360;
            break;
            case 's':
            case 'S':
                  xrot -= 1;
              if (xrot < -360) 
              xrot += 360;
            break;
            case 'a':
            case 'A':
                 yrot += 1;
              if (yrot >360) 
              yrot -= 360;
            break;
            case 'd':
            case 'D':
                 yrot -= 1;
              if (yrot < -360)
              yrot += 360;
            //case 'q':
            //case 'Q':
            break;
            case 'f':
            case 'F':
                 putary -= 90;
              if (putary < -360)
              putary += 360;
            break;
            case 'h':
            case 'H':
                 putary += 90;
              if (putary >360) 
              putary -= 360;
            break;
            case 27:
                  //glutLeaveGameMode();
                 exit (0);
                }
     
}

void mouse(int klik, int state, int xx, int yy)
{
     
     switch(klik){
                  case GLUT_LEFT_BUTTON:
                       
                       if((putary==-270)||(putary==90))
                       {
                       pindahx  +=0.1f;
                       }
                       if((putary==180)||(putary==-180))
                       {
                       pindahy  -=0.1f;
                       }
                       if((putary==-90)||(putary==270))
                       {
                       pindahx  -=0.1f;
                       }
                       if((putary==0) || (putary==360) || (putary==-360))
                       {
                       pindahy  +=0.1f;
                       }
                  break;
                  case GLUT_RIGHT_BUTTON:
                       
                       if((putary==-270)||(putary==90))
                       {
                       pindahx  -=0.1f;
                       }
                       if((putary==180)||(putary==-180))
                       {
                       pindahy  +=0.1f;
                       }
                       if((putary==-90)||(putary==270))
                       {
                       pindahx  +=0.1f;
                       }
                       if((putary==0) || (putary==360) || (putary==-360))
                       {
                       pindahy  -=0.1f;
                       }
                  }
}

void ketik(int key, int xx, int yy)
{
     int mod;
     float fraksi = 0.1f;
          switch(key){
         case GLUT_KEY_F1:
              mod = glutGetModifiers();
              if(mod == (GLUT_ACTIVE_ALT)){
                     sumbux=1.0;
                     sumbuy=0.0;
                     sumbuz=0.0;
                     }
              break;
         case GLUT_KEY_F2:
              sumbux=0.0;
              sumbuy=1.0;
              sumbuz=0.0;
              break;
         case GLUT_KEY_F3:
              sumbux=0.0;
              sumbuy=0.0;
              sumbuz=1.0;
              break;
         case GLUT_KEY_LEFT:
              sudut -=0.01f;
              vektorx = sin(sudut);
              vektorz = -cos(sudut);
              break;
         case GLUT_KEY_RIGHT:
              sudut += 0.01f;
			  vektorx = sin(sudut);
			  vektorz = -cos(sudut);
			break;
         case GLUT_KEY_UP:
              x += vektorx * fraksi;
			  z += vektorz * fraksi;
			break;
         case GLUT_KEY_DOWN:
              x -= vektorx * fraksi;
			  z -= vektorz * fraksi;
         break; 
         case GLUT_KEY_PAGE_UP:
              xrot += 1;
              if (xrot >360) 
              xrot -= 360;
         break;
         case GLUT_KEY_PAGE_DOWN:
              xrot -= 1;
              if (xrot < -360) 
              xrot += 360;
         break;
         case GLUT_KEY_HOME:
              yrot -= 1;
              if (yrot < -360)
              yrot += 360;
         break;
         case GLUT_KEY_END:
              yrot += 1;
              if (yrot >360) 
              yrot -= 360;
         }
              
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    //glutGameModeString( "1024×768:32@75" ); //the settings for fullscreen mode
    //glutEnterGameMode(); //set glut to fullscreen using the settings in the line above


    glutInitWindowSize(1010,690);
    glutInitWindowPosition(0,0);
    glutCreateWindow("BIGBEN LONDON");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(bangun);
    glutTimerFunc(33, TimerFunction, 1);
    glutIdleFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(ketik);
    glutMouseFunc(mouse);
    
    glEnable(GL_DEPTH_TEST);
    
    glutMainLoop();
    return 0;
}




