#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"
#include "model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <random>
#include <cmath>
#include <stack>

#include <irrklang/irrKlang.h>
#pragma comment(lib, "irrKlang.lib") 

//#include "render_text.h"

void renderQuad();
void renderQuad2();
void renderCube();

const float PI = 3.14159265358979323846;
int playGameOver = 1;
int playGameWin = 1;

using namespace irrklang;

ISoundEngine* soundEngine;
ISoundSource* mainTheme;
ISoundSource* rimbalzoMattoni;
ISoundSource* rimbalzoMattoniOro;
ISoundSource* rimbalzoBordo;
ISoundSource* rimbalzoPiattaforma;
ISoundSource* gameOver;
ISoundSource* gameStart;
ISoundSource* gameWin;
ISound* ambientSound;

// settings
int SCR_WIDTH = 1920;
int SCR_HEIGHT = 1080;

//Bloom settings
bool bloom = true;
bool bloomKeyPressed = false;
float exposure = 1.0f;
unsigned int hdrFBO;
unsigned int pingpongFBO[2];
unsigned int pingpongColorbuffers[2];
unsigned int colorBuffers[2];

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

std::stack<glm::mat4> glm_ModelViewMatrix;

vector<glm::vec3> windows
{
	glm::vec3(-1.5f, 0.0f, -0.48f),
	glm::vec3(1.5f, 0.0f, 0.51f),
	glm::vec3(0.0f, 0.0f, 0.7f),
	glm::vec3(-0.3f, 0.0f, -2.3f),
	glm::vec3(0.5f, 0.0f, -0.6f)
};

float vertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

int mapCorner[14][14] = { {7,7,7,7,7,7,7,7,7,7,7,7,7,7 },
						  {7,0,0,0,0,0,0,0,0,0,0,0,0,0 },
						  {7,0,0,0,0,0,0,0,0,0,0,0,0,0 },
						  {7,0,0,0,0,0,0,0,0,0,0,0,0,0 },
						  {7,0,0,0,0,0,0,0,0,0,0,0,0,0 },
						  {7,0,0,0,0,0,0,0,0,0,0,0,0,0 },
						  {7,0,0,0,0,0,0,0,0,0,0,0,0,0 },
						  {7,0,0,0,0,0,0,0,0,0,0,0,0,0 },
						  {7,0,0,0,0,0,0,0,0,0,0,0,0,0 },
						  {7,0,0,0,0,0,0,0,0,0,0,0,0,0 },
						  {7,0,0,0,0,0,0,0,0,0,0,0,0,0 },
						  {7,0,0,0,0,0,0,0,0,0,0,0,0,0 },
						  {7,0,0,0,0,0,0,0,0,0,0,0,0,0 },
						  {7,7,7,7,7,7,7,7,7,7,7,7,7,7 } };

int mapMattoni[8][5] = { {5,6,3,2,1},
						 {6,4,6,2,1},
						 {5,4,3,2,1},
						 {5,4,6,2,1},
						 {6,4,3,2,6},
						 {5,4,3,6,1},
						 {5,4,3,2,6},
						 {5,6,3,2,1},};

int vecMattoniOro[8] = { 1,1,1,1,1,1,1,1 };

//Vista isometrica frontale dall'alto
glm::vec3 cameraPos(-3.5, 12.5, 12.5);  // Posizione camera
glm::vec3 cameraAt(-3.5, 0.0, -5.0);	// Punto in cui "guarda" la camera

//Vista isometrica dall'alto
//glm::vec3 cameraPos(-3.5f, 19.0f, -2.9f);  // Posizione camera
//glm::vec3 cameraAt(-3.5f, 1.0f, -3.0f);	// Punto in cui "guarda" la camera

////Vista isometrica frontale
//glm::vec3 cameraPos(-3.5f, 0.0f, 19.0f);  // Posizione camera
//glm::vec3 cameraAt(-3.5f, 0.0f, -3.0f);	// Punto in cui "guarda" la camera

glm::vec3 cameraUp(0.0, 1.0, 0.0);		// Vettore up...la camera e sempre parallela al piano
glm::vec3 cameraDir(0.0, 0.0, -0.1);	// Direzione dello sguardo
glm::vec3 cameraSide(1.0, 0.0, 0.0);	// Direzione spostamento laterale

unsigned int cubeVAO;
unsigned int cubeVBO;

// lighting
glm::vec3 lightPos(-3.4, 0.0, 2.9);

int cubiEliminati = 0;
int mattoniSpecialiEliminati = 0;

//Proprieta palla
glm::vec3 pallaPos(-3.4, 0.0, 2.9); // Posizione palla
glm::vec3 pallaAt(0.0, 0.0, 0.0); // Punto in cui e diretta la palla
glm::vec3 pallaUp(0.0, 1.0, 0.0); // Vettore up...la palla e sempre parallela al piano
const float lunghezzaPalla = 0.3f;
const float larghezzaPalla = 0.3f;
const float altezzaPalla = 0.3f;
float speedPalla = 6.5; // velocita della sfera
float translateSpeedPalla;

//Proprieta cubi bordo,pavimento 
float dimCubo = 1.0;

//Proprieta mattoni
const float lunghezzaMattone = 1.35f;
const float larghezzaMattone = 0.7f;
const float altezzaMattone = 0.8f;
const float spazioMattoni = 1.05f;
const float posXmattoni = -8.2f;
const float posYmattoni = 0.3f;
const float posZmattoni = -6.0f;
const float posZmattoniOro = posZmattoni - 1.0 * larghezzaMattone;

//Proprieta piattaforma
glm::vec3 piattaformaPos(-3.4, 0.0, 3.7);
float translateRight = 0;
float translateLeft = 0;
float speedPiattaforma = 6;  // velocita della piattaforma
const float lunghezzaPiattaforma = 2.0f;
const float larghezzaPiattaforma = 0.4f;
const float altezzaPiattaforma = 1.0f;
float translateSpeedPiattaforma;
float limX_pos = posXmattoni + dimCubo*10 - 0.1;
float limX_neg = posXmattoni + 0.1;

//Timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//Mosse possibili
bool moveLeft = false;
bool moveRight = false;
bool lanciaPalla = false;
bool stopLancio = false;
bool exitGame = false;

unsigned int texture_bordo;
unsigned int texture_palla;
unsigned int texture_msgWin;
unsigned int texture_msgLost;
unsigned int texture_sfondo;

unsigned int pavimentoDiffuse;
unsigned int pavimentoNormal;
unsigned int pavimentoSpecular;

//Texture materiali
unsigned int tx_goldSpecular;
unsigned int tx_goldDiffuse;
unsigned int tx_ironDiffuse;
unsigned int tx_copperDiffuse;
unsigned int tx_silverDiffuse;
unsigned int tx_greenPlastic;
unsigned int tx_bluePlastic;

//Texture trasparenza
unsigned int transparentTexture;

unsigned int frameCount = 0;
double previousTime = 0;
double timeInterval = 0;
unsigned int fps = 0;

Shader* bordoShader;
Shader* blendingShader;
Shader* mattoniShader;
Shader* pavimentoShader;

float random_x;

void calculateFPS() {
	//  Incrementiamo il contatore
	frameCount++;
	//  Determiniamo il numero di millisecondi trascorsi dalla glutInit
	double currentTime = glfwGetTime();
	//  Calcoliamo il tempo trascorso
	timeInterval = currentTime - previousTime;

	// Se e passato un secondo aggiorna la variabile fps
	if (timeInterval > 1.0f) {
		//  frameCount mantiene il numero di frame generati in un secondo
		fps = frameCount;

		//  Salviamo il tempo trascorso per riutilizzarlo la prossima volta
		previousTime = currentTime;

		//  Azzeriamo il contatore dei tempi
		frameCount = 0;
	}
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || exitGame == true) {
		glfwSetWindowShouldClose(window, true);
		soundEngine->drop();
	}
		

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		moveLeft = true;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		moveRight = true;
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		lanciaPalla = true;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
		moveLeft = false;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
		moveRight = false;
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE)
		lanciaPalla = false;
}

// La funzione idle continuera a spostare la camera
// fino a quando le variabili booleane sono a true
void idle()
{
	double currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	translateSpeedPiattaforma = speedPiattaforma * deltaTime;
	translateSpeedPalla = speedPalla * deltaTime;

	//Definisco la posizione della palla
	pallaPos = pallaPos + translateSpeedPalla * pallaAt;
	lightPos = pallaPos;

	// Spostamento piattaforma laterale destro
	if (moveRight && piattaformaPos.x < limX_pos)
	{
		piattaformaPos = glm::vec3(piattaformaPos.x + translateSpeedPiattaforma, piattaformaPos.y, piattaformaPos.z);

	}
	// Spostamento piattaforma laterale sinistro
	if (moveLeft && piattaformaPos.x > limX_neg)
	{
		piattaformaPos = glm::vec3(piattaformaPos.x - translateSpeedPiattaforma, piattaformaPos.y, piattaformaPos.z);
	}

	//Inizia il movimento della  palla
	if (lanciaPalla && !stopLancio)
	{
		soundEngine->play2D(gameStart, false);

		pallaAt = glm::vec3(random_x, 0.0f, -10.0f);
		//pallaAt = glm::vec3(-10.0f, 0.0f, -10.0f);
		pallaAt = glm::normalize(pallaAt);
		stopLancio = true;
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

float generaNumeroCasuale(float estremoInferiore, float estremoSuperiore) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(estremoInferiore, estremoSuperiore);
	float random = dis(gen);
	return random;
}

float generaNumeroCasualeStart(float estremoInferiore, float estremoSuperiore) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(estremoInferiore, estremoSuperiore);
	float random = dis(gen);
	while (random < 0.5 && random > -0.5) {
		random = dis(gen);
	}
	return random;
}

glm::vec3 getNormaleCuboBordo(int i, int j) {
	//Lato bordo SX
	if (i == 0 && j < 14) {
		glm::vec3 normale = glm::vec3(1.0f, 0.0f, 0.0f);
		return normale;
	}
	//Lato bordo DX
	if (i == 13 && j < 14) {
		glm::vec3 normale = glm::vec3(-1.0f, 0.0f, 0.0f);
		return normale;
	}
	//Lato superiore
	if (1 <= i < 14 && j == 0) {
		glm::vec3 normale = glm::vec3(0.0f, 0.0f, -1.0f);
		return normale;
	}
	return glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 getNormaleMattone(float x_mattone, float z_mattone) {

	//std::cout << "pallaAt_x " << pallaAt.x << std::endl;
	//std::cout << "pallaAt_z " << pallaAt.z << std::endl;

	//std::cout << "posPalla_x " << pallaPos.x << std::endl;
	//std::cout << "posPalla_z " << pallaPos.z << std::endl;

	//std::cout << "x_mattone "   << x_mattone << std::endl;
	//std::cout << "z_mattone "   << z_mattone << std::endl;

	float deltaPosX = abs(x_mattone - pallaPos.x);
	float deltaPosZ = abs(z_mattone - pallaPos.z);
	float rangeCollisionX = (lunghezzaPalla + lunghezzaMattone) / 2;
	float rangeCollisionZ = (larghezzaPalla + larghezzaMattone) / 2;
	float x0_cube = x_mattone - (lunghezzaMattone / 2 + lunghezzaPalla / 2);
	float x1_cube = x_mattone + (lunghezzaMattone / 2 + lunghezzaPalla / 2);
	float z0_cube = z_mattone - (larghezzaMattone / 2 + larghezzaPalla / 2);
	float z1_cube = z_mattone + (larghezzaMattone / 2 + larghezzaPalla / 2);

	//La palla colpisce la faccia dx
	if (pallaAt.x < 0 && deltaPosZ <= rangeCollisionZ && pallaPos.z >= z0_cube && pallaPos.z <= z1_cube) {
		//speedPalla = 0;
		//std::cout << "Hit face DX " << std::endl;
		glm::vec3 normale = glm::vec3(1.0f, 0.0f, 0.0f);
		return normale;
	}
	//La palla colpisce la faccia sx
	else if (pallaAt.x > 0 && deltaPosZ <= rangeCollisionZ && pallaPos.z >= z0_cube && pallaPos.z <= z1_cube) {
		//speedPalla = 0;
		//std::cout << "Hit face SX " << std::endl;
		glm::vec3 normale = glm::vec3(-1.0f, 0.0f, 0.0f);
		return normale;
	}
	//La palla colpisce la faccia inferiore
	else if (pallaAt.z < 0 && deltaPosX <= rangeCollisionX && pallaPos.x >= x0_cube && pallaPos.x <= x1_cube) {
		//speedPalla = 0;
		//std::cout << "Hit face DOWN " << std::endl;
		glm::vec3 normale = glm::vec3(0.0f, 0.0f, -1.0f);
		return normale;
	}
	//La palla colpisce la faccia superiore
	else if (pallaAt.z > 0 && deltaPosX <= rangeCollisionX && pallaPos.x >= x0_cube && pallaPos.x <= x1_cube) {
		//speedPalla = 0;
		//std::cout << "Hit face UP " << std::endl;
		glm::vec3 normale = glm::vec3(0.0f, 0.0f, 1.0f);
		return normale;
	}
	//La palla colpisce lo spigolo
	else {
		//std::cout << "Hit face SPIGOLO " << std::endl;
		//float random_x = generaNumeroCasuale(-1.0f, 1.0f);
		//float random_z = generaNumeroCasuale(-1.0f, 1.0f);
		//glm::vec3 normale = glm::vec3(random_x, 0.0f, random_z);
		//return normale;
		//La palla colpisce lo spigolo in alto a DX
		if (pallaAt.x < 0 && pallaAt.z > 0) {
			//std::cout << "Hit SPIGOLO alto DX " << std::endl;
			glm::vec3 normale = glm::vec3(1.0f, 0.0f, 1.0f);
			return normale;
		}
		//La palla colpisce lo spigolo in alto a SX
		if (pallaAt.x > 0 && pallaAt.z > 0) {
			//std::cout << "Hit SPIGOLO alto SX " << std::endl;
			glm::vec3 normale = glm::vec3(-1.0f, 0.0f, 1.0f);
			return normale;
		}
		//La palla colpisce lo spigolo in basso a DX
		if (pallaAt.x < 0 && pallaAt.z < 0) {
			//std::cout << "Hit SPIGOLO basso DX " << std::endl;
			glm::vec3 normale = glm::vec3(1.0f, 0.0f, -1.0f);
			return normale;
		}
		//La palla colpisce lo spigolo in basso a SX
		if (pallaAt.x > 0 && pallaAt.z < 0) {
			//std::cout << "Hit SPIGOLO basso SX " << std::endl;
			glm::vec3 normale = glm::vec3(-1.0f, 0.0f, -1.0f);
			return normale;
		}
	}
	//TODO gestire il caso in cui la palla elimina due cubi contemporaneamente
	return glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 getNormalePiattaforma() {

	float posFaceDownPalla = pallaPos.z + larghezzaPalla / 2;
	float posFaceSxPalla = pallaPos.x - larghezzaPalla / 2;
	float posFaceDxPalla = pallaPos.x + larghezzaPalla / 2;

	float posFaceUpPiattaforma = piattaformaPos.z - larghezzaPiattaforma / 2;
	float posFaceDxPiattaforma = piattaformaPos.x + larghezzaPiattaforma / 2;
	float posFaceSxPiattaforma = piattaformaPos.x + larghezzaPiattaforma / 2;

	float diffX_dx = posFaceSxPalla - posFaceDxPiattaforma;
	float diffX_sx = posFaceSxPiattaforma - posFaceDxPalla;
	float diffZ_up = posFaceUpPiattaforma - posFaceDownPalla;

	//std::cout << "posFaceDownPalla: " << posFaceDownPalla << std::endl;
	//std::cout << "posFaceUpPiattaforma: "   << posFaceUpPiattaforma << std::endl;
	//std::cout << "diffX_dx: " << diffX_dx << std::endl;
	//std::cout << "diffZ_up: " << diffZ_up << std::endl;

	float deltaPosX = abs(piattaformaPos.x - pallaPos.x);
	float deltaPosZ = abs(piattaformaPos.z - pallaPos.z);
	float rangeCollisionX = (larghezzaPalla + lunghezzaPiattaforma) / 2;
	float rangeCollisionZ = (larghezzaPalla + larghezzaPiattaforma) / 2;
	float x0_piattaforma = piattaformaPos.x - (lunghezzaPiattaforma / 2 + larghezzaPalla / 2);
	float x1_piattaforma = piattaformaPos.x + (lunghezzaPiattaforma / 2 + larghezzaPalla / 2);
	float z0_piattaforma = piattaformaPos.z - (larghezzaPiattaforma / 2 + larghezzaPalla / 2);
	float z1_piattaforma = piattaformaPos.z + (larghezzaPiattaforma / 2 + larghezzaPalla / 2);

	//La palla colpisce la faccia superiore
	if (pallaAt.z > 0 && deltaPosX <= rangeCollisionX && pallaPos.x > x0_piattaforma && pallaPos.x < x1_piattaforma && diffZ_up > 0.04) {
		//std::cout << "* Hit face PLATFORM UP " << std::endl;
		glm::vec3 normale = glm::vec3(0.0f, 0.0f, 1.0f);
		return normale;
	}
	//La palla colpisce la faccia destra
	else if (pallaAt.x < 0 && deltaPosZ <= rangeCollisionZ && pallaPos.z > z0_piattaforma && pallaPos.z < z1_piattaforma && diffX_dx > 0.04) {
		//std::cout << "* Hit face PLATFORM DX " << std::endl;
		glm::vec3 normale = glm::vec3(1.0f, 0.0f, 1.0f);
		return normale;
	}
	//La palla colpisce la faccia sx
	else if (pallaAt.x > 0 && deltaPosZ <= rangeCollisionZ && pallaPos.z > z0_piattaforma && pallaPos.z < z1_piattaforma && diffX_sx > 0.04) {
		//std::cout << "* Hit face PLATFORM SX " << std::endl;
		glm::vec3 normale = glm::vec3(-1.0f, 0.0f, 1.0f);
		return normale;
	}
	////La palla colpisce lo spigolo
	//else {
	//	std::cout << "Hit spigolo PLATFORM " << std::endl;
	//	glm::vec3 normale = glm::vec3(-1.0f, 0.0f, 1.0f);
	//	return normale;
	//}
	return glm::vec3(0.0f, 0.0f, 0.0f);
}

void controllaCollisioneBordo(int i, int j, float x, float z) {

	if (abs(x - pallaPos.x) <= dimCubo && abs(z - pallaPos.z) <= dimCubo) {

		soundEngine->play2D(rimbalzoBordo, false);

		glm::vec3 normale = getNormaleCuboBordo(i, j);

		pallaAt = pallaAt - 2.0f * glm::dot(pallaAt, normale) * normale;
		pallaAt = glm::normalize(pallaAt);

		pallaPos = pallaPos + (translateSpeedPalla)*pallaAt;
	}
}

void controllaCollisioneMattoni(int i, int j, float x, float z) {

	float rangeCollision = (larghezzaPalla + lunghezzaMattone) / 2;

	if (abs(x - pallaPos.x) < rangeCollision && abs(z - pallaPos.z) < rangeCollision) {

		soundEngine->play2D(rimbalzoMattoni, false);

		mapMattoni[i][j] = 0;
		cubiEliminati = cubiEliminati++;

		float random = generaNumeroCasuale(-0.25f, 0.25f);

		glm::vec3 normale = getNormaleMattone(x, z);

		pallaAt = pallaAt - 2.0f * glm::dot(pallaAt, normale) * normale;
		pallaAt = glm::vec3(pallaAt.x + random, pallaAt.y, pallaAt.z + random);
		pallaAt = glm::normalize(pallaAt);

		pallaPos = pallaPos + (translateSpeedPalla)*pallaAt;

	}
}

bool controllaCollisioneMattoniOro(float x, float z) {

	float rangeCollision = (larghezzaPalla + lunghezzaMattone) / 2;

	bool collision = false;

	if (abs(x - pallaPos.x) < rangeCollision && abs(z - pallaPos.z) < rangeCollision) {

		soundEngine->play2D(rimbalzoMattoniOro, false);

		collision = true;

		float random = generaNumeroCasuale(-0.25f, 0.25f);

		glm::vec3 normale = getNormaleMattone(x, z);

		pallaAt = pallaAt - 2.0f * glm::dot(pallaAt, normale) * normale;
		pallaAt = glm::vec3(pallaAt.x + random, pallaAt.y, pallaAt.z + random);
		pallaAt = glm::normalize(pallaAt);

		pallaPos = pallaPos + (translateSpeedPalla)*pallaAt;

	}

	return collision;
}

void controllaCollisionePiattaforma() {
	float deltaPosX = abs(piattaformaPos.x - pallaPos.x);
	float rangeCollisionX = (lunghezzaPalla + lunghezzaPiattaforma) / 2;
	float deltaPosZ = abs(piattaformaPos.z - pallaPos.z);
	float rangeCollisionZ = ((larghezzaPalla + larghezzaPiattaforma) / 2 + 0.1);

	if (deltaPosX <= rangeCollisionX && deltaPosZ <= rangeCollisionZ) {

		soundEngine->play2D(rimbalzoPiattaforma, false);

		float random = generaNumeroCasuale(-0.25f, 0.25f);

		glm::vec3 normale = getNormalePiattaforma();
		pallaAt = pallaAt - 2.0f * glm::dot(pallaAt, normale) * normale;
		pallaAt = glm::vec3(pallaAt.x + random, pallaAt.y, pallaAt.z + random);
		pallaAt = glm::normalize(pallaAt);
		pallaPos = pallaPos + (translateSpeedPalla)*pallaAt;

	}
}

// load and create a texture standard per jpg
unsigned int loadTexture1(std::string filename)
{
	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return -1;
	}
	stbi_image_free(data);

	return texture;
}

// load and create a texture per le trasparenti, le carica storte
unsigned int loadTexture2(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// utility function for loading a 2D texture from file con gammaCorrection
unsigned int loadTexture3(char const* path, bool gammaCorrection)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum internalFormat;
		GLenum dataFormat;
		if (nrComponents == 1)
		{
			internalFormat = dataFormat = GL_RED;
		}
		else if (nrComponents == 3)
		{
			internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void render(glm::mat4 projection, Shader pallaShader, Shader piattaformaShader, Model modelSfera, Model modelPlatform, Shader shaderBlur, Shader shaderBloomFinal)
{

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

	// 1. render scene into floating point framebuffer
	// -----------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Disegno il bordo
	bordoShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_bordo);

	glBindVertexArray(cubeVAO);

	for (int i = 0; i < 14; i++)
	{
		for (int j = 0; j < 14; j++)
		{
			if (mapCorner[i][j] != 0)
			{

				/* Ogni cubo del bordo dista 0 unita dai vicini */
				float x = posXmattoni - 1.55 + i;
				float z = -9 + j;

				glm::mat4 bordo = glm::mat4(1.0f);	//identity matrix
				bordo = glm::translate(bordo, glm::vec3(x, 0.0f, z));
				bordo = glm::scale(bordo, glm::vec3(dimCubo, dimCubo, dimCubo));
				bordoShader->setMat4("model", bordo);

				glDrawArrays(GL_TRIANGLES, 0, 36);

				controllaCollisioneBordo(i,j,x,z);
			}
		}
	}

	//Mesaggio hai perso se la pallina supera la piattaforma
	if (pallaPos.z > piattaformaPos.z + 2.0f) {
		if (playGameOver == 1) {
			ambientSound->stop();
			soundEngine->play2D(gameOver, false);
			
			playGameOver = 0;
		}
		
		speedPalla = 0;
		//Sposto la camera
		cameraPos = glm::vec3(0.0f, 20.0f, 1.8f);  // Posizione camera
		cameraAt = glm::vec3(0.0f, 20.0f, 0.0f);	// Punto in cui "guarda" la camera
		glm::mat4 view = glm::lookAt(cameraPos, cameraAt, cameraUp);
		bordoShader->use();
		bordoShader->setMat4("view", view);
		glm::mat4 modelDialogBox = glm::mat4(1.0f);	//identity matrix
		modelDialogBox = glm::translate(modelDialogBox, glm::vec3(0.0f, 20.0f, 0.0f));
		modelDialogBox = glm::scale(modelDialogBox, glm::vec3(1.2f, 1.2f, 1.2f));
		bordoShader->setMat4("model", modelDialogBox);
		bordoShader->setInt("myTexture1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_msgLost);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	//Mesaggio hai vinto se non sono rimasti cubi
	if (cubiEliminati + mattoniSpecialiEliminati == 48) {
		if (playGameWin == 1) {
			ambientSound->stop();
			soundEngine->play2D(gameWin, false);
			playGameWin = 0;
		}
		speedPalla = 0;
		//Sposto la camera
		cameraPos = glm::vec3(0.0f, 20.0f, 1.8f);  // Posizione camera
		cameraAt = glm::vec3(0.0f, 20.0f, -1.0f);	// Punto in cui "guarda" la camera
		glm::mat4 view = glm::lookAt(cameraPos, cameraAt, cameraUp);
		bordoShader->use();
		bordoShader->setMat4("view", view);
		glm::mat4 modelDialogBox = glm::mat4(1.0f);	//identity matrix
		modelDialogBox = glm::translate(modelDialogBox, glm::vec3(0.0f, 20.0f, 0.0f));
		modelDialogBox = glm::scale(modelDialogBox, glm::vec3(1.2f, 1.2f, 1.2f));
		bordoShader->setMat4("model", modelDialogBox);
		bordoShader->setInt("myTexture1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_msgWin);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	//Disegno lo sfondo
	bordoShader->use();
	glm::mat4 modelCuboSfondo = glm::mat4(1.0f);	//identity matrix
	modelCuboSfondo = glm::translate(modelCuboSfondo, glm::vec3(-3.5f, -10.0f, -5.0f));
	modelCuboSfondo = glm::rotate(modelCuboSfondo, -45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	modelCuboSfondo = glm::scale(modelCuboSfondo, glm::vec3(55.0f, 55.0f, 0.1f));
	bordoShader->setMat4("model", modelCuboSfondo);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_sfondo);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	////Disegno la palla (per ora e un cubo)
	//bordoShader->use();
	//bordoShader->setMat4("projection", projection);
	//bordoShader->setMat4("view", view);
	//glm::mat4 modelPalla = glm::mat4(1.0f);	//identity matrix
	//modelPalla = glm::translate(modelPalla, glm::vec3(pallaPos.x, pallaPos.y, pallaPos.z));
	//modelPalla = glm::scale(modelPalla, glm::vec3(lunghezzaPalla, altezzaPalla, larghezzaPalla));
	//bordoShader->setMat4("model", modelPalla);
	//bordoShader->setInt("myTexture1", 1);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, texture_palla);
	//glDrawArrays(GL_TRIANGLES, 0, 36);

	//Disegno il modello 3D sfera
	pallaShader.use();
	glm::mat4 modelSfera2 = glm::mat4(1.0f);
	modelSfera2 = glm::translate(modelSfera2, glm::vec3(pallaPos.x, pallaPos.y, pallaPos.z + 0.15));
	modelSfera2 = glm::scale(modelSfera2, glm::vec3(0.06f, 0.06f, 0.06f));
	pallaShader.setMat4("model", modelSfera2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_palla);
	modelSfera.Draw(pallaShader);

	piattaformaShader.use();
	//Disegno il modello 3D piattaforma
	glm::mat4 platform = glm::mat4(1.0f);
	platform = glm::translate(platform, piattaformaPos);
	platform = glm::scale(platform, glm::vec3(1.0f, 1.0f, 1.0f));
	platform = glm::rotate(platform, -PI/2 , glm::vec3(1.0f, 0.0f, 0.0f));
	piattaformaShader.setMat4("model", platform);
	modelPlatform.Draw(piattaformaShader);

	//Disegno il pavimento

	pavimentoShader->use();
	pavimentoShader->setVec3("lightPos", lightPos);
	pavimentoShader->setVec3("viewPos", lightPos);

	// light properties
	pavimentoShader->setVec3("light.ambient", 0.4f, 0.4f, 0.4f);
	pavimentoShader->setVec3("light.diffuse", 0.4f, 0.4f, 0.4f);
	pavimentoShader->setVec3("light.specular", 0.4f, 0.4f, 0.4f);
	pavimentoShader->setFloat("light.constant", 1.0f);
	pavimentoShader->setFloat("light.linear", 0.09f);
	pavimentoShader->setFloat("light.quadratic", 0.0f);

	// material properties
	pavimentoShader->setFloat("material.shininess", 32.0f);

	// world transformation
	glm::mat4 pavimento = glm::mat4(1.0f);	//identity matrix
	pavimento = glm::translate(pavimento, glm::vec3(-3.25f, -1.0f, -2.9f));
	pavimento = glm::scale(pavimento, glm::vec3(7.0f, 1.0f, 7.0f));
	pavimento = glm::rotate(pavimento, glm::radians((float) -90), glm::vec3(1.0, 0.0, 0.0));
	pavimentoShader->setMat4("model", pavimento);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pavimentoDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pavimentoNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, pavimentoSpecular);

	renderQuad();

	//Disegno i mattoni di oro (con due vite)

	mattoniShader->use();
	mattoniShader->setVec3("light.position", lightPos);
	mattoniShader->setVec3("viewPos", cameraPos);

	// light properties
	mattoniShader->setFloat("light.constant", 1.0f);
	mattoniShader->setFloat("light.linear", 0.09f);
	mattoniShader->setFloat("light.quadratic", 0.00f);

	//gold properties - simulo le proprietà dei materiali con i coefficienti della luce
	mattoniShader->setVec3("light.ambient", 0.24725f, 0.1995f, 0.0745f);
	mattoniShader->setVec3("light.diffuse", 0.75164, 0.60648, 0.22648);
	mattoniShader->setVec3("light.specular", 0.628281, 0.555802, 0.366065);
	mattoniShader->setFloat("material.shininess", 51.2f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tx_goldDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tx_goldSpecular);

	glBindVertexArray(cubeVAO);

	for (int i = 0; i < 8; i++)
	{

		if (vecMattoniOro[i] != 0)
		{

			float x = posXmattoni + i * lunghezzaMattone * spazioMattoni;
			float z = posZmattoniOro - 0.05f;

			glm::mat4 mattoneSpeciale = glm::mat4(1.0f);	//identity matrix
			mattoneSpeciale = glm::translate(mattoneSpeciale, glm::vec3(x, posYmattoni, z));
			mattoneSpeciale = glm::scale(mattoneSpeciale, glm::vec3(lunghezzaMattone, altezzaMattone, larghezzaMattone));
			mattoniShader->setMat4("model", mattoneSpeciale);

			glDrawArrays(GL_TRIANGLES, 0, 36);

			bool collision = controllaCollisioneMattoniOro(x, z);

			if (collision) {
				vecMattoniOro[i] = vecMattoniOro[i] + 1;
				if (vecMattoniOro[i] == 3) {
					vecMattoniOro[i] = 0;
					mattoniSpecialiEliminati = mattoniSpecialiEliminati++;
				}
				collision = false;
			}
		}

	}

	//Disegno i mattoni da eliminare
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if (mapMattoni[i][j] != 0)
			{
				/* Ogni cubo dista 1.1 unita dai vicini */
				float x = posXmattoni + i * lunghezzaMattone * spazioMattoni;
				float z = posZmattoni + j * larghezzaMattone * spazioMattoni;

				glm::mat4 model = glm::mat4(1.0f);	//identity matrix
				model = glm::translate(model, glm::vec3(x, posYmattoni, z));
				model = glm::scale(model, glm::vec3(lunghezzaMattone, altezzaMattone, larghezzaMattone));


				if (mapMattoni[i][j] == 5) {
					mattoniShader->use();
					mattoniShader->setMat4("model", model);
					//chrome properties
					mattoniShader->setVec3("light.ambient", 0.25f, 0.25f, 0.25f);
					mattoniShader->setVec3("light.diffuse", 0.4f, 0.4f, 0.4f);
					mattoniShader->setVec3("light.specular", 0.774597f, 0.774597f, 0.774597f);
					mattoniShader->setFloat("material.shininess", 76.8f);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, tx_ironDiffuse);
				}else if(mapMattoni[i][j] == 4) {
					mattoniShader->use();
					mattoniShader->setMat4("model", model);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, tx_goldSpecular);
					//copper properties
					mattoniShader->setVec3("light.ambient", 0.19125f, 0.0735f, 0.0225f);
					mattoniShader->setVec3("light.diffuse", 0.5508f, 0.2118f, 0.066f);
					mattoniShader->setVec3("light.specular", 0.580594f, 0.223257f, 0.0695701f);
					mattoniShader->setFloat("material.shininess", 51.2f);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, tx_copperDiffuse);
				}
				else if (mapMattoni[i][j] == 3) {
					mattoniShader->use();
					mattoniShader->setMat4("model", model);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, tx_goldSpecular);
					mattoniShader->use();
					//blue plastic properties
					mattoniShader->setVec3("light.ambient", 0.0f, 0.05f, 0.05f);
					mattoniShader->setVec3("light.diffuse", 0.0f, 0.50980392f, 0.50980392f);
					mattoniShader->setVec3("light.specular", 0.50196078f, 0.50196078f, 0.50196078f);
					mattoniShader->setFloat("material.shininess", 32.0f);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, tx_bluePlastic);
				}
				else if (mapMattoni[i][j] == 2) {
					mattoniShader->use();
					mattoniShader->setMat4("model", model);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, tx_goldSpecular);
					//silver properties
					mattoniShader->setVec3("light.ambient", 0.19225f, 0.19225f, 0.19225f);
					mattoniShader->setVec3("light.diffuse", 0.2775f, 0.2775f, 0.2775f);
					mattoniShader->setVec3("light.specular", 0.773911f, 0.773911f, 0.773911f);
					mattoniShader->setFloat("material.shininess", 89.6f);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, tx_silverDiffuse);
				}
				else if (mapMattoni[i][j] == 1) {
					mattoniShader->use();
					mattoniShader->setMat4("model", model);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, tx_goldSpecular);
					//green plastic properties
					mattoniShader->setVec3("light.ambient", 0.0f, 0.0f, 0.0f);
					mattoniShader->setVec3("light.diffuse", 0.1f, 0.35f, 0.1f);
					mattoniShader->setVec3("light.specular", 0.45f, 0.55f, 0.45f);
					mattoniShader->setFloat("material.shininess", 32.0f);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, tx_greenPlastic);
				}
				else if (mapMattoni[i][j] == 6) {
					blendingShader->use();
					blendingShader->setMat4("model", model);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, transparentTexture);
				}

				glDrawArrays(GL_TRIANGLES, 0, 36);

				controllaCollisioneMattoni(i, j, x, z);
			}

		}
	}


	//Inverto la direzione se la palla colpisce la piattaforma
	controllaCollisionePiattaforma();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. blur bright fragments with two-pass Gaussian Blur 
	// --------------------------------------------------
	bool horizontal = true, first_iteration = true;
	unsigned int amount = 10;
	shaderBlur.use();
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		shaderBlur.setInt("horizontal", horizontal);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
		renderQuad2();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
	// --------------------------------------------------------------------------------------------------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderBloomFinal.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
	shaderBloomFinal.setInt("bloom", bloom);
	shaderBloomFinal.setFloat("exposure", exposure);
	renderQuad2();
}

int main()
{
	bool schermoIntero = true;
	const GLFWvidmode* videoMode = NULL;
	GLFWmonitor* primaryMonitor = NULL;

	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	soundEngine = createIrrKlangDevice();
	mainTheme = soundEngine->addSoundSourceFromFile("../src/music/soundtrack.mp3");
	rimbalzoMattoni = soundEngine->addSoundSourceFromFile("../src/music/rimbalzoMattoni.wav");
	rimbalzoMattoniOro = soundEngine->addSoundSourceFromFile("../src/music/rimbalzoMattoniOro.mp3");
	rimbalzoPiattaforma = soundEngine->addSoundSourceFromFile("../src/music/rimbalzoPiattaforma.wav");
	rimbalzoBordo = soundEngine->addSoundSourceFromFile("../src/music/rimbalzoBordo.mp3");
	gameOver = soundEngine->addSoundSourceFromFile("../src/music/gameOver.wav");
	gameStart = soundEngine->addSoundSourceFromFile("../src/music/gameStart.wav");
	gameWin = soundEngine->addSoundSourceFromFile("../src/music/gameWin.mp3");
	ambientSound = soundEngine->play2D(mainTheme, true, false, true);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	if (schermoIntero) {
		// Ottenere il monitor primario
		primaryMonitor = glfwGetPrimaryMonitor();
		// Ottenere la modalita video corrente del monitor primario
		videoMode = glfwGetVideoMode(primaryMonitor);
		SCR_WIDTH = videoMode->width;
		SCR_HEIGHT = videoMode->height;
	}

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL 3.3 - Arkanoid!", primaryMonitor, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	if (schermoIntero) {
		// Impostare la finestra in modalita schermo intero
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glEnable(GL_CULL_FACE);

	bordoShader = new Shader("bordo.vs", "bordo.fs");
	blendingShader = new Shader("blending.vs", "blending.fs");
	pavimentoShader = new Shader("pavimento.vs", "pavimento.fs"); //Pavimento con normal mapping
	mattoniShader = new Shader("mattoni.vs", "mattoni.fs");//Mattoni con texture che fanno da materiali

	//Modelli
	Shader pallaShader("palla.vs", "palla.fs");
	Shader piattaformaShader("piattaforma.vs", "piattaforma.fs");

	// build and compile shaders
	// -------------------------
	Shader shaderBlur("blur.vs", "blur.fs");
	Shader shaderBloomFinal("bloom_final.vs", "bloom_final.fs");

	//Textures
	texture_bordo = loadTexture3("../src/textures/tiles3.jpg", true);

	texture_palla = loadTexture3("../src/textures/oro1.jpg",true);
	texture_msgLost = loadTexture3("../src/textures/msgLost.jpg",true);
	texture_msgWin = loadTexture3("../src/textures/msgWin.jpg",true);
	texture_sfondo = loadTexture3("../src/textures/sfondo.jpg",true);

	pavimentoDiffuse = loadTexture3("../src/textures/paving.jpg",true);
	pavimentoNormal = loadTexture3("../src/textures/paving_normal.jpg",true);
	pavimentoSpecular = loadTexture3("../src/textures/paving_specular.jpg",true);

	//Materiali
	tx_goldDiffuse = loadTexture3("../src/textures/oro3.jpg",true);
	tx_goldSpecular = loadTexture3("../src/textures/paving_specular.jpg",true);
	tx_ironDiffuse = loadTexture3("../src/textures/iron_diffuse.jpg",true);
	tx_bluePlastic = loadTexture3("../src/textures/plastica_blu.jpg",true);
	tx_copperDiffuse = loadTexture3("../src/textures/copper_diffuse.jpg",true);
	tx_greenPlastic = loadTexture3("../src/textures/plastica_verde.jpg",true);
	tx_silverDiffuse = loadTexture3("../src/textures/silver_diffuse.jpg",true);

	//Trasparenza
	transparentTexture = loadTexture3("../src/textures/window.png",true);

	// load models
	Model modelSfera("../src/models/sfera.obj");
	Model modelPlatform("../src/models/platform/piattaforma.obj");

// configure (floating point) framebuffers
// ---------------------------------------
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	// create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
	
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ping-pong-framebuffer for blurring

	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

	//Binding per mattoni con texture diffuse e speculari
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//Operazioni finali
	glBindVertexArray(0);

	// create transformations
	//glm::mat4 view = glm::mat4(1.0f);	//identity matrix;
	glm::mat4 projection = glm::mat4(1.0f);	//identity matrix
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(cameraPos, cameraAt, cameraUp);

	// Genera un numero casuale float tra -12 e 12
	random_x = generaNumeroCasualeStart(-6.0f, 6.0f);

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	bordoShader->use();
	bordoShader->setMat4("projection", projection);
	bordoShader->setMat4("view", view);
	bordoShader->setInt("myTexture1", 0);

	pallaShader.use();
	pallaShader.setMat4("projection", projection);
	pallaShader.setMat4("view", view);

	piattaformaShader.use();
	piattaformaShader.setMat4("projection", projection);
	piattaformaShader.setMat4("view", view);

	// shader configuration
	// --------------------
	shaderBlur.use();
	shaderBlur.setInt("image", 0);

	shaderBloomFinal.use();
	shaderBloomFinal.setInt("scene", 0);
	shaderBloomFinal.setInt("bloomBlur", 1);

	pavimentoShader->use();
	pavimentoShader->setInt("material.diffuseMap", 0);
	pavimentoShader->setInt("material.normalMap", 1);
	pavimentoShader->setInt("material.specularMap", 2);
	pavimentoShader->setMat4("projection", projection);
	pavimentoShader->setMat4("view", view);

	mattoniShader->use();
	mattoniShader->setInt("material->diffuseMap", 0);
	mattoniShader->setInt("material.specularMap", 1);
	mattoniShader->setMat4("projection", projection);;
	mattoniShader->setMat4("view", view);

	blendingShader->use();
	blendingShader->setMat4("projection", projection);
	blendingShader->setMat4("view", view);
	blendingShader->setInt("texture1", 0);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);
		idle();

		render(projection,pallaShader, piattaformaShader, modelSfera, modelPlatform, shaderBlur, shaderBloomFinal);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
		calculateFPS();
		std::string framePerSecond = std::to_string(fps);
		glfwSetWindowTitle(window, framePerSecond.c_str());
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

// renders a 1x1 quad in NDC with manually calculated tangent vectors
// ------------------------------------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		// positions
		glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
		glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
		glm::vec3 pos3(1.0f, -1.0f, 0.0f);
		glm::vec3 pos4(1.0f, 1.0f, 0.0f);
		// texture coordinates
		glm::vec2 uv1(0.0f, 1.0f);
		glm::vec2 uv2(0.0f, 0.0f);
		glm::vec2 uv3(1.0f, 0.0f);
		glm::vec2 uv4(1.0f, 1.0f);
		// normal vector
		glm::vec3 nm(0.0f, 0.0f, 1.0f);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;
		// triangle 1
		// ----------
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		tangent1 = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
		bitangent1 = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

		// triangle 2
		// ----------
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		tangent2 = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
		bitangent2 = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);


		float quadVertices[] = {
			// positions            // normal         // texcoords  // tangent                          // bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};
		// configure plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

// renderQuad2() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO2 = 0;
unsigned int quadVBO2;
void renderQuad2()
{
	if (quadVAO2 == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO2);
		glGenBuffers(1, &quadVBO2);
		glBindVertexArray(quadVAO2);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO2);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			 // bottom face
			 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 // top face
			 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

