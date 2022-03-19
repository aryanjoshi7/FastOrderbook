#include <unordered_map>
#include <map>
#include <string>
#include <list>
#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

mutex mtx;

class Order {
    public:
        bool bid;
        int id;
        int volume; 
        float price;
        Order* before;
        Order* next;

        Order(int i, int v, float p, bool b):
            id(i), volume(v), price(p), bid(b), before(nullptr), next(nullptr){}
        
};

class Limit {
    private:
        int size;
        Order* head;
        Order* tail;
    
    public:
        int volume;
        float price;
        void append_order(Order * order){    
            mtx.lock();
            if (size==0){
                head = order;
                tail = order;
                size++;
                volume += order->volume;
                price = order->price;
                return;
            }
            order->next = head;
            head->before = order;
            head = order;
            head->before = nullptr;
            size++;
            volume += order->volume;

            mtx.unlock();
        }

        void cancel_order(Order * order){
            if (order->before != nullptr)
                order->before->next = order->next;
            if (order->next != nullptr)
                order->next->before = order->before;
            size--;
            volume -= order->volume;
        }

        bool pop_front(int volume_pop){
            volume -= volume_pop;
            while (volume_pop>0){
                int temp_volume = head->volume;
                if (head->volume <= volume_pop){
                    cout << "EXECUTE ORDER " << head->id << endl;
                    Order * new_head = head->next;
                    delete head;
                    head = new_head;
                }
                else {
                    cout << "FILL " << volume_pop << " SHARES FROM " << head->id << endl;
                    head->volume -= volume_pop;
                }
                volume_pop -= temp_volume;
            }
            return volume==0;
        }
};

class Orderbook {
    private:
        unordered_map<float, Limit*> limit_map_bids;
        unordered_map<float, Limit*> limit_map_asks;

        map<float, Limit, greater<float>> bids;
        map<float, Limit> asks;

        unordered_map<int, Order*> id_to_order;

    public:
        int add_order(int i, int v, float p, bool bid){
            Order * order = new Order(i, v, p, bid);
            if (bid){
                bids[p].append_order(order); 
                limit_map_bids[p] = &bids[p];
                
            } else {
                asks[p].append_order(order);
                limit_map_asks[p] = &asks[p];
            }
            id_to_order[i] = order;
            execute();
            return i;
        }

        void remove_order(int id){
            Order * o = id_to_order[id];
            // TODO: remove from bids and asks maps, remove id from map if deleted elsewhere
            if (o->bid){
                limit_map_bids[o->price]->cancel_order(o);
            } else {
                limit_map_asks[o->price]->cancel_order(o);
            }
        }

        void execute(){
            if (bids.size()==0 || asks.size()==0){
                return;
            }
            auto bids_iter = bids.begin();
            auto asks_iter = asks.begin();
            // cout << bids_iter->second.price << endl;
            // cout << asks_iter->second.price << endl;
            while (asks_iter->second.price <= bids_iter->second.price){
                //cout << "HI" << endl;
                // cout << asks_iter->second.price << " "<< bids_iter->second.price << endl;
                int m = min(asks_iter->second.volume, bids_iter->second.volume);
                // pop the front of the bids and asks linkedlists depending on volume available
                if (asks_iter->second.pop_front(m)){
                    limit_map_asks.erase(asks_iter->second.price);
                    asks.erase(asks_iter);
                    asks_iter = asks.begin();
                }
                if (bids_iter->second.pop_front(m)){
                    limit_map_bids.erase(bids_iter->second.price);
                    bids.erase(bids_iter);
                    bids_iter = bids.begin();
                }
                if (bids.size()==0 || asks.size()==0){
                    return;
                }
            }
        }
};


Orderbook * book = new Orderbook();
// delete book;

int concat_order(int i, int v, float p, bool bid){

}

int main(){
    // cout << "ADD BID" << endl;
    // book->add_order(1, 1, 100, true);

    // cout << "ADD ASK" << endl;
    // book->add_order(2, 1, 101, false);
    

    thread first (book->add_order, 1, 1, 100, true);
    thread second (book->add_order, 2, 1, 100, true);

    first.join();
    second.join();

    // book->remove_order(2);
    // cout << "ORDER 2 REMOVED" << endl;

    // book->remove_order(1);
    // cout << "ORDER 1 REMOVED" << endl;

    return 0;
}