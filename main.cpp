#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "Customer.h"
#include "QueueList.h"
#include "Register.h"
#include "RegisterList.h"

using namespace std;

// Function Declarations:

// Set mode of the simulation
string getMode();

// Register
void parseRegisterAction(stringstream &lineStream, string mode);
void openRegister(stringstream &lineStream, string mode);  // register opens (it is upto customers to join)
void closeRegister(stringstream &lineStream, string mode);  // register closes 

// Customer
void addCustomer(stringstream &lineStream, string mode);  // customer wants to join


// Helper functions
bool getInt(stringstream &lineStream, int &iValue);
bool getDouble(stringstream &lineStream, double &dValue);
bool foundMoreArgs(stringstream &lineStream);
double get_max_wait_time();
double get_avg_wait_time();
double get_std_dev_wait_time();
void print_doneList();


// Global variables
RegisterList *registerList; // holding the list of registers
QueueList *doneList; // holding the list of customers served
QueueList *singleQueue; // holding customers in a single virtual queue
double expTimeElapsed; // time elapsed since the beginning of the simulation

// List of commands:
// To open a register
// register open <ID> <secPerItem> <setupTime> <timeElapsed>
// To close register
// register close <ID> <timeElapsed>
// To add a customer
// customer <items> <timeElapsed>

int main() {
  registerList = new RegisterList();
  doneList = new QueueList();
  singleQueue = new QueueList();
  expTimeElapsed = 0;

  // Set mode by the user
  string mode = getMode();

  string line;
  string command;

  cout << "> ";  // Prompt for input
  getline(cin, line);

  while (!cin.eof()) {
    stringstream lineStream(line);
    lineStream >> command;
    if (command == "register") {
      parseRegisterAction(lineStream, mode);
    } else if (command == "customer") {
      addCustomer(lineStream, mode);
    } else {
      cout << "Invalid operation" << endl;
      // print_doneList();
    }
    cout << "> ";  // Prompt for input
    getline(cin, line);
  }
  //print the statistic of the wait time of the customers
  cout << "Finished at time " << expTimeElapsed << endl;
  cout << "Statistics:" << endl;
  cout << "Maximum wait time: " << get_max_wait_time() << endl;
  cout << "Average wait time: " << get_avg_wait_time() << endl;
  cout << "Standard Deviation of wait time: " << get_std_dev_wait_time() << endl;
  
  // You have to make sure all dynamically allocated memory is freed 
  // before return 0

  //
  // if (mode == "single") {
  //   delete singleQueue;
  //   singleQueue = nullptr;
  //   delete registerList;
  //   registerList = nullptr;
  //   delete doneList;
  //   doneList = nullptr;
  // }

  // if (mode == "multiple") {
  //   delete registerList;
  //   registerList = nullptr;
  //   delete doneList;
  //   doneList = nullptr;
  // }

  return 0;
}
//test function
void print_doneList() {
  Customer* temp = doneList->get_head();
  
  // Check if doneList is empty
  if (temp == nullptr) {
    cout << "doneList is empty." << endl;
    return;
  }

  cout << "Contents of doneList:" << endl;
  
  while (temp != nullptr) {
    // Print customer information (e.g., ID, departure and arrival times)
    cout << "Customer ID: " << temp ->get_numOfItems() 
         << ", Arrival Time: " << temp->get_arrivalTime() 
         << ", Departure Time: " << temp->get_departureTime() << endl;
    
    temp = temp->get_next();  // Move to the next customer
  }
}

//function to calculate the max wait time of the customer
double get_max_wait_time() {
  double maxWaitTime = 0;
  Customer* temp = doneList->get_head();
  
  while (temp != nullptr) {
    double waitTime = temp->get_departureTime() - temp->get_arrivalTime();
    
    // Ignore negative wait times (optional, based on requirements)
    if (waitTime > 0 && waitTime > maxWaitTime) {
      maxWaitTime = waitTime;
    }
    
    temp = temp->get_next();
  }
  
  return maxWaitTime;
}


double get_avg_wait_time() {
  double totalWaitTime = 0;
  double numCustomers = 0;
  Customer* temp = doneList -> get_head();
  while (temp != nullptr) {
    totalWaitTime += temp -> get_departureTime() - temp -> get_arrivalTime();
    numCustomers++;
    temp = temp -> get_next();
  }
  return totalWaitTime / numCustomers;
}

double get_std_dev_wait_time() {
  double avgWaitTime = get_avg_wait_time();
  double total = 0;
  double numCustomers = 0;
  Customer* temp = doneList -> get_head();
  while (temp != nullptr) {
    total += pow((temp -> get_departureTime() - temp -> get_arrivalTime()) - avgWaitTime, 2);
    numCustomers++;
    temp = temp -> get_next();
  }
  return sqrt(total / numCustomers);
}

string getMode() {
  string mode;
  cout << "Welcome to ECE 244 Grocery Store Queue Simulation!" << endl;
  cout << "Enter \"single\" if you want to simulate a single queue or "
          "\"multiple\" to "
          "simulate multiple queues: \n> ";
  getline(cin, mode);

  if (mode == "single") {
    cout << "Simulating a single queue ..." << endl;
  } else if (mode == "multiple") {
    cout << "Simulating multiple queues ..." << endl;
  }

  return mode;
}

void addCustomer(stringstream &lineStream, string mode) {
  int items;
  double timeElapsed;
  if (!getInt(lineStream, items) || !getDouble(lineStream, timeElapsed)) {
    cout << "Error: too few arguments." << endl;
    return;
  }
  if (foundMoreArgs(lineStream)) {
    cout << "Error: too many arguments." << endl;
    return;
  }
  // Depending on the mode of the simulation (single or multiple),
  // add the customer to the single queue or to the register with
  // fewest items
  //if there is a customer at the register for single mode, then it is remains in the queueList


  expTimeElapsed += timeElapsed;
  if (mode == "single") {
    //need to check if the customer's at the registers seconds is overtime
    //if it is, then the customer is removed from the register

    //test while loop
    while (registerList->calculateMinDepartTimeRegister(expTimeElapsed) != nullptr) {
      Register *minDepartTimeRegister = registerList->calculateMinDepartTimeRegister(expTimeElapsed);
      cout << "Departed a customer at register ID " << minDepartTimeRegister->get_ID() << " at " << minDepartTimeRegister->calculateDepartTime() << endl;
      minDepartTimeRegister->get_queue_list()->get_head()->set_departureTime(minDepartTimeRegister->calculateDepartTime());
      minDepartTimeRegister->set_availableTime(expTimeElapsed);
      minDepartTimeRegister->departCustomer(doneList);
    }

    //if there is no leaving customer we can now check the new customer in, then the customer is added to the register
      Customer *customer = new Customer(timeElapsed, items);
      customer->set_arrivalTime(expTimeElapsed);
      cout << "A customer entered" << endl;

      //if there is no customer at the register, then the customer is added to the register
      //if there is no free register, then the customer is added to the singleQueue
      if (registerList -> get_free_register() == nullptr) {
        singleQueue -> enqueue(customer);
        cout << "No free registers" << endl;
      }
      else {
        Register *freeRegister = registerList -> get_free_register();
        freeRegister -> get_queue_list() -> enqueue(customer);
        cout << "Queued a customer with free register " << freeRegister -> get_ID() << endl;
      }
  }

  else if (mode == "multiple") {
    //checking for customers that need to depart
    while (registerList->calculateMinDepartTimeRegister(expTimeElapsed) != nullptr) {
      Register *minDepartTimeRegister = registerList->calculateMinDepartTimeRegister(expTimeElapsed);
      cout << "Departed a customer at register ID " << minDepartTimeRegister->get_ID() << " at " << minDepartTimeRegister->calculateDepartTime() << endl;
      minDepartTimeRegister->get_queue_list()->get_head()->set_departureTime(minDepartTimeRegister->calculateDepartTime());
      minDepartTimeRegister->set_availableTime(expTimeElapsed);
      minDepartTimeRegister->departCustomer(doneList);
      
    }
    //if there is no leaving customer we can now check the new customer in, then the customer is added to the register with the least number of items
    Customer *customer = new Customer(timeElapsed, items);
    customer->set_arrivalTime(expTimeElapsed);
    cout << "A customer entered" << endl;

    // the customer should be added to the register with the least number of items
    //we have a function that returns the register with the least number of items

    Register *temp = registerList -> get_head(); // do we need ot check if the head is null?
    //check if there are any customers in the register
    if (registerList -> get_free_register() != nullptr) { // there is a free register
      Register *freeRegister = registerList -> get_free_register();
      freeRegister -> get_queue_list() -> enqueue(customer);
      cout << "Queued a customer with quickest register " << freeRegister -> get_ID() << endl;
      return;
    }
    else {
    //if there is no open register, then the customer is added to the register with the least number of items
    Register* leastitemsregister = registerList -> get_min_items_register();
    leastitemsregister -> get_queue_list() -> enqueue(customer);
    cout << "Queued a customer with quickest register " << leastitemsregister -> get_ID() << endl;
    }
  }
}

void parseRegisterAction(stringstream &lineStream, string mode) {
  string operation;
  lineStream >> operation;
  if (operation == "open") {
    openRegister(lineStream, mode);
  } else if (operation == "close") {
    closeRegister(lineStream, mode);
  } else {
    cout << "Invalid operation" << endl;
  }
}

void openRegister(stringstream &lineStream, string mode) {
  int ID;
  double secPerItem, setupTime, timeElapsed;

  // convert strings to int and double
  if (!getInt(lineStream, ID) || !getDouble(lineStream, secPerItem) ||
      !getDouble(lineStream, setupTime) ||
      !getDouble(lineStream, timeElapsed)) {
    cout << "Error: too few arguments." << endl;
    return;
  }
  if (foundMoreArgs(lineStream)) {
    cout << "Error: too many arguments" << endl;
    return;
  }
  // Check if the register is already open
  // If it's open, print an error message
  // Otherwise, open the register
  // If we were simulating a single queue, 
  // and there were customers in line, then 
  // assign a customer to the new register

  if (registerList->foundRegister(ID)) {
    cout << "Error: register " << ID << " is already open." << endl;
    return;
  }

  expTimeElapsed += timeElapsed;
  
  //check if customer needs to depart
  while (registerList->calculateMinDepartTimeRegister(expTimeElapsed) != nullptr) {
    Register *minDepartTimeRegister = registerList->calculateMinDepartTimeRegister(expTimeElapsed);
    cout << "Departed a customer at register ID " << minDepartTimeRegister->get_ID() << " at " << minDepartTimeRegister->calculateDepartTime() << endl;
    minDepartTimeRegister->get_queue_list()->get_head()->set_departureTime(minDepartTimeRegister->calculateDepartTime());
    minDepartTimeRegister->set_availableTime(expTimeElapsed);
    minDepartTimeRegister->departCustomer(doneList);
  }
  
  // we can open the register now
  Register* newRegister = new Register(ID, secPerItem, setupTime, timeElapsed);
  registerList->enqueue(newRegister);
  //set the available time of the register to the time elapsed
  newRegister -> set_availableTime(expTimeElapsed);

  //print statement for opened register with ID
  cout << "Opened register " << ID << endl;


  if (mode == "single") {
    // assign a customer to the new register
    // if there are customers in line in QueueList

    if (singleQueue -> get_head() != nullptr) {
      Customer* customer = singleQueue->dequeue();
      newRegister->get_queue_list()->enqueue(customer);
      cout << "Queued a customer with free register " << newRegister -> get_ID() << endl;
    }
  }

}

void closeRegister(stringstream &lineStream, string mode) {
  int ID;
  double timeElapsed;
  
  // convert string to int
  if (!getInt(lineStream, ID) || !getDouble(lineStream, timeElapsed)) {
    cout << "Error: too few arguments." << endl;
    return;
  }
  if (foundMoreArgs(lineStream)) {
    cout << "Error: too many arguments" << endl;
    return;
  }

  expTimeElapsed += timeElapsed;
  // Check if the register is open
  // If it is open dequeue it and free it's memory
  // Otherwise, print an error message 
  
    while (registerList->calculateMinDepartTimeRegister(expTimeElapsed) != nullptr) {
      Register *minDepartTimeRegister = registerList->calculateMinDepartTimeRegister(expTimeElapsed);
      cout << "Departed a customer at register ID " << minDepartTimeRegister->get_ID() << " at " << minDepartTimeRegister->calculateDepartTime() << endl;
      minDepartTimeRegister->get_queue_list()->get_head()->set_departureTime(minDepartTimeRegister->calculateDepartTime());
      minDepartTimeRegister->set_availableTime(expTimeElapsed);
      minDepartTimeRegister->departCustomer(doneList);

      //for single mode, if there are still customers in the singleQueue, then they are added to the register as soon as the customer at register is dequeued
      if (mode == "single") {
        if (singleQueue -> get_head() != nullptr) {
          Customer* customer = singleQueue->dequeue();
          minDepartTimeRegister->get_queue_list()->enqueue(customer);
          cout << "Queued a customer with free register " << minDepartTimeRegister -> get_ID() << endl;
        }
      }
    }


  Register* temp = registerList->get_head();
  Register* prev = nullptr;
  while (temp != nullptr) {
    if (temp->get_ID() == ID) {
      // we found the register
      if (temp->get_queue_list()->get_items() != 0) { // here we need to check if there is still a customer at the register
        cout << "Error: register " << ID << " is not empty" << endl;
        return;
      }
      else {
      // we can close the register now
        registerList->dequeue(ID);
      }

        cout << "Closed register " << ID << endl;
        delete temp;
        temp = nullptr;
        return;

    }
    else { // if the register is not found
      cout << "Error: register " << ID << " is not open" << endl;
      return;
    }
    prev = temp;
    temp = temp->get_next();
  }
  
}

bool getInt(stringstream &lineStream, int &iValue) {
  // Reads an int from the command line
  string command;
  lineStream >> command;
  if (lineStream.fail()) {
    return false;
  }
  iValue = stoi(command);
  return true;
}

bool getDouble(stringstream &lineStream, double &dvalue) {
  // Reads a double from the command line
  string command;
  lineStream >> command;
  if (lineStream.fail()) {
    return false;
  }
  dvalue = stod(command);
  return true;
}

bool foundMoreArgs(stringstream &lineStream) {
  string command;
  lineStream >> command;
  if (lineStream.fail()) {
    return false;
  } else {
    return true;
  }
}