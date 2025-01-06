#include "RegisterList.h"

#include <iostream>

#include "Register.h"
using namespace std;

RegisterList::RegisterList() {
  head = nullptr;
  size = 0;
}

RegisterList::~RegisterList() {
  // Delete all registers in the list
    Register* temp = head;
    while (temp != nullptr) {
        Register* next = temp->get_next();
        delete temp;
        temp = next;
    }
}

Register* RegisterList::get_head() { 
    return head;
 }

int RegisterList::get_size() { 
  // return number of register
    return size;
}

Register* RegisterList::get_min_items_register() { //this is for mode = multiple
  // loop all registers to find the register with least number of items
    // if all registers are empty, return nullptr
    bool firstRound = true;
    int minItems = 0;
    Register* minRegister = nullptr;
    Register* temp = head;

    while(temp != nullptr) {
      int numItems = temp -> get_queue_list() -> get_items();
      if (numItems == 0) { //this means that the register is empty
        return temp;
      }
      if (firstRound || minItems > numItems) {
        minItems = numItems;
        minRegister = temp;
        firstRound = false;
      }
      temp = temp -> get_next();
    }
    return minRegister;
}

Register* RegisterList::get_free_register() {
  // return the register with no customers
  // if all registers are occupied, return nullptr

    Register* temp = head;
    while (temp != nullptr) {
        if (temp->get_queue_list()->get_head() == nullptr) {
            return temp;
        }
        temp = temp->get_next();
    }
    return nullptr;
}

void RegisterList::enqueue(Register* newRegister) {
  // a register is placed at the end of the queue
  // if the register's list is empty, the register becomes the head
  // Assume the next of the newRegister is set to null
  // You will have to increment size 
  if (head == nullptr) {
    head = newRegister;
    size++;
    return;
  } 
  Register* temp = head;
  while (temp->get_next() != nullptr) {
    temp = temp->get_next();
  }

  temp->set_next(newRegister);
  size++;
}

bool RegisterList::foundRegister(int ID) {
  // look for a register with the given ID
  // return true if found, false otherwise
  Register* temp = head;
  while (temp != nullptr) {
    if (temp -> get_ID() == ID) {
      return true;
    }
    else {
        temp = temp -> get_next();
    }
  }
  return false;
}

Register* RegisterList::dequeue(int ID) {
  // dequeue the register with given ID

  // return the dequeued register
  // return nullptr if register was not found
    Register* temp = head;
    Register* prev = nullptr;
    while (temp != nullptr) {
        if (temp->get_ID() == ID) {
            if (prev == nullptr) { // this means that the register we are deleting is the head
                head = temp->get_next();
            } else { // if not at the head
                prev->set_next(temp->get_next());
            }
            size--;

            return temp;
        }
        prev = temp;
        temp = temp->get_next();
    }
    return nullptr;
}

Register* RegisterList::calculateMinDepartTimeRegister(double expTimeElapsed) {
  // return the register with minimum time of departure of its customer
  // if all registers are free, return nullptr
  // if all registers are occupied, check for the minimum departure time and if the time is less than expTimeElapsed, return the register
  //if the time is greater than expTimeElapsed, return nullptr
  bool firstRound = true;
  bool allRegistersEmpty = true;

  Register* minRegister = nullptr;
  double minDepartTime = 0;
  Register* temp = head;

  while (temp != nullptr) {
    double departTime = temp -> calculateDepartTime();
    if (departTime == -1) { // this means that a register is empty and we return null ptr to indicate that there is no customer to depart
      if (allRegistersEmpty && temp -> get_next() == nullptr) { // only return -1 if all registers are empty
        return nullptr;
      }
    }
    if (firstRound || minDepartTime > departTime) {
      if (departTime != -1) {
        minDepartTime = departTime;
        minRegister = temp;
        allRegistersEmpty = false;
        firstRound = false;
      }
      
      temp = temp->get_next();
    }
  }
  //this means we have looped through all the registerList and either found a customer to depart or not depart
  if (minDepartTime <= expTimeElapsed) {
    return minRegister;
  }
  else {
    return nullptr;
  }
}

void RegisterList::print() {
  Register* temp = head;
  while (temp != nullptr) {
    temp->print();
    temp = temp->get_next();
  }
}