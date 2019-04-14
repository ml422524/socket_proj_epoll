OBJECT = main.o server.o listenaddr.o utils.o connection.o log.o
TARGET = svr
CXX = g++ -std=c++11 -g

$(TARGET) : $(OBJECT)
	$(CXX) -o $(TARGET) $(OBJECT) -lpthread -llog4cplus

main.o : main.cc
	$(CXX) -c main.cc

listenaddr.o:listenaddr.cc
	$(CXX) -c listenaddr.cc

utils.o : utils.cc
	$(CXX) -c utils.cc

mutex.o : mutex.cc
	$(CXX) -c mutex.cc

event.o : event.cc
	$(CXX) -c event.cc

connection.o : connection.cc
	$(CXX) -c connection.cc

log.o : log.cc
	$(CXX) -c log.cc -I /usr/local/include

clean:
	rm -f $(TARGET) $(OBJECT)
