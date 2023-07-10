#include <iostream>

#include <string>

#include <vector>

#include <thread>

#include <chrono>

#include <unordered_map>


const int PACKET_SIZE = 1024;

const int TIMEOUT = 1000;


struct Packet {

    int sequenceNumber;

    std::string data;

    std::string message;

};


void simpleProtocol();

void stopAndWait();

void goBackN();

void selectiveRepeat();

void printPacket(const Packet& packet);


int main() {

    int choice;


    do {

        std::cout << "Select a flow control protocol:\n";

        std::cout << "1. Stop-and-Wait\n";

        std::cout << "2. Go-Back-N\n";

        std::cout << "3. Selective Repeat\n";

        std::cout << "4. Simple Protocol\n";

        std::cout << "5. Exit Program\n";

        std::cin >> choice;


        switch (choice) {

            case 1:

                stopAndWait();

                break;

            case 2:

                goBackN();

                break;

            case 3:

                selectiveRepeat();

                break;

            case 4:

                simpleProtocol();

                break;

            case 5:

                std::cout<<"Exiting Program.\n";

                break;

            default:

                std::cout << "Invalid choice. Please try again.\n";

                break;

        }

    } while (choice != 5);

    return 0;

}

void stopAndWait() {

    std::cout << "Stop-and-Wait flow control protocol selected.\n";


    std::vector<Packet> channel;

    int nextSequenceNumber = 0;


    while (true) {

        std::string message;

        std::cout << "Enter a message to send (or type 'exit' to quit): ";

        std::getline(std::cin, message);

        if (message == "exit") {

            break;

        }


        Packet packet{nextSequenceNumber, message};

        std::cout << "Sending packet with sequence number " << nextSequenceNumber << "...\n";

        channel.push_back(packet);


        bool ackReceived = false;

        while (!ackReceived) {

            std::cout << "Waiting for acknowledgement...\n";

            std::this_thread::sleep_for(std::chrono::milliseconds(TIMEOUT));


            for (auto it = channel.begin(); it != channel.end(); ++it) {

                if (it->sequenceNumber == nextSequenceNumber) {

                    std::cout << "Acknowledgement received for packet with sequence number " << nextSequenceNumber << ".\n";

                    channel.erase(it);

                    ackReceived = true;

                    break;

                }

            }

        }


        nextSequenceNumber = (nextSequenceNumber + 1) % 2;

    }

}

void goBackN() {

    std::cout << "Go-Back-N flow control protocol selected.\n";


    std::vector<Packet> channel; 

    int nextSequenceNumber = 0;

    int windowSize;

    std::cout << "Enter the window size: ";

    std::cin >> windowSize;


    while (true) {

        // Send packets

        std::cout << "Sending packets...\n";

        for (int i = 0; i < windowSize; ++i) {

            std::string message = "Packet " + std::to_string(nextSequenceNumber + i);

            Packet packet{nextSequenceNumber + i, message};

            channel.push_back(packet);

            printPacket(packet);

        }


        int base = nextSequenceNumber;

        bool timeout = false;

        while (true) {

            std::this_thread::sleep_for(std::chrono::milliseconds(TIMEOUT));

            timeout = true;

            for (int i = base; i < base + windowSize; ++i) {

                for (auto it = channel.begin(); it != channel.end(); ++it) {

                    if (it->sequenceNumber == i) {

                        timeout = false;

                        std::cout << "Acknowledgement received for packet with sequence number " << i << ".\n";

                        channel.erase(it);

                        if (i == base) {

                            base++;

                            nextSequenceNumber++;

                            break;

                        }

                    }

                }

                if (timeout) {

                    std::cout << "Timeout occurred. Resending packets...\n";

                    break;

                }

            }

            if (!timeout) {

                break;

            }

        }

    }

}


void selectiveRepeat() {

    std::cout << "Selective Repeat flow control protocol selected.\n";


    std::vector<Packet> channel;

    std::unordered_map<int, bool> received; 

    int nextSequenceNumber = 0;

    int windowSize;

    std::cout << "Enter the window size: ";

    std::cin >> windowSize;


    while (true) {

  

        std::cout << "Sending packets...\n";

        for (int i = 0; i < windowSize; ++i) {

            std::string message = "Packet " + std::to_string(nextSequenceNumber + i);

            Packet packet{nextSequenceNumber + i, message};

            channel.push_back(packet);

            printPacket(packet);

        }


     

        int base = nextSequenceNumber;

        bool timeout = false;

        while (true) {

            std::this_thread::sleep_for(std::chrono::milliseconds(TIMEOUT));

            timeout = true;

            for (auto it = channel.begin(); it != channel.end(); ++it) {

                if (received[it->sequenceNumber]) {

                    continue;

                }

                for (auto jt = channel.begin(); jt != channel.end(); ++jt) {

                    if (jt->sequenceNumber == it->sequenceNumber) {

                        continue;

                    }

                    if (!received[jt->sequenceNumber]) {

                        timeout = false;

                        break;

                    }

                }

                if (timeout) {

                    std::cout << "Timeout occurred. Resending packet with sequence number " << it->sequenceNumber << "...\n";

                    break;

                }

                std::cout << "Acknowledgement received for packet with sequence number " << it->sequenceNumber << ".\n";

                received[it->sequenceNumber] = true;

                channel.erase(it);

                if (it->sequenceNumber == base) {

                    while (received[base]) {

                        received.erase(base);

                        base++;

                        nextSequenceNumber++;

                    }

                }

                break;

            }

            if (timeout) {

                break;

            }

        }

    }

}

void simpleProtocol() {

    std::cout << "Simple Protocol flow control protocol selected.\n";


    std::vector<Packet> channel; 

    int sequenceNumber = 0;

    bool received = false;


    while (true) {

        std::string message = "Packet " + std::to_string(sequenceNumber);

        Packet packet{sequenceNumber, message};


        std::cout << "Sending packet with sequence number " << sequenceNumber << "...\n";

        channel.push_back(packet);

        printPacket(packet);


        while (!received) {

            std::this_thread::sleep_for(std::chrono::milliseconds(TIMEOUT));

            for (auto it = channel.begin(); it != channel.end(); ++it) {

                if (it->sequenceNumber == sequenceNumber) {

                    received = true;

                    channel.erase(it);

                    std::cout << "Acknowledgement received for packet with sequence number " << sequenceNumber << ".\n";

                    break;

                }

            }

            if (!received) {

                std::cout << "Timeout occurred. Resending packet with sequence number " << sequenceNumber << "...\n";

            }

        }


        sequenceNumber++;


        std::cout << "Enter 'c' to continue or 'x' to exit: ";

        char input;

        std::cin >> input;

        if (input == 'x') {

            break;

        }

        received = false;

    }

}

void printPacket(const Packet& packet) {

    std::cout << "Packet contents:\n";

    std::cout << "Sequence number: " << packet.sequenceNumber << "\n";

    std::cout << "Message: " << packet.message << "\n";

}

