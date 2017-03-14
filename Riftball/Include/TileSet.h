#pragma once


struct NumberArray {
	float x, y; //position
	float u, v; //texture
};
const NumberArray NumberArrayQuad[] = {
	{ 0.0f, 0.0f, 0.0f }, //bottom left		//--
	{ 0.0f, 1.0f, 0.0f }, //top left		//-+
	{ 1.0f, 1.0f, 0.0f }, //top right		//++
	{ 1.0f, 0.0f, 0.0f }  //bottom right	//+-
};
const int NumberArrayIndices[] = {
	0, 1, 2,
	0, 2, 3
};
//glVertexPointer(3, GL_FLOAT, 0, NumberArrayQuad);
//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NumberArrayIndices);
void InitializeNumberArray() {
	NumberArray NumberVertex[400];
	int index = 0;
	float xOffset = -0.95f;
	float yOffset = -0.95f;
	for (int i = 0; i != 10; i++) {
		for (int j = 0; j != 10; j++) {
			for (int ij = 0; ij != 10; ij++) {
				NumberVertex[index].x = NumberArrayQuad[ij].x + xOffset;
				NumberVertex[index].y = NumberArrayQuad[ij].y + yOffset;
				NumberVertex[index].u = NumberArrayQuad[ij].u;
				NumberVertex[index].v = NumberArrayQuad[ij].v;
				index++;
			}
			xOffset += 0.2f;
		}
		yOffset += 0.2f;
		xOffset = -0.95f;
	}
}