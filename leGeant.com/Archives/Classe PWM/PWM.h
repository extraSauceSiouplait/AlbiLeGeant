
#ifndef PWM_H
#define PWM_H

class PWM {
public:
    PWM(int frequence = 0, int pourcentage = 0, bool marcheArriere = false);
    void setFrequence(int frequence);
    void setPourcentage(int pourcentage);
    void setDirection(bool marcheArriere);
    
    
private:
    int frequence_;
    int pourcentage_;
    bool marcheArriere_;
};
#endif


