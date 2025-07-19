#include<bits/stdc++.h>
#include "include/Order.hpp"
#include "include/Trade.hpp"
using namespace std;


/* Q. Why we are not using Move Constructor and Move Assignment operator in Trade
   Ans-> We can use it, but it is redundant because
   🔧 When Should You Add Move to Trade?
✅ Add it only if:

-> You add std::string (e.g. symbol, timestamp)

-> You store Order inside Trade by value (not by pointer or reference)

-> You want deep control/logs for profiling

Otherwise, you can skip move/copy implementations in Trade safely.
*/

void runDeafultCode(){
    //creating two objects

    shared_ptr<Order> buyOrder = make_shared<Order>(1, "TATASTEEL", 100, 150.5, Side::BUY);
    shared_ptr<Order> sellOrder = make_shared<Order>(2, "ITC", 100, 150.5, Side::SELL);
    
    Trade trade(101, buyOrder, sellOrder);
}

void runWithMoveConstructorAndMoveAssignment(){

        Order o1(101, "RELIANCE", 100, 2735.5, Side::BUY);
        o1.printDetails();

        std::vector<Order> orders;
        orders.push_back(std::move(o1));  // Move constructor gets called

        Order o2(102, "TCS", 50, 3790.0, Side::SELL);
        Order o3 = std::move(o2);         // Move constructor again

        orders.emplace_back(std::move(o3));  // Another move

}

int main(){
    
   // runDeafultCode();

    /*To CHECK How Move Constructor and Move Assignment Operator Works-- add the code below*/
    runWithMoveConstructorAndMoveAssignment(); 
    cout << "Inside scope...\n";
     
}
