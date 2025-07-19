#include<bits/stdc++.h>
#include "../include/Trade.hpp"

Trade :: Trade(int trade, shared_ptr<Order>buyOrder, shared_ptr<Order> sellOrder)
: m_TradeId(trade), m_BuyOrder(buyOrder), m_SellOrder(sellOrder){

    cout << "{/*TRADE CONSTRUCTOR CALLED*/}\n[Trade Created] ID: " << m_TradeId
              << ", But Order: " << m_BuyOrder
              << ", Sell Order: " << m_SellOrder<< "\n\n";
}

//Destructor
Trade :: ~Trade(){
    cout << "{/*TRADE DESTRUCTOR CALLED*/}\n[Trade Destroyed] ID: " << m_TradeId << "\n\n";
}

int Trade::getTradeId() const { return m_TradeId; }
shared_ptr<Order> Trade::getBuyOrder() const { return m_BuyOrder; }
shared_ptr<Order> Trade::getSellOrder() const { return m_SellOrder; }

