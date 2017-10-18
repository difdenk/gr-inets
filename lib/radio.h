
#ifndef INCLUDED_RADIO
#define INCLUDED_RADIO

#include <vector>

class radio {
  private:
    int node_number;
    std::vector<double> snr_radio;
    std::vector<double> angle_radio;
  public:
    radio();
    ~radio();
    void insert_element(double element);
    std::vector<double>::iterator find_max_snr();
    double find_coresponding_angle(std::vector<double>::iterator it);
    void set_node_number(int number);
    bool check_node_number(int address);
    int get_node_number();
};

#endif
