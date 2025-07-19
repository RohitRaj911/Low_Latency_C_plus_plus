#pragma once


#include<bits/stdc++.h>
#include "Order.hpp"
using namespace std;

class Trade{

    public:
        Trade(int trade, shared_ptr<Order>buyOrder, shared_ptr<Order> sellOrder);
        ~Trade();

        int getTradeId() const;
        shared_ptr<Order> getBuyOrder() const;
        shared_ptr<Order> getSellOrder() const;

    private:
        int m_TradeId;
        shared_ptr<Order> m_BuyOrder;
        shared_ptr<Order> m_SellOrder;
};