#pragma once
#ifndef WareHouse_h
#define WareHouse_h
#include <string>
#include <vector>
#include "Action.h"
#include "Volunteer.h"
#include "Order.h"
#include "Customer.h"

#include <iostream>
#include <fstream>
#include <sstream> // For istringstream

class BaseAction;
class Volunteer;

// Warehouse responsible for Volunteers, Customers Actions, and Orders.
class WareHouse {

    public:
    //Rule Of 5
      ~WareHouse();  // Destructor
       WareHouse(const WareHouse& other); // Copy Constructor 
       WareHouse& operator=(const WareHouse& other);  // Copy Assignment Operator
       WareHouse& operator=(WareHouse&& other);    // Move Assignment Operator
       WareHouse(WareHouse&& other);  // Move Constructor

        WareHouse(const string &configFilePath); 
        void start();
        void addOrder(Order* order);
        void addAction(BaseAction* action);
        Customer &getCustomer(int customerId) const;
        Volunteer &getVolunteer(int volunteerId) const;
        Order &getOrder(int orderId) const;
        const vector<BaseAction*> &getActions() const;
        void close();
        void open();
        void addCustomer(Customer* customer);
        int getNextCustomerId() const;
        int getNextOrderId();
        void printFinal(Order& order) const;
        vector<Volunteer*>& getVolunteers();
        vector<Order*>& getPendingOrders();
        vector<Order*>& getInProcessOrders();
        vector<Order*>& getCompletedOrders();
        Order getOrderById(int orderId);
        bool volunteerExists(int volunteerId) const;
        void clearMemory(); //helper function 
        void stealResources(WareHouse&& other);  //helper function 

    private:
        vector<Volunteer*> volunteers;
        bool isOpen;
        vector<BaseAction*> actionsLog;
        vector<Order*> pendingOrders;
        vector<Order*> inProcessOrders;
        vector<Order*> completedOrders;
        vector<Customer*> customers;
        int customerCounter; //For assigning unique customer IDs
        int volunteerCounter; //For assigning unique volunteer IDs
        int orderCounter; //For assigning unique order IDs

        
};
#endif