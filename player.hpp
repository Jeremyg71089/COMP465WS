
# ifndef __INCLUDES465__
# include "../includes465/include465.hpp"
#include "glm/gtx/rotate_vector.hpp"
# define __INCLUDES465__
# endif

class Player{
private:
    int step = 0;
	int stepDistance = 20;
	int pitch = 0, roll = 0 , yaw = 0;
    float radians = 0.02;
	glm::vec3 forward = glm::vec3(0.0f);
    glm::mat4 RM = glm::mat4();
	glm::mat4 OM = glm::mat4();
	glm::mat4 TM = glm::mat4();
	glm::mat4 SM = glm::mat4();
	glm::vec3 position;

public:

	Player(glm::vec3 t , glm::vec3 s) {
		position = t;
		TM = glm::translate(glm::mat4(),t);
		SM = glm::scale(glm::mat4(), s);
		OM = TM * RM * SM;

	}
	void setMove(int i){
		step = i;
    }
    void setPitch(int i){
        pitch = i;
    }
    void setRoll(int i){
        roll = i;
    }
	void setYaw(int i) {
		yaw = i;
	}
	glm::mat4 getOM() {
		return OM;
	}
	void warp(glm::vec3 t) {
		TM = glm::translate(glm::mat4(), t);
		
	}
	glm::vec3 getPos() {
		return position;
	}
	glm::mat4 getRM() {

		return RM;
	}
	
    void update(){
		if (pitch != 0){
			RM = glm::rotate(RM,pitch*radians,glm::vec3(1,0,0));
		}
		else if (yaw != 0) {
			RM = glm::rotate(RM, yaw*radians, glm::vec3(0, 1, 0));
			
		}
		else if (roll != 0 ) {
			RM = glm::rotate(RM, roll*radians, glm::vec3(0, 0, 1));
		}
		forward = getIn(OM) * (float)step * (float)stepDistance;
		TM = glm::translate(TM, forward);
		OM = TM * RM * SM;
		step = 0;
		yaw = 0;
		roll = 0;
		pitch = 0;
    }

};



