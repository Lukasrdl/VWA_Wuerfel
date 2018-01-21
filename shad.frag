#version 410

// Vektor, welcher die Farbe für jedes Fragment speichert 
out vec4 color;

// Starten der Hauptfunktion 
void main() {
	// Zuweisen einer Farbe im RGBA-Format [1]
	color = vec4(1.0, 0.0, 0.0, 1.0);
}

/*

[1] RGBA = Rot, Grün, Blau, Alpha (Transparenz)

*/