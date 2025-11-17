#include <iostream>
#include <vector>
#include <random>
#include <ctime>

/* Creating User Items Incomplete Matrix */
/* Les eleves doivent rajouter des méthodes pour faire le TP - à importer */

class RecommenderSystem {
private:
    std::vector<std::vector<float>> users;
    int nbUsers;
    int nbItems;

public:
    RecommenderSystem() : nbUsers(0), nbItems(0) {}

    void createFakeData(int nbUsers, int nbItems, float sparsity) {
        this->nbItems = nbItems;
        this->nbUsers = nbUsers;
        
        std::vector<std::vector<float>> userProfiles;
        std::vector<std::vector<float>> itemsProfiles;
        
        int profileDimension = 3;
        float maxNote = 5.0f;
        
        // Initialisation du générateur de nombres aléatoires
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        // creating users profiles
        for (int i = 0; i < nbUsers; i++) {
            std::vector<float> currentUserProfile;
            
            for (int j = 0; j < profileDimension; j++) {
                currentUserProfile.push_back(dis(gen) * maxNote);
            }
            userProfiles.push_back(currentUserProfile);
        }
        
        // creating items profiles
        for (int i = 0; i < nbItems; i++) {
            std::vector<float> currentItemProfile;
            
            for (int j = 0; j < profileDimension; j++) {
                currentItemProfile.push_back(dis(gen) * maxNote);
            }
            itemsProfiles.push_back(currentItemProfile);
        }
        
        // generating rates from users and items profiles
        for (int i = 0; i < nbUsers; i++) {
            std::vector<float> currentUserNote;
            
            for (int j = 0; j < nbItems; j++) {
                float newNote = dis(gen);
                
                if (newNote < sparsity) {
                    float note = 0.0f;
                    
                    for (int k = 0; k < profileDimension; k++) {
                        note += userProfiles[i][k] * itemsProfiles[j][k];
                    }
                    currentUserNote.push_back(note / (maxNote * profileDimension));
                } else {
                    currentUserNote.push_back(-1.0f);
                }
            }
            users.push_back(currentUserNote);
        }
        
        /*for (int i = 0; i < this->nbUsers; i++) {
            for (int j = 0; j < this->nbItems; j++) {
                std::cout << users[i][j] << " ";
            }
            std::cout << std::endl;
        }*/
    }
    
    // Getters
    int getNbUsers() const { return nbUsers; }
    int getNbItems() const { return nbItems; }
    const std::vector<std::vector<float>>& getUsers() const { return users; }
};

int main() {
    RecommenderSystem recSys;
    
    recSys.createFakeData(20000, 20000, 0.1f);
    
    return 0;
}