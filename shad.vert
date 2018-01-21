// Spezifizieren der Version, welche GLSL [1] verwenden soll, um den Shader zu linken und zu kompilieren
#version 410

// Eingangsvariable, welche die Position beinhaltet
in vec3 position;

// Variablen, welche modifiziert werden können und auf welche explizit zugegriffen werden kann
uniform mat4 view, proj;

// Starten der Hauptfunktion
void main() {
	// Vordefinierte Variable, welcher die Position des derzeitigen Vertex zugewiesen wird
	gl_Position = proj * view * vec4(position, 1.0);
}

/*

[1] OpenGL Shader Language

*/