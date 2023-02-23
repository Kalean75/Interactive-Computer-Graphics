//Devin White
//Cs 5610 Interactive Computer Graphics
//Project 5 - RenderBuffer
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

int DEG2RAD(int degrees);

float windowWidth = 1920.0f;
float windowHeight = 1080.0f;
float Xcoord = 0;
float Ycoord = 0;
int mouseButton;
int mouseState;
int oldX;
int oldY;


//Intial position of object
cy::Vec3f pos = cy::Vec3f(0.0f, 0.0f, 50.0f);
//light position
cy::Vec3f lightPos = cy::Vec3f(100.0f, 0.0f, 50.0f);
//vertex array and vertex buffer
GLuint vertexArray;
GLuint buff;
GLuint nbuff;
GLuint tbuff;
GLuint textb1;
GLuint textb2;
GLuint loc;
GLuint depthBuffer;
GLuint frameBuffer = 0;
GLuint planeVertexArray;
GLuint planeVertexBuff;

std::vector<cy::Vec3f> vertices;
std::vector<cy::Vec3f> Normals;
std::vector<cy::Vec3f> textureCoords;

cy::GLSLProgram prog;
cy::GLSLProgram prog2;
cy::TriMesh mesh;
cy::GLSLShader shader;
cy::GLSLShader planeShader;
cy::GLRenderTexture2D renderText;

//MVP matrices
cy::Matrix3f NormMatrix = cy::Matrix3f::Identity();
cy::Matrix4f rotMatrix = cy::Matrix4f::Identity();
cy::Matrix4f viewMatrix = cy::Matrix4f::Identity();
cy::Matrix4f ModelMatrix = cy::Matrix4f::Identity();
cy::Matrix4f projectionMatrix = cy::Matrix4f::Perspective(DEG2RAD(60), float(windowWidth) / float(windowHeight), 0.1f, 1000.0f);
cy::Matrix4f mvp; //MVP matrix
cy::Matrix4f mv; //mv matrix

//not sure if needed..
cy::Matrix4f objmvp; //MVP matrix
cy::Matrix4f objmv; //mv matrix

cy::Matrix4f light = cy::Matrix4f::Identity();

std::vector<unsigned char> texture1;
std::vector<unsigned char> texture2;
unsigned txtW = 600;
unsigned txtH = 600; 
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
                light += cy::Matrix4f::RotationX(((y - Ycoord) * M_PI) / 360 * 0.05);
                light += cy::Matrix4f::RotationY(((x - Xcoord) * M_PI) / 360 * 0.05);
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
        viewMatrix *= cy::Matrix4f::RotationX(((y - Ycoord) * M_PI) / 360 * 0.08);
        viewMatrix *= cy::Matrix4f::RotationY(((x - Xcoord) * M_PI) / 360 * 0.08);
    }
    else
    {
        light += cy::Matrix4f::RotationX(((y - Ycoord) * M_PI) / 360 *0.05);
        light += cy::Matrix4f::RotationY(((x - Xcoord) * M_PI) / 360 * 0.05);
    }
    int oldX = Xcoord;
    int oldY = Ycoord;
    Xcoord = x;
    Ycoord = y;
    glUseProgram(prog.GetID());
    mv = viewMatrix * ModelMatrix;
    NormMatrix = mv.GetSubMatrix3();
    NormMatrix.Invert();
    NormMatrix.Transpose();
    mvp = projectionMatrix * viewMatrix * ModelMatrix;
    //prog["mv"] = objmv;
    //prog["normalMatrix"] = NormMatrix;
    //prog["mvp"] = objmvp;
    //prog["lightDirection"] = lightPos;
    prog["light"] = light;
    prog2["mv"] = mv;
    prog2["mvp"] = mvp;
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

void initObject(char* file, bool load)
{
    bool success = mesh.LoadFromFileObj(file, load);
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
    prog["mv"] = objmv;
    prog["normalMatrix"] = NormMatrix;
    prog["mvp"] = objmvp;
    prog["light"] = light;
    prog["lightPos"] = lightPos;
    prog2.BuildFiles("vert2.vert", "frag2.frag");
    prog2.Bind();
    prog2["mv"] = mv;
    prog2["mvp"] = mvp;
}

//Initialize MVP matrices
void initMatrices()
{
    //Model Matrix
    mesh.ComputeBoundingBox();
    ModelMatrix *= cy::Matrix4f::RotationX(DEG2RAD(-90));
    //view Matrix
    viewMatrix.SetView(pos, cy::Vec3f(0.0f, 5.0f, 2.0f), cy::Vec3f(0.0f, 0.0f, 1.0f));
    //light
    light.SetView(lightPos, cy::Vec3f(0.0f, 0.0f, 0.0f), cy::Vec3f(0.0f, 0.0f, 0.0f));
    light = light.Scale(10.0f);
    //projection Matrix
    projectionMatrix.SetPerspective(DEG2RAD(60.0f), windowWidth / windowHeight, 0.1f, 1000.0f);
    objmv = viewMatrix * ModelMatrix;
    ModelMatrix.SetScale(10.0);
    mv = viewMatrix * ModelMatrix;
    //norm matrix, 3x3 inv-transpose of MV
    NormMatrix = mv.GetSubMatrix3();
    NormMatrix.Invert();
    NormMatrix.Transpose();
    mvp = projectionMatrix * viewMatrix * ModelMatrix;
    objmvp = projectionMatrix * objmv;
}

void updateLight()
{
    if (glutControl == GLUT_ACTIVE_CTRL)
    {
        if (mouseState == GLUT_DOWN) {
            if (mouseState == GLUT_LEFT_BUTTON) {
                light += cy::Matrix4f::RotationX(((Ycoord) * M_PI) / 360 * 0.05);
                light += cy::Matrix4f::RotationY(((Xcoord) * M_PI) / 360 * 0.05);
            }
        }
    }
    prog["light"] = light;
    prog["lightPos"] = lightPos;
}

void initPlane()
{
    static const GLfloat planeVert[] = { -0.5f,-0.5f,0.0f, 0.5f,-0.5f,0.0f, -0.5f,0.5f,0.0f, -0.5f,0.5f,0.0f, 0.5f,-0.5f,0.0f, 0.5f,0.5f,0.0f };

    glGenVertexArrays(1, &planeVertexArray);
    glBindVertexArray(planeVertexArray);

    glGenBuffers(1, &planeVertexBuff);
    glBindBuffer(GL_ARRAY_BUFFER, planeVertexBuff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVert), planeVert, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, planeVertexBuff);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);

;
}
void initRenderBuffers()
{
    if (!renderText.Initialize(true, //create deoth buffer
        3, //RGB
        txtW, //texture width
        txtH, //texture height
        cyGL::TYPE_UBYTE))
    {
        printf("Problem initializing\n");
    }
    renderText.BuildTextureMipmaps();
    //renderText.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    //renderText.SetTextureAnisotropy(true);
}

void myDisplay()
{
    renderText.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(prog.GetID());

    updateLight();
    prog.Bind();
    //glDrawArrays(GL_POINTS, 0, sizeof(cy::Vec3f) * mesh.NF()); //old vertices
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(buff);

    glUniform1i(glGetUniformLocation(prog.GetID(), "texDiffuse"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textb1);

    glUniform1i(glGetUniformLocation(prog.GetID(), "texSpecular"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textb2);

    glBindBuffer(GL_ARRAY_BUFFER, buff);
    glBindBuffer(GL_ARRAY_BUFFER, nbuff);
    glBindBuffer(GL_ARRAY_BUFFER, tbuff);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(cy::Vec3f) * mesh.NF());
    //glutSwapBuffers();

    renderText.Unbind();
    renderText.Unbind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //renderText.Unbind();
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glViewport(0, 0, windowWidth, windowHeight);
    //render to plane
    glUseProgram(prog2.GetID());
    prog2.Bind();
    //render to the texture
    glUniform1i(glGetUniformLocation(prog2.GetID(), "text"), 0);
    renderText.BindTexture(0);
    glBindVertexArray(planeVertexArray);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //renderText.Unbind();
    glutSwapBuffers();
}

void initializeTextures()
{
    //diffuse material
    std::string text1(mesh.M(0).map_Kd);
    unsigned success = lodepng::decode(texture1, txtW, txtH, text1);
    if (success == 1)
    {
        printf("Error loading texture\n");
    }
    glGenTextures(1, &textb1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textb1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, txtW, txtH, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texture1[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);


    //specular material
    std::string text2(mesh.M(0).map_Ks);
    success = lodepng::decode(texture2, txtW, txtH, text2);
    if (success == 1)
    {
        printf("Error loading texture\n");
    }
    glGenTextures(1, &textb2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textb2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, txtW, txtH, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texture2[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
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
    glBindBuffer(GL_ARRAY_BUFFER, buff);
    prog.SetAttribBuffer("pos", buff, 3);
    prog2.SetAttribBuffer("pos", buff, 3);

    //Normals
    glGenBuffers(1, &nbuff);
    glBindBuffer(GL_ARRAY_BUFFER, nbuff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * mesh.NF() * 3, &Normals[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, nbuff);
    prog.SetAttribBuffer("norms", nbuff, 3);

    //Textures
    glGenBuffers(1, &tbuff);
    glBindBuffer(GL_ARRAY_BUFFER, tbuff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * mesh.NF() * 3, &textureCoords[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, tbuff);
    prog.SetAttribBuffer("txc", tbuff, 3);
    prog2.SetAttribBuffer("txc", tbuff, 3);


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
    for (unsigned i = 0; i < mesh.NF(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            textureCoords.push_back(mesh.VT(mesh.FT(i).v[j]));
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
    glutCreateWindow("CS5610 Project 5 RenderBuffer");
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
    glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
}

int main(int argc, char** argv)
{
    //GLUT initializations
    initGlut(argc, argv);
    //initialize object
    initObject(argv[1],true);
    //create buffers
    initMatrices();
    //Compile shaders
    setshaders();
    //initialize buffers
    initBuffers();
    //create textures
    initializeTextures();
    //Other initializations
    initRenderBuffers();
    initPlane();
    //call main loop
    glutMainLoop();
    //Exit when main loop is done
    return 0;
}