#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <thread>
#include <chrono>

class Block {
public:
    //attributes of block is data , timestamp , current_hash , previous_hash , nonce(number used once for mining purposes)


    std::string previousHash;
    std::string data;
    std::string timestamp;
    std::string hash;
    int nonce;

    Block(std::string prevHash, std::string blkData, std::string blkTimestamp, int blkNonce) {
        previousHash = prevHash;
        data = blkData;
        timestamp = blkTimestamp;
        nonce = blkNonce;
        hash = calculateHash();
    }

    std::string calculateHash() const {

        //creating the hash of current block from all the available data we have

        std::stringstream ss;
        ss << previousHash << timestamp << data << nonce;
        std::string toHash = ss.str();

        //std::cout<<"calulating hash from the above "<<ss.str()<<std::endl;

        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(toHash.c_str()), toHash.size(), hash);

        std::stringstream hexStream;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            hexStream << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }

        //std::cout<<"calculated hash is " <<hexStream.str()<<std::endl;

        return hexStream.str();
    }
};

class Blockchain {
public:
    std::vector<Block> chain;

    Blockchain() {
        chain.push_back(createGenesisBlock());
    }

    Block createGenesisBlock() {
        time_t now = time(0);
        return Block("0", "Genesis Block", ctime(&now), 0);
    }

    Block getLastBlock() const {
        return chain.back();
    }

    void addBlock(std::string data) {
        Block lastBlock = getLastBlock();
        time_t now = time(0);
        Block newBlock(lastBlock.hash, data, ctime(&now), 0);
        
        while (newBlock.hash.substr(0, 4) != "0000") {  // Simple PoW
            newBlock.nonce++;
            newBlock.hash = newBlock.calculateHash();
        }

        chain.push_back(newBlock);
        saveBlock(newBlock);
    }

    bool isChainValid() const {
        for (size_t i = 1; i < chain.size(); ++i) {
            const Block& currentBlock = chain[i];
            const Block& previousBlock = chain[i - 1];

            // Check if the hash of the current block is valid
            if (currentBlock.hash != currentBlock.calculateHash()) {
                std::cout << "Blockchain is corrupted. Corrupted block at index: " << i << std::endl;
                return false;
            }

            // Check if the previousHash of the current block matches the hash of the previous block
            if (currentBlock.previousHash != previousBlock.hash) {
                std::cout << "Blockchain is corrupted. Corrupted block at index: " << i << std::endl;
                return false;
            }
        }
        std::cout << "Blockchain is valid. No corruption detected." << std::endl;
        return true;
    }

    void saveBlock(const Block& block) {
        std::ofstream outfile("blockchain.txt", std::ios::app);
        if (outfile.is_open()) {
            outfile << block.previousHash << std::endl;
            outfile << block.data << std::endl;
            outfile << block.timestamp << std::endl;
            outfile << block.hash << std::endl;
            outfile << block.nonce << std::endl;
            outfile << "----------" << std::endl;
            outfile.close();
        }
    }
};

int main() {
    Blockchain bChain;
    bChain.addBlock("First block after genesis");
    bChain.addBlock("Second block after genesis");

    bChain.isChainValid();


    //std::cout<<"yes working";
    //std::this_thread::sleep_for(std::chrono::seconds(5));
    
    return 0;
}
