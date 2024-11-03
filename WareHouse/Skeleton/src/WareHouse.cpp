#include "WareHouse.h"
#include <iostream>
#include <fstream>
#include <sstream> // For istringstream
#include <vector>
#include <algorithm>


// Rule of 5
WareHouse::~WareHouse()
{ // Destructor
    clearMemory();
}
WareHouse::WareHouse(const WareHouse &other) : // copy constructor
  volunteers(), isOpen(other.isOpen), actionsLog(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter)
{
    for (Customer *customer : other.customers)
    { // copy customers
        customers.push_back(customer->clone());
    }
    // Copy volunteers
    for (int i = 0; i < volunteerCounter; ++i)
    {
        volunteers.push_back(other.volunteers[i]->clone());
    }
    // copy orders (pending, in process, and completed)
    for (Order *order : other.pendingOrders)
    {
        pendingOrders.push_back(new Order(*order));
    }
    for (Order *order : other.inProcessOrders)
    {
        inProcessOrders.push_back(new Order(*order));
    }
    for (Order *order : other.completedOrders)
    {
        completedOrders.push_back(new Order(*order));
    }
    // copy actions log
    for (size_t i = 0; i < other.actionsLog.size(); ++i)
    {
        actionsLog.push_back(other.actionsLog[i]->clone());
    }
}
WareHouse &WareHouse::operator=(const WareHouse &other)
{ // Copy Assignment Operator
    if (this != &other){
        clearMemory(); // remove the existing data
        // copy data
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
        for (Customer *customer : other.customers)
        {
            customers.push_back(customer->clone());
        }
        for (Volunteer *volunteer : other.volunteers)
        {
            volunteers.push_back(volunteer->clone());
        }
        // copy orders (pending, in process, and completed)
        for (Order *order : other.pendingOrders)
        {
            pendingOrders.push_back(new Order(*order));
        }
        for (Order *order : other.inProcessOrders)
        {
            inProcessOrders.push_back(new Order(*order));
        }
        for (Order *order : other.completedOrders)
        {
            completedOrders.push_back(new Order(*order));
        }
        for (BaseAction *action : other.actionsLog)
        {
            actionsLog.push_back(action->clone());
        }
    }    
    return *this;
}
WareHouse &WareHouse::operator=(WareHouse &&other)
{ // Move Assignment Operator
    if (this != &other)
    {
        clearMemory();
        stealResources(std::move(other));
    }
    return *this;
}

// Move Constructor
WareHouse::WareHouse(WareHouse &&other) : volunteers(other.volunteers), isOpen(other.isOpen), actionsLog(other.actionsLog), pendingOrders(other.pendingOrders), inProcessOrders(other.inProcessOrders), completedOrders(other.completedOrders), customers(other.customers), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter)
{
    stealResources(std::move(other));
}
WareHouse::WareHouse(const string &configFilePath)
    : volunteers(), isOpen(false), actionsLog(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), customerCounter(0), volunteerCounter(0), orderCounter(0)
{
    std::fstream file(configFilePath); // Open the config file
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open configuration file: " << configFilePath << std::endl;
        return; // Handle the error
    }
    string line;
    while (getline(file, line))
    {
        std::istringstream iss(line);
        string type;
        iss >> type;
        if(type == "customer"){
            string name, role;
            int num1, num2;
            iss >> name >> role >> num1 >> num2;
            if(role == "soldier"){
                customers.push_back(new SoldierCustomer(customerCounter, name, num1, num2));
                customerCounter++;
            } else if(role == "civilian"){
                customers.push_back(new CivilianCustomer(customerCounter, name, num1, num2));
                customerCounter++;
            }
        } else if(type == "volunteer"){
            string name, role;
            int num1, num2, num3;
            iss >> name >> role >> num1 >> num2 >> num3;
            if(role == "collector"){
                volunteers.push_back(new CollectorVolunteer(volunteerCounter, name, num1));
                volunteerCounter++;
            } else if(role == "limited_collector"){
                volunteers.push_back(new LimitedCollectorVolunteer(volunteerCounter, name, num1, num2));
                volunteerCounter++;
            } else if(role == "driver"){
                volunteers.push_back(new DriverVolunteer(volunteerCounter, name, num1, num2));
                volunteerCounter++;
            } else if(role == "limited_driver"){
                volunteers.push_back(new LimitedDriverVolunteer(volunteerCounter, name, num1, num2, num3));
                volunteerCounter++;
            }
        }
    }
}
void WareHouse::start()
{
    isOpen = true;
    std::cout << "Warehouse is open!" << std::endl;
    std::string line;
    while (isOpen && std::getline(std::cin,line))
    {
        std::istringstream iss(line);
        std::string input;
        iss >> input;
        if (input == "customer")
        {
            std::string name, type;
            int distance, maxOrders;
            iss >> name >> type >> distance >> maxOrders;
            AddCustomer addCustomer(name, type, distance, maxOrders);
            addCustomer.act(*this);
        }
        else if (input == "volunteerStatus")
        {
            int volunteerId;
            iss  >> volunteerId;
            PrintVolunteerStatus printVolunteerStatus(volunteerId);
            printVolunteerStatus.act(*this);
        }
        else if (input == "order")
        {
            int customerId;
            iss  >> customerId;
            AddOrder addOrder(customerId);
            addOrder.act(*this);
        }
        else if (input == "orderStatus")
        {
            int orderId;
            iss >> orderId;
            PrintOrderStatus printOrderStatus(orderId);
            printOrderStatus.act(*this);
        }
        else if (input == "customerStatus")
        {
            int customerId;
            iss  >> customerId;
            PrintCustomerStatus printCustomerStatus(customerId);
            printCustomerStatus.act(*this);
        }
        else if (input == "step")
        {
            int numOfSteps;
            iss  >> numOfSteps;
            SimulateStep simulateStep(numOfSteps);
            simulateStep.act(*this);
        }
        else if (input == "log")
        {
            PrintActionsLog printActionsLog;
            printActionsLog.act(*this);
        }
    
        else if (input == "restore")
        {
            RestoreWareHouse restoreWareHouse;
            restoreWareHouse.act(*this);
        }
         else if (input == "backup")
        {
            BackupWareHouse backupWareHouse;
            backupWareHouse.act(*this);
        }
        else if (input == "close")
        {
            Close close;
            close.act(*this);
        }
        else
        {
            std::cerr << "Error: Invalid input" << std::endl;
        }
    }
}
void WareHouse::addOrder(Order *order)
{
    std::cout << "added order " << order->getId() << std::endl;
    pendingOrders.push_back(order);
}
void WareHouse::addAction(BaseAction *action)
{
    actionsLog.push_back(action);
}
Customer &WareHouse::getCustomer(int customerId) const
{
    for (Customer *customer : customers){
        if (customer->getId() == customerId){
            return *customer;
        }
    }
    Customer *c = new SoldierCustomer(-1, "fail", -1, -1);
    return *c;
}
Volunteer &WareHouse::getVolunteer(int volunteerId) const
{
    for (Volunteer *volunteer : volunteers)
    {
        if (volunteer->getId() == volunteerId)
        {
            return *volunteer;
        }
    }
    // case when the volunteer is not found
    Volunteer *c = new CollectorVolunteer(-1, "fail", -1);
    return *c;
}

Order &WareHouse::getOrder(int orderId) const
{
    for (Order *order : pendingOrders)
    {
        if (order->getId() == orderId)
        {
            return *order;
        }
    }

    for (Order *order : inProcessOrders)
    {
        if (order->getId() == orderId)
        {
            return *order;
        }
    }

    for (Order *order : completedOrders)
    {
        if (order->getId() == orderId)
        {
            return *order;
        }
    }
    // case when the order is not found
    Order *c = new Order(-1, -1, -1);
    return *c;
}

const vector<BaseAction *> &WareHouse::getActions() const{
    return actionsLog;
}

void WareHouse::close(){
    for (Order *order : pendingOrders){ 
        printFinal(*order);
    }
    for (Order *order : inProcessOrders){
        printFinal(*order);

    }
    for (Order *order : completedOrders){
        printFinal(*order);
    }
    isOpen = false;
}

void WareHouse::printFinal(Order& order) const{
        std::cout << "OrderID: " << std::to_string(order.getId()) << " , CustomerID: " << std::to_string(order.getCustomerId())
                  << " , OrderStatus: " << order.toString() << std::endl;
}
void WareHouse::open()
{
    isOpen = true;
}
// helper fuctions implementation:
void WareHouse::clearMemory(){
    // release the memory occupied by each dynamically allocated object in the vectors
    for (BaseAction *action : actionsLog){
        if (action){
            delete action;
            action = nullptr;
        }
    }
    for (Volunteer *volunteer : volunteers){
        if (volunteer){
            delete volunteer;
        }
    }
    for (Order *order : pendingOrders){
        if (order){
            delete order;
            order = nullptr;
        }
    }
    for (Order *order : inProcessOrders){
        if (order)
        {
            delete order;
            order = nullptr;
        }
    }
    for (Order *order : completedOrders){
        if (order)
        {
            delete order;
            order = nullptr;
        }
    }
    for (Customer *customer : customers){
        if (customer)
        {
            delete customer;
            customer = nullptr;
        }
    }
    // After deleting the objects, we calls clear() on each vector to remove all elements
    actionsLog.clear();
    volunteers.clear();
    pendingOrders.clear();
    inProcessOrders.clear();
    completedOrders.clear();
    customers.clear();
}

// Helper function to steal resources from the source object
void WareHouse::stealResources(WareHouse &&other){
    // Move data from the source to the current object
    isOpen = other.isOpen;
    customerCounter = other.customerCounter;
    volunteerCounter = other.volunteerCounter;
    orderCounter = other.orderCounter;
    customers = std::move(other.customers);   // Move customers
    volunteers = std::move(other.volunteers); // Move volunteers
    // Move orders
    pendingOrders = std::move(other.pendingOrders);
    inProcessOrders = std::move(other.inProcessOrders);
    completedOrders = std::move(other.completedOrders);
    actionsLog = std::move(other.actionsLog); // Move actions log

    other.isOpen = false;
    other.customerCounter = 0;
    other.volunteerCounter = 0;
    other.orderCounter = 0;
}
// Find the highest existing orderID and return the next one
// the purpose of "getNextOrdreId()" is to generate a unique orderId for a new order
int WareHouse::getNextOrderId(){
    int temp = orderCounter;
    orderCounter++;
    return temp;
}
int WareHouse::getNextCustomerId() const{
    int maxOrderId = 0;
    for (Customer *customer : customers)
    {
        maxOrderId = std::max(maxOrderId, customer->getId());
    }
    return maxOrderId + 1;
}
void WareHouse::addCustomer(Customer *customer){
    customers.push_back(customer);
}

vector<Order *> &WareHouse::getPendingOrders(){
    return pendingOrders;
}
vector<Order *> &WareHouse::getInProcessOrders(){
    return inProcessOrders;
}
vector<Order *> &WareHouse::getCompletedOrders(){
    return completedOrders;
}
bool WareHouse::volunteerExists(int volunteerId) const{
    for (const Volunteer *volunteer : volunteers)
    {
        if (volunteer->getId() == volunteerId)
        {
            return true;
        }
    }
    return false;
}
vector<Volunteer*>& WareHouse::getVolunteers(){
    return volunteers;
}