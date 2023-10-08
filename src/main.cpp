#include <main.hpp>
#include <camera.hpp>
#include <utils.hpp>

using namespace std;

float vertices[] = {
    // positions          // colors           // texture coords
    0.5f,  0.5f,  0.1f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
    0.5f,  -0.5f, 0.1f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
};

unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

const int startImage = 5;
const int endImage = 5;
const string directions = "NEWSU";

int x = 5, y = 5;
char dir = 'N';

vector<vector<map<char, unsigned int>>> textureIds;
unsigned int VBO, VAO, EBO;

int main(int argc, char **argv) {

    glutInit(&argc, argv);

    const int width = 500;
    const int height = 500;
    const unsigned int displayMode =
        GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL;

    glutInitDisplayMode(displayMode);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(width, height);

    int window = glutCreateWindow(argv[0]);
    glload::LoadFunctions();

    cout << "Initializing..." << endl;
    init();
    glutDisplayFunc(display);
    // glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    // glutMouseFunc(mouse);

    cout << "Starting main loop..." << endl;
    glutMainLoop();
}

void display() {
    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glutSwapBuffers();
}

void init() {

    // cameraInit();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    string path = "data/000.jpg";
    textureIds = vector<vector<map<char, unsigned int>>>(10, vector<map<char, unsigned int>>(10));

    for (int i = startImage; i <= endImage; i++) {
        for (int j = startImage; j <= endImage; j++) {
            for (char c : directions) {

                path[5] = '0' + i;
                path[6] = '0' + j;
                path[7] = c;

                auto res = glimg::loaders::stb::LoadFromFile(path);
                textureIds[i][j][c] = glimg::CreateTexture(res, 0);
            }
        }
    }

    GLuint program = initShaders();

    int uid = glGetUniformLocation(program, "ourTexture");
    cout << uid << endl;
    glUniform1i(uid, 0);
    glBindTexture(GL_TEXTURE_2D, textureIds[x][y][dir]);
}

int initShaders() {
    std::vector<GLuint> shaderList;

    // get the location of embedded fragment shader
    getEmbeddedData(shader_vertex_vert);
    getEmbeddedData(shader_fragment_frag);

    shaderList.push_back(LoadShader(GL_VERTEX_SHADER, shader_vertex_vert));
    shaderList.push_back(LoadShader(GL_FRAGMENT_SHADER, shader_fragment_frag));

    GLuint programId = CreateProgram(shaderList);

    std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
    glUseProgram(programId);
    return programId;
}
