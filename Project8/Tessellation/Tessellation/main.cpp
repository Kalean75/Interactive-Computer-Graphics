//Devin White
//Cs 5610 Interactive Computer Graphics
//Project 8 - Tessellation
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>
#include <cyCodeBase/cyMatrix.h>
#include <cyCodeBase/cyGL.h>
#include <cyCodeBase/cyTriMesh.h>
#include <lodepng/lodepng.h>
#include <lodepng/lodepng.cpp>
#include <ctype.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "main.h"
#include <string>
//I had to put this to make it work...for some reason
#pragma comment(lib, "glew32.lib")

float windowWidth = 1920.0f;
float windowHeight = 1080.0f;
float Xcoord = 0;
float Ycoord = 0;
int mouseButton;
int mouseState;
int oldX;
int oldY;

bool showtriangulation = true;


//Intial position of object
cy::Vec3f pos = cy::Vec3f(0.0f, 10.0f, 50.0f);
cy::Vec3f tar = cy::Vec3f(0.0f, 1.0f, 0.0f);
//light position
cy::Vec3f lightPos = cy::Vec3f(50.0f, 0.0f, 50.0f);
//vertex array and vertex buffer
GLuint vao;
GLuint vbo;
GLuint tvao;
GLuint tvbo;
GLuint textCoords;
GLuint textNormals;

std::vector<cy::Vec3f> planeCoords;
std::vector<cy::Vec2f> quadTextureCoords;
std::vector<cy::Vec3f> quadNormCoords;

cy::GLSLProgram prog;
cy::GLSLProgram triangulationProg;
cy::GLSLShader shader;
cy::GLSLShader planeShader;

//MVP matrices
cy::Matrix3f NormMatrix = cy::Matrix3f::Identity();
cy::Matrix4f viewMatrix = cy::Matrix4f::Identity();
cy::Matrix4f ModelMatrix = cy::Matrix4f::Identity();
cy::Matrix4f projectionMatrix = cy::Matrix4f::Perspective(DEG2RAD(60), float(windowWidth) / float(windowHeight), 0.1f, 1000.0f);
cy::Matrix4f mvp; //MVP matrix
cy::Matrix4f mv; //mv matrix

cy::Matrix4f light = cy::Matrix4f::Identity();

std::vector<unsigned char> texture;
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
        //ESC
    case 27:
        glutLeaveMainLoop();
        break;
    //spacebar
    case 32:
        if (showtriangulation)
        {
            showtriangulation = false;
        }
        else
        {
            showtriangulation = true;
        }
        break;
        //p key
    case 'p':
        //normal perspective
        if (perspective)
        {
            perspective = false;
            projectionMatrix.SetPerspective(DEG2RAD(60), windowWidth / windowHeight, 0.1f, 1000.0f);
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
        break;
    }
    glutControl = glutGetModifiers();
    glutPostRedisplay();
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
    if (glutGetModifiers() == GLUT_ACTIVE_CTRL && state == GLUT_LEFT_BUTTON)
    {
        light *= cy::Matrix4f::RotationX(((y - Ycoord) * M_PI) / 360 * 0.005);
        light *= cy::Matrix4f::RotationY(((x - Xcoord) * M_PI) / 360 * 0.005);
    }
    prog["light"] = light;
    prog["lightPos"] = lightPos;
    glutPostRedisplay();
}
//Handle mouse motion here while a button is down
void myMouseMotion(int x, int y)
{
    //right mouse button
    if (mouseButton == GLUT_RIGHT_BUTTON && glutControl == GLUT_ACTIVE_ALT)
    {
        viewMatrix.AddTranslation(cy::Vec3f(0.0f, 0.0f, 1.0f) * 0.05f * (x - Xcoord));
        viewMatrix.AddTranslation(cy::Vec3f(0.0f, 0.0f, 1.0f) * 0.05f * (y - Ycoord));
    }
    if (mouseButton == GLUT_LEFT_BUTTON && glutControl == GLUT_ACTIVE_CTRL)
    {
        light *= cy::Matrix4f::RotationX(((y - Ycoord) * M_PI) / 360 * 0.005);
        light *= cy::Matrix4f::RotationY(((x - Xcoord) * M_PI) / 360 * 0.005);
    }
    if (mouseButton == GLUT_LEFT_BUTTON && glutControl == GLUT_ACTIVE_ALT)
    {
        viewMatrix *= cy::Matrix4f::RotationX(((y - Ycoord) * M_PI) / 360 * 0.08);
        viewMatrix *= cy::Matrix4f::RotationY(((x - Xcoord) * M_PI) / 360 * 0.08);
    }
    if (mouseButton == GLUT_LEFT_BUTTON && glutControl != GLUT_ACTIVE_ALT && glutControl != GLUT_ACTIVE_CTRL)
    {
        viewMatrix *= cy::Matrix4f::RotationX(((y - Ycoord) * M_PI) / 360 * 0.08);
        viewMatrix *= cy::Matrix4f::RotationY(((x - Xcoord) * M_PI) / 360 * 0.08);
    }
    if (mouseButton == GLUT_RIGHT_BUTTON && glutControl != GLUT_ACTIVE_ALT && glutControl != GLUT_ACTIVE_CTRL)
    {
        viewMatrix.AddTranslation(cy::Vec3f(0.0f, 0.0f, 1.0f) * 0.05f * (x - Xcoord));
        viewMatrix.AddTranslation(cy::Vec3f(0.0f, 0.0f, 1.0f) * 0.05f * (y - Ycoord));
    }
    int oldX = Xcoord;
    int oldY = Ycoord;
    Xcoord = x;
    Ycoord = y;
    mv = viewMatrix * ModelMatrix;

    NormMatrix = mv.GetSubMatrix3();
    NormMatrix.Invert();
    NormMatrix.Transpose();

    mvp = projectionMatrix * viewMatrix * ModelMatrix;
    prog["mv"] = mv;
    prog["normalMatrix"] = NormMatrix;
    prog["mvp"] = mvp;
    triangulationProg["mvp"] = mvp;
    prog["lightPos"] = lightPos;
    prog["light"] = light;
    glutPostRedisplay();
}
//Handle mouse motion while a button is NOT down
void myMousePassive(int x, int y)
{
    //Handle mouse motion here
    //while a button is NOT down
    int oldX = Xcoord;
    int oldY = Ycoord;
    Xcoord = x;
    Ycoord = y;
}
void myReshape(int width, int height)
{
    //Do what you want when window size changes
    glViewport(1, 1, width, height);
}

void myIdle()
{
    int red = 0.3f;
    int green = 0.5f;
    int blue = 0.3f;
    int alpha = 1.0f;
    //handle animations here
    glClearColor(red, green, blue, alpha);
    //Tell GLUT to redraw
    glutPostRedisplay();
}

//Initialize MVP matrices
void initMatrices()
{
    //Model Matrix
    //ModelMatrix *= cy::Matrix4f::RotationX(DEG2RAD(0));
    ModelMatrix.SetRotationY(DEG2RAD(0)); 

    //view Matrix
    viewMatrix.SetView(pos, tar, cy::Vec3f(0.0f, 0.0f, 1.0f));

    //light
    light.SetView(lightPos, tar, cy::Vec3f(0.0f, 1.0f, 0.0f));
    //light.SetRotationY(DEG2RAD(90));

    //projection Matrix
    projectionMatrix.SetPerspective(DEG2RAD(60.0f), windowWidth / windowHeight, 0.1f, 1000.0f);

    //mv matrix
    mv = viewMatrix * ModelMatrix;

    //norm matrix, 3x3 inv-transpose of MV

    NormMatrix = mv.GetSubMatrix3();
    NormMatrix.Invert();
    NormMatrix.Transpose();

    mvp = projectionMatrix * viewMatrix * ModelMatrix;

}
//Compile the shaders
void setshaders()
{
    //prog1
    //prog.BuildFiles("shaders/vertexShader.vert", "shaders/fragmentShader.frag", "shaders/geometryShader.geom", "shaders/tesscon.tesc", "shaders/tessev.tese");
    //prog.BuildFiles("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
    prog.BuildFiles("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
    prog.Bind();
    prog["mv"] = mv;
    prog["normalMatrix"] = NormMatrix;
    prog["mvp"] = mvp;
    prog["light"] = light;
    prog["lightPos"] = lightPos;

    triangulationProg.BuildFiles("shaders/vertexShader2.vert", "shaders/fragmentShader2.frag", "shaders/geometryShader.geom");
    triangulationProg.Bind();
    triangulationProg["mvp"] = mvp;

}

//initialize Buffers
void initBuffers()
{
    planeCoords.push_back(cy::Vec3f(-20.0f, -20.0f, 0.0f));
    planeCoords.push_back(cy::Vec3f(20.0f, -20.0f, 0.0f));
    planeCoords.push_back(cy::Vec3f(-20.0f, 20.0f, 0.0f));
    planeCoords.push_back(cy::Vec3f(20.0f, 20.0f, 0.0f));

    quadTextureCoords.push_back(cy::Vec2f(0.0f, 0.0f));
    quadTextureCoords.push_back(cy::Vec2f(1.0f, 0.0f));
    quadTextureCoords.push_back(cy::Vec2f(0.0f, 1.0f));
    quadTextureCoords.push_back(cy::Vec2f(1.0f, 1.0f));
    
    glUseProgram(prog.GetID());
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    cy::Vec3f edge1 = planeCoords[0] - planeCoords[1];
    cy::Vec3f edge2 = planeCoords[0] - planeCoords[2];
    cy::Vec3f normVec = edge1.Cross(edge2);
    normVec.Normalize();

    quadNormCoords.push_back(normVec);
    quadNormCoords.push_back(normVec);
    quadNormCoords.push_back(normVec);
    quadNormCoords.push_back(normVec);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * 4, &planeCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    prog.SetAttribBuffer("pos", vbo, 3);

    glGenBuffers(1, &textCoords);
    glBindBuffer(GL_ARRAY_BUFFER, textCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * 4, &quadTextureCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, textCoords);
    prog.SetAttribBuffer("txc", textCoords, 2);

    glGenBuffers(1, &textNormals);
    glBindBuffer(GL_ARRAY_BUFFER, textNormals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * 4, &quadNormCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, textNormals);
    prog.SetAttribBuffer("norms", textNormals, 3);

    glUseProgram(triangulationProg.GetID());
    glGenVertexArrays(1, &tvao);
    glBindVertexArray(tvao);
    glGenBuffers(1, &tvbo);
    glBindBuffer(GL_ARRAY_BUFFER, tvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * 4, &planeCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, tvbo);
    triangulationProg.SetAttribBuffer("pos", tvbo, 3);
}
void initializeTextures(std::string normName)
{
    //diffuse material
    unsigned txtW, txtH;
    unsigned success = lodepng::decode(texture, txtW, txtH, normName);
    if (success == 1)
    {
        printf("Error loading texture\n");
    }

    glUseProgram(prog.GetID());
    glGenTextures(1, &textNormals);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textNormals);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, txtW, txtH, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glUniform1i(glGetUniformLocation(prog.GetID(), "normalMap"), 0);

}

void updateLight()
{
    if (glutControl == GLUT_ACTIVE_CTRL && mouseState == GLUT_LEFT_BUTTON)
    {
        light *= cy::Matrix4f::RotationX((-(oldY - Ycoord) * M_PI) / 360 * 0.001);
        light *= cy::Matrix4f::RotationY((-(oldX - Xcoord) * M_PI) / 360 * 0.001);
    }
    prog["light"] = light;
    prog["lightPos"] = lightPos;
}


void myDisplay()
{
    glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    updateLight();
    glUseProgram(prog.GetID());
    prog.Bind();
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //triangulation lines
    if (showtriangulation)
    {
        glUseProgram(triangulationProg.GetID());
        triangulationProg.Bind();
        glBindVertexArray(tvao);
        glBindBuffer(GL_ARRAY_BUFFER, tvbo);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    glutSwapBuffers();
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
    glutCreateWindow("CS5610 Project 8 Tessellation");
    //initialize GLEW
    glewInit();
    glutInitContextFlags(GLUT_DEBUG);
    CY_GL_REGISTER_DEBUG_CALLBACK;
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
    glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
}

int main(int argc, char** argv)
{
    //GLUT initializations
    initGlut(argc, argv);
    //initialize object
    //create buffers
    initMatrices();
    //Compile shaders
    setshaders();
    //initialize buffers
    initBuffers();
    //create textures
    initializeTextures(argv[1]);
    //Other initializations
    //call main loop
    glutMainLoop();
    //Exit when main loop is done
    return 0;
}