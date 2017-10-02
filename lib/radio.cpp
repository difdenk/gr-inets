

#ifndef INCLUDED_RADIO
#define INCLUDED_RADIO

#include "radio.h"
#include <vector>

radio::radio(){
  int node_number = 0;
  std::vector<double> snr_radio;
  std::vector<double> angle_radio;
}

radio::~radio(){}

void radio::insert_element(double element) {
  this->snr_radio.push_back(element);
  this->angle_radio.push_back(element);
}

std::vector<double>::iterator radio::find_max_snr(){
  return std::max_element(this->snr_radio.begin(), this->snr_radio.end());
}

double radio::find_coresponding_angle(std::vector<double>::iterator it){
  auto index = std::distance(this->snr_radio.begin(), it);
  double corresponding_angle = this->angle_radio[index];
  return corresponding_angle;
}

void radio::set_node_number(int number) {
  if (this-> node_number == 0) {
    this->node_number = number;
  }
  else
    std::cout << "Node number was already set." << '\n';
}

bool radio::check_node_number(int address) {
  if (node_number == address && node_number != 0) {
    return true;
  }
  else
    return false;
}

int radio::get_node_number() {
  return node_number;
}



#endif
