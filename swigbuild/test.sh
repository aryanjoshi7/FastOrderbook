swig -python -c++ orderbook.i
g++ -fPIC -c orderbook.cpp -I /usr/include/python3.8/
g++ -fPIC -c orderbook_wrap.cxx $(python3-config --cflags) -I /usr/include/python3.8/
g++ -shared orderbook.o orderbook_wrap.o $(python3-config --ldflags) -o _orderbook.so -I /usr/include/python3.8/
