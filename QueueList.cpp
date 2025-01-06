#include "QueueList.h"

#include "Customer.h"

QueueList::QueueList() { head = nullptr; }

QueueList::QueueList(Customer* customer) { head = customer; }

QueueList::~QueueList() {  
    // delete all customers in the queue
    Customer* temp = head;
    while (temp != nullptr) {
        Customer* next = temp -> get_next();
        delete temp;
        temp = NULL; //check idk if right
    }
}

Customer* QueueList::get_head() { 
    return head;
 }

void QueueList::enqueue(Customer* customer) {
  // a customer is placed at the end of the queue
  // if the queue is empty, the customer becomes the head

  if (head == nullptr) {
    head = customer;
    return;
  }
    Customer* temp = head;
    while (temp->get_next() != nullptr) {
      temp = temp->get_next();
    }
    temp->set_next(customer);

}

Customer* QueueList::dequeue() {
  // remove a customer from the head of the queue 
  // and return a pointer to it
  if (head == nullptr) {
    return nullptr;
  }

  // need to return pointer to the customer
  Customer* leavingCustomer = head;
  head = head->get_next();
  leavingCustomer->set_next(nullptr);
  return leavingCustomer;
}


int QueueList::get_items() {
  // count total number of items each customer in the queue has
    int totalItems = 0;
    Customer* temp = head;
    while (temp != nullptr) {
        totalItems += temp -> get_numOfItems();
        temp = temp -> get_next();
    }
    return totalItems;
}

void QueueList::print() {
  // print customers in a queue
  Customer* temp = head;
  while (temp != nullptr) {
    temp->print();
    temp = temp->get_next();
  }
}
