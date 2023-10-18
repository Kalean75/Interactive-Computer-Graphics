//Devin White
//Cs 5610 Interactive Computer Graphics
//Project 1
#include <GL/freeglut.h>

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
}

void myMouseMotion(int x, int y)
{
    //Handle mouse motion here while a button is down
}

void myMousePassive(int x, int y)
{
    //Handle mouse motion here
    //while a button is NOT down
}

void myReshape(int x, int y)
{
    //Do what you want when window size changes
}

void myIdle()
{
    int red = 0.3;
    int green = 1.0;
    int blue = 1.0;
    int alpha = 1.0;
    //handle animations here
    glClearColor(red, green, blue, alpha);
    green += 200.0;
    //Tell GLUT to redraw
    glutPostRedisplay();
}

void myDisplay()
{
    //clear the viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //OpenGL draw calls here

    //swap buffers
    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    //GLUT initializations
        //initialize GLUT
    glutInit(&argc, argv);

    //create a window
    glutInitWindowSize(1920, 1080);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("CS5610 Project 1 HelloWorld");
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

    //openGL initializations
        //set the background color
    glClearColor(1, 1, 1, 1);
    //create buffers
    //create textures
    //Compile shaders

//Other initializations


//call main loop
    glutMainLoop();
    //Exit when main loop is done
    return 0;
}