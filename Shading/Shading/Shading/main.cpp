//Devin White
//Cs 5610 Interactive Computer Graphics
//Project 3 - Shading
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>
#include <cyCodeBase/cyMatrix.h>
#include <cyCodeBase/cyGL.h>
#include <cyCodeBase/cyTriMesh.h>
#include <ctype.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "main.h"
//I had to put this to make it work...for some reason
#pragma comment(lib, "glew32.lib")

int DEG2RAD(int degrees);

float window_width = 1920.0f;
float window_height = 1080.0f;
float Xcoord = 0;
float Ycoord = 0;
int mouseButton;
int mouseState;


//Intial position of object
cy::Vec3f pos = cy::Vec3f(0.0f, 0.0f, 50.0f);
//light position
cy::Vec3f lightPos = cy::Vec3f(100.0f, 0.0f, 50.0f);
//vertex array and vertex buffer
GLuint vertexArray;
GLuint buff;
GLuint nbuff;
GLuint tbuff;
GLuint textob1;
GLuint textob2;
GLuint loc;

std::vector<cy::Vec3f> vertices;
std::vector<cy::Vec3f> Normals;

cy::GLSLProgram prog;
cy::TriMesh mesh;
cy::GLSLShader shader;

//MVP matrices
cy::Matrix3f NormMatrix = cy::Matrix3f::Identity();
cy::Matrix4f rotMatrix = cy::Matrix4f::Identity();
cy::Matrix4f viewMatrix = cy::Matrix4f::Identity();
cy::Matrix4f ModelMatrix = cy::Matrix4f::Identity();
cy::Matrix4f projectionMatrix = cy::Matrix4f::Perspective(DEG2RAD(60), float(window_width) / float(window_height), 0.1f, 1000.0f);
cy::Matrix4f mvp; //MVP matrix
cy::Matrix4f mv; //mv matrix

cy::Matrix4f light = cy::Matrix4f::Identity();

unsigned txtW = 600; //512
unsigned txtH = 600; //512
int glutControl;
//flag for ortho perspective
bool perspective = false;
//converts degrees to radians
int DEG2RAD(int degrees)
{
    int radians = (degrees * M_PI) / 180;

    return radians;
}
//converts radians to degrees
int RAD2DEG(float radians)
{
    int degrees = (radians * 180.0f) / M_PI;

    return degrees;
}

void myKeyboard(unsigned char key, int x, int y)
{
    //Handle keyboard input here
    switch (key)
    {
    case 27: //ESC
        glutLeaveMainLoop();
        break;
        //p key
    case 'p':
        //normal perspective
        if (perspective)
        {
            perspective = false;
            projectionMatrix.SetPerspective(DEG2RAD(60), float(window_width) / float(window_height), 0.1f, 1000.0f);
        }
        //orthogonal perspective
        else
        {
            perspective = true;
            projectionMatrix.OrthogonalizeX();
            projectionMatrix.OrthogonalizeY();
            projectionMatrix.OrthogonalizeZ();
        }
        break;
    }
    glutPostRedisplay();
}

void myKeyboard2(int key, int x, int y)
{
    //Handle special keyboard input here
    switch (key)
    {
        //F6
    case GLUT_KEY_F6:
        //recompile shaders
        prog.BuildFiles("shader.vert", "shader.frag");
        prog.Bind();
        break;
    }
    glutGetModifiers();
}

//Handles mouse input
void myMouse(int button, int state, int x, int y)
{
    //handle mouse buttons here
    Xcoord = x;
    Ycoord = y;
    mouseButton = button;
    mouseState = state;
    glutControl = glutGetModifiers();
    //rotate light
    if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
    {
        if (state == GLUT_DOWN) {
            if (state == GLUT_LEFT_BUTTON) {
                light = cy::Matrix4f::RotationX(((y - Ycoord) * M_PI) / 360 * 0.05);
                light = cy::Matrix4f::RotationY(((x - Xcoord) * M_PI) / 360 * 0.05);
            }

        }
    }
    prog["light"] = light;
    prog["lightPos"] = lightPos;
}
//Handle mouse motion here while a button is down
void myMouseMotion(int x, int y)
{
    //right mouse button
    if (mouseButton == GLUT_RIGHT_BUTTON)
    {
        viewMatrix.AddTranslation(pos * 0.0005 * (x - Xcoord));
        viewMatrix.AddTranslation(pos * 0.0005 * (y - Ycoord));
    }
    //left mouse button
    if (mouseButton == GLUT_LEFT_BUTTON && glutControl != GLUT_ACTIVE_CTRL)
    {
        //Y is reversed for some reason....
        viewMatrix *= cy::Matrix4f::RotationX(((y - Ycoord) * M_PI) / 360 * 0.05);
        viewMatrix *= cy::Matrix4f::RotationY(((x - Xcoord) * M_PI) / 360 * 0.05);
        light = cy::Matrix4f::RotationX(((y - Ycoord) * M_PI) / 360 * 0.05);
        light = cy::Matrix4f::RotationY(((x - Xcoord) * M_PI) / 360 * 0.05);
    }
    Xcoord = x;
    Ycoord = y;
    glUseProgram(prog.GetID());
    mv = viewMatrix * ModelMatrix;
    NormMatrix = mv.GetSubMatrix3();
    NormMatrix.Invert();
    NormMatrix.Transpose();
    mvp = projectionMatrix * viewMatrix * ModelMatrix;

    prog["mv"] = mv;
    prog["normalMatrix"] = NormMatrix;
    prog["mvp"] = mvp;
    prog["lightDirection"] = lightPos;
    prog["light"] = light;
}
//Handle mouse motion while a button is NOT down
void myMousePassive(int x, int y)
{
    //Handle mouse motion here
    //while a button is NOT down
    Xcoord = x;
    Ycoord = y;
}
void myReshape(int width, int height)
{
    //Do what you want when window size changes
    glViewport(0, 0, width, height);
}

void myIdle()
{
    int red = 0.0f;
    int green = 0.0f;
    int blue = 0.0f;
    int alpha = 1.0f;
    //handle animations here
    //black for clarity
    glClearColor(red, green, blue, alpha);
    //Tell GLUT to redraw
    glutPostRedisplay();
}

void initObject(bool load)
{
    bool success = mesh.LoadFromFileObj("teapot.obj", load);
    if (!success)
    {
        printf("Failed");
    }
    mesh.ComputeNormals();
}
//Compile the shaders
void setshaders()
{
    prog.BuildFiles("shader.vert", "shader.frag");
    prog.Bind();
    prog["mv"] = mv;
    prog["normalMatrix"] = NormMatrix;
    prog["mvp"] = mvp;
    prog["light"] = light;
    prog["lightPos"] = lightPos;
}

//Initialize MVP matrices
void initMatrices()
{
    //Model Matrix
    mesh.ComputeBoundingBox();
    ModelMatrix *= cy::Matrix4f::RotationX(DEG2RAD(-90));
    //ModelMatrix.Scale(10.0f);
    //view Matrix
    viewMatrix.SetView(pos, cy::Vec3f(0.0f, 5.0f, 2.0f), cy::Vec3f(0.0f, 0.0f, 1.0f));
    //light
    light.SetView(lightPos, cy::Vec3f(0.0f, 0.0f, 0.0f), cy::Vec3f(0.0f, 0.0f, 0.0f));
    light = light.Scale(10.0f);
    //projection Matrix
    projectionMatrix.SetPerspective(DEG2RAD(60.0f), float(window_width) / float(window_height), 0.1f, 1000.0f);
    mv = viewMatrix * ModelMatrix;
    //norm matrix, 3x3 inv-transpose of MV
    NormMatrix = mv.GetSubMatrix3();
    NormMatrix.Invert();
    NormMatrix.Transpose();
    mvp = projectionMatrix * viewMatrix * ModelMatrix;
}

void updateLight()
{
    if (glutControl == GLUT_ACTIVE_CTRL)
    {
        if (mouseState == GLUT_DOWN) {
            if (mouseState == GLUT_LEFT_BUTTON) {
                light = cy::Matrix4f::RotationX(((Ycoord)*M_PI) / 360 * 0.05);
                light = cy::Matrix4f::RotationY(((Xcoord)*M_PI) / 360 * 0.05);
            }
        }
    }
    prog["light"] = light;
    prog["lightPos"] = lightPos;
}


void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(prog.GetID());

    updateLight();
    prog.Bind();
    //glDrawArrays(GL_POINTS, 0, sizeof(cy::Vec3f) * mesh.NF()); //old vertices
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(buff);

    glBindBuffer(GL_ARRAY_BUFFER, buff);
    glBindBuffer(GL_ARRAY_BUFFER, nbuff);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(cy::Vec3f) * mesh.NF());
    glBindVertexArray(0);

    glutSwapBuffers();
}

//initialize Buffers
void initBuffers()
{
    //vertices
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    setMesh();

    //Triangular mesh
    glGenBuffers(1, &buff);
    glBindBuffer(GL_ARRAY_BUFFER, buff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * mesh.NF() * 3, &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, buff);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    prog.SetAttribBuffer("pos", buff, 3);

    //Normals
    glGenBuffers(1, &nbuff);
    glBindBuffer(GL_ARRAY_BUFFER, nbuff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * mesh.NF() * 3, &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, nbuff);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    prog.SetAttribBuffer("norms", nbuff, 3);

    glBindVertexArray(0);


}

void setMesh()
{

    for (unsigned i = 0; i < mesh.NF(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            vertices.push_back(mesh.V(mesh.F(i).v[j]));
        }
    }
    for (unsigned i = 0; i < mesh.NF(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            Normals.push_back(mesh.VN(mesh.FN(i).v[j]));
        }
    }
}

//GLUT initializations
void initGlut(int argc, char** argv)
{
    //initialize GLUT
    glutInit(&argc, argv);

    //create a window
    glutInitWindowSize(1920, 1080);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("CS5610 Project 3 Shading");
    //initialize GLEW
    glewInit();
    //initialize matrices
    //Register display callback function
    glutDisplayFunc(myDisplay);
    //Register keyboard callback function
    glutKeyboardFunc(myKeyboard);
    //Register special keyboard callback
    glutSpecialFunc(myKeyboard2);
    //Register mouse callback function
    glutMouseFunc(myMouse);
    //Register mouse motion callback
    glutMotionFunc(myMouseMotion);
    //Register mouse "passive" motion
    glutPassiveMotionFunc(myMousePassive);
    //Register window reshape callback
    glutReshapeFunc(myReshape);
    //Register idle callback function
    glutIdleFunc(myIdle);
    //initialize model
    //openGL initializations
    //set the background color
    //black for clarity
    glClearColor(0.5f, 1.0f, 0.0f, 1.0f);
}

int main(int argc, char** argv)
{
    //GLUT initializations
    initGlut(argc, argv);
    //initialize object
    initObject(true);
    //create buffers
    initMatrices();
    //initBuffers();
    //create textures
    //Compile shaders
    setshaders();
    initBuffers();
    //Other initializations
    //call main loop
    glutMainLoop();
    //Exit when main loop is done
    return 0;
}