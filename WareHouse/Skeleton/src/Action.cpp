#include "Action.h"
#include "WareHouse.h"
#include <vector>
#include <algorithm>  // Include the algorithm header for std::find
#include <string>
extern WareHouse* backup; 

//class BaseAction
BaseAction::BaseAction(): errorMsg(""),status(ActionStatus::ERROR) {}//constructor
BaseAction::~BaseAction(){}//destructor
ActionStatus BaseAction::getStatus() const{
    return status;
}
void BaseAction::complete(){
    status = ActionStatus:: COMPLETED;
}
void BaseAction::error(string errorMsg){
    status= ActionStatus::ERROR;
    this->errorMsg=errorMsg;
    std:: cout <<"Error: " << errorMsg << std :: endl;
}
string BaseAction:: getErrorMsg() const{
    return errorMsg;
}

//class SimulateStep
SimulateStep::SimulateStep(int numOfSteps):numOfSteps(numOfSteps){}
void SimulateStep::act(WareHouse &wareHouse){
    //Update the state of the warehouse based on the number of steps
    for(int step=0; step<numOfSteps; step++){
        //stage 1:
        processPendingOrders(wareHouse);
        //stage 2:
        decrease(wareHouse); //decrease Time And Distance For Volunteers
        //stage 3:
        processCompletedOrders(wareHouse);
        //stage 4:
        removeFinishedVolunteers(wareHouse);
    }
    complete();
    wareHouse.addAction(this->clone()); 
}

void SimulateStep::processPendingOrders(WareHouse &wareHouse) { //stage1
    vector<Order*> &pending = wareHouse.getPendingOrders();
    vector<Order*> &inProcess = wareHouse.getInProcessOrders();
    vector<Volunteer*> &volunteers = wareHouse.getVolunteers();
    for (auto it = pending.begin(); it != pending.end();) {
        bool Processed = false;
        Order* orderPtr = *it;
        for (Volunteer* volunteerPtr : volunteers) {
            if (volunteerPtr->canTakeOrder(*orderPtr)) {
                volunteerPtr->acceptOrder(*orderPtr);
                if (orderPtr->getStatus() == OrderStatus::PENDING){
                    orderPtr->setStatus(OrderStatus::COLLECTING);
                    orderPtr->setCollectorId(volunteerPtr->getId());
                    inProcess.push_back(orderPtr);
                    it = pending.erase(it);
                    Processed = true;
                    break;    
                } else if ( orderPtr->getStatus() == OrderStatus::COLLECTING){
                    orderPtr->setStatus(OrderStatus::DELIVERING);
                    orderPtr->setDriverId(volunteerPtr->getId());
                    inProcess.push_back(orderPtr);
                    it = pending.erase(it);
                    Processed = true;
                    break;
                }
               
            }
        }
        if (!Processed) {
            ++it; // Move to the next order if not processed
        }
    }
}
void SimulateStep::decrease(WareHouse &wareHouse) {//stage 2
     for(unsigned int i=0; i<wareHouse.getVolunteers().size(); i++){
        if (wareHouse.getVolunteers()[i]->isBusy()){
            wareHouse.getVolunteers()[i]->step();
        }
     }
}
void SimulateStep::processCompletedOrders(WareHouse &wareHouse) {//stage 3
    vector<Order*> &inProcess = wareHouse.getInProcessOrders();
    vector<Order*> &completed = wareHouse.getCompletedOrders();
    vector<Volunteer*> &volunteers = wareHouse.getVolunteers();
    for (Volunteer* volunteer : volunteers) {
        if(volunteer->getCompletedOrderId() != -1){
            int orderId = volunteer->getCompletedOrderId();
            volunteer->setCompletedOrderId(-1);
            bool foundOrder = false;
            auto it = inProcess.begin();
            while(it != inProcess.end()){
                if((*it)->getId() == orderId){
                    foundOrder = true;
                    break;
                }
                it++;
            }
            if(foundOrder){
                OrderStatus status = (*it)->getStatus();
                if (status == OrderStatus::COLLECTING) {
                    wareHouse.getPendingOrders().push_back(*it);
                    inProcess.erase(it);
                } else if (status == OrderStatus::DELIVERING) {
                    (*it)->setStatus(OrderStatus::COMPLETED);
                    completed.push_back(*it);
                    inProcess.erase(it);
                } 
        }
    }
}
}

void SimulateStep::removeFinishedVolunteers(WareHouse &wareHouse) {//stage 4
    vector<Volunteer*> &volunteers = wareHouse.getVolunteers();
    for (auto it = volunteers.begin(); it != volunteers.end();) {
        Volunteer* volunteer = *it;
        if (!volunteer->isBusy() && !volunteer->hasOrdersLeft()) {
            it = volunteers.erase(it);
        } else {
            ++it;
        }
    }
}
std::string SimulateStep::toString() const{
    return "SimulateStep " + std::to_string(numOfSteps) + "COMPLETED";
}
SimulateStep *SimulateStep::clone() const{
    return new SimulateStep(*this);
}

//.....................................class AddOrder...........................................................
AddOrder::AddOrder(int id):customerId(id){}
void AddOrder::act(WareHouse &wareHouse){
    Customer& customer = wareHouse.getCustomer(customerId);
    if(customer.getId() != -1){
        if(customer.canMakeOrder()){
            Order* order= new Order(wareHouse.getNextOrderId(),customerId, customer.getCustomerDistance()); 
            wareHouse.addOrder(order);
            customer.addOrder(order->getId()); // add the orderId to the customer orders
            complete();
        }
        else{
            error("Cannot place this order");
        }
    } else {
        delete &customer;
        error("Customer doesn't exist!");
    }

    wareHouse.addAction(this->clone());
}
string AddOrder::toString() const{
    string result="order";
    result +=std::to_string(customerId);
    if(getStatus() == ActionStatus::COMPLETED){
        result += " COMPLETED"; 
    }else{ result += " ERROR";}
    return result;
}
AddOrder *AddOrder::clone() const{
    return new AddOrder(*this);
}
 
//.........................class AddCustomer......................................................................................
AddCustomer::AddCustomer(const string &customerName, const string &customerType, int distance, int maxOrders)
    : customerName(customerName),customerType(stringToCustomerType(customerType)), distance(distance), maxOrders(maxOrders) {}
CustomerType AddCustomer::stringToCustomerType(const string &type) {
    if (type == "Soldier")
        return CustomerType::Soldier;
    else
        return CustomerType::Civilian;
} 
void AddCustomer::act(WareHouse &wareHouse){
    if (customerType == CustomerType::Soldier) {
        SoldierCustomer* soldierCustomer = new SoldierCustomer(wareHouse.getNextCustomerId(), customerName, distance, maxOrders);
        wareHouse.addCustomer(soldierCustomer);
    }
    else {
        CivilianCustomer* civilianCustomer = new CivilianCustomer(wareHouse.getNextCustomerId(), customerName, distance, maxOrders);
        wareHouse.addCustomer(civilianCustomer);
    }
    complete();
    wareHouse.addAction(this->clone());
}
AddCustomer *AddCustomer::clone() const{
    return new AddCustomer(*this);
}
std::string AddCustomer::toString() const{
    string typeS;
    if (customerType == CustomerType::Soldier){
        typeS= "Soldier";}
    else{
        typeS = "Civilian";}
    string result = "Customer" + typeS +" "+ customerName + " "+ std::to_string(distance)+" " + std::to_string(maxOrders);
    if(BaseAction::getStatus() == ActionStatus::COMPLETED){
        result += " COMPLETED";  }
        else{ result += " ERROR";}
    return result;
}
//.....................class PrintOrderStatus................................................................
PrintOrderStatus::PrintOrderStatus(int id) : orderId(id) {}
void PrintOrderStatus::act(WareHouse &wareHouse){
    Order &order= wareHouse.getOrder(orderId);
    if(order.getId() != -1){
        std::cout << "OrderId: " << order.getId() << std::endl;
        std::cout << "OrderStatus: ";
        if(order.getStatus()== OrderStatus::PENDING){
           std::cout << "PENDING" << std::endl;
           std::cout << "CustomerID: " << order.getCustomerId() <<std::endl;
           std::cout << "Collector: None" << std::endl;
           std::cout << "Driver: None" << std::endl;
        }
        else if(order.getStatus()== OrderStatus::COLLECTING){
           std::cout << "COLLECTING" << std::endl;
           std::cout << "CustomerID: " << order.getCustomerId() <<std::endl;
           std::cout << "Collector: " << order.getCollectorId()<<std::endl;
           std::cout << "Driver: None" << std::endl;
        }
        else if(order.getStatus()== OrderStatus::DELIVERING){
           std::cout << "DELIVERING" << std::endl;
           std::cout << "CustomerID: " << order.getCustomerId() <<std::endl;
           std::cout << "Collector: " << order.getCollectorId()<<std::endl;
           std::cout << "Driver: " << order.getDriverId() <<std::endl;
        }
        else if(order.getStatus()== OrderStatus::COMPLETED){
           std::cout << "COMPLETED" << std::endl;
           std::cout << "CustomerID: " << order.getCustomerId() <<std::endl;
           std::cout << "Collector: " << order.getCollectorId()<<std::endl;
           std::cout << "Driver: " << order.getDriverId() <<std::endl;
        }
        complete();
    } 
    else {
        delete &order;
        error("Order doesn't exists!");
    }
    wareHouse.addAction(this->clone());
}
PrintOrderStatus *PrintOrderStatus::clone() const{
    return new PrintOrderStatus(*this);
}
std::string PrintOrderStatus::toString() const{
    string result="orderStatus ";
    result +=std::to_string(orderId);
    if(BaseAction::getStatus() == ActionStatus::COMPLETED){
        result += " COMPLETED";  }
        else{ result += " ERROR";}
    return result;
}
//.................................class PrintCustomerStatus................................
PrintCustomerStatus::PrintCustomerStatus(int customerId):customerId(customerId){}
void PrintCustomerStatus::act(WareHouse &wareHouse){
    Customer &customer= wareHouse.getCustomer(customerId);
    if(customer.getId() != -1){
        std::cout << "CustomerId: "<< customer.getId()<<std::endl;
        const std::vector<int> &ordersIds= customer.getOrdersIds();
        for(int orderId: ordersIds){
            Order &order=wareHouse.getOrder(orderId);
            if(order.getId() != -1){
                std::cout<<"OrderId: "<< order.getId()<<std::endl;
                std::cout<< "OrderStatus: "<<order.toString() << std::endl;
            } else {
                delete &order;
            }
        }
        std::cout<<"numOrdersLeft: "<< customer.getNumOrdersLeft()<< std::endl;
        complete();
    } else {
        delete &customer;
        error("customer doesn't exist!");
    }
    wareHouse.addAction(this->clone());
}

PrintCustomerStatus *PrintCustomerStatus::clone() const{
    return new PrintCustomerStatus(*this);
}
std::string PrintCustomerStatus::toString() const{
    string result= "CustomerStatus " + std::to_string(customerId);
    if(BaseAction::getStatus() == ActionStatus::COMPLETED){
        result += " COMPLETED";  }
        else{ result += " ERROR";}
    return result;
}
//..............................class PrintVolunteerStatus.........................
PrintVolunteerStatus::PrintVolunteerStatus(int id):volunteerId(id){}
void PrintVolunteerStatus::act(WareHouse &wareHouse){
    if (wareHouse.volunteerExists(volunteerId)) {
        for (Volunteer *volunteer : wareHouse.getVolunteers()) {
            if (volunteer->getId() == volunteerId) {
                std::cout << volunteer->toString() << std::endl;
            }
        }
    complete();    
    } 
    else {
        error("Volunteer doesn't exist");
    }
    wareHouse.addAction(this->clone());
}
    
PrintVolunteerStatus *PrintVolunteerStatus::clone() const{
    return new PrintVolunteerStatus(*this);
}
string PrintVolunteerStatus::toString() const{
    string result= "VolunteerStatus " + std::to_string(volunteerId);
    if(BaseAction::getStatus() == ActionStatus::COMPLETED){
        result += " COMPLETED";  }
        else{ result += " ERROR";}
    return result;
}
//..........................class PrintActionsLog........................
PrintActionsLog::PrintActionsLog(){}
void PrintActionsLog::act(WareHouse &wareHouse){
    const vector<BaseAction*> &actions= wareHouse.getActions();
    for( BaseAction* action: actions){
        std::cout<< action->toString() << std::endl;
    }
    complete();
    wareHouse.addAction(this->clone());
}

PrintActionsLog *PrintActionsLog::clone() const{
    return new PrintActionsLog(*this);
}
std::string PrintActionsLog::toString() const{ 
    return "log COMPLETED";
}

//......................class Close...................................
Close::Close(){}
void Close::act(WareHouse &wareHouse){
    wareHouse.close();
    wareHouse.addAction(this->clone());
    complete();
    
}
Close *Close::clone() const{
    return new Close(*this);
}
std::string Close::toString() const{
    return "close";
}

//.....................class BackupWareHouse..........................
BackupWareHouse::BackupWareHouse(){}
void  BackupWareHouse::act(WareHouse &wareHouse){
    if (backup == nullptr) {
        backup = new WareHouse(wareHouse); // use the copy constructor
    } 
    else {
        delete backup; // free the existing memory
        backup = new WareHouse(wareHouse); //copy asssignment
    }
    complete();
    wareHouse.addAction(this->clone());

}
BackupWareHouse *BackupWareHouse::clone() const{
    return new  BackupWareHouse(*this);
}
std::string  BackupWareHouse::toString() const{
    string result="backup ";
    if(BaseAction::getStatus()== ActionStatus::COMPLETED){
       result += " COMPLETED"; 
    }
    else{ result += " ERROR";}
    return result;
}


//................class RestoreWareHouse..................
RestoreWareHouse::RestoreWareHouse(){}
void RestoreWareHouse::act(WareHouse &wareHouse){
    if(backup == nullptr){
        error("No backup available");
    }
    else{
        wareHouse=*backup;
    }
    complete();
    wareHouse.addAction(this->clone());
}
RestoreWareHouse *RestoreWareHouse::clone() const{
    return new RestoreWareHouse(*this);
}
std::string RestoreWareHouse::toString() const{
    string result="restore ";
    if(BaseAction::getStatus() == ActionStatus::COMPLETED){
        result += " COMPLETED";  }
        else{ result += " ERROR";}
    return result;
}
