#define __Windows__
#include "../includes465/include465.hpp"

class Player{
private:
    int step;
	int stepDistance = 1;
    int pitch, roll , yaw;
    float radians = 0.02;
	glm::vec3 forward;
    glm::mat4 RM = glm::mat4();
	glm::mat4 OM = glm::mat4();
	glm::mat4 TM = glm::mat4();
	glm::vec3 axis;

public:
	void setMove(int i){
        step = i;
    }
    void setPitch(int i){
        pitch = i;
    }
    void setRoll(int i){
        roll = i;
    }
    void setYaw(int i){
        yaw = i;
    }

	glm::mat4 getOM() {
		return OM;

	}

    void update(){
		if (pitch != 0)
		{
			RM = glm::rotate(RM,pitch*radians,glm::vec3(1,0,0));
		}
		else if (yaw != 0) {
			RM = glm::rotate(RM, pitch*radians, glm::vec3(0, 1, 0));

		}
		else if (roll != 0 ) {

			RM = glm::rotate(RM, pitch*radians, glm::vec3(0, 0, 1));
		}
		forward = getIn(OM) *glm::vec3(step) * glm:: vec3( stepDistance);
		axis = glm::vec3(pitch, yaw, roll);
		TM = glm::translate(TM, forward);
		OM = TM * RM;
		step = yaw = roll = 0;
    }

};



