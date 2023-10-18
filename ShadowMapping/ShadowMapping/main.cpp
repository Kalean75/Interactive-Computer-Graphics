//Devin White
//Cs 5610 Interactive Computer Graphics
//Project 7 - Shadow Mapping
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


//Intial position of object
cy::Vec3f pos = cy::Vec3f(0.0f, 0.0f, 50.0f);
cy::Vec3f pos2 = cy::Vec3f(0.0f, 0.0f, 50.0f);
cy::Vec3f pos3 = cy::Vec3f(0.0f, 0.0f, 10.0f);
//light position
cy::Vec3f lightPos = cy::Vec3f(40.0f, 6.5f, 0.5f);
cy::Vec3f lightTar = cy::Vec3f(0.0f, 5.0f, 0.0f);
cy::Vec3f lightDir = (lightPos - lightTar);
//teapot
GLuint vertexArray;
GLuint buff;
GLuint nbuff;
GLuint tbuff;
GLuint textbuff1;
GLuint textbuff2;
GLuint depthMap;
//plane
//GLuint frameBuffer = 0;
GLuint quadVao;
GLuint quadVbo;
GLuint quadNorms;
GLuint quadTxtCoords;
//GLuint shadowProgram;
GLuint shadow;
//teapot
std::vector<cy::Vec3f> vertices;
std::vector<cy::Vec3f> Normals;
std::vector<cy::Vec3f> textureCoords;
//plane
std::vector<cy::Vec3f> planeCoords;
std::vector<cy::Vec2f> quadTextureCoords;
std::vector<cy::Vec3f> quadNormCoords;


cy::GLSLProgram prog;
cy::GLSLProgram prog2;
cy::GLSLProgram shadowProg;

cy::TriMesh mesh;
cy::GLSLShader shader;
cy::GLSLShader planeShader;
cy::GLRenderTexture2D shadText;
cy::GLRenderDepth2D depthMaptext;

//MVP matrices
//teapot
cy::Matrix4f potViewMatrix = cy::Matrix4f::Identity();
cy::Matrix4f potModelMatrix = cy::Matrix4f::Identity();
cy::Matrix4f potProjectionMatrix = cy::Matrix4f::Perspective(DEG2RAD(60), float(windowWidth) / float(windowHeight), 0.1f, 1000.0f);
cy::Matrix3f NormMatrix = cy::Matrix3f::Identity();
cy::Matrix4f objmvp; //MVP matrix
cy::Matrix4f objmv; //mv matrix
//Plane
cy::Matrix4f planeViewMatrix = cy::Matrix4f::Identity();
cy::Matrix4f planeModelMatrix = cy::Matrix4f::Identity();
cy::Matrix4f planeProjectionMatrix = cy::Matrix4f::Perspective(DEG2RAD(60), float(windowWidth) / float(windowHeight), 0.1f, 1000.0f);
cy::Matrix4f planemvp; //MVP matrix
cy::Matrix4f planemv; //mv matrix
//light
cy::Matrix4f light = cy::Matrix4f::Identity();
cy::Matrix4f matrixMLP = cy::Matrix4f::Identity();
cy::Matrix4f matrixMLPPlane = cy::Matrix4f::Identity();
cy::Matrix4f matrixShadow = cy::Matrix4f::Identity();
cy::Matrix4f matrixShadowPlane = cy::Matrix4f::Identity();

//teapot textures
std::vector<unsigned char> texture1; //diffuse
std::vector<unsigned char> texture2; //specular
//special glut keypresses
int glutControl;
float shadWidth = windowWidth * 0.5;
float shadHeight = windowHeight * 0.5;
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
        //p key
    case 'p':
        //normal perspective
        if (perspective)
        {
            perspective = false;
            potProjectionMatrix.SetPerspective(DEG2RAD(60), windowWidth / windowHeight, 0.1f, 1000.0f);
        }
        //orthogonal perspective
        else
        {
            perspective = true;
            potProjectionMatrix.OrthogonalizeX();
            potProjectionMatrix.OrthogonalizeY();
            potProjectionMatrix.OrthogonalizeZ();
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
        prog.BuildFiles("shaders/shader.vert", "/shaders/shader.frag");
        prog.Bind();
        prog2.BuildFiles("shaders/PlaneVert.vert", "/shaders/PlaneFrag.frag");
        prog2.Bind();
        shadowProg.BuildFiles("shaders/ShadVert.vert", "/shaders/ShadFrag.frag");
        shadowProg.Bind();
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
    prog["lightPos"] = lightDir;

    shadowProg["light"] = light;
    shadowProg["lightPos"] = lightDir;
    glutPostRedisplay();
}
//Handle mouse motion here while a button is down
void myMouseMotion(int x, int y)
{
    //right mouse button
    if (mouseButton == GLUT_RIGHT_BUTTON && glutControl == GLUT_ACTIVE_ALT)
    {
        //planeViewMatrix.AddTranslation(cy::Vec3f(0.0f, 0.0f, 1.0f) * 0.05f * (x - Xcoord));
        //planeViewMatrix.AddTranslation(cy::Vec3f(0.0f, 0.0f, 1.0f) * 0.05f * (y - Ycoord));
        potViewMatrix *= cy::Matrix4f::RotationX(((y - Ycoord) * M_PI) / 360 * 0.08);
        potViewMatrix *= cy::Matrix4f::RotationY(((x - Xcoord) * M_PI) / 360 * 0.08);
    }
    if (mouseButton == GLUT_LEFT_BUTTON && glutControl == GLUT_ACTIVE_CTRL)
    {
        light *= cy::Matrix4f::RotationX(((y - Ycoord) * M_PI) / 360 * 0.005);
        light *= cy::Matrix4f::RotationY(((x - Xcoord) * M_PI) / 360 * 0.005);
    }
    if (mouseButton == GLUT_LEFT_BUTTON && glutControl == GLUT_ACTIVE_ALT)
    {
        //planeViewMatrix *= cy::Matrix4f::RotationX(((y - Ycoord) * M_PI) / 360 * 0.08);
        //planeViewMatrix *= cy::Matrix4f::RotationY(((x - Xcoord) * M_PI) / 360 * 0.08);
        potViewMatrix *= cy::Matrix4f::RotationX(((y - Ycoord) * M_PI) / 360 * 0.08);
        potViewMatrix *= cy::Matrix4f::RotationY(((x - Xcoord) * M_PI) / 360 * 0.08);
    }
    if (mouseButton == GLUT_LEFT_BUTTON && glutControl != GLUT_ACTIVE_ALT && glutControl != GLUT_ACTIVE_CTRL)
    {
        //potViewMatrix *= cy::Matrix4f::RotationX(((y - Ycoord) * M_PI) / 360 * 0.08);
        potViewMatrix *= cy::Matrix4f::RotationY(((x - Xcoord) * M_PI) / 360 * 0.08);
    }
    if (mouseButton == GLUT_RIGHT_BUTTON && glutControl != GLUT_ACTIVE_ALT && glutControl != GLUT_ACTIVE_CTRL)
    {
        potViewMatrix.AddTranslation(cy::Vec3f(0.0f, 0.0f, 1.0f) * 0.05f * (x - Xcoord));
        potViewMatrix.AddTranslation(cy::Vec3f(0.0f, 0.0f, 1.0f) * 0.05f * (y - Ycoord));
    }
    int oldX = Xcoord;
    int oldY = Ycoord;
    Xcoord = x;
    Ycoord = y;

    planemv = planeViewMatrix * planeModelMatrix;
    objmv = potViewMatrix * potModelMatrix;

    NormMatrix = planemv.GetSubMatrix3();
    NormMatrix.Invert();
    NormMatrix.Transpose();

    objmvp = potProjectionMatrix * objmv;
    planemvp = planeProjectionMatrix * planeViewMatrix * planeModelMatrix;

    matrixMLP = potProjectionMatrix * light * potModelMatrix;
    matrixShadow = cy::Matrix4f::Translation(cy::Vec3f(0.5, 0.5, 0.5 - 0.0001)) * cy::Matrix4f::Scale(0.5) * matrixMLP;

    matrixMLPPlane = planeProjectionMatrix * light * planeModelMatrix;
    matrixShadowPlane = cy::Matrix4f::Translation(cy::Vec3f(0.5, 0.5, 0.5)) * cy::Matrix4f::Scale(0.5) * matrixMLPPlane;


    prog["mv"] = objmv;
    prog["normalMatrix"] = NormMatrix;
    prog["mvp"] = objmvp;
    prog["lightPos"] = lightDir;
    prog["light"] = light;
    prog["matrixShadow"] = matrixShadow;

    prog2["mvp"] = planemvp;
    prog2["matrixShadow"] = matrixShadowPlane;

    shadowProg["mvp"] = matrixMLP;
    //shadowProg["mvp"] = objmvp;
    shadowProg["matrixShadow"] = matrixShadow;



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

void initObject(char* file, bool load)
{
    bool success = mesh.LoadFromFileObj(file, load);
    if (!success)
    {
        printf("Failed");
    }
    mesh.ComputeNormals();
    mesh.ComputeBoundingBox();
}

//Initialize MVP matrices
void initMatrices()
{
    //Model Matrix
    //plane
    planeModelMatrix *= cy::Matrix4f::RotationX(DEG2RAD(-90));
    //teapot
    potModelMatrix *= cy::Matrix4f::RotationX(DEG2RAD(-90));
    //view Matrix
    // cy::Vec3f pos = cy::Vec3f(0.0f, 0.0f, 50.0f);
    //plane
    planeViewMatrix.SetView(pos, cy::Vec3f(0.0f, 5.0f, 2.0f), cy::Vec3f(0.0f, 0.0f, 1.0f));
    //cy::Vec3f pos2 = cy::Vec3f(0.0f, 0.0f, 50.0f);
    //teapot
    potViewMatrix.SetView(pos2, cy::Vec3f(0.0f, 5.0f, 2.0f), cy::Vec3f(0.0f, 0.0f, 1.0f));;

    //light
    light.SetView(lightPos, cy::Vec3f(0.0f, 5.0f, 0.0f), cy::Vec3f(0.0f, 1.0f, 0.0f));
    //light.SetTranslation(cy::Vec3f(0.0f, 0.0f, 0.0f));

    //projection Matrix
    //plane
    planeProjectionMatrix.SetPerspective(DEG2RAD(60.0f), windowWidth / windowHeight, 0.1f, 1000.0f);
    //teapot
    potProjectionMatrix.SetPerspective(DEG2RAD(60.0f), windowWidth / windowHeight, 0.1f, 1000.0f);
   
    //mv matrix
    //teapot
    objmv = potViewMatrix * potModelMatrix;
    //plane
    planemv = planeViewMatrix * planeModelMatrix;

    //norm matrix, 3x3 inv-transpose of MV
    //teapot normals
    NormMatrix = planemv.GetSubMatrix3();
    NormMatrix.Invert();
    NormMatrix.Transpose();

    //MVP
    //teapot
    objmvp = potProjectionMatrix * objmv;
    //plane
    planemvp = planeProjectionMatrix * planeViewMatrix * planeModelMatrix;

    matrixMLP = potProjectionMatrix * light * potModelMatrix;
    matrixShadow = cy::Matrix4f::Translation(cy::Vec3f(0.5, 0.5, 0.5 - 0.0001)) * cy::Matrix4f::Scale(0.5) * matrixMLP;

    matrixMLPPlane = planeProjectionMatrix * light * planeModelMatrix;
    matrixShadowPlane = cy::Matrix4f::Translation(cy::Vec3f(0.5, 0.5, 0.5)) * cy::Matrix4f::Scale(0.5) * matrixMLPPlane;

}
//compile the shaders
void setshaders()
{
    //prog1
    prog.BuildFiles("shaders/shader.vert", "shaders/shader.frag");
    prog.Bind();
    prog["mv"] = objmv;
    prog["normalMatrix"] = NormMatrix;
    prog["mvp"] = objmvp;
    prog["light"] = light;
    prog["lightPos"] = lightPos;
    prog["matrixShadow"] = matrixShadow;

    //prog2
    prog2.BuildFiles("shaders/PlaneVert.vert", "shaders/PlaneFrag.frag");
    prog2.Bind();
    prog2["mvp"] = planemvp;
    prog2["matrixShadow"] = matrixShadowPlane;

    shadowProg.BuildFiles("shaders/shadVert.vert", "shaders/shadFrag.frag");
    shadowProg.Bind();
    shadowProg["mvp"] = matrixMLP;
    //shadowProg["mvp"] = objmvp;
    shadowProg["matrixShadow"] = matrixShadow;

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

    prog.SetAttribBuffer("pos", buff, 3);
    shadowProg.SetAttribBuffer("pos", buff, 3);

    //Normals
    glGenBuffers(1, &nbuff);
    glBindBuffer(GL_ARRAY_BUFFER, nbuff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * mesh.NF() * 3, &Normals[0], GL_STATIC_DRAW);
    prog.SetAttribBuffer("norms", nbuff, 3);
    //shadowProg.SetAttribBuffer("norms", nbuff, 3);

    //Textures
    glGenBuffers(1, &tbuff);
    glBindBuffer(GL_ARRAY_BUFFER, tbuff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * mesh.NF() * 3, &textureCoords[0], GL_STATIC_DRAW);
    prog.SetAttribBuffer("txc", tbuff, 3);
    //shadowProg.SetAttribBuffer("txc", tbuff, 3);
}
void initializeTextures()
{
    prog.Bind();
    unsigned success;
    unsigned txtW, txtH;
    //diffuse material
    if (mesh.M(0).map_Kd)
    {
        std::string text1(mesh.M(0).map_Kd);
        success = lodepng::decode(texture1, txtW, txtH, text1);
        if (success == 1)
        {
            printf("Error loading texture\n");
        }
        glGenTextures(1, &textbuff1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textbuff1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, txtW, txtH, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texture1[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        glUniform1i(glGetUniformLocation(prog.GetID(), "texKd"), 0);
    }

    if (mesh.M(0).map_Ks)
    {
        //specular material
        std::string text2(mesh.M(0).map_Ks);
        success = lodepng::decode(texture2, txtW, txtH, text2);
        if (success == 1)
        {
            printf("Error loading texture\n");
        }
        glGenTextures(1, &textbuff2);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textbuff2);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, txtW, txtH, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texture2[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        glUniform1i(glGetUniformLocation(prog.GetID(), "texKs"), 0);
    }
}
//Render to depth texture
void initDepthTexture()
{
    //initialization 
    //shadowProgram = glCreateProgram();

    //Create frame buffer
    //shadText.Initialize(true, 3, shadWidth, shadHeight);

    //Create depth buffer
    depthMaptext.Initialize(true, shadWidth, shadHeight);
    
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadWidth, shadHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    prog.Bind();
    shadow = glGetUniformLocation(prog.GetID(), "shadow");
    glUniform1i(shadow, 0);
    glGenerateMipmap(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);

    prog2.Bind();
    shadow = glGetUniformLocation(prog2.GetID(), "shadow");
    glUniform1i(shadow, 0);
    glGenerateMipmap(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);

    //configure frame buffer
    //glBindFramebuffer(GL_FRAMEBUFFER, shadText.GetID());
    //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
    //glDrawBuffer(GL_NONE);
    //glReadBuffer(GL_NONE);
    
    //if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //{
      // return;
    //}
    depthMaptext.BuildTextureMipmaps();
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

void initPlane()
{
    planeCoords.push_back(cy::Vec3f(-40.0f, -40.0f, 0.0f));
    planeCoords.push_back(cy::Vec3f(40.0f, -40.0f, 0.0f));
    planeCoords.push_back(cy::Vec3f(-40.0f, 40.0f, 0.0f));
    planeCoords.push_back(cy::Vec3f(40.0f, 40.0f, 0.0f));

    quadTextureCoords.push_back(cy::Vec2f(0.0f, 0.0f));
    quadTextureCoords.push_back(cy::Vec2f(1.0f, 0.0f));
    quadTextureCoords.push_back(cy::Vec2f(0.0f, 1.0f));
    quadTextureCoords.push_back(cy::Vec2f(1.0f, 1.0f));

    glUseProgram(prog2.GetID());
    glGenVertexArrays(1, &quadVao);
    glBindVertexArray(quadVao);

    cy::Vec3f edge1 = planeCoords[0] - planeCoords[1];
    cy::Vec3f edge2 = planeCoords[0] - planeCoords[2];
    cy::Vec3f normVec = edge1.Cross(edge2);
    normVec.Normalize();

    quadNormCoords.push_back(normVec);
    quadNormCoords.push_back(normVec);
    quadNormCoords.push_back(normVec);
    quadNormCoords.push_back(normVec);

    glGenBuffers(1, &quadVbo);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * 4, &planeCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
    prog2.SetAttribBuffer("pos", quadVbo, 3);

    glGenBuffers(1, &quadTxtCoords);
    glBindBuffer(GL_ARRAY_BUFFER, quadTxtCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * 4, &quadTextureCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quadTxtCoords);
    prog2.SetAttribBuffer("txc", quadTxtCoords, 2);

    glGenBuffers(1, &quadNorms);
    glBindBuffer(GL_ARRAY_BUFFER, quadNorms);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * 4, &quadNormCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quadNorms);
    prog2.SetAttribBuffer("norms", quadNorms, 3);
}

void myDisplay()
{
    //bind teapot vao
    glBindVertexArray(vertexArray);
    //bind shadow map program
    glUseProgram(shadowProg.GetID());
    shadowProg.Bind();
    //bind render buffer
    depthMaptext.Bind();
    //glViewport(0, 0, shadWidth, shadHeight);
    //glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, (sizeof(cy::Vec3f) * mesh.NF()));
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    depthMaptext.Unbind();
    //clear color and depth bit
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    
    glBindVertexArray(vertexArray);
    glUseProgram(prog.GetID());
    prog.Bind();
    glDrawArrays(GL_TRIANGLES, 0, (sizeof(cy::Vec3f) * mesh.NF()));
    //render teapot
    //glGenVertexArrays(1, &vertexArray);
    //glBindVertexArray(vertexArray);

    /*glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(prog.GetID(), "texKd"), 0);
    glBindTexture(GL_TEXTURE_2D, textbuff1);

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(prog.GetID(), "texKs"), 0);
    glBindTexture(GL_TEXTURE_2D, textbuff2);**/

    //glBindBuffer(GL_ARRAY_BUFFER, buff);
    //glBindBuffer(GL_ARRAY_BUFFER, nbuff);
    //glBindBuffer(GL_ARRAY_BUFFER, tbuff);
    //glDrawArrays(GL_TRIANGLES, 0, (sizeof(cy::Vec3f) * mesh.NF()));

    glClearColor(0.2f, 0.5f, 0.3f, 1.0f);

    //bind plane vao
    glBindVertexArray(quadVao);
    //bind quad program
    glUseProgram(prog2.GetID());
    prog2.Bind();;
    //render plane
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glutSwapBuffers();
}

void setMesh()
{
    for (int i = 0; i < mesh.NF(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            vertices.push_back(mesh.V(mesh.F(i).v[j]));
        }
    }
    for (int i = 0; i < mesh.NF(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            Normals.push_back(mesh.VN(mesh.FN(i).v[j]));
        }
    }
    for (int i = 0; i < mesh.NF(); i++)
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
    glutCreateWindow("CS5610 Project 7 ShadowMapping");
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
    initObject(argv[1], true);
    //create buffers
    initMatrices();
    //Compile shaders
    setshaders();
    //initialize buffers
    initBuffers();
    //create textures
    initializeTextures();
    //Other initializations
    initDepthTexture();
    initPlane();
    //call main loop
    glutMainLoop();
    //Exit when main loop is done
    return 0;
}