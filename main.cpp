//
//  main.cpp
//  CS 418 MP3
//  UIUC SPRING 2015
//  Created by Abhishek Chatterjee [chattrj3] on 4/9/15.
//

#include <stdlib.h>
#include <GLUT/glut.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <float.h>

#include "SOIL/SOIL.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * The Vector API used in MP2, with float changed to double.
 */
struct Vec3 {
    double x;
    double y;
    double z;
};

/**
 * Returns the distance between two vectors.
 */
double vecDistance(Vec3 a, Vec3 b) {
    return sqrtf((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) + (a.z-b.z)*(a.z-b.z));
}

/**
 * Scales a vector by a scalar factor.
 */
void vecConstProduct(Vec3 *thing, float scalar) {
    thing->x *= scalar;
    thing->y *= scalar;
    thing->z *= scalar;
}

/**
 * Adds source to garget, and puts the result in "result".
 */
void vecAdd(Vec3 *result, Vec3 target, Vec3 source) {
    result->x = target.x + source.x;
    result->y = target.y + source.y;
    result->z = target.z + source.z;
}

/**
 * Subtracts source from target, and puts the result in "result".
 */
void vecSubtract(Vec3 *result, Vec3 target, Vec3 source) {
    result->x = target.x - source.x;
    result->y = target.y - source.y;
    result->z = target.z - source.z;
}

/**
 * Vector dot product of a and b.
 */
double vecDotProduct(Vec3 a, Vec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

/**
 * Vector cross product. Puts the result in "result".
 */
void vecCrossProduct(Vec3 *result, Vec3 a, Vec3 b) {
    result->x = (a.y*b.z - a.z*b.y);
    result->y = -(a.x*b.z - a.z*b.x);
    result->z = (a.x*b.y - a.y*b.x);
}

/**
 * Returns the length of a vector.
 */
double vecLength(Vec3 a) {
    return sqrtf(vecDotProduct(a, a));
}

/**
 * Prints the elements of a vector.
 */
void vecPrint(Vec3 a) {
    std::cout << "x: " << a.x << " y: " << a.y << " z: " << a.z << "\n";
}

/**
 * Translates a vector in the direction given by the parameter.
 */
void vecTranslate(Vec3 *thing, Vec3 direction, float distance) {
    vecConstProduct(&direction, distance);
    vecAdd(thing, *thing, direction);
}
/**
 * End Vector API.
 */


/**
 * A struct representing a 3d point.
 */
struct point3 {
    struct Vec3 pos; //Position vector of this point
    struct Vec3 vnormal; //Vertex normal
};

/**
 * A struct representing a triangle face.
 */
struct face3 {
    int idx1;
    int idx2;
    int idx3;
    struct Vec3 fnormal; //Face normal
};

std::vector<struct point3> vertices; //list of vertices
std::vector<struct face3> faces; //list of faces

char *loadshader(char *filename)
{
    std::string strbuf;
    std::string line;
    std::ifstream in(filename);
    while(std::getline(in,line))
        strbuf += line + '\n';
    
    char *buf = (char *)malloc(strbuf.size()*sizeof(char));
    strcpy(buf,strbuf.c_str());
    
    return buf;
}

GLuint programObj = 0;
GLint vstime;

std::string textureImageFilename = "images/china.png"; //The filename for the texture image.
std::string environmentImageFilename = "images/stpeters_probe.hdr"; //The filename for the environment image.
GLuint texture[2]; //Texture ids.

/**
 * Uses the SOIL library that has been conveniently
 * copied into this proejct's directory to 
 * load the texture from an image file.
 */
void loadTextureImage() {
    std::string filename = textureImageFilename;
    
    texture[0] = SOIL_load_OGL_texture(filename.c_str(),
                                          SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
                                          SOIL_FLAG_MIPMAPS |
                                          SOIL_FLAG_INVERT_Y |
                                          SOIL_FLAG_NTSC_SAFE_RGB |
                                          SOIL_FLAG_COMPRESS_TO_DXT);
    
    if(!texture[0])
        std::cerr << "SOIL loading error: " << SOIL_last_result() << "\n";
}

unsigned char *env_data;

/**
 * Loads the image used in environent mapping.
 */
void loadEnvironmentImage(void) {
    std::string filename = environmentImageFilename;

    texture[1] = SOIL_load_OGL_texture(filename.c_str(),
                                       SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
                                       SOIL_FLAG_MIPMAPS |
                                       SOIL_FLAG_INVERT_Y |
                                       SOIL_FLAG_NTSC_SAFE_RGB |
                                       SOIL_FLAG_COMPRESS_TO_DXT);
}

/**
 * From discussion.
 */
void texture0(void) {
    ////////////////////// texture 0 //////////////////////////
    glActiveTextureARB (GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}

/**
 * From discussion.
 */
void texture1(void) {
    ////////////////////// texture 1 //////////////////////////
    glActiveTextureARB (GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    // Make sure the mipmap initialization is completed
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

/**
 * Sets the material for the object in the absence of
 * textures.
 */
void materialize(void) {
    /**
     * Material attributes from:
     * http://devernay.free.fr/cours/opengl/materials.html
     */
    
    //Simulates emerald
    /*
     GLfloat amb[]  = {0.0215, 0.1745, 0.0215, 1.0};
     GLfloat diff[] = {0.07568, 0.61424, 0.07568, 1.0};
     GLfloat spec[] = {0.633, 0.727811, 0.633, 1.0};
     GLfloat shiniess = 0.6*128.0;
     */
    
    //Simulates brass
    /*
     GLfloat amb[]  = {0.329412, 0.223529, 0.027451, 1.0};
     GLfloat diff[] = {0.780392, 0.568627, 0.113725, 1.0};
     GLfloat spec[] = {0.992157, 0.941176, 0.807843, 1.0};
     GLfloat shiniess = 0.21794872*128.0;
     */
    
    //Simulates chrome
    GLfloat amb[]  = {0.25, 0.25, 0.25, 1.0};
    GLfloat diff[] = {0.4, 0.4, 0.4, 1.0};
    GLfloat spec[] = {0.774597, 0.774597, 0.774597, 1.0};
    GLfloat shiniess = 0.6*128.0;
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiniess);
}

bool texture_flag = false;  //Turns the surface texture on and off.
bool environment_flag = false; //Turns environment mapping on and off.
bool lighting_flag = true; //Turns lighting on and off.

void setLighting(void) {
    if(lighting_flag) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    } else {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }
}

/**
 * From the multi-texture slides in discussion.
 * texture0() and texture1() specifically contain code from discussion.
 */
void textures(void) {
    if(texture_flag && environment_flag) {
        texture0();
        texture1();
    }
    else if(texture_flag && !environment_flag) {
        glDisable(GL_TEXTURE_2D);
        texture0();
    }
    else if(!texture_flag && environment_flag) {
        glDisable(GL_TEXTURE_2D);
        texture1();
    }
    else glDisable(GL_TEXTURE_2D);
}

char *vertex_shader = "phong.vs";
char *fragment_shader = "phong.fs";
void init(char *vsfilename, char *fsfilename)
{
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    
    GLfloat xplane[] = {1,0,0,0};
    GLfloat yplane[] = {0,1,0,0};
    glTexGenfv(GL_S,GL_OBJECT_PLANE, xplane);
    glTexGenfv(GL_T,GL_OBJECT_PLANE, yplane);
    
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    loadTextureImage();
    loadEnvironmentImage();
    materialize();
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    glEnable(GL_DEPTH_TEST);
    
    //phong.vs and phong.fs from lecture.
    GLchar *vertexShaderCodeStr = loadshader(vertex_shader);
    const GLchar **vertexShaderCode = (const GLchar **)&vertexShaderCodeStr;
    
    GLchar *fragmentShaderCodeStr = loadshader(fragment_shader);
    const GLchar **fragmentShaderCode = (const GLchar **)&fragmentShaderCodeStr;
    
    int status;
    GLint infologLength = 0;
    GLint charsWritten = 0;
    GLchar infoLog[10000];
    
    GLuint vertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderObj, 1, vertexShaderCode, 0);
    glCompileShader(vertexShaderObj); /* Converts to GPU code */
    
    GLuint fragmentShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObj, 1, fragmentShaderCode, 0);
    glCompileShader(fragmentShaderObj); /* Converts to GPU code */
}

bool spinobj = true, spinlight = false;
double ymax = -DBL_MAX; //Initializing ymax with the smallest double value.

void drawTeapot(void) {
    for(int i=0; i< faces.size(); i++) {
        glBegin(GL_TRIANGLES);
        
        double s, t, x, y, z, theta;
        
        x = vertices[faces[i].idx1].pos.x;
        y = vertices[faces[i].idx1].pos.y;
        z = vertices[faces[i].idx1].pos.z;
        theta = atan2(z, x);
        s = (theta+M_PI)/(2.0*M_PI);
        t = y/ymax;
        glTexCoord2d(s, t);
        glNormal3d(vertices[faces[i].idx1].vnormal.x,vertices[faces[i].idx1].vnormal.y, vertices[faces[i].idx1].vnormal.z);
        glVertex3d(vertices[faces[i].idx1].pos.x, vertices[faces[i].idx1].pos.y, vertices[faces[i].idx1].pos.z);
        
        x = vertices[faces[i].idx2].pos.x;
        y = vertices[faces[i].idx2].pos.y;
        z = vertices[faces[i].idx2].pos.z;
        theta = atan2(z, x);
        s = (theta+M_PI)/(2.0*M_PI);
        t = y/ymax;
        glTexCoord2d(s, t);
        glNormal3d(vertices[faces[i].idx2].vnormal.x,vertices[faces[i].idx2].vnormal.y, vertices[faces[i].idx2].vnormal.z);
        glVertex3d(vertices[faces[i].idx2].pos.x, vertices[faces[i].idx2].pos.y, vertices[faces[i].idx2].pos.z);
        
        x = vertices[faces[i].idx3].pos.x;
        y = vertices[faces[i].idx3].pos.y;
        z = vertices[faces[i].idx3].pos.z;
        theta = atan2(z, x);
        s = (theta+M_PI)/(2.0*M_PI);
        t = y/ymax;
        glTexCoord2d(s, t);
        glNormal3d(vertices[faces[i].idx3].vnormal.x,vertices[faces[i].idx3].vnormal.y, vertices[faces[i].idx3].vnormal.z);
        glVertex3d(vertices[faces[i].idx3].pos.x, vertices[faces[i].idx3].pos.y, vertices[faces[i].idx3].pos.z);
        
        glEnd();
    }
}

/**
 * Print instructions onscreen - when it works...
 */
void printInstructions(void) {
    glRasterPos2i(10, 10);

    std::string s = "Click 't' to toggle texture\n.";
    s += "Click 'e' to toggle environment mapping\n.";
    s += "Click 'l' to toggle lighting.";
    
    void * font = GLUT_BITMAP_9_BY_15;
    for (std::string::iterator i = s.begin(); i != s.end(); i++) {
        glColor3f(1.0, 1.0, 1.0);
        char c = *i;
        glutBitmapCharacter(font, c);
    }
}

void display(void)
{
    textures();
    
    static GLfloat angle = 0.0;
    
    GLfloat theta = M_PI*angle/180.0;
    
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUniform1fARB(vstime,angle);   /* use angle for the time variable */
    
    glLoadIdentity ();             /* clear the ModelView matrix */
    gluLookAt(4.f, 4.f, 4.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
    
    //gluLookAt(4.f, 7.f, 4.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
    
    GLfloat white[] = {1.0,1.0,1.0,1.0};
    GLfloat redamb[] = {0.2,0.0,0.0,0.0};
    GLfloat reddiff[] = {0.8,0.0,0.0,0.0};
    GLfloat lpos[] = {2.0,5.0,2.0,1.0};
    
    setLighting();
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);    /* light transformed by lookat but not rotate */
    glLightfv(GL_LIGHT0, GL_AMBIENT, white);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    if (spinobj)
        glRotatef(angle, 0.0, 1.0, 0.0);
    
    glShadeModel(GL_SMOOTH);
    
    drawTeapot();
    printInstructions(); //Print instructions onscreen - when it works...
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, 500, 0.0, 500);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glFlush();
    glutSwapBuffers();
    
    angle += 1.0;
    
    glutPostRedisplay();
}

void reshape (int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glFrustum (-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
    gluPerspective(0.0,1.0,1.5,20.0);
    glMatrixMode (GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 's':
            glUseProgram(programObj);
            break;
        case 'f':
            glUseProgram(0);
            break;
        case 'o':
            spinobj = !spinobj;
            break;
        case 't':
            texture_flag = !texture_flag;
            break;
        case 'e':
            environment_flag = !environment_flag;
            break;
        case 'l':
            lighting_flag = !lighting_flag;
            break;
        case 27:
            exit(0);
            break;
    }
}

/**
 * String splitter. Splits a string across a character specified by "delim".
 * If multiple "delim" characters appear in a row, they are returned in the
 * vector as empty strings.
 * Source: http://stackoverflow.com/a/236803
 */
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}
/**
 * End string splitter.
 */

/**
 * Scans file for "v d d d" and "f  x x x".
 */
int fileReader(std::string filename) {
    std::ifstream infile(filename);
    
    if(infile.fail())
        return 1; //Error opening file.
    
    std::string line;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        
        
        std::vector<std::string> elems;
        elems = split(line, ' ', elems);
        
        if(!elems[0].compare("v")) {
            struct point3 point;
            point.pos.x = stod(elems[1]);
            point.pos.y = stod(elems[2]);
            point.pos.z = stod(elems[3]);
            
            if(point.pos.y > ymax)
                ymax = point.pos.y; //Updates ymax as it goes along.
            
            point.vnormal.x = 0.0;
            point.vnormal.y = 0.0;
            point.vnormal.z = 0.0;
            
            vertices.push_back(point);
        } else if(!elems[0].compare("f")) {
            struct face3 face;
            face.idx1 = stoi(elems[2]) - 1; //the 'f' entries have two spaces after 'f'.
            face.idx2 = stoi(elems[3]) - 1;
            face.idx3 = stoi(elems[4]) - 1;
            faces.push_back(face);
        }
    }
    
    return 0;
}

/**
 * Computes vertex and face normals.
 */
int normalizer(void) {
    //Computing per-face normals
    for(int i=0; i<faces.size(); i++) {
        struct face3 curr = faces[i];
        
        struct Vec3 adj1 = vertices[curr.idx1].pos;
        struct Vec3 adj2 = vertices[curr.idx2].pos;
        struct Vec3 adj3 = vertices[curr.idx3].pos;
        
        struct Vec3 side1;
        vecSubtract(&side1, adj2, adj1);
        
        struct Vec3 side2;
        vecSubtract(&side2, adj3, adj1);
        
        struct Vec3 normal;
        vecCrossProduct(&normal, side1, side2);
        
        double length = vecLength(normal);
        length = 1.0/length;
        vecConstProduct(&normal, length);
        
        curr.fnormal.x = normal.x;
        curr.fnormal.y = normal.y;
        curr.fnormal.z = normal.z;
        
        //Adds this face normal to each of its vertices.
        vecAdd(&vertices[curr.idx1].vnormal, vertices[curr.idx1].vnormal, normal);
        vecAdd(&vertices[curr.idx2].vnormal, vertices[curr.idx2].vnormal, normal);
        vecAdd(&vertices[curr.idx3].vnormal, vertices[curr.idx3].vnormal, normal);
    }
    
    //Normalizes the vertex normals
    for(int i=0; i<vertices.size(); i++) {
        double len = vecLength(vertices[i].vnormal);
        len = 1.0/len;
        vecConstProduct(&vertices[i].vnormal, len);
    }
    
    return 0;
}

/**
 * Prints out the contents of the faces vector.
 * Useful for debugging.
 */
void printFaces(void) {
    for(int i=0; i<faces.size();i++) {
        struct face3 curr = faces[i];
        std::cout << i << ": " << curr.idx1 << " " << curr.idx2 << " " << curr.idx3 << "\n";
    }
}

/**
 * Prints out the contents of the vertices vector.
 * Useful for debugging.
 */
void printVertices(void) {
    for(int i=0; i<vertices.size();i++) {
        struct point3 curr = vertices[i];
        std::cout << i << ": " << curr.pos.x << " " << curr.pos.y << " " << curr.pos.z << "\n";
    }
}

int main(int argc, char** argv)
{
    //Extract vertices and faces from the obj file
    int j = fileReader("teapot_0.obj");
    
    if(j != 0) {
        std::cerr << "Error opening file\n";
        return 1;
    }
    
    std::cout << "Opened file.\n";
    
    normalizer();
    
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (500, 500);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);
    glutSetWindowTitle("SP15 CS 418 MP3");
    init(argv[1],argv[2]);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
