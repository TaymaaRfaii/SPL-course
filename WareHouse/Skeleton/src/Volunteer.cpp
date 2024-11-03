#include "Volunteer.h"
Volunteer::Volunteer(int id, const string &name):completedOrderId(NO_ORDER),activeOrderId(NO_ORDER),id(id), name(name), type(Volunteer::getType()){}
int Volunteer:: getId() const{
    return id;
}
Volunteer::~Volunteer(){}
const std::string& Volunteer::getName() const{
    return name;
}
int Volunteer:: getActiveOrderId() const{
    return activeOrderId;
}
int Volunteer::getCompletedOrderId() const{
    return completedOrderId;
}
bool Volunteer:: isBusy() const{ // Signal whether the volunteer is currently processing an order    
    return activeOrderId!=NO_ORDER;
}    
void Volunteer::setCompletedOrderId(int orderId){
    completedOrderId = orderId;
}
VolunteerType  Volunteer::getType(){
   return type;
}
//.......................class CollectorVolunteer.............................................................................................................
CollectorVolunteer:: CollectorVolunteer(int id, const string &name, int coolDown): Volunteer(id,name),coolDown(coolDown), timeLeft(-1) {}
CollectorVolunteer* CollectorVolunteer::clone() const{
    return new CollectorVolunteer(*this);
}
void CollectorVolunteer:: step(){
    if(decreaseCoolDown()){
        completedOrderId=activeOrderId;
        activeOrderId=NO_ORDER;
    }
}

int CollectorVolunteer:: getCoolDown() const{
    return coolDown;
}
int CollectorVolunteer:: getTimeLeft() const{
    return timeLeft;
}
bool CollectorVolunteer:: decreaseCoolDown(){//Decrease timeLeft by 1,return true if timeLeft=0,false otherwise
    if(timeLeft>0){
        timeLeft--;
        if(timeLeft==0){
            return true; 
        }    
    }     
    return false;  
}
bool CollectorVolunteer:: hasOrdersLeft() const{
    return true; //Always true for CollectorVolunteer
}
bool CollectorVolunteer::canTakeOrder(const Order &order) const{
    return ( order.getStatus()==OrderStatus::PENDING && !isBusy()) ;
}
void CollectorVolunteer:: acceptOrder(const Order &order){
    if (canTakeOrder(order)){
        activeOrderId=order.getId();
        timeLeft=coolDown;
    }
}
void CollectorVolunteer::setTimeLeft(int timeLeft){
    this->timeLeft=timeLeft;
}
string CollectorVolunteer::toString() const{
    string result = "VolunteerID: " + std::to_string(getId()) + "\n";
    if(isBusy()){
        result += "isBusy: True\n";
        result += "OrderID: " + std::to_string(getActiveOrderId()) + "\n";
        result += "TimeLeft: " + std::to_string(getTimeLeft()) + "\n";

    }
    else {
        result += "isBusy: False\n";
        result += "OrderID: None\n";
        result += "TimeLeft: None\n";
        result +="OrdersLeft: No Limit";
    }
    return result;
}
//...........................class LimitedCollectorVolunteer...............................................................................

LimitedCollectorVolunteer:: LimitedCollectorVolunteer(int id, const string &name, int coolDown ,int maxOrders)
       : CollectorVolunteer(id, name, coolDown), maxOrders(maxOrders),ordersLeft(maxOrders){}
LimitedCollectorVolunteer* LimitedCollectorVolunteer::clone() const{
    return new LimitedCollectorVolunteer(*this);
} 
bool LimitedCollectorVolunteer:: hasOrdersLeft() const {
    return ordersLeft>0;
}
bool LimitedCollectorVolunteer:: canTakeOrder(const Order &order) const{ 
    return  (order.getStatus()==OrderStatus::PENDING && !isBusy() && hasOrdersLeft());
}
void LimitedCollectorVolunteer:: acceptOrder(const Order &order) {
    if (canTakeOrder(order)){
        activeOrderId=order.getId();
        setTimeLeft(getCoolDown());
        ordersLeft--;
    }
}
int LimitedCollectorVolunteer:: getMaxOrders() const{
    return maxOrders;
}
int LimitedCollectorVolunteer:: getNumOrdersLeft() const{
    return ordersLeft;
}
string LimitedCollectorVolunteer:: toString() const{
    string result = "VolunteerID: " + std::to_string(getId()) + "\n";
    if(isBusy()){
        result += "isBusy: True\n";
        result += "OrderID: " + std::to_string(getActiveOrderId()) + "\n";
        result += "TimeLeft: " + std::to_string(getTimeLeft()) + "\n";

    }
    else {
        result += "isBusy: False\n";
        result += "OrderID: None\n";
        result += "TimeLeft: None\n";
    }
    result += "OrdersLeft: " + std::to_string(getNumOrdersLeft()) + "\n";
    return result;
}
    
//..............................class DriverVolunteer..................................................................................................

DriverVolunteer::DriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep): 
    Volunteer(id, name),maxDistance(maxDistance), distancePerStep(distancePerStep),distanceLeft(0){}
DriverVolunteer* DriverVolunteer::clone() const{
    return new DriverVolunteer(*this);
}
int DriverVolunteer::getDistanceLeft() const{
    return distanceLeft;
}
int DriverVolunteer::getMaxDistance() const{
    return maxDistance;
}
int DriverVolunteer::getDistancePerStep() const{
    return distancePerStep;
}
bool DriverVolunteer::decreaseDistanceLeft(){ //Decrease distanceLeft by distancePerStep,return true if distanceLeft<=0,false otherwise
    if(distanceLeft > 0){
        setDistanceLeft(distanceLeft - distancePerStep);
    }
    else{
       setDistanceLeft(0);
    }
    return distanceLeft<=0;
}

bool DriverVolunteer::hasOrdersLeft() const{
    return true; // Always true for DriverVolunteer
}
bool DriverVolunteer::canTakeOrder(const Order &order) const{// Signal if the volunteer is not busy and the order is within the maxDistance
    return !isBusy() && order.getDistance() <= maxDistance && order.getStatus()==OrderStatus::COLLECTING;
} 
void DriverVolunteer::acceptOrder(const Order &order){ // Assign distanceLeft to order's distance
    if(canTakeOrder(order)){
        activeOrderId=order.getId();
        distanceLeft = order.getDistance();
    }
}

void DriverVolunteer::step() {
    if(decreaseDistanceLeft()){
        completedOrderId=activeOrderId;
        activeOrderId= NO_ORDER;
    }
}
void DriverVolunteer::setDistanceLeft(int distance){
    distanceLeft=distance;
}

string DriverVolunteer::toString() const{
    string result = "VolunteerID: " + std::to_string(getId()) + "\n";
    if(isBusy()){
        result += "isBusy: True\n";
        result += "OrderID: " + std::to_string(getActiveOrderId()) + "\n";
        result += "TimeLeft: " + std::to_string(getDistanceLeft()) + "\n";
    }
    else {
        result += "isBusy: False\n";
        result += "OrderID: None\n";
        result += "TimeLeft: None\n";
        result +="OrdersLeft: No Limit";        
    }
    return result;
}
//.................................class LimitedDriverVolunteer..........................................................................................
LimitedDriverVolunteer::LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep,int maxOrders):
    DriverVolunteer(id,name,maxDistance,distancePerStep), maxOrders(maxOrders),ordersLeft(maxOrders){}
LimitedDriverVolunteer* LimitedDriverVolunteer::clone() const{
    return new LimitedDriverVolunteer(*this);
}
int LimitedDriverVolunteer::getMaxOrders() const{
    return maxOrders;
}
int LimitedDriverVolunteer:: getNumOrdersLeft() const{
    return ordersLeft;
}
bool LimitedDriverVolunteer::hasOrdersLeft() const{
    return ordersLeft>0;
}
bool LimitedDriverVolunteer::canTakeOrder(const Order &order) const { // Signal if the volunteer is not busy, the order is within the maxDistance.
    return (!isBusy() && hasOrdersLeft() && order.getDistance() <= maxDistance  && order.getStatus()==OrderStatus::COLLECTING);
}
void LimitedDriverVolunteer::acceptOrder(const Order &order) { // Assign distanceLeft to order's distance and decrease ordersLeft
        if(canTakeOrder(order)){
            activeOrderId=order.getId();
            setDistanceLeft (order.getDistance());
            ordersLeft--;
    }
}
string LimitedDriverVolunteer::toString() const{
    string result = "VolunteerID: " + std::to_string(getId()) + "\n";
    if(isBusy()){
        result += "isBusy: True\n";
        result += "OrderID: " + std::to_string(getActiveOrderId()) + "\n";
        result += "TimeLeft: " + std::to_string(getDistanceLeft()) + "\n";
    }
    else {
        result += "isBusy: False\n";
        result += "OrderID: None\n";
        result += "TimeLeft: None\n";
    }
    result += "OrdersLeft: " + std::to_string(getNumOrdersLeft()) + "\n";
    return result;
}

