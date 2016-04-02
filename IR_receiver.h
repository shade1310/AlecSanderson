#define sizex 8
#define sizey 8
#define threshval 250
#define numcycle 3

class IR_receiver{
    private:
      void setThreshold(int frame[][sizey], int &threshold, int &threshcount);

    public:
      IR_receiver();
      void IR_get(unsigned int spiValue[], int &threshold, int &threshcount);
};
