#include <stack>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <irrklang/irrKlang.h>
//#include "render_text.h"
#include "shader_m.h"
#include "camera.h"
#include "model.h"

#include <iostream>

#include <random>

// settings
int SCR_WIDTH = 1920;
int SCR_HEIGHT = 1080;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

std::stack<glm::mat4> glm_ModelViewMatrix; 

float cube_mesh[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

int mapCorner[14][15] = { {7,7,7,7,7,7,7,7,7,7,7,7,7,7,7 },
						  {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
						  {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
					      {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
					      {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
					      {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
					      {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
					      {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
					      {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
					      {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
					      {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
					      {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
					      {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
						  {7,7,7,7,7,7,7,7,7,7,7,7,7,7,7 } };

int mapMattoni[8][5] = {{5,4,3,2,1},
					    {5,4,3,2,1},
					    {5,4,3,2,1},
					    {5,4,3,2,1},
					    {5,4,3,2,1},
					    {5,4,3,2,1},
					    {5,4,3,2,1},
					    {5,4,3,2,1},};


int vecMattoniOro[8] = {1,1,1,1,1,1,1,1};

/* Array dei colori da utilizzare */
const glm::vec3 colors[8] = {	{ 1.0, 1.0, 1.0 },
								{ 1.0, 0.0, 0.0 },
								{ 0.0, 1.0, 0.0 },
								{ 0.0, 0.0, 1.0 },
								{ 0.0, 1.0, 1.0 },
								{ 1.0, 1.0, 1.0 },
								{ 1.0, 0.0, 1.0 },
								{ 0.0, 0.0, 0.0 }
											};

//Vista isometrica frontale dall'alto
glm::vec3 cameraPos(-3.5, 12.5, 12.5);  // Posizione camera
glm::vec3 cameraAt(-3.5, 0.0, -5.0);	// Punto in cui "guarda" la camera

//Vista isometrica dall'alto
//glm::vec3 cameraPos(-3.5f, 19.0f, -2.9f);  // Posizione camera
//glm::vec3 cameraAt(-3.5f, 1.0f, -3.0f);	// Punto in cui "guarda" la camera

//Vista isometrica frontale
//glm::vec3 cameraPos(-3.5f, 0.0f, 19.0f);  // Posizione camera
//glm::vec3 cameraAt(-3.5f, 0.0f, -3.0f);	// Punto in cui "guarda" la camera

glm::vec3 cameraUp(0.0, 1.0, 0.0);		// Vettore up...la camera è sempre parallela al piano
glm::vec3 cameraDir(0.0, 0.0, -0.1);	// Direzione dello sguardo
glm::vec3 cameraSide(1.0, 0.0, 0.0);	// Direzione spostamento laterale

int cubiEliminati = 0;
int mattoniSpecialiEliminati = 0;

//Proprietà palla
glm::vec3 pallaPos(-3.4, 0.0, 2.2); // Posizione palla
glm::vec3 pallaAt(0.0, 0.0, 0.0); // Punto in cui è diretta la palla
glm::vec3 pallaUp(0.0, 1.0, 0.0); // Vettore up...la palla è sempre parallela al piano
const float lunghezzaPalla = 0.3f;
const float larghezzaPalla = 0.3f;
const float altezzaPalla = 0.3f;
float speedPalla = 7; // velocità della sfera
float translateSpeedPalla;

//Proprietà piattaforma
glm::vec3 piattaformaPos(-3.4, 0.0, 3.7);
float translateRight = 0;
float translateLeft = 0;
float limX_pos = 1.3386;
float limX_neg = -8.34348;
float speedPiattaforma = 6.5;  // velocità della piattaforma
const float lunghezzaPiattaforma = 2.0f;
const float larghezzaPiattaforma = 0.4f;
const float altezzaPiattaforma = 1.0f;
float translateSpeedPiattaforma;

//Proprietà cubi bordo,pavimento 
float dimCubo = 1.0;

//Proprietà mattoni
const float lunghezzaMattone = 1.35f;
const float larghezzaMattone = 0.7f;
const float altezzaMattone = 0.8f;
const float posXmattoni = -8.2f;
const float posYmattoni = 0.1f;
const float posZmattoni = -6.0f;
const float posZmattoniOro = posZmattoni - 1.0 * larghezzaMattone;

//Timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//Mosse possibili
bool moveLeft = false;
bool moveRight = false;
bool lanciaPalla = false;
bool stopLancio = false;
bool exitGame = false;

unsigned int VBO, VAO;
unsigned int texture_piattaforma;
unsigned int texture_bordo;
unsigned int texture_pavimento;
unsigned int texture_mattoni;
unsigned int texture_mattoni_oro;
unsigned int texture_palla;
unsigned int texture_msgWin;
unsigned int texture_msgLost;
unsigned int texture_sfondo;
unsigned int frameCount = 0;
double previousTime = 0;
double timeInterval = 0;
unsigned int fps = 0;

Shader *pallaShader;
Shader *piattaformaShader;
Shader *mattoniShader;
Shader *bordoShader;

float random_x;


void calculateFPS() {
	//  Incrementiamo il contatore
	frameCount++;
	//  Determiniamo il numero di millisecondi trascorsi dalla glutInit
	double currentTime = glfwGetTime();
	//  Calcoliamo il tempo trascorso
	timeInterval = currentTime - previousTime;

	// Se è passato un secondo aggiorna la variabile fps
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
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || exitGame == true)
		glfwSetWindowShouldClose(window, true);

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

// La funzione idle continuerà a spostare la camera
// fino a quando le variabili booleane sono a true
void idle()
{
	double currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	translateSpeedPiattaforma = speedPiattaforma * deltaTime;
	translateSpeedPalla = speedPalla * deltaTime;

	// Spostamento piattaforma laterale destro
	if (moveRight && piattaformaPos.x < limX_pos)
	{
		piattaformaPos = glm::vec3(piattaformaPos.x+ translateSpeedPiattaforma,piattaformaPos.y,piattaformaPos.z);
		//posPiattaforma_x = posPiattaforma_x + translateSpeedPiattaforma;

	}
	// Spostamento piattaforma laterale sinistro
	if (moveLeft && piattaformaPos.x > limX_neg)
	{
		//posPiattaforma_x = posPiattaforma_x - translateSpeedPiattaforma;
		piattaformaPos = glm::vec3(piattaformaPos.x - translateSpeedPiattaforma, piattaformaPos.y, piattaformaPos.z);
	}

	//Inizia il movimento della  palla
	if (lanciaPalla && !stopLancio)
	{
		pallaAt = glm::vec3(random_x , 0.0f, -10.0f);
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
	if (1<=i<14 && j == 0) {
		glm::vec3 normale = glm::vec3(0.0f, 0.0f, -1.0f);
		return normale;
	}
	return glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 getNormaleMattone(float x_cube,float z_cube) {

	//std::cout << "pallaAt_x " << pallaAt.x << std::endl;
	//std::cout << "pallaAt_z " << pallaAt.z << std::endl;

	//std::cout << "posPalla_x " << pallaPos.x << std::endl;
	//std::cout << "posPalla_z " << pallaPos.z << std::endl;

	//std::cout << "x_cube "   << x_cube << std::endl;
	//std::cout << "z_cube "   << z_cube << std::endl;

	float deltaPosX = abs(x_cube - pallaPos.x);
	float deltaPosZ = abs(z_cube - pallaPos.z);
	float rangeCollisionX = (lunghezzaPalla + lunghezzaMattone) / 2;
	float rangeCollisionZ = (larghezzaPalla + larghezzaMattone) / 2;
	float x0_cube = x_cube - (lunghezzaMattone / 2 + lunghezzaPalla / 2 - 0.1);
	float x1_cube = x_cube + (lunghezzaMattone / 2 + lunghezzaPalla / 2 - 0.1);
	float z0_cube = z_cube - (larghezzaMattone / 2 + larghezzaPalla / 2 - 0.1);
	float z1_cube = z_cube + (larghezzaMattone / 2 + larghezzaPalla / 2 - 0.1);

	//La palla colpisce la faccia inferiore
	if (pallaAt.z < 0 && deltaPosX <= rangeCollisionX && pallaPos.x > x0_cube && pallaPos.x < x1_cube) {
		//speedPalla = 0;
		//std::cout << "Hit face DOWN " << std::endl;
		glm::vec3 normale = glm::vec3(0.0f, 0.0f, -1.0f);
		return normale;
	}
	//La palla colpisce la faccia superiore
	else if (pallaAt.z > 0 && deltaPosX <= rangeCollisionX && pallaPos.x > x0_cube && pallaPos.x < x1_cube) {
		//speedPalla = 0;
		//std::cout << "Hit face UP " << std::endl;
		glm::vec3 normale = glm::vec3(0.0f, 0.0f, 1.0f);
		return normale;
	}
	//La palla colpisce la faccia dx
	else if (pallaAt.x < 0 && deltaPosZ <= rangeCollisionZ && pallaPos.z > z0_cube && pallaPos.z < z1_cube) {
		//speedPalla = 0;
		//std::cout << "Hit face DX " << std::endl;
		glm::vec3 normale = glm::vec3(1.0f, 0.0f, 0.0f);
		return normale;
	}
	//La palla colpisce la faccia sx
	else if (pallaAt.x > 0 && deltaPosZ <= rangeCollisionZ && pallaPos.z > z0_cube && pallaPos.z < z1_cube) {
		//speedPalla = 0;
		//std::cout << "Hit face SX " << std::endl;
		glm::vec3 normale = glm::vec3(-1.0f, 0.0f, 0.0f);
		return normale;
	}
	//La palla colpisce lo spigolo
	else {
		//std::cout << "Hit face SPIGOLO " << std::endl;
		//float random_x = generaNumeroCasuale(-1.0f, 1.0f);
		//float random_z = generaNumeroCasuale(-1.0f, 1.0f);
		//glm::vec3 normale = glm::vec3(random_x, 0.0f, random_z);
		 
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
			glm::vec3 normale = glm::vec3(-1.0f, 0.0f, -1.0f);
			return normale;
		}
		//La palla colpisce lo spigolo in basso a SX
		if (pallaAt.x > 0 && pallaAt.z < 0) {
			//std::cout << "Hit SPIGOLO basso SX " << std::endl;
			glm::vec3 normale = glm::vec3(1.0f, 0.0f, -1.0f);
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


void render(glm::mat4 projection)
{

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
	glm::mat4 view = glm::lookAt(cameraPos, cameraAt, cameraUp);

	//Disegno la piattaforma
	piattaformaShader->use();
	piattaformaShader->setMat4("projection", projection);
	piattaformaShader->setMat4("view", view);
	glm::mat4 piattaforma = glm::mat4(1.0f);	//identity matrix
	//std::cout << "posPiattaforma_x: " << posPiattaforma_x << std::endl;
	piattaforma = glm::translate(piattaforma, glm::vec3(piattaformaPos.x, 0.0f, piattaformaPos.z));
	piattaforma = glm::scale(piattaforma, glm::vec3(lunghezzaPiattaforma, altezzaPiattaforma, larghezzaPiattaforma));
	piattaformaShader->setMat4("model", piattaforma);
	piattaformaShader->setInt("myTexture1", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_piattaforma);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//Disegno la palla (per ora è un cubo)
	pallaShader->use();
	pallaShader->setMat4("projection", projection);
	pallaShader->setMat4("view", view);
	pallaPos = pallaPos + translateSpeedPalla * pallaAt;
	glm::mat4 modelPalla = glm::mat4(1.0f);	//identity matrix
	modelPalla = glm::translate(modelPalla, glm::vec3(pallaPos.x, pallaPos.y, pallaPos.z));
	modelPalla = glm::scale(modelPalla, glm::vec3(lunghezzaPalla, altezzaPalla, larghezzaPalla));
	pallaShader->setMat4("model", modelPalla);
	pallaShader->setVec3("colorcube", colors[0]);
	pallaShader->setInt("myTexture1", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_palla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//Disegno il bordo
	bordoShader->use();
	bordoShader->setMat4("projection", projection);
	bordoShader->setMat4("view", view);
	bordoShader->setInt("myTexture1", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_bordo);
	for (int i = 0; i < 14; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			if (mapCorner[i][j] != 0)
			{

				/* Ogni cubo del bordo dista 0 unità dai vicini */
				float x = posXmattoni - 1.8 + i;
				float z = -10 + j;

				glm::mat4 bordo = glm::mat4(1.0f);	//identity matrix
				bordo = glm::translate(bordo, glm::vec3(x, 0.0f, z));
				bordo = glm::scale(bordo, glm::vec3(dimCubo, dimCubo, dimCubo));
				bordoShader->setMat4("model", bordo);

				glDrawArrays(GL_TRIANGLES, 0, 36);

				if (abs(x - pallaPos.x) <= dimCubo && abs(z - pallaPos.z) <= dimCubo) {

					glm::vec3 normale = getNormaleCuboBordo(i,j);
					
					pallaAt = pallaAt - 2.0f * glm::dot(pallaAt, normale) * normale;
					pallaAt = glm::normalize(pallaAt);

					pallaPos = pallaPos + (translateSpeedPalla) * pallaAt;
				}
			}
		}
	}

	//Disegno il pavimento
	glBindTexture(GL_TEXTURE_2D, texture_pavimento);
	glm::mat4 pavimento = glm::mat4(1.0f);	//identity matrix
	pavimento = glm::translate(pavimento, glm::vec3(-3.5f, -1.0f, -3.0f));
	pavimento = glm::scale(pavimento, glm::vec3(14.0f, 0.1f, 15.0f));
	bordoShader->setMat4("model", pavimento);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	//Disegno i mattoni con due vite da eliminare
	glBindTexture(GL_TEXTURE_2D, texture_mattoni_oro);
	for (int i = 0; i < 8 ; i++)
	{

		if (vecMattoniOro[i] != 0)
		{

			/* Ogni cubo dista 1.1 unità dai vicini */
			float x = posXmattoni + i * lunghezzaMattone;
			float z = posZmattoniOro;

			glm::mat4 mattoneSpeciale = glm::mat4(1.0f);	//identity matrix
			mattoneSpeciale = glm::translate(mattoneSpeciale, glm::vec3(x, posYmattoni, z));
			mattoneSpeciale = glm::scale(mattoneSpeciale, glm::vec3(lunghezzaMattone, altezzaMattone, larghezzaMattone));
			bordoShader->setMat4("model", mattoneSpeciale);

			glDrawArrays(GL_TRIANGLES, 0, 36);

			float rangeCollision = (larghezzaPalla + lunghezzaMattone) / 2;

			bool collision = false;

			if (abs(x - pallaPos.x) < rangeCollision && abs(z - pallaPos.z) < rangeCollision) {

				collision = true;

				float random = generaNumeroCasuale(-0.25f, 0.25f);

				glm::vec3 normale = getNormaleMattone(x, z);

				pallaAt = pallaAt - 2.0f * glm::dot(pallaAt, normale) * normale;
				pallaAt = glm::vec3(pallaAt.x + random, pallaAt.y, pallaAt.z + random);
				pallaAt = glm::normalize(pallaAt);

				pallaPos = pallaPos + (translateSpeedPalla)*pallaAt;

			}

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
	mattoniShader->use();
	mattoniShader->setMat4("projection", projection);
	mattoniShader->setMat4("view", view);
	mattoniShader->setInt("myTexture1", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_mattoni);
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if (mapMattoni[i][j] != 0)
			{
				mattoniShader->setVec3("colorcube", colors[mapMattoni[i][j]]);

				/* Ogni cubo dista 1.1 unità dai vicini */
				float x = posXmattoni + i * lunghezzaMattone;
				float z = posZmattoni + j * larghezzaMattone;

				glm::mat4 model = glm::mat4(1.0f);	//identity matrix
				model = glm::translate(model, glm::vec3(x, posYmattoni, z));
				model = glm::scale(model, glm::vec3(lunghezzaMattone, altezzaMattone, larghezzaMattone));
				mattoniShader->setMat4("model", model);

				glDrawArrays(GL_TRIANGLES, 0, 36);

				float rangeCollision = (larghezzaPalla + lunghezzaMattone) / 2;

				if ( abs(x - pallaPos.x) < rangeCollision && abs(z - pallaPos.z) < rangeCollision) {

					mapMattoni[i][j] = 0;
					cubiEliminati = cubiEliminati++;

					float random = generaNumeroCasuale(-0.25f, 0.25f);

					glm::vec3 normale = getNormaleMattone(x,z);

					pallaAt = pallaAt - 2.0f * glm::dot(pallaAt, normale) * normale;
					pallaAt = glm::vec3(pallaAt.x + random, pallaAt.y, pallaAt.z + random);
					pallaAt = glm::normalize(pallaAt);

					pallaPos = pallaPos + (translateSpeedPalla) * pallaAt;

				}
			}

		}
	}



	//Mesaggio hai perso se la pallina supera la piattaforma
	if (pallaPos.z > piattaformaPos.z + 2.0f) {
		speedPalla = 0;
		//Sposto la camera
		cameraPos = glm::vec3(0.0f, 20.0f, 1.8f);  // Posizione camera
		cameraAt = glm::vec3(0.0f, 20.0f, -1.0f);	// Punto in cui "guarda" la camera
		bordoShader->use();

		glm::mat4 modelDialogBox = glm::mat4(1.0f);	//identity matrix
		modelDialogBox = glm::translate(modelDialogBox, glm::vec3(0.0f, 20.0f, 0.0f));
		modelDialogBox = glm::scale(modelDialogBox, glm::vec3(1.0f, 1.0f, 1.0f));
		bordoShader->setMat4("model", modelDialogBox);
		bordoShader->setVec3("colorcube", colors[0]);
		bordoShader->setInt("myTexture1", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_msgLost);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	//Mesaggio hai vinto se non sono rimasti cubi
	if (cubiEliminati + mattoniSpecialiEliminati == 48) {
		speedPalla = 0;
		//Sposto la camera
		cameraPos = glm::vec3(0.0f, 20.0f, 1.8f);  // Posizione camera
		cameraAt = glm::vec3(0.0f, 20.0f, -1.0f);	// Punto in cui "guarda" la camera
		bordoShader->use();

		glm::mat4 modelDialogBox = glm::mat4(1.0f);	//identity matrix
		modelDialogBox = glm::translate(modelDialogBox, glm::vec3(0.0f, 20.0f, 0.0f));
		modelDialogBox = glm::scale(modelDialogBox, glm::vec3(1.0f, 1.0f, 1.0f));
		bordoShader->setMat4("model", modelDialogBox);
		bordoShader->setVec3("colorcube", colors[0]);
		bordoShader->setInt("myTexture1", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_msgWin);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	float deltaPosX = abs(piattaformaPos.x - pallaPos.x);
	float rangeCollisionX = (lunghezzaPalla + lunghezzaPiattaforma) / 2;
	float deltaPosZ = abs(piattaformaPos.z - pallaPos.z);
	float rangeCollisionZ = ((larghezzaPalla + larghezzaPiattaforma) / 2 + 0.1) ;

	//Inverto la direzione se la palla colpisce la piattaforma
	if (deltaPosX <= rangeCollisionX && deltaPosZ <= rangeCollisionZ) {

		float random = generaNumeroCasuale(-0.25f, 0.25f);

		glm::vec3 normale = getNormalePiattaforma();
		pallaAt = pallaAt - 2.0f * glm::dot(pallaAt, normale) * normale;
		pallaAt = glm::vec3(pallaAt.x + random, pallaAt.y, pallaAt.z + random);
		pallaAt = glm::normalize(pallaAt);
		pallaPos = pallaPos + (translateSpeedPalla)*pallaAt;

	}

	//Disegno lo sfondo
	bordoShader->use();
	glm::mat4 modelCuboSfondo = glm::mat4(1.0f);	//identity matrix
	modelCuboSfondo = glm::translate(modelCuboSfondo, glm::vec3(-3.5f, -10.0f, -5.0f));
	modelCuboSfondo = glm::rotate(modelCuboSfondo, -45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	modelCuboSfondo = glm::scale(modelCuboSfondo, glm::vec3(55.0f, 55.0f, 0.1f));
	bordoShader->setMat4("model", modelCuboSfondo);
	bordoShader->setVec3("colorcube", colors[0]);
	bordoShader->setInt("myTexture1", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_sfondo);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

}


// load and create a texture 
unsigned int loadtexture(std::string filename)
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
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
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

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	//Decommentare per schermo intero
	/*
	// settings
	// Ottenere il monitor primario
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();

	// Ottenere la modalità video corrente del monitor primario
	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

	SCR_WIDTH = videoMode->width;
	SCR_HEIGHT = videoMode->height;
	*/
	
	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL 3.3 - Arkanoid!", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Impostare la finestra in modalità schermo intero
	/*
	glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
	*/

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	pallaShader       = new Shader("vs_palla.vs", "fs_palla.fs");
	bordoShader       = new Shader("vs_bordo.vs", "fs_bordo.fs");
	mattoniShader     = new Shader("vs_mattoni.vs", "fs_mattoni.fs");
	piattaformaShader = new Shader("vs_piattaforma.vs", "fs_piattaforma.fs");

	texture_mattoni     = loadtexture("../src/bricks.jpg");
	texture_mattoni_oro = loadtexture("../src/oro.jpg");
	texture_piattaforma = loadtexture("../src/tiles.jpg");
	texture_bordo       = loadtexture("../src/tiles3.jpg");
	texture_pavimento   = loadtexture("../src/tiles22.jpg");
	texture_palla       = loadtexture("../src/metal.jpg");
	texture_msgLost     = loadtexture("../src/msgLost.jpg");
	texture_msgWin      = loadtexture("../src/msgWin.jpg");
	texture_sfondo      = loadtexture("../src/sfondo.jpg");

	// load models
	Model sphere("../src/planet2.obj");

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_mesh), cube_mesh, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);

	// create transformations
	//glm::mat4 view = glm::mat4(1.0f);	//identity matrix;
	glm::mat4 projection = glm::mat4(1.0f);	//identity matrix
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	// Genera un numero casuale float tra -12 e 12
	random_x = generaNumeroCasualeStart(-6.0f, 6.0f);
	
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);
		idle();

		render(projection);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
		calculateFPS();
		std::string framePerSecond = std::to_string(fps);
		glfwSetWindowTitle(window, framePerSecond.c_str());
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

