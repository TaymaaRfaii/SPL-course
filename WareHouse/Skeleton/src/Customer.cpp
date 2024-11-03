#include "Customer.h"

Customer:: Customer(int id, const string &name, int locationDistance, int maxOrders) : id(id),name(name), locationDistance(locationDistance), maxOrders(maxOrders),ordersId(0){}
const std::string& Customer::getName() const{
    return name;
}
Customer::~Customer() {} 
int Customer:: getId() const{
    return id;
}
int Customer::getCustomerDistance() const{
    return locationDistance;
}
int Customer:: getMaxOrders() const{ //Returns maxOrders
    return maxOrders;
}
int Customer:: getNumOrders() const{//Returns num of orders the customer has made so far
    return ordersId.size();
} 
bool Customer:: canMakeOrder() const{ //Returns true if the customer didn't reach max orders
    return getNumOrders() < maxOrders;
}
const std::vector<int>& Customer:: getOrdersIds() const{
    return ordersId;
}
int Customer:: addOrder(int orderId){ //return OrderId if order was added successfully, -1 otherwise
    if(canMakeOrder()){
        ordersId.push_back(orderId);
        return orderId;
        }
    return -1;   
}
int Customer::getNumOrdersLeft() const{
    return (maxOrders - ordersId.size());
}
//SoldierCustomer Class
SoldierCustomer::SoldierCustomer(int id, const string &name, int locationDistance, int maxOrders): Customer(id, name, locationDistance, maxOrders){}
SoldierCustomer* SoldierCustomer::clone() const{
    return new SoldierCustomer(*this);
}
//CivilianCustomer Cladd
CivilianCustomer::CivilianCustomer(int id, const string &name, int locationDistance, int maxOrders):Customer(id, name, locationDistance, maxOrders){}
CivilianCustomer* CivilianCustomer::clone() const{
    return new CivilianCustomer(*this);
}
