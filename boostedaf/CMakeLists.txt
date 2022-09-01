PYTHON_ADD_MODULE(orderbook Orderbook_base.cpp)
FILE(COPY boosted.py DESTINATION .)
ADD_TEST(NAME boostedaf COMMAND ${PYTHON_EXECUTABLE} boosted.py)

