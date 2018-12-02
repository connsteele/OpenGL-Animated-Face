#version 330 core

/***** LOCATIONS FROM BINDING IN .CPP ******/
//A Ah face
layout(location = 0) in vec3 vertPos1; //2x things for each face, normals and textures
layout(location = 1) in vec3 vertNor1; //If u want textures you need another parameter for each face

//E face
layout(location = 2) in vec3 vertPos2;
layout(location = 3) in vec3 vertNor2;

//FV Face
layout(location = 4) in vec3 vertPos3;
layout(location = 5) in vec3 vertNor3;

//LD
layout(location = 6) in vec3 vertPos4;
layout(location = 7) in vec3 vertNor4;

//O
layout(location = 8) in vec3 vertPos5;
layout(location = 9) in vec3 vertNor5;

//MB
layout(location = 10) in vec3 vertPos6;
layout(location = 11) in vec3 vertNor6;

//STCh
layout(location = 12) in vec3 vertPos7;
layout(location = 13) in vec3 vertNor7;

//UR
layout(location = 14) in vec3 vertPos8;
layout(location = 15) in vec3 vertNor8;
//end normal faces

//start t800 faces
//layout(location = 16) in vec3 vertPos9; //using these in my func breaks it
//layout(location = 17) in vec3 vertNor9;

/***** UNIFORMS ******/
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform float t; //use for what % of facet and facet2 to use, 0% is 100% facet and 100% is 100% facet2
//need uniforms to pick what % of each face to use
uniform int facet; //my uniform for the current face to use
uniform int facetPrev; //my uniform for the prev face to use

/***** OUT TO FRAG SHADER ******/
out vec3 fragPos;
out vec3 fragNor;
out vec3 lightPos;

/**** FUNCTIONS ********/

vec3 getNewFace(int face) //Do all face changing here
{
	//FACE KEY: Face AAh is 1, Face E is 2, Face FV is 3, Face LD is 4, Face O is 5, face slient/m/b is 6, face STCh is 7, face UR is 8
	if (face == 1 ) //AAh
	{
		return vertPos1;
	}
	else if (face == 2 ) //E
	{
		return vertPos2;
	}
	else if (face == 3 ) //FV
	{
		return vertPos3;
	}
	else if (face == 4 ) //LD
	{
		return vertPos4;
	}
	else if (face == 5 ) //O
	{
		return vertPos5;
	}
	else if (face == 6 ) //MBsilent
	{
		return vertPos6;
	}
	else if (face == 7 ) //STCh
	{
		return vertPos7;
	}
	else if (face == 8 ) //UR
	{
		return vertPos8;
	}
	else //return default face
	{
		return (vertPos1+vertPos2)/2; //TEMP return the average
	}
}

vec3 getNormFace(int face) //combine this with face functon if you have time
{
	
	if (face == 1 ) //AAh
	{
		return vertNor1;
	}
	else if (face == 2 ) //E
	{
		return vertNor2;
	}
	else if (face == 3 ) //FV
	{
		return vertNor3;
	}
	else if (face == 4 ) //LD
	{
		return vertNor4;
	}
	else if (face == 5 ) //O
	{
		return vertNor5;
	}
	else if (face == 6 ) //MBsilent
	{
		return vertNor6;
	}
	else if (face == 7 ) //STCh
	{
		return vertNor7;
	}
	else if (face == 8 ) //UR
	{
		return vertNor8;
	}
	else //return default face
	{
		return (vertNor1+vertNor2)/2; //TEMP return the average
	}
}

void main() {

	//Get variables to use in interpolation
	vec3 newVertPos = getNewFace(facet); //get current face
	vec3 oldVertPos = getNewFace(facetPrev); //get prev face
	vec3 newNorms = getNormFace(facet); //get current normals
	vec3 oldNorms = getNormFace(facetPrev); //get old normals

	//Get the actual postion and norms using interpolation
	vec3 interpolPos = ( (1 - t) * oldVertPos ) + ( t * newVertPos ); //fixed order, had it reversed before
	vec3 interpolNorm = ( (1 - t) * oldNorms ) + ( t * newNorms );


	//Send to the frag shader to be drawn
	interpolPos.z *= -1; //Flip how the face renders
    gl_Position = P * V * M * vec4(interpolPos, 1.0);
    fragPos = vec4(V * M * vec4(interpolPos, 1.0)).xyz; //Update Verts
    fragNor = vec4(V * M * vec4(interpolNorm, 0.0)).xyz; //Update Norms
    lightPos = vec3(V * vec4(100, 100, 100, 1));
}
