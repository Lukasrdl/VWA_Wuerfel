/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*															Vorwissenschaftliche Arbeit von Lukas Riedl															*/
/*												Darstellung dreidimensionaler Objekte auf Flächen mithilfe von Dreiecken										*/
/*																																								*/
/*															Kapitel 5.1 Darstellung eines Würfels in OpenGL														*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*																																								

Folgendes Kapitel beschäftigt sich mit der Darstellung eines dreidimensionalen Würfels in OpenGL. Dafür richte ich mich nach einem etablierten Schema.
Für die Behandlung des gestellten Problems werden folgende Schritte benötigt[1]:

	1. Definieren der benötigten mathematischen Strukturen wie Vektoren und Matrizen und deren Operationen, wie sie im Theorieteil dieser Arbeit besprochen wurden
	2. Definieren der benötigten Punkte im dreidimensionalen Raum
	3. Festlegung von Indizes, welche mit den in Schritt 2 definierten Punkten multipliziert werden, um die Reihenfolge der Punkte felstlegen, wie sie für die Darstellung eines Würfels benötigt werden
	4. OpenGL Vorbereitungen und Buffer
	5. Durchlaufen der Grafikpipeline
	6. Einbinden eines Shaders
	7. Game-Loop und das Schicken der Daten an OpenGL und das damit verbundene Darstellen auf einem Bildschirm


Ich beschränke das Kommentieren des Codes nicht nur auf die in der Arbeit direkt angesprochenen Abschnitte, sondern auch um andere wichtige Schritte, welche in OpenGL erfüllt werden müssen, 
um ein korrektes Ergebnis zu erhalten.

*/


/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*																		Anfang des Programms																	*/
/*														  Einbinden der benötigten Header und Bibliotheken														*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/


// Hilfsbibliotheken: 
//	"OpenGL Extension Wrangler Library"
#include <GL\glew.h>
//	Eine Utility-Bibliothek, welche unter anderem auch beim erstellen eines Fensters hilft
#include <GLFW\glfw3.h> 

//	Standarddirektiven von C++
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <string>


/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*											1. Definieren von Strukturen zu generieren der benötigten Objekte (Matrix, Vektor)	[2]								*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/


// Eine Struktur mit 4 Dezimalzahlen(float), welche als Vektor dient
struct vec4 {
	vec4();
	vec4(float x, float y, float z, float w);
	float e[4] = { 0 };
};

// Konstruktoren der Vektoren
vec4::vec4() {};
vec4::vec4(float x, float y, float z, float w) {
	e[0] = x;
	e[1] = y;
	e[2] = z;
	e[3] = w;
}


// Eine Struktur mit 16 Dezimalzahlen, welche als Matrizen dient
struct mat4 {
	mat4();
	mat4(float null, float eins, float zwei, float drei,
		float vier, float fuenf, float sechs, float sieben,
		float acht, float neun, float zehn, float elf,
		float zwoelf, float dreizehn, float vierzehn, float fuenfzehn);
	float e[16] = { 0 };
};

// Konstuktoren der Matrizen
mat4::mat4() {
	// Identitätsmatrix
	e[0] = 1;
	e[5] = 1;
	e[10] = 1;
	e[15] = 1;
}
mat4::mat4(float null, float eins, float zwei, float drei,
	float vier, float fuenf, float sechs, float sieben,
	float acht, float neun, float zehn, float elf,
	float zwoelf, float dreizehn, float vierzehn, float fuenfzehn) {
	e[0] = null;
	e[1] = eins;
	e[2] = zwei;
	e[3] = drei;
	e[4] = vier;
	e[5] = fuenf;
	e[6] = sechs;
	e[7] = sieben;
	e[8] = acht;
	e[9] = neun;
	e[10] = zehn;
	e[11] = elf;
	e[12] = zwoelf;
	e[13] = dreizehn;
	e[14] = vierzehn;
	e[15] = fuenfzehn;
}


/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*																	Beginn der main-Funktion																	*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/


int main() {


	/*---------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*															   2. + 3. Definieren der Daten					                                                     */
	/*---------------------------------------------------------------------------------------------------------------------------------------------------------------*/


	// Die Daten für die Eckpunkte eines Würfels mit der Seitenlänge 1 werden in einem GLfloat (entspricht float) Array gespeichert
	GLfloat punkte[] = {
		-0.5f, -0.5f, -0.5f, // unten vorne rechts
		-0.5f, -0.5f, 0.5f,  // unten hinten rechts
		0.5f, -0.5f, -0.5f,  // unten vorne links
		0.5f, -0.5f, 0.5f,   // unten hinten links

		-0.5f, 0.5f, -0.5f,  // oben vorne rechts
		-0.5f, 0.5f, 0.5f,	 // oben hinten rechts
		0.5f, 0.5f, -0.5f,	 // oben vorne links
		0.5f, 0.5f, 0.5f,	 // oben hinten links
	};

	// Ein Index repräsentiert 3 aufeinanderfolgende Dezimalzahlen (3 * n, 3 * n + 1, 3 * n + 2) und somit einen vollständigen Eckpunkt
	GLuint indexe[] = {
		// Zwei Dreiecke für die obere Seite
		4, 
		5, 
		6, 
		
		5,
		6, 
		7,

		// Zwei Dreiecke für die rechte Seite
		0,
		1,
		4,

		1,
		4,
		5,

		// Zwei Dreiecke für die hintere Seite
		1,
		3,
		5,

		3,
		5,
		7,

		// Zwei Dreiecke für die linke Seite
		2,
		3,
		6,

		3,
		6,
		7,

		// Zwei Dreiecke für die untere Seite
		0,
		1,
		2,

		2,
		3,
		4,

		// Zwei Dreiecke für die vordere Seite
		0,
		2,
		4,

		2,
		4,
		6
	};


	/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*												4. Vorbereitung von GLEW, GLFW und Erstellen eines Fensters														*/
	/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/


	// Initialisierung von GLFW
	glfwInit();

	// Erstellen eines 640x480 Fensters
	GLFWwindow *window = glfwCreateWindow(640, 480, "Wuerfel in OpenGL 4", NULL, NULL);

	// Das Fenster "aktiv" stellen um es modifizieren zu können
	glfwMakeContextCurrent(window);

	// Initialisieren von GLEW
	glewInit();

	// GLEW Extension Handler (Kümmert sich automatisch um die optionale Erweiterungen von GLEW)
	glewExperimental = GL_TRUE;

	// Ausgabe der aktuellen OpenGL Version
	std::cout << "OpenGL version " << glGetString(GL_VERSION) << std::endl;


	/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*													VBOs und VAOs; Datenspeicherung und Datenreferenzierung														*/
	/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/


	// Erstellen eines VBOs ("Vertex Buffer Object", welcher Daten in einem Array gespeichert. 
	
	// Generieren des VBOs, welches später die Punkte speichern soll.
	GLuint vbo;
	glGenBuffers(1, &vbo);
	// Durch das Binden eines Buffers setzt man diesen "aktiv"
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Kopieren der Punkte in den eben erstellten Buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(punkte), &punkte, GL_STATIC_DRAW);

	// Ein VAO ("Vertex Attribute Buffer"). Dieses Objekt speichert die Adresse von VBOs, um auf diese referenzieren zu können

	// Erstellen des VAOs
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// Binden des VAOs
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Das Layout der Daten wird dem VAO übergeben, Attribut Nr. 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	// Aktivierung des 0ten Attributs
	glEnableVertexAttribArray(0);

	// Ein Index Buffer wird dazu verwendet um auf Daten zuzugreifen und wird auch in einem VBO gespeichert.
	// Er wird hauptsächlich dafür verwendet, dass man jeden Punkt nur ein Mal definieren muss und ihn dann mehrmals ansprechen kann

	// Erstellen eines Index Buffers
	GLuint index_vbo;
	glGenBuffers(1, &index_vbo);
	// Binden des Index Buffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);
	// Daten dem Buffer zuweisen
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexe), indexe, GL_STATIC_DRAW);


	/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*																	5. Grafikpipeline																			*/
	/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	

	/* ----- Erstellen der View Matrix ----- */

	// Position der Kamera, bei (0|0|0) wird der Würfel gezeichnet
	float kamera_position[] = { 0.0f, 0.0f, 1.0f };
	// Rotation der Kamera um den Würfel in verschiedenen Winkeln anzusehen
	float kamera_rotiert = 0.0f;

	// Erstellen einer Transformations-Matrix
	mat4 transform_mat;
	transform_mat.e[12] = -kamera_position[0];
	transform_mat.e[13] = -kamera_position[1];
	transform_mat.e[14] = -kamera_position[2];

	// Erstellen einer Rotations-Matrix um die y-Achse
	mat4 rotations_mat = mat4();
	float kamera_rot_in_rad = kamera_rotiert * (67.0 * (3.14159265359) / 180.0);
	rotations_mat.e[0] = std::cos(kamera_rot_in_rad);
	rotations_mat.e[2] = -std::sin(kamera_rot_in_rad);
	rotations_mat.e[8] = std::sin(kamera_rot_in_rad);
	rotations_mat.e[10] = std::cos(kamera_rot_in_rad);

	// Zusammenstellen der View Matrix
	mat4 view_mat;
	int zeilen_index = 0;
	for (int spalte = 0; spalte < 4; spalte++) {
		for (int zeile = 0; zeile < 4; zeile++) {
			float sum = 0.0f;
			for (int i = 0; i < 4; i++) {
				sum += transform_mat.e[i + spalte * 4] * rotations_mat.e[zeile + i * 4];
			}
			view_mat.e[zeilen_index] = sum;
			zeilen_index++;
		}
	}

	/* ----- Erstellen der Projection Matrix ----- */

	// Definieren der benötigte Daten
	float naheEbene = 0.1f;
	float ferneEbene = 1000.0f;
	float sv = (float)640 / (float)480;
	// Field of View in Radiant
	float fov = 90.0 * (3.14159265359 / 180.0);
	float d = 1 / tan(fov / 2);

	// Zusammenstellen der Projection Matrix
	mat4 proj_mat;
	proj_mat.e[0] = 1 / (sv * tan(fov / 2));
	proj_mat.e[5] = d;
	proj_mat.e[10] = -(naheEbene -ferneEbene ) / (naheEbene - ferneEbene);
	proj_mat.e[11] = -(2.0f * ferneEbene * naheEbene) / (naheEbene - ferneEbene);
	proj_mat.e[14] = -1.0f;


	/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*																		6. Shader																				*/
	/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/


	// Streams um die Shader zu lesen
	std::ifstream vertex_shader_file;
	std::ifstream fragment_shader_file;

	// Öffnen der Shader-Dateien
	vertex_shader_file.open("shad.vert");
	fragment_shader_file.open("shad.frag");

	// Streams zum konvertieren der Daten
	std::stringstream vertex_shader_stream;
	std::stringstream fragment_shader_stream;

	// Lesen der rohen Dateien
	vertex_shader_stream << vertex_shader_file.rdbuf();
	fragment_shader_stream << fragment_shader_file.rdbuf();

	// Objekte um die Daten zu speichern
	std::string vertex_shader_data_string;
	std::string fragment_shader_data_string;

	// Anfordern der Daten aus dem Stream
	vertex_shader_data_string = vertex_shader_stream.str();
	fragment_shader_data_string = fragment_shader_stream.str();

	// Konvertieren der Daten in C-style Strings, da OpenGL dieses Format erwartet
	const GLchar *vertex_shader_data = vertex_shader_data_string.c_str();
	const GLchar *fragment_shader_data = fragment_shader_data_string.c_str();


	// Das Erstellen eines sogenannten Programmes (Vereint einen oder mehrere Shader in einem Objekt)
	GLuint shader_program = glCreateProgram();


	// Erstellen eines Vertex Shaders, welcher Daten manipuliert (siehe file.vert)
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

	// Zuweisung der Daten an den Vertex Shader
	glShaderSource(vertex_shader, 1, &vertex_shader_data, NULL);

	// Compilieren des Vertex Shaders
	glCompileShader(vertex_shader);

	// Hinzufügen des Shaders an das Programm
	glAttachShader(shader_program, vertex_shader);


	// Erstellen eines Fragment Shaders, welcher für Farben und Licht zuständig ist (siehe file.frag)
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	// Zuweisung der Daten an den Fragment Shader
	glShaderSource(fragment_shader, 1, &fragment_shader_data, NULL);

	// Compilieren des Fragment Shaders
	glCompileShader(fragment_shader);

	// Hinzufügen des Shaders an das Programm
	glAttachShader(shader_program, fragment_shader);


	// Verbinden der Shader / Linken des Programmes
	glLinkProgram(shader_program);

	// Programm in den Fukus bringen
	glUseProgram(shader_program);

	// Erstellen von zwei "Uniforms", mit welchen Shader Daten modifiziert und ausgetauscht werden können
	// Eine Variable für die View- und eine für die Projektionsmatrix
	GLint view_mat_location = glGetUniformLocation(shader_program, "view");
	GLint prog_mat_location = glGetUniformLocation(shader_program, "proj");
	
	// Zuweisen neuer Daten an die Shader
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat.e);
	glUniformMatrix4fv(prog_mat_location, 1, GL_FALSE, proj_mat.e);


	/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*																		7. Game-Loop																			*/
	/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/


	while (!glfwWindowShouldClose(window)) {
		// Definieren einer Hintergrundfarbe
		glClearColor(0.3f, 0.3f, 0.9f, 1.0f);
		// Leeren einiger Bits
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Verbinden der Punkte nur mit Linien, nicht mit Flächen [3]
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Setzen des Bildschirmes
		glViewport(0, 0, 640, 480);

		// Aktivierung der "Tiefentestung" [4]
		glEnable(GL_DEPTH_TEST);
		// Nur die näheste Fläche soll dargestellt werden, andere Fragmente werden verworfen
		glDepthFunc(GL_LESS);

		// Zeichnen von 12 bzw. (36 / 3) Punkten
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

		// GLFW Überprüfungen
		glfwPollEvents();
		// Der Framebuffer[5] (Bildspeicher) muss gewechselt werden 
		glfwSwapBuffers(window);
	}

	// Beenden der GLFW Komponenten
	glfwTerminate();

	return 0;
}



/*
	Fußnoten

	[1] Der gesamt Ablauf der OpenGL-Funktionsaufrufe in diesem Programm basiert auf den Erklärungen aus dem Buch "Anton's OpenGL 4 turorials"
	
	[2] Die Idee Strukturen für Vektoren und andere Objekte zu benutzen ist sehr gängig. Eine Andere Möglichkeit wäre eine weitere Bibliothek, welche diese Strukturen verwaltet
		und meist auch rechnerische Operatoren zur Verfügung stellt. Aus Darstellungszwecken habe ich jedoch von so einer Bibliothek abgelassen.

	[3] Diese Einstellung ist für Demonstrationszwecke gedacht. Sie ist jedoch nicht für ein laufendes Programm nötig.

	[4] Depth Testing (dt. Tiefentestung) beschreibt den Vorgang der Verglichung der z-Komponenten, welche an der gleichen Stelle dargestelt werden müssten

	[5] Der Framebuffer ermöglicht das Konzept der sogenannten Doppelpufferung. Dabei werden in einen Buffer die momentanen Bildinformaitonen gespeichert, während der andere, 
		welche schon im Frame davor gespeichert wurde, von OpenGL gezeichnet wird. Dies verringert die Möglichkeit, dass noch nicht zur Gänze gespeicherte Frames auf dem Bildschirm gezeichnet werden,
		da für die Speicherung immer doppelt so viel Zeit vorhanden ist.

*/