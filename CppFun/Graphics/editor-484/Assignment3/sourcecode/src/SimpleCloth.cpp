#include "SimpleCloth.h"
#include <vector>

SimpleCloth::SimpleCloth(float x, float y, float z, float scale, int colorIndex, int id, float length, float mass, int clothSize)
    : PendulumSystem(x, y, z, scale, colorIndex, id, clothSize * clothSize)  
{
	m_clothSize = clothSize;
	m_length = length;
	m_mass = mass;

	shapeType = "Simple Cloth";  // Set the type as "Simple Cloth"

	isCloth = true;

	float restL = 0.20f;
	float distance = 0.20f;
	float springC = 200.0f;

	for (int row=0; row < clothSize; row++){
	    for (int col=0; col < clothSize; col++){

		if ((row == 0 && col == 0) || (row == 0 && col == clothSize-1)){	//endpoints are fixed
		        particles.push_back(glm::vec4(col*distance, 0.0f, row*distance, 1.0f));}
		else{   particles.push_back(glm::vec4(col*distance, 0.0f, row*distance, 0.0f));}
		
		//Structural Springs
		//======================================================
		//Link(row,col) to Link(row+1,col)
		if (row < clothSize-1){
		    float p0 = (float)((row)   * clothSize + (col));
		    float p1 = (float)((row+1) * clothSize + (col));
		    springs.push_back(glm::vec4(p0, p1, restL, springC));
		}
		//Link(row,col) to Link(row,col+1)
		if (col < clothSize-1){
		    float p0 = (float)((row) * clothSize + (col)  );
		    float p1 = (float)((row) * clothSize + (col+1));
		    springs.push_back(glm::vec4(p0, p1, restL, springC));
		}
		
		//Shear Springs
		//======================================================
		//Link(row,col) to Link(row+1,col+1)
		if (row < clothSize-1 && col < clothSize-1){
		    float p0 = (float)((row)   * clothSize + (col)  );
		    float p1 = (float)((row+1) * clothSize + (col+1));
		    springs.push_back(glm::vec4(p0, p1, restL*sqrt(2), springC));
		}
		//Link(row,col) to Link(row+1,col-1)
		if (row < clothSize-1 && col > 0){
		    float p0 = (float)((row) * clothSize + (col)  );
		    float p1 = (float)((row+1) * clothSize + (col-1));
		    springs.push_back(glm::vec4(p0, p1, restL*sqrt(2), springC));
		}
		
		//Flex Springs
		//======================================================
		//Link(row,col) to Link(row+2,col)
		if (row < clothSize-2){
		    float p0 = (float)((row)   * clothSize + (col));
		    float p1 = (float)((row+2) * clothSize + (col));
		    springs.push_back(glm::vec4(p0, p1, restL*2, springC));
		}
		//Link(row,col) to Link(row,col+2)
		if (col < clothSize-2){
		    float p0 = (float)((row) * clothSize + (col)  );
		    float p1 = (float)((row) * clothSize + (col+2));
		    springs.push_back(glm::vec4(p0, p1, restL*2, springC));
		}

		//Faces
		
		//======================================================
		if (row < clothSize-1 && col < clothSize-1){

		    //Front - normals pointing forward
		    
		    glm::vec3 face1 = glm::vec3((float)((row+1)*clothSize + (col+1)),
					      (float)((row)  *clothSize + (col+1)),
					      (float)((row+1)*clothSize + (col)  ));
		    faces.push_back(face1);

		    glm::vec3 face2 = glm::vec3((float)((row)  *clothSize + (col+1)),
					      (float)((row)  *clothSize + (col)  ),
					      (float)((row+1)*clothSize + (col)  ));
		    faces.push_back(face2);


		    
		}
		
		
	    }
	}

	setupParticles(particles, springs, faces);


}


