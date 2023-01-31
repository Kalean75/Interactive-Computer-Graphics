//Devin White
//Cs 5610 Interactive Computer Graphics
//Project 2
#include <iostream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>
//#include <GL/glfw3.h>
//#include <cyCodeBase/cyMatrix.h>
//#include <cyCodeBase/cyGL.h>
//#include <cyCodeBase/cyTriMesh.h>
//#include <cmath>
//#include <GL/vmath.h>
#include <ctype.h>
#include <string.h>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#pragma comment(lib, "glew32.lib")


#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

int DEG2RAD(int degrees);

int window_width = 1920;
int window_height = 1080;
int mouseX, mouseY, mouseButton;
cy::Vec3f position;
cy::Vec3f  front;
cy::Vec3f  up;
cy::Vec3f cameraCoords;
GLuint VAO;
GLuint VBO;
GLuint buff;

cy::GLSLProgram prog;
cy::TriMesh mesh;
cy::GLSLShader shader;

cy::Matrix4<float> matrix4_obj;
//cy::Point3<float> objTrans;

//cy::Vec3f getArcballVector(int x, int y);
//identity matrix
cy::Matrix4f translMatrix = cy::Matrix4f::Identity();
cy::Matrix4f rotMatrix = cy::Matrix4f::Identity();
cy::Matrix4f viewMatrix;
cy::Matrix4f ModelMatrix = cy::Matrix4f::Identity();
cy::Matrix4f projectionMatrix = cy::Matrix4f::Perspective(DEG2RAD(40), float(window_width) / float(window_height), 0.1f, 100.0f);
cy::Matrix4f mvp;
//cy::Vec3f axis_in_camera_coord;

int DEG2RAD(int degrees)
{
    int radians = degrees * (M_PI / 180);
    
    return radians;
}

int RAD2DEG(int radians)
{
    int degrees = radians * (180 / M_PI);

    return degrees;
}

// Function that return
// dot product of two vector array.
int dotProduct(cy::Vec3f vec1, cy::Vec3f vec2)
{

    int product = 0;
    for (int i = 0; i < vec1.Length(); i++)
    {
        product = product + vec1[i] * vec2[i];
    }
    return product;
}

// Function to find
// cross product of two vector array.
cy::Vec3f crossProduct(cy::Vec3f vec1, cy::Vec3f vec2)
{
    cy::Vec3f result;

    result[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
    result[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
    result[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];

    return result;
}


void myKeyboard(unsigned char key, int x, int y)
{
    //Handle keyboard input here
    switch (key)
    {
    case 27: //ESC
        glutLeaveMainLoop();
        break;
    }
    glutPostRedisplay();
}

void myKeyboard2(int key, int x, int y)
{
    //Handle special keyboard input here
} //glutGetModifiers();

void myMouse(int button, int state, int x, int y)
{
    //handle mouse buttons here
    mouseX = x;
    mouseY = y;
    mouseButton = button;
}

void myMouseMotion(int x, int y)
{
    //Handle mouse motion here while a button is down
    //left mouse button
    if (mouseButton == GLUT_LEFT_BUTTON)
    {
        cy::Matrix4f idMat = cy::Matrix4f::Identity();
        int newX, newY;
        newX = x - mouseX;
        newY = y - mouseY;
        cy::Matrix4f rotY = idMat.RotationY(-newY * (M_PI / 360 * 0.1));
        cy::Matrix4f rotX = idMat.RotationX(-newX * (M_PI / 360 * 0.1));
        front = cyMatrix3f(rotX * rotY) * front;

    }
    //right mouse button
    if (mouseButton == GLUT_RIGHT_BUTTON)
    {
        int newX, newY;
        newX = x - mouseX;
        newY = y - mouseY;
        cy::Vec3f  Max = mesh.GetBoundMax();
        cy::Vec3f  Min = mesh.GetBoundMin();
        cy::Vec3f objectCenter = cyMatrix3f(ModelMatrix) * cy::Vec3f((Max.x + Min.x) / 2, (Max.y + Min.y) / 2, (Max.z + Min.z) / 2);
        if (newY > 0) position -= 0.1f * (objectCenter - position);
        if (newY < 0) position += 0.1f * (objectCenter - position);
    }
}

void myMousePassive(int x, int y)
{
    //Handle mouse motion here
    //while a button is NOT down
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
    glClearColor(red, green, blue, alpha);
    //Tell GLUT to redraw
    glutPostRedisplay();
}

void initObject(bool load)
{
    bool success = mesh.LoadFromFileObj("teapot.obj",load);
}
//Compile the shaders
void setshaders()
{
    //compile shaders
    bool success = prog.BuildFiles("shader.vert", "shader.frag");
    //vs_id = shader.GetID();
    //fs_id = shader.GetID();
    //program1 = prog.GetID();
}

void initBuffers()
{
    cameraCoords = cy::Vec3f(0.0f, 1.0f, 0.0f);
    rotMatrix.SetRotation(cameraCoords, 0.0f);
    //projection matrix
    projectionMatrix = cy::Matrix4f::Perspective(DEG2RAD(40), float(window_width) / float(window_height), 0.1f, 1000.0f);
    viewMatrix.SetView(cy::Vec3f(0.0f, 30.0f, 100.0f), cy::Vec3f(0.0f, 0.0f, 0.0f), cy::Vec3f(0.0f, 1.0f, 0.0f));
    viewMatrix *= translMatrix;
    viewMatrix *= rotMatrix;
    ModelMatrix.Scale(20.0f);
    mvp = projectionMatrix * viewMatrix * ModelMatrix;
    //generate and bind the vertex array object
    glClearColor(0.0, 0.0, 0.0, 1.0);
    initObject(true);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    //next, we generate a vertex buffer and set the vertices as its data
    glGenBuffers(1, &buff);
    glBindBuffer(GL_ARRAY_BUFFER, buff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec4f) * mesh.NV(), &mesh.V(0), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindVertexArray(0);
}
void myDisplay()
{
    /*cameraCoords = cy::Vec3f(0.0f, 1.0f, 0.0f);
    rotMatrix.SetRotation(cameraCoords, 0.0f);
    //projection matrix
    projectionMatrix = cy::Matrix4f::Perspective(DEG2RAD(40), float(window_width) / float(window_height), 0.1f, 1000.0f);
    viewMatrix.SetView(cy::Vec3f(0.0f, 30.0f, 100.0f), cy::Vec3f(0.0f, 0.0f, 0.0f), cy::Vec3f(0.0f, 1.0f, 0.0f));
    //printf("%f", viewMatrix[5]);
    viewMatrix *= translMatrix;
    viewMatrix *= rotMatrix;
    ModelMatrix.Scale(20.0f);
    mvp = projectionMatrix * viewMatrix * ModelMatrix;
    //clear the viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);;
    glGetUniformLocation(prog.GetID(), "mvp");
    glUniformMatrix4fv(glGetUniformLocation(prog.GetID(), "mvp"), 1,GL_FALSE, &mvp[10]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clearing the buffer with the preset color

    glEnableVertexAttribArray(0);
    glBindVertexArray(VAO);
    prog.Bind();
    glDrawArrays(GL_POINTS, 0, sizeof(cy::Vec3f) * mesh.NV());
    glDisableVertexAttribArray(0);*/
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.5f,-0.5f);
    glVertex2f(0.0f,0.5f);
    glVertex2f(0.5f,-0.5f);
    glEnd();

    //OpenGL draw calls here

    //swap buffers
    glutSwapBuffers();
}

//GLUT initializations
void init(int argc, char** argv)
{
    //initialize GLUT
    glutInit(&argc, argv);

    //create a window
    glutInitWindowSize(1920, 1080);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("CS5610 Project 2 Transformations");
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
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

int main(int argc, char** argv)
{
    //GLUT initializations
    init(argc, argv);
    //initialize object
    //create buffers
    initBuffers();
    //create textures
    //Compile shaders
    setshaders();

    //Other initializations
    //call main loop
    glutMainLoop();
    //Exit when main loop is done
    return 0;
}