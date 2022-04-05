/*
*/

#include "trajectory.hpp"

GLuint shaderProgram;
GLuint vboControl, vboSpline, vaoSpline, vaoControl;
GLuint vControlPosPtr, vSplinePosPtr;

std::vector<GLuint> vaoTr;
std::vector<GLuint> vboTr;
std::vector<GLuint> vPosTr;
std::vector<glm::vec4> splineColors;

glm::mat4 lookat_matrix;
glm::mat4 projection_matrix;
glm::mat4 modelview_matrix;
glm::mat4 invView_matrix;
GLuint uModelViewMatrix;
GLuint uColor;

int selectIndex = -1;

//-----------------------------------------------------------------

int numControlPoints = 0;
const int numBreaks = 40;
glm::vec4 v_control[200];
glm::vec4 vSpline[numBreaks + 1]; // for storing the spline points which are
                                  // numBreaks + 1 points including the two end points
glm::vec4 red = glm::vec4(1.0, 0.0, 0.0, 1.0);
glm::vec4 yellow = glm::vec4(1.0, 1.0, 0.0, 0.0);
glm::vec4 green = glm::vec4(0.0, 1.0, 0.0, 1.0);
glm::vec4 controlColor = red;
glm::vec4 splineColor = glm::vec4(0.0, 0.0, 1.0, 1.0);
glm::vec4 controlLineColor = green;

std::vector<std::vector<int> /**/> pascalTri(100, std::vector<int>(101, 0));

float nearLimit = 0.05;

void fillPascal()
{
    pascalTri[0][0] = 1;
    pascalTri[0][1] = 1;
    for (int i = 1; i < 100; i++)
    {
        pascalTri[i][0] = 1;
        for (int j = 1; j <= i + 1; j++)
        {
            pascalTri[i][j] = pascalTri[i - 1][j - 1] + pascalTri[i - 1][j];
        }
    }
}

int ncr(int n, int r)
{
    return pascalTri[n - 1][r];
}

void drawSpline(glm::vec4 vControlPtr[], glm::vec4 vSplinePtr[], int numPoints)
{
    float t = 0.0;
    float t1k, tk; // for storing t^k and (1-t)^(n-k)
    vSplinePtr[0] = vControlPtr[0];
    for (int i = 1; i < numBreaks; i++)
    {
        //std::cout << i << " ";
        t = float(i) / float(numBreaks);
        t1k = 1.0;
        tk = 1.0;
        for (int j = 0; j < numPoints - 1; j++)
        {
            t1k *= (1 - t);
        }
        float xPos, yPos, zPos;
        xPos = yPos = zPos = 0.0;
        for (int j = 0; j < numPoints; j++)
        {
            xPos += vControlPtr[j][0] * float(ncr(numPoints - 1, j)) * tk * t1k;
            yPos += vControlPtr[j][1] * float(ncr(numPoints - 1, j)) * tk * t1k;
            zPos += vControlPtr[j][2] * float(ncr(numPoints - 1, j)) * tk * t1k;
            tk = tk * t;
            t1k = t1k / (1.0 - t);
        }
        vSplinePtr[i] = glm::vec4(xPos, yPos, zPos, 1.0);
    }
    vSplinePtr[numBreaks] = vControlPtr[numPoints - 1];
    return;
}

void initBuffersGL(void)
{
    //Ask GL for a Vertex Attribute Object (vao)
    glGenVertexArrays(1, &vaoControl);
    //Set it as the current array to be used by binding it
    glBindVertexArray(vaoControl);

    //Ask GL for a Vertex Buffer Object (vbo)
    glGenBuffers(1, &vboControl);
    //Set it as the current buffer to be used by binding it
    glBindBuffer(GL_ARRAY_BUFFER, vboControl);
    //Copy the points into the current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(v_control), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v_control), v_control);

    // Load shaders and use the resulting shader program
    std::string vertex_shader_file("tr_vshader.glsl");
    std::string fragment_shader_file("tr_fshader.glsl");

    std::vector<GLuint> shaderList;
    shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
    shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

    shaderProgram = csX75::CreateProgramGL(shaderList);
    glUseProgram(shaderProgram);

    // set up vertex arrays
    GLuint vControlPosPtr = glGetAttribLocation(shaderProgram, "vPosition");
    glEnableVertexAttribArray(vControlPosPtr);
    glVertexAttribPointer(vControlPosPtr, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    uModelViewMatrix = glGetUniformLocation(shaderProgram, "uModelViewMatrix");
    uColor = glGetUniformLocation(shaderProgram, "colorVar");

    //Ask GL for a Vertex Attribute Object (vao)
    glGenVertexArrays(1, &vaoSpline);
    //Set it as the current array to be used by binding it
    glBindVertexArray(vaoSpline);

    //Ask GL for a Vertex Buffer Object (vbo)
    glGenBuffers(1, &vboSpline);
    //Set it as the current buffer to be used by binding it
    glBindBuffer(GL_ARRAY_BUFFER, vboSpline);
    //Copy the points into the current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vSpline), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vSpline), vSpline);

    glUseProgram(shaderProgram);

    // set up vertex arrays
    GLuint vSplinePosPtr = glGetAttribLocation(shaderProgram, "vPosition");
    glEnableVertexAttribArray(vSplinePosPtr);
    glVertexAttribPointer(vSplinePosPtr, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    uModelViewMatrix = glGetUniformLocation(shaderProgram, "uModelViewMatrix");
    uColor = glGetUniformLocation(shaderProgram, "colorVar");
}

float dist(glm::vec4 a, glm::vec4 b)
{
    float dis = (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2]);
    dis = sqrt(dis);
    return dis;
}

void delPoint(int index)
{
    for (int i = index; i < numControlPoints - 1; i++)
    {
        v_control[i] = v_control[i + 1];
    }
    numControlPoints--;
}

void addTrajectory(glm::vec4 vSplinePtr[])
{
    GLuint vaoTemp, vboTemp, posTemp;
    vaoTr.push_back(vaoTemp);
    vboTr.push_back(vboTemp);
    vPosTr.push_back(posTemp);
    glm::vec4 colorTemp;
    colorTemp = glm::vec4(float(rand()) / float(RAND_MAX), float(rand()) / float(RAND_MAX), float(rand()) / float(RAND_MAX), 1.0);
    splineColors.push_back(colorTemp);
    int index = vaoTr.size() - 1;
    glGenVertexArrays(1, &vaoTr[index]);
    //Set it as the current array to be used by binding it
    glBindVertexArray(vaoTr[index]);

    //Ask GL for a Vertex Buffer Object (vbo)
    glGenBuffers(1, &vboTr[index]);
    //Set it as the current buffer to be used by binding it
    glBindBuffer(GL_ARRAY_BUFFER, vboTr[index]);
    //Copy the points into the current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vSplinePtr[0]) * 201, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vSplinePtr[0]) * 201, vSplinePtr);

    glUseProgram(shaderProgram);

    // set up vertex arrays
    vPosTr[index] = glGetAttribLocation(shaderProgram, "vPosition");
    glEnableVertexAttribArray(vPosTr[index]);
    glVertexAttribPointer(vPosTr[index], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    uModelViewMatrix = glGetUniformLocation(shaderProgram, "uModelViewMatrix");
    uColor = glGetUniformLocation(shaderProgram, "colorVar");
}

void saveToFile()
{
    std::ofstream outfile;
    outfile.open("trajectory.txt", std::ios_base::app);
    outfile << std::to_string(numControlPoints) << std::endl;
    for (int i = 0; i < numControlPoints; i++)
    {
        outfile << std::to_string(v_control[i][0]) << " " << std::to_string(v_control[i][1]) << " " << std::to_string(v_control[i][2]) << std::endl;
    }
    outfile.close();
    addTrajectory(vSpline);
    numControlPoints = 0;
}

void readFile()
{
    std::ifstream infile;
    infile.open("trajectory.txt");
    std::string numPoints;
    vaoTr.clear();
    vboTr.clear();
    vPosTr.clear();
    splineColors.clear();
    while (infile >> numPoints)
    {
        int N = atoi(numPoints.c_str());
        glm::vec4 *controlPtr = new glm::vec4[N];
        for (int i = 0; i < N; i++)
        {
            float x, y, z;
            infile >> x >> y >> z;
            controlPtr[i] = glm::vec4(x, y, z, 1.0);
        }
        glm::vec4 vSplineTemp[201];
        drawSpline(controlPtr, vSplineTemp, N);
        std::cout << vSplineTemp[0][0] << " " << vSplineTemp[0][1] << std::endl;
        addTrajectory(vSplineTemp);
        delete[] controlPtr;
    }
}

/**
 *  returns the nearest point to the mouse, or returns -1 if the mouse is far away 
 */
int findNearest(glm::vec4 mousePose)
{
    if (numControlPoints == 0)
    {
        return -1;
    }
    float minDis = dist(mousePose, v_control[0]);
    int index = 0;
    std::cout << "Dis : " << minDis << std::endl;
    for (int i = 1; i < numControlPoints; i++)
    {
        float disCur = dist(mousePose, v_control[i]);
        std::cout << "Dis : " << disCur << std::endl;
        if (disCur < minDis)
        {
            minDis = disCur;
            index = i;
        }
    }
    std::cout << "Min Dis : " << minDis << std::endl;
    if (minDis < nearLimit)
    {
        return index;
    }
    else
    {
        return -1;
    }
}

void renderGL(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 c_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(c_xrot), glm::vec3(1.0f, 0.0f, 0.0f));
    c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_yrot), glm::vec3(0.0f, 1.0f, 0.0f));
    c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_zrot), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec4 c_pos = glm::vec4(c_xpos, c_ypos, c_zpos, 1.0);
    glm::vec4 c_up = glm::vec4(c_up_x, c_up_y, c_up_z, 1.0) * c_rotation_matrix;
    glm::vec4 c_lookat = glm::vec4(0.0, 0.0, -1.0, 1.0) * c_rotation_matrix;
    c_lookat = c_lookat + c_pos;
    glm::mat4 lookat_matrix = glm::lookAt(glm::vec3(c_pos), glm::vec3(c_lookat), glm::vec3(c_up));
    projection_matrix = glm::frustum(-0.5, 0.5, -0.5, 0.5, 0.5, 500.0);

    modelview_matrix = projection_matrix * lookat_matrix;

    if (mode == 0)
    {

        if (mouseLeftPress)
        {
            v_control[numControlPoints] = glm::inverse(modelview_matrix) * glm::vec4(mouseX * 2.0 - 1.0, mouseY * (-2.0) + 1.0, 0.0, 1.0);
            numControlPoints++;
            if (numControlPoints > 1)
                drawSpline(v_control, vSpline, numControlPoints);
            //std::cout << mouseX << " " << mouseY << std::endl;

            glBindVertexArray(vaoControl);
            //Set it as the current buffer to be used by binding it
            glBindBuffer(GL_ARRAY_BUFFER, vboControl);
            //Copy the points into the current buffer
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v_control), v_control);

            glBindVertexArray(vaoSpline);
            //Set it as the current buffer to be used by binding it
            glBindBuffer(GL_ARRAY_BUFFER, vboSpline);
            //Copy the points into the current buffer
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vSpline), vSpline);
            mouseLeftPress = false;
        }
        if (savePoints)
        {
            saveToFile();
            savePoints = false;
        }
        if (loadPoints)
        {
            readFile();
            loadPoints = false;
        }
        if (deletePoint)
        {
            deletePoint = false;
        }

        if (mouseRightPress)
        {
            glm::vec4 mouseCord = glm::inverse(modelview_matrix) * glm::vec4(2.0 * mouseX - 1.0, (-2.0) * mouseY + 1.0, mouseZ, 1.0);
            selectIndex = findNearest(mouseCord);
            if (selectIndex > -1)
            {
                mode = 1;
                controlColor = yellow;
                std::cout << "Mode is 1\n";
            }
            mouseRightPress = false;
        }
    }

    if (mode == 1)
    {
        v_control[selectIndex] = glm::inverse(modelview_matrix) * glm::vec4(mouseX * 2.0 - 1.0, mouseY * (-2.0) + 1.0, 0.0, 1.0);

        if (deletePoint)
        {
            delPoint(selectIndex);
            mode = 0;
            controlColor = red;
            deletePoint = false;
        }

        if (savePoints)
        {
            savePoints = false;
        }
        if (loadPoints)
        {
            loadPoints = false;
        }
        if (numControlPoints > 1)
            drawSpline(v_control, vSpline, numControlPoints);
        //std::cout << mouseX << " " << mouseY << std::endl;

        glBindVertexArray(vaoControl);
        //Set it as the current buffer to be used by binding it
        glBindBuffer(GL_ARRAY_BUFFER, vboControl);
        //Copy the points into the current buffer
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v_control), v_control);

        glBindVertexArray(vaoSpline);
        //Set it as the current buffer to be used by binding it
        glBindBuffer(GL_ARRAY_BUFFER, vboSpline);
        //Copy the points into the current buffer
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vSpline), vSpline);

        if (mouseRightPress)
        {
            mode = 0;
            controlColor = red;
            mouseRightPress = false;
        }
        if (mouseLeftPress)
        {
            mouseLeftPress = false;
        }
    }

    for (int i = 0; i < vaoTr.size(); i++)
    {
        //std::cout << "hi";
        glUseProgram(shaderProgram);
        glBindVertexArray(vaoTr[i]);
        glBindBuffer(GL_ARRAY_BUFFER, vboTr[i]);
        glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
        glUniform4f(uColor, splineColors[i][0], splineColors[i][1], splineColors[i][2], splineColors[i][3]);
        glDrawArrays(GL_LINE_STRIP, 0, numBreaks + 1);
    }
    if (numControlPoints > 0)
    {
        glUseProgram(shaderProgram);
        glBindVertexArray(vaoControl);
        glBindBuffer(GL_ARRAY_BUFFER, vboControl);
        glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
        glUniform4f(uColor, controlColor[0], controlColor[1], controlColor[2], controlColor[3]);
        glDrawArrays(GL_POINTS, 0, numControlPoints);

        if (numControlPoints > 1)
        {
            glUseProgram(shaderProgram);
            glBindVertexArray(vaoSpline);
            glBindBuffer(GL_ARRAY_BUFFER, vboSpline);
            glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
            glUniform4f(uColor, splineColor[0], splineColor[1], splineColor[2], splineColor[3]);
            glDrawArrays(GL_LINE_STRIP, 0, numBreaks + 1);

            glUseProgram(shaderProgram);
            glBindVertexArray(vaoControl);
            glBindBuffer(GL_ARRAY_BUFFER, vboControl);
            glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
            glUniform4f(uColor, controlLineColor[0], controlLineColor[1], controlLineColor[2], controlLineColor[3]);
            glDrawArrays(GL_LINE_STRIP, 0, numControlPoints);
        }
    }
    //glDrawArrays(GL_TRIANGLES, 0, num_vertices);
}

int main(int argc, char **argv)
{
    //! The pointer to the GLFW window
    GLFWwindow *window;

    //! Setting up the GLFW Error callback
    glfwSetErrorCallback(csX75::error_callback);

    //! Initialize GLFW
    if (!glfwInit())
        return -1;

    //We want OpenGL 4.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //This is for MacOSX - can be omitted otherwise
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //We don't want the old OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //! Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(512, 512, "CS475/CS675 Tutorial 3: Rotating  Colorcube", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    fillPascal();

    //! Make the window's context current
    glfwMakeContextCurrent(window);

    //Initialize GLEW
    //Turn this on to get Shader based OpenGL
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    glEnable(GL_PROGRAM_POINT_SIZE);
    if (GLEW_OK != err)
    {
        //Problem: glewInit failed, something is seriously wrong.
        std::cerr << "GLEW Init Failed : %s" << std::endl;
    }

    //Print and see what context got enabled
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    //Mouse Callback
    glfwSetMouseButtonCallback(window, csX75::mouse_button_callback);
    glfwSetCursorPosCallback(window, csX75::mouse_callback);

    //Keyboard Callback
    glfwSetKeyCallback(window, csX75::key_callback);
    //Framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    //Initialize GL state
    csX75::initGL();
    initBuffersGL();

    // Loop until the user closes the window
    while (glfwWindowShouldClose(window) == 0)
    {

        // Render here
        renderGL();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

//-------------------------------------------------------------------------
