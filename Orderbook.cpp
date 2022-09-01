#include <unordered_map>
#include <map>
#include <string>
#include <list>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

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
            if (size==0){
                head = order;
                tail = order;
                price = order->price;
            } else {
                order->next = head;
                head->before = order;
                head = order;
                head->before = nullptr;
            }
            size++;
            volume += order->volume;
        }

        void cancel_order(Order * order){
            if (order->before != nullptr)
                order->before->next = order->next;
            if (order->next != nullptr)
                order->next->before = order->before;
            size--;
            volume -= order->volume;
        }

        bool pop_front(int volume_pop, unordered_map<int, Order*> * id_to_order){
            volume -= volume_pop;
            // cout << volume_pop << " " << volume << endl;
            while (volume_pop>0){
                // cout << head->volume << endl;
                int temp_volume = head->volume;
                if (head->volume <= volume_pop){
                    cout << "EXECUTE ORDER " << head->id << endl;
                    Order * new_head = head->next;
                    id_to_order->erase(head->id);
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
        float mark = 100;
        vector<float> marks;

        int add_order(int i, int v, float p, bool bid){
            mtx.lock();
            Order * order = new Order(i, v, p, bid);
            if (bid){
                bids[p].append_order(order); 
                limit_map_bids[p] = &bids[p];
                
            } else {
                asks[p].append_order(order);
                limit_map_asks[p] = &asks[p];
            }
            id_to_order[i] = order;
            mtx.unlock();
            execute();
            return i;
        }

        void remove_order(int id){
            Order * o = id_to_order[id];
            // TODO: remove from bids and asks maps, remove id from map if deleted elsewhere
            // Check volume for 0 case
            if (o->bid){
                limit_map_bids[o->price]->cancel_order(o);
            } else {
                limit_map_asks[o->price]->cancel_order(o);
            }
        }

        void execute(){
            mtx.lock();
            if (bids.size()==0 || asks.size()==0){
                mtx.unlock();
                return;
            }
            auto bids_iter = bids.begin();
            auto asks_iter = asks.begin();
            while (asks_iter->second.price <= bids_iter->second.price){
                // cout << "STARTED" << endl;
                int m = min(asks_iter->second.volume, bids_iter->second.volume);

                if (asks_iter->second.pop_front(m, &id_to_order)){
                    limit_map_asks.erase(asks_iter->second.price);
                    asks.erase(asks_iter);
                    asks_iter = asks.begin();
                }

                if (bids_iter->second.pop_front(m, &id_to_order)){
                    limit_map_bids.erase(bids_iter->second.price);
                    bids.erase(bids_iter);
                    bids_iter = bids.begin();
                }

                if (bids.size()==0 || asks.size()==0){
                    mtx.unlock();
                    return;
                } else {
                    mark = (asks_iter->second.price + bids_iter->second.price)/2;
                    cout << "MARK PRICE: " << mark << endl;
                    marks.push_back(mark);
                }
            }
            mtx.unlock();
        }
};


Orderbook * book = new Orderbook();
// delete book;
int current_i = 0;


void exec(){
    while (true){
        book->execute();
    }
}

void spam_bids(){ 
    float p;
    while (current_i<100000){
        p = book->mark+((float)(rand()%100)-49.5)/10;
        book->add_order(current_i++, 1, p, true);
    }
}

void spam_asks(){
    float p;
    while (current_i<100000){
        p = book->mark+((float)(rand()%100)-49.5)/10;
        book->add_order(current_i++, 1, p, false);
    }
}

int main(){
    // cout << "ADD BID" << endl;
    // book->add_order(1, 1, 100, true);

    // cout << "ADD ASK" << endl;
    // book->add_order(2, 1, 101, false);

    thread first (spam_bids);
    thread second (spam_asks);

    first.join();
    second.join();

    // for (int i = 0; i < (int)book->marks.size(); i++){
    //     cout << book->marks[i] << ", ";
    // }

    return 0;
}