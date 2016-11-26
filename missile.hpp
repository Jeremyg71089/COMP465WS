
# ifndef __INCLUDES465__
# include "../includes465/include465.hpp"
# define __INCLUDES465__
# endif

class Missile{
private:
	bool isVisible;
	bool isFired;
	bool isDetonated;
	bool isCollided;
	bool isSite;
	bool targetSet = false;
	int targetVal = -1;
	int updates;
	int step = 0;
	int stepDistance = 0;
	float angle = 0.0f;
	glm::mat4 target;
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

	Missile(glm::vec3 translate , glm::vec3 scale, bool site) {
		isVisible = false;
		isFired = false;
		isDetonated = false;
		isCollided = false;
		isSite = site;
		if (site) {
			stepDistance = 5;
		}
		else {
			stepDistance = 20;
		}
		updates = 0;
		position = translate;
		TM = glm::translate(TM, translate);
		SM = glm::scale(glm::mat4(), scale);
		//RM = glm::rotate(RM, 0.0f, glm::vec3(0, 1, 0));
		OM = TM * RM * SM;
	}
	
	glm::mat4 getOM() {
		return OM;
	}

	void setTranslate(glm::vec3 translate) {
		TM = glm::translate(glm::mat4(), translate);
	}

	//Finish this method
	void setRotate(glm::mat4 inputRM) {
		RM = inputRM;
	}

	//Method to fire missile
	void fireMissile() {
		if (isFired == false && isDetonated == false) {
			isFired = true;
			isVisible = true;
		}		
	}


	//Will take source OM, target1 OM, target2 OM
	//Will return 0 if there is no target within range, 1 if first target is in range, and 
	//2 if second target is within range
	void setClosestTarget(glm::mat4 target1, glm::mat4 target2, int index1, int index2) {

		float target1Distance = distance(getPosition(OM), getPosition(target1));
		float target2Distance = distance(getPosition(OM), getPosition(target2));

		if (target1Distance > 5000.0f && target2Distance > 5000.0f) {

		}
		else if (target1Distance < target2Distance) {
			target = target1;
			targetSet = true;
			targetVal = index1;
		}
		else if (target2Distance < target1Distance) {
			target = target2;
			targetSet = true;
			targetVal = index2;
		}
	}

	//Returns if target has been set
	bool getTargetSet() {
		return targetSet;
	}

	//Returns OM of target
	glm::mat4 getTarget() {
		return target;
	}

	void updateTargetPos(glm::mat4 targetPos) {
		target = targetPos;
	}

	//Returns which target has been set
	int getTargetVal() {
		return targetVal;
	}


	glm::mat4 getTM() {
		return TM;
	}

	void setTM(glm::mat4 inTM) {
		TM = inTM;
	}
	glm::mat4 getRM() {

		return RM;
	}

	//Get distance
	float getDistance(glm::vec3 pos1, glm::vec3 pos2) {
		return glm::distance(pos1, pos2);
	}

	//Method to fire missile
	bool getFired() {
		return isFired;
	}

	//Method to check if detonated
	bool detonated() {
		return isDetonated;
	}

	//Need a fucntion to detect collisions

	void setVisible(bool visible) {
		isVisible = visible;
	}

	bool getVisible() {
		return isVisible;
	}

	//Check if there is a collision
	bool checkCollision(glm::vec3 pos1, glm::vec3 pos2, float radius1, float radius2) {
		//Check for collision, 
		if (distance(pos1, pos2) <= (radius1 + radius2)) {
			isCollided = true;
			isVisible = false;
			isDetonated = true;
		}
		return isCollided;
	}

	//function to chase target
	void faceTarget(glm::mat4 target) {
		//Get the looking at vector from the the chaser
		L = glm::normalize(getIn(OM));

		//Get distance between the target and chaser
		T = glm::normalize(getPosition(target) - getPosition(OM));

		if (colinear(L, T, 0.1f)) {
			float CLATtoE = getDistance(getPosition(OM) + getIn(OM), getPosition(target));
			float CtoE = getDistance(getPosition(OM), getPosition(target));

			if (CtoE < CLATtoE) {
				RM = glm::rotate(glm::mat4(), PI, getUp(OM));
			}
			OM = glm::translate(TM, getPosition(OM)) * RM * SM;
		}
		else {
			axis = glm::normalize(glm::cross(T, L)); //AOR
			angle = (2 * PI) - glm::clamp(acos(glm::dot(T, L)), -1.0f, 1.0f);
			if (getDistance(getPosition(OM), getPosition(target)) > 0.1f){
				RM = glm::rotate(glm::mat4(), angle, axis);
			}

			OM = glm::translate(TM, getPosition(OM)) * RM * SM;
		}
	}

	glm::vec3 getPos() {
		return position;
	}
	glm::mat4 getSM() {

		return SM;
	}
	void setOM(glm::mat4 inOM) {
		OM = inOM;
	}
	void setRM(glm::mat4 r) {
		RM = r;
	}
	void setSM(glm::mat4 inSM) {
		SM = inSM;
	}
	glm::vec3 getForward() {
		return forward;
	}





    void update(){

		updates++;

		//Check for max updates and detonate
		if (!targetSet && updates == 2000) {
			isDetonated = true;
			isVisible = false;
		}
		
		step++;
		forward = getIn(OM) * (float)step * (float)stepDistance;
		TM = glm::translate(TM, forward);
		OM = TM * RM * SM;
		step = 0;
    }
};



