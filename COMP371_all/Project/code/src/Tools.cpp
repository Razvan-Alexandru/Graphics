#include "Tools.h"

using namespace std;

void getReferenceTime() {
    const std::time_t epochJ2000 = 946728000;
    std::time_t current_time = std::time(nullptr);
    double time_elapsed = difftime(current_time, epochJ2000);
    referenceTime = time_elapsed / 3600.0;
}

GLFWwindow* init() {
    // Initialize GLFW and OpenGL version
    glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Solar System", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    
    glfwSetWindowSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, width, height);

    return window;
}

void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
    width = w;
    height = h;
    glViewport(0, 0, w, h);
}

GLuint setUpShaders() {
    string vertexShaderPath = "../src/shaders/vertex_shader.glsl";
    string fragmentShaderPath = "../src/shaders/fragment_shader.glsl";
    GLuint shaderProgram = loadShaders(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
    if(!shaderProgram) {
        cerr << "Error in loading Shaders";
        exit(0);
    }

    glUseProgram(shaderProgram);

    return shaderProgram;
}

void processInput(GLFWwindow *window, float &delta, vector<Planet> &p) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(!lb_pressed) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if(lb_pressed && !focus) {
        glfwSetCursorPosCallback(window, cam_rotate_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    const float cameraSpeed = 200.0f * delta; 
    if (!focus) {

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.translateFront(-cameraSpeed);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.translateFront(cameraSpeed);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.translateSide(-cameraSpeed);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.translateSide(cameraSpeed);
        if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera.translateUp(cameraSpeed);
        if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.translateUp(-cameraSpeed);

        glfwSetMouseButtonCallback(window, lb_pressed_callback);
        
    }
    else {
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            camAngle += 2.5f * delta;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            camAngle -= 2.5f * delta;
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
            focus = false;
            firstMouse = true;
            glm::vec3 direction = glm::normalize(camera.target() - camera.position());
            pitch = 0;

            yaw = glm::degrees(acos(direction.x));
            if(direction.z < 0)
                yaw *= -1;

            lastX =  width / 2.0;
            lastY =  height / 2.0;
        }
    }
    

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
        isKeyPressed = false;

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        if(!isKeyPressed) { 
            isKeyPressed = true;
            focus = true;
            current_cam_focus++;
            
            if(current_cam_focus > p.size() - 1) { current_cam_focus = 0; }
        }
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        glm::vec3 newpos = glm::vec3(414.0f, 0.0f, 0.0f);
        glm::vec3 newtarget = glm::vec3(0.0f, 0.0f, 0.0f);
        camera.setUp(newpos, newtarget);
        focus = false;
        current_cam_focus = -1;
        firstMouse = true;
        yaw   = 180.0f;
        pitch =  0.0f;
        lastX =  width / 2.0;
        lastY =  height / 2.0;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {

        if(simSpeed >= 25 * 24)
            simSpeed = 25 * 24;
        else if(simSpeed >= 24) 
            simSpeed += 24;
        else if(simSpeed < -24)
            simSpeed += 24;
        else
            simSpeed += 1;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {

        if(simSpeed <= -25 * 24)
            simSpeed = -25 * 24;
        else if(simSpeed <= -24) 
            simSpeed -= 24;
        else if(simSpeed > 24)
            simSpeed -= 24;
        else
            simSpeed -= 1;
    }
}

void followPlanet(Planet &p) {
   
    float posX = p.currentPosition.x + 3.8 * p.radius * glm::cos(camAngle);
    float posY = p.currentPosition.y;
    float posZ = p.currentPosition.z + 3.8 * p.radius * glm::sin(camAngle);
    glm::vec3 camPos = glm::vec3(posX, posY, posZ);

    glm::vec3 newtarget = p.currentPosition;
    camera.setUp(camPos, newtarget);
}

GLuint loadShaders(const char* vertexShaderFilePath, const char* fragmentShaderFilePath) {

    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    string VertexShaderCode;
    ifstream VertexShaderStream(vertexShaderFilePath, ios::in);
    if(VertexShaderStream.is_open()){
        string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    } else {
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertexShaderFilePath);
        getchar();
        return 0;
    }
    
    // Read the Fragment Shader code from the file
    string FragmentShaderCode;
    ifstream FragmentShaderStream(fragmentShaderFilePath, ios::in);
    if(FragmentShaderStream.is_open()){
        string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }
    
    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertexShaderFilePath);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
    
    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }
    
    
    
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragmentShaderFilePath);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
    
    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
    
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

GLuint loadTexture(const char* path) {
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
}

void setPlanet(Planet &p, string &modelfilename) {
    if(!parseOBJ(modelfilename, p.vertexInfo, p.vertices, p.TextureCoords, p.normals, p.faces)) {
        cerr << ".obj parsing failed\n" << endl;
    }
    
    glGenVertexArrays(1, &p.VAO);
    glBindVertexArray(p.VAO);

    glGenBuffers(1, &p.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, p.VBO);
    glBufferData(GL_ARRAY_BUFFER, p.vertexInfo.size() * sizeof(VertexInfo), &p.vertexInfo[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
}

void setRings(Rings &r, string &modelfilename) {
    if(!parseOBJ(modelfilename, r.vertexInfo, r.vertices, r.TextureCoords, r.normals, r.faces)) {
        cerr << ".obj parsing failed\n" << endl;
    }
        
    glGenVertexArrays(1, &r.VAO);
    glBindVertexArray(r.VAO);

    glGenBuffers(1, &r.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, r.VBO);
    glBufferData(GL_ARRAY_BUFFER, r.vertexInfo.size() * sizeof(VertexInfo), &r.vertexInfo[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
}

void setPlanets(vector<Planet> &planets, Rings &saturnRings) {

    float distanceScale = 2.8;

    // Sun
    Planet Sun;
    Sun.name = "Sun";
    Sun.id = 0;

    Sun.radius = 109;
    Sun.radius_str = "696'000 km";
    setRotation(Sun, 7.25, 609.12);

    Sun.currentPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    Sun.description = sun_description();
    Sun.mass = "332'950 Earths";
    Sun.volume = "1,300,000 Earths";
    Sun.gravity = "28 g";
    Sun.temperature = "15'700'000 K at centre, 5'000'000 K at corona";

    string sun_model = "../assets/sun.obj";
    string sun_texture = "../assets/sun.jpg";

    Sun.texture = loadTexture(sun_texture.c_str());
    setPlanet(Sun, sun_model);
    planets.push_back(Sun);
    cout << "Sun added!" << endl;

    // Mercery
    Planet Mercury;
    Mercury.name = "Mercury";
    Mercury.id = 1;

    Mercury.radius = 0.383;
    Mercury.radius_str = "2'440 km";
    setRotation(Mercury, 0.03, 1407.6);
    setOrbit(Mercury, 57.91 * distanceScale, 0.205630, 87.9691 * 24, 174.796, 7.01, 48.331, 29.124);
    createOrbit(Mercury);

    Mercury.description = mercury_description();
    setDescription(Mercury, "0.055 Earths", "0.056 Earths", "0.38 g", "67 °C", "none", "58.646 days", "115.88 days", "0.39 au", "~11 km/h", "47.36 km/s");

    string mercury_model = "../assets/mercury.obj";
    string mercury_texture = "../assets/mercury.jpg";

    Mercury.texture = loadTexture(mercury_texture.c_str());
    setPlanet(Mercury, mercury_model);
    planets.push_back(Mercury);
    cout << "Mercury added!" << endl;

    // Venus
    Planet Venus;
    Venus.name = "Venus";
    Venus.id = 2;

    Venus.radius = 0.95;
    Venus.radius_str = "6,052 km";
    setRotation(Venus, 117.36, 5832.6);
    setOrbit(Venus, 108.21 * distanceScale, 0.006772, 224.701 * 24, 50.115, 3.396, 76.680, 54.884);
    createOrbit(Venus);
    Venus.orbit.colour = glm::vec3((float) 234/255, (float) 176/255, (float) 73/255);

    Venus.description = venus_description();
    setDescription(Venus, "0.815 Earths", "0.857 Earths", "0.904 g", "464 °C", "none", "243 days", "~225 days", "0.72 au", "6.52 km/h", "35.02 km/s");

    string venus_model = "../assets/venus.obj";
    string venus_texture = "../assets/venus.jpg";

    Venus.texture = loadTexture(venus_texture.c_str());
    setPlanet(Venus, venus_model);
    planets.push_back(Venus);
    cout << "Venus added!" << endl;

    // Earth
    Planet Earth;
    Earth.name = "Earth";
    Earth.id = 3;
    
    Earth.radius = 1;
    Earth.radius_str = "6'378 km";
    setRotation(Earth, 23.44, 23.93);
    setOrbit(Earth, 149.598 * distanceScale, 0.0167086, 365.256 * 24, 358.617, 0.0f, -11.26, 114.207);
    createOrbit(Earth);
    Earth.orbit.colour = glm::vec3((float) 0/255, (float) 95/255, (float) 152/255);

    Earth.description = earth_description();
    setDescription(Earth, "5.97 x 10^24 kg", "1.08 x 10^12 km^3", "1g or 9.80665 m/s^2", "14.76 °C", "1, the Moon", "23h 56min 4s", "~365.256 days", "1 au or 1.496 x 10^8 km", "1674.4 km/h", "29.78 km/s");

    string earth_model = "../assets/earth.obj";
    string earth_texture = "../assets/earth.jpg";

    Earth.texture = loadTexture(earth_texture.c_str());
    setPlanet(Earth, earth_model);
    planets.push_back(Earth);
    cout << "Earth added!" << endl;

    // Mars
    Planet Mars;
    Mars.name = "Mars";
    Mars.id = 4;
    
    Mars.radius = 0.532;
    Mars.radius_str = "3'396 km";
    setRotation(Mars, 25.19, 24.62);
    setOrbit(Mars, 227.939 * distanceScale, 0.0934, 686.980 * 24, 19.412, 1.850, 49.578, 286.5);
    createOrbit(Mars);
    Mars.orbit.colour = glm::vec3((float) 240/255, (float) 116/255, (float) 68/255);

    Mars.description = mars_description();
    setDescription(Mars, "0.107 Earths", "0.151 Earths", "0.3794 g", "-60 °C", "2", "24h 37min 22.7s", "~1.88 years", "1.52 au", "870 km/h", "24.07 km/s");

    string mars_model = "../assets/mars.obj";
    string mars_texture = "../assets/mars.jpg";

    Mars.texture = loadTexture(mars_texture.c_str());
    setPlanet(Mars, mars_model);
    planets.push_back(Mars);
    cout << "Mars added!" << endl;

    // Jupiter
    Planet Jupiter;
    Jupiter.name = "Jupiter";
    Jupiter.id = 5;
    
    Jupiter.radius = 10.933;
    Jupiter.radius_str = "71'492 km";
    setRotation(Jupiter, 3.13, 9.93);
    setOrbit(Jupiter, 778.479 * distanceScale, 0.0489, 398.88 * 24, 20.020, 1.303, 100.464, 273.867);
    createOrbit(Jupiter);
    Jupiter.orbit.colour = glm::vec3((float) 178/255, (float) 156/255, (float) 132/255);

    Jupiter.description = jupiter_description();
    setDescription(Jupiter, "317.8 Earths", "1,321 Earths", "2.528 g", "-108.15 °C", "95", "9h 55min 30s", "11.862 years", "5.2 au", "45'000 km/h", "13.07 km/s");

    string jupiter_model = "../assets/jupiter.obj";
    string jupiter_texture = "../assets/jupiter.jpg";

    Jupiter.texture = loadTexture(jupiter_texture.c_str());
    setPlanet(Jupiter, jupiter_model);
    planets.push_back(Jupiter);
    cout << "Jupiter added!" << endl;

    // Saturn
    Planet Saturn;
    Saturn.name = "Saturn";
    Saturn.id = 6;
    
    Saturn.radius = 9.140;
    Saturn.radius_str = "60'268 km";
    setRotation(Saturn, 26.73, 10.66);
    setOrbit(Saturn, 1433.53 * distanceScale, 0.0565, 10755.70 * 24, 317.020, 2.485, 113.665, 339.392);
    createOrbit(Saturn); 
    Saturn.orbit.colour = glm::vec3((float) 235/255, (float) 211/255, (float) 165/255);
    
    Saturn.description = saturn_description();
    setDescription(Saturn, "95.159 Earths", "763.59 Earths", "1.065 g", "-139.15 °C", "146", "10h 33min 38s", "29.448 years", "9.54 au", "35'500 km/h", "9.68 km/s");

    string saturn_model = "../assets/saturn.obj";
    string saturn_texture = "../assets/saturn.jpg";

    Saturn.texture = loadTexture(saturn_texture.c_str());
    setPlanet(Saturn, saturn_model);
    planets.push_back(Saturn);
    cout << "Saturn added!" << endl;

    // Saturn Rings
    saturnRings.radius = 14.934;

    string saturn_rings_model = "../assets/saturn_rings.obj";
    string saturn_rings_texture = "../assets/saturn_rings.jpg";

    saturnRings.texture = loadTexture(saturn_rings_texture.c_str());
    setRings(saturnRings, saturn_rings_model);
    cout << "Saturn's rings added!" << endl;

    // Uranus
    Planet Uranus;
    Uranus.name = "Uranus";
    Uranus.id = 7;
    
    Uranus.radius = 3.98;
    Uranus.radius_str = "25'559 km";
    setRotation(Uranus, 97.77, 17.24);
    setOrbit(Uranus, 2870.972 * distanceScale, 0.04717, 30688.5 * 24, 142.238, 0.773, 74.006, 96.998);
    createOrbit(Uranus);
    Uranus.orbit.colour = glm::vec3((float) 152/255, (float) 180/255, (float) 192/255);

    Uranus.description = uranus_description();
    setDescription(Uranus, "14.536 Earths", "63.086 Earths", "0.886 g", "-197.2 °C", "28", "17h 14min 24s", "84 years", "19.2 au", "9'320 km/h", "5.43 km/s");

    string uranus_model = "../assets/uranus.obj";
    string uranus_texture = "../assets/uranus.jpg";

    Uranus.texture = loadTexture(uranus_texture.c_str());
    setPlanet(Uranus, uranus_model);
    planets.push_back(Uranus);
    cout << "Uranus added!" << endl;
    
    // Neptune
    Planet Neptune;
    Neptune.name = "Neptune";
    Neptune.id = 8;
    
    Neptune.radius = 3.865;
    Neptune.radius_str = "24'764 km";
    setRotation(Neptune, 28.32, 16.11);
    setOrbit(Neptune, 4500 * distanceScale, 0.008678, 60195 * 24, 259.883, 1.770, 131.783, 273.187);
    createOrbit(Neptune);
    Neptune.orbit.colour = glm::vec3((float) 60/255, (float) 108/255, (float) 201/255);

    Neptune.description = neptune_description();
    setDescription(Neptune, "17.147 Earths", "57.74 Earths", "1.14 g", "-201 °C", "16", "16h 6min 36s", "~165 years", "30.06 au", "9'650 km/h", "6.80 km/s");

    string neptune_model = "../assets/neptune.obj";
    string neptune_texture = "../assets/neptune.jpg";

    Neptune.texture = loadTexture(neptune_texture.c_str());
    setPlanet(Neptune, neptune_model);
    planets.push_back(Neptune);
    cout << "Neptune added!" << endl;
    cout << planets.size() - 1 << " Planets and 1 Sun!";
}

void setDescription(Planet &p, string mass, string volume, string gravity, string temperature, string satelites, string dayPeriod_str, string orbitPeriod_str, string averageDistanceToSun, string equatorialSpeed, string orbitalSpeed) {
    p.mass =                    mass;
    p.volume =                  volume;
    p.gravity =                 gravity;
    p.temperature =             temperature;
    p.satelites =               satelites;
    p.dayPeriod_str =           dayPeriod_str;
    p.orbitPeriod_str =         orbitPeriod_str;
    p.averageDistanceToSun =    averageDistanceToSun;
    p.equatorialSpeed =         equatorialSpeed;
    p.orbitalSpeed =            orbitalSpeed;
}

void setRotation(Planet &p, float inclination, float period) {
    p.inclination = glm::radians(inclination);
    p.dayPeriod = period;
}

void setOrbit(Planet &p, float majorAxis, float eccentricity, float orbitalPeriod, float anomalyJ200, float orbitInclination, float longitude, float argument) {
    p.majorAxis = majorAxis;
    p.eccentricity = eccentricity;
    p.orbitalPeriod = orbitalPeriod;
    p.anomalyJ2000 = glm::radians(anomalyJ200);
    p.anomalyM0 = p.anomalyJ2000 + (2 * PI / p.orbitalPeriod) * referenceTime;  // anomalyJ200 + 2pi/T * (2000 + (referenceTime) ÷ 365.25) 
    p.orbitInclination = glm::radians(orbitInclination);
    p.longitude = glm::radians(longitude);
    p.argument = glm::radians(argument);

    findPlane(p);
    p.k = sqrt((1 + p.eccentricity) / (1 - p.eccentricity));
    p.a = p.majorAxis * (1 - p.eccentricity * p.eccentricity);
}

void findPlane(Planet &p) {
    glm::vec3 ecliptic(1.0f, 0.0f, 0.0f);
    glm::vec3 eclipticNormal(0.0f, 1.0f, 0.0f);
    glm::vec3 ascendingNodeVec = glm::cos(p.longitude) * ecliptic + glm::sin(p.longitude) * glm::cross(eclipticNormal, ecliptic);
    glm::vec3 planeNormal = glm::cos(p.orbitInclination) * eclipticNormal + glm::sin(p.orbitInclination) * glm::cross(ascendingNodeVec, eclipticNormal);
    p.orbitP = glm::cos(p.argument) * ascendingNodeVec + glm::sin(p.argument) * glm::cross(planeNormal, ascendingNodeVec);
    p.orbitQ = glm::cross(planeNormal, p.orbitP);
}

void createOrbit(Planet &p) {
    float segmentLength = 2 * PI / p.orbit.nSegments;
    float r = p.a / (1 + p.eccentricity);
    glm::vec3 firstPoint = r * p.orbitP;
    Vertex last_v;
    last_v.x = firstPoint.x;
    last_v.y = firstPoint.y;
    last_v.z = firstPoint.z;

    for(float theta = 0; theta <= 2 * PI; theta += segmentLength) {
        float r = p.a / (1 + p.eccentricity * glm::cos(theta));
        glm::vec3 point = r * glm::cos(theta) * p.orbitP + r * glm::sin(theta) * p.orbitQ;
        Vertex v;
        v.x = point.x;
        v.y = point.y;
        v.z = point.z;

        p.orbit.vertices.push_back(last_v);
        p.orbit.vertices.push_back(v);

        last_v.x = v.x;
        last_v.y = v.y;
        last_v.z = v.z;
    }

    glGenVertexArrays(1, &p.orbit.VAO);
    glBindVertexArray(p.orbit.VAO);

    glGenBuffers(1, &p.orbit.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, p.orbit.VBO);
    glBufferData(GL_ARRAY_BUFFER, p.orbit.vertices.size() * sizeof(Vertex), &p.orbit.vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
}

void setSkybox(Skybox &s) {
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

	glGenVertexArrays(1, &s.VAO);
	glGenBuffers(1, &s.VBO);

	glBindVertexArray(s.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, s.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	vector<string> faces;

    faces.push_back("../assets/skybox/right.jpg");
    faces.push_back("../assets/skybox/left.jpg");
    faces.push_back("../assets/skybox/top.jpg");
    faces.push_back("../assets/skybox/bottom.jpg");
    faces.push_back("../assets/skybox/front.jpg");
    faces.push_back("../assets/skybox/back.jpg");

    GLuint textureID;
	glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    stbi_set_flip_vertically_on_load(false);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    s.texture = textureID;
    
}

double newtowns(double M, float e) {

    // Initial guess for E is M
    double E = M;
    
    while(true) {
        double nextE = E - ((E - (double) e * glm::sin(E) - M) / (1 - (double) e * glm::cos(E)));

        if(abs(nextE - E) < 0.000000000001) {
            break;
        }

        E = nextE;
    }

    return E;
}

void renderPlanets(GLuint shaderProgram, vector<Planet> &planets, Rings rings, glm::mat4 &projection) {

    glUseProgram(shaderProgram);
    setShaderMatrix(shaderProgram, "view", camera.view());
    setShaderMatrix(shaderProgram, "projection", projection);
    for(auto &planet : planets) {

        if(planet.id != 0) {
            glm::mat4 model(1.0f);

            // Orbit Calculation
            // Step 1 - Find the Mean Anomaly at time t
            double meanAnomaly = -planet.anomalyM0 + 2 * PI * timeElapsed / planet.orbitalPeriod;

            // Step 2 - Find Eccentric Anomaly using Newton's method as a numerical method
            double E = newtowns(meanAnomaly, planet.eccentricity);

            // Step 3 - Find true anomaly and r-vector in polar coordinate
            float trueAnomaly = glm::atan((double) planet.k * glm::tan(E / 2)) * 2;
            float r = planet.a / (1 + planet.eccentricity * glm::cos(trueAnomaly));

            // Step 4 - Find current position of planet using the r-position vector, the trueAnomaly and p and q vectors that defines the plane
            planet.currentPosition = r * cos(trueAnomaly) * planet.orbitP + r * sin(trueAnomaly) * planet.orbitQ;
            //cout << trueAnomaly << " ";
            //planet.currentPosition = glm::vec3(0.0f,0.0f,0.0f);

            if(focus) {
                followPlanet(planets[current_cam_focus]);
                setShaderMatrix(shaderProgram, "view", camera.view());
            }

            // Axial angle calculation
            planet.currentAxisAngle += 2 * (float) PI * deltaTime * simSpeed / planet.dayPeriod;

            model = glm::translate(model, planet.currentPosition);
            model = glm::rotate(model, planet.inclination, glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::rotate(model, planet.currentAxisAngle, glm::vec3(0.0f, 1.0f, 0.0f));
            
            setShaderMatrix(shaderProgram, "model", model);
            setShaderFloat(shaderProgram, "alpha", 1.0f);
            glBindTexture(GL_TEXTURE_2D, planet.texture);
            
            glBindVertexArray(planet.VAO);
            glDrawArrays(GL_TRIANGLES, 0, planet.vertexInfo.size());

            if(planet.id == 6) {
                rings.currentPosition = planet.currentPosition;
                glm::mat4 ringModel(1.0f);
                ringModel = glm::translate(ringModel, rings.currentPosition);
                ringModel = glm::rotate(ringModel, planet.inclination, glm::vec3(0.0f, 0.0f, 1.0f));

                setShaderMatrix(shaderProgram, "model", ringModel);
                setShaderFloat(shaderProgram, "alpha", 0.85f);
                glBindTexture(GL_TEXTURE_2D, rings.texture);
                glBindVertexArray(rings.VAO);
                glDrawArrays(GL_TRIANGLES, 0, rings.vertexInfo.size());
            }
        }
    }
    //cout << endl;
}

void renderSun(GLuint shaderProgram, Planet &sun, glm::mat4 &projection) {
    glBindTexture(GL_TEXTURE_2D, sun.texture);

    glUseProgram(shaderProgram);

    glm::mat4 model(1.0f);
    sun.currentAxisAngle += 2 * (float) PI * deltaTime * simSpeed / (sun.dayPeriod);

    model = glm::rotate(model, sun.inclination, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, sun.currentAxisAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    setShaderMatrix(shaderProgram, "model", model);
    setShaderMatrix(shaderProgram, "view", camera.view());
    setShaderMatrix(shaderProgram, "projection", projection);
    glBindVertexArray(sun.VAO);
    glDrawArrays(GL_TRIANGLES, 0, sun.vertexInfo.size());
    glBindVertexArray(0);
}

void renderOrbits(GLuint shaderProgram, vector<Planet> &planets, glm::mat4 &projection) {

    glUseProgram(shaderProgram);
    setShaderMatrix(shaderProgram, "view", camera.view());
    setShaderMatrix(shaderProgram, "projection", projection);

    for(auto &planet : planets) {

        if(planet.id != 0) {
            setShaderVector(shaderProgram, "colour", planet.orbit.colour);
            glBindVertexArray(planet.orbit.VAO);
            glDrawArrays(GL_LINES, 0, planet.orbit.vertices.size());
        }
    }
    glBindVertexArray(0);
}

void renderSkybox(GLuint skyboxShaderProgram, Skybox &s, glm::mat4 &projection) {
    glDepthFunc(GL_LEQUAL);
    glUseProgram(skyboxShaderProgram);
    
    setShaderMatrix(skyboxShaderProgram, "view", glm::mat4(glm::mat3(camera.view())));
    setShaderMatrix(skyboxShaderProgram, "projection", projection);
    
    glBindVertexArray(s.VAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, s.texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

bool parseOBJ(const string &filename, vector<VertexInfo> &vertexInfo, vector<Vertex> &vertices, vector<TextureCoord> &TextureCoords, vector<Normal> &normals, vector<Face> &faces) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file: " << filename << endl;
        return false;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string type;
        iss >> type;

        if (type == "v") {
            Vertex vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        } else if (type == "vt") {
            TextureCoord texCoord;
            iss >> texCoord.u >> texCoord.v;
            TextureCoords.push_back(texCoord);
        } else if (type == "vn") {
            Normal normal;
            iss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        } else if (type == "f") {
            Face face;
            for (int i = 0; i < 3; ++i) {
                string vertexData;
                iss >> vertexData;
                istringstream vertexIss(vertexData);
                vertexIss >> face.vIndex[i];
                if (vertexIss.peek() == '/') {
                    vertexIss.ignore();
                    if (vertexIss.peek() != '/') {
                        vertexIss >> face.tIndex[i];
                    }
                    if (vertexIss.peek() == '/') {
                        vertexIss.ignore();
                        vertexIss >> face.nIndex[i];
                    }
                }
            }
            faces.push_back(face);
        }
    }

    for(int i = 0; i < faces.size(); i++) {
        for(int j = 0; j < 3; j++){
            VertexInfo vInfo;
            vInfo.v = vertices[faces[i].vIndex[j] - 1];
            vInfo.n = normals[faces[i].nIndex[j] - 1];
            vInfo.t = TextureCoords[faces[i].tIndex[j] - 1];

            vertexInfo.push_back(vInfo);
        }
    }

    file.close();
    return true;
}

void setShaderMatrix(const GLuint program, const string &name, const glm::mat4 &mat) {
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void setShaderVector(const GLuint program, const string &name, const glm::vec3 &vec) {
    glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &vec[0]);
}

void setShaderFloat(const GLuint program, const string &name, GLfloat value) {
    glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void setShaderInt(const GLuint program, const string &name, GLfloat value) {
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void cam_rotate_callback(GLFWwindow* window, double xposIn, double yposIn) {
    
    if(!lb_pressed) { return; }

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 90.0f)
        pitch = 90.0f;
    if (pitch < -90.0f)
        pitch = -90.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera.setDirection(-glm::normalize(front));
    camera.updateCam();
}

void lb_pressed_callback(GLFWwindow* window, int button, int action, int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT && GLFW_PRESS == action && !focus)
        lb_pressed = true;
    else if(button == GLFW_MOUSE_BUTTON_LEFT && GLFW_RELEASE == action) {
        lb_pressed = false;
        firstMouse = true;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 90.0f)
        fov = 90.0f; 
}

