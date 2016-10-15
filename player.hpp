

class Player{

    int step;
    int pitch, roll , yaw;
    float radians;
    glm::vec3 forward;
    glm::mat4 RM = glm::mat4();
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

    void update(){
        if(pitch!= 0)


    }

};







