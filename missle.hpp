
# ifndef __INCLUDES465__
# include "../includes465/include465.hpp"
#include "glm/gtx/rotate_vector.hpp"
# define __INCLUDES465__
# endif

class Missle{
private:
	bool isVisible;
	bool isFired;
	bool isDetonated;
	bool isCollided;
	int objectId;
	int updates;
	int step = 0;
	int stepDistance = 20;
	float angle = 0.0f;
	glm::vec3 axis = glm::vec3(0.0f);
	glm::vec3 forward = glm::vec3(0.0f);
    glm::mat4 RM = glm::mat4();
	glm::mat4 OM = glm::mat4();
	glm::mat4 TM = glm::mat4();
	glm::mat4 SM = glm::mat4();
	glm::vec3 position;
	glm::vec3 T;
	glm::vec3 L;
	float AORdirection;
	bool isWarping = false;

public:

	Missle(glm::vec3 translate , glm::vec3 scale, int id) {
		objectId = id;
		isVisible = false;
		isFired = false;
		isDetonated = false;
		isCollided = false;
		updates = 0;
		position = translate;
		TM = glm::translate(glm::mat4(), translate);
		SM = glm::scale(glm::mat4(), scale);
		//RM = glm::rotate(RM, -1.571f, glm::vec3(1, 0, 0));
		OM = TM * RM * SM;
	}
	
	glm::mat4 getOM() {
		return OM;
	}

	void setTranslate(glm::vec3 translate) {
		TM = glm::translate(glm::mat4(), translate);
		OM = TM * RM * SM;
	}

	//Finish this method
	void setRotate(glm::mat4 inputRM) {
		//RM = glm::rotate(inputRM, 0.0f, glm::vec3(1, 0, 0));;
		OM = TM * RM * SM;
	}

	//Method to fire missle
	void fireMissle() {
		if (isFired == false && isDetonated == false) {
			isFired = true;
			isVisible = true;
		}		
	}

	//Method to fire missle
	bool getFired() {
		return isFired;
	}

	//Method to check if detonated
	bool detonated() {
		return isDetonated;
	}

	//Method to check if collided
	bool collided() {
		return isCollided;
	}
	//Need a fucntion to detect collisions

	


	//function to chase target
	void chaseTarget(glm::mat4 targetPos) {
		//Get the looking at vector from the the chaser
		L = getIn(OM);

		//Get distance between the target and chaser
		T = getPosition(targetPos) - getPosition(OM);

		//normalize both vectors
		T = glm::normalize(T);
		L = glm::normalize(L);

		//cross product of T and L to get orthogonal vector 
		//this is the axis of rotation
		axis = glm::cross(T, L);

		//normalize axis
		axis = glm::normalize(axis);

		//to determine the direction of the rotation
		//acos gives the radians for rotation but only from 0 to PI
		//however since glm::rotate adjust the rotation if the axis is negative
		//it is not necessary to do anything to acos other than subtract it from 2PI
		angle = (2.0f*PI) - acos(glm::dot(T,L));

		//use the found rotational angle and axis to rotate the space ship
		//we rotate it around the identity because we do not need take the current
		//RM into account

		RM = glm::rotate(RM, angle, axis);
		OM = TM * RM * SM;
		
	}

	glm::vec3 getPos() {
		return position;
	}
	glm::mat4 getRM() {
		return RM;
	}
	void setRM(glm::mat4 r) {
		RM = r;
	}
	glm::vec3 getForward() {
		return forward;
	}

    void update(){
		updates++;
		if (updates == 2000) {
			isDetonated = true;
		}
		
		//put code to keep updating the missle to go after its target
		//put code to detect if there is a collision
		step++;
		forward = getIn(OM) * (float)step * (float)stepDistance;
		TM = glm::translate(TM, forward);
		OM = TM * RM * SM;
		step = 0;
    }
};



