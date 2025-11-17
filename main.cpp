#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>

/* Creating User Items Incomplete Matrix */
/* Les eleves doivent rajouter des méthodes pour faire le TP - à importer */

class RecommenderSystem {
private:
    std::vector<std::vector<float>> m_users;
    std::vector<std::vector<float>> m_predicted;
    std::vector<std::vector<float>> m_similarities;
    int m_nbUsers;
    int m_nbItems;

public:
    RecommenderSystem() : m_nbUsers(0), m_nbItems(0) {}

    void createFakeData(int _nbUsers, int _nbItems, float _sparsity) {
        this->m_nbItems = _nbItems;
        this->m_nbUsers = _nbUsers;
        
        std::vector<std::vector<float>> userProfiles;
        std::vector<std::vector<float>> itemsProfiles;
        
        int profileDimension = 3;
        float maxNote = 5.0f;
        
        // Initialisation du générateur de nombres aléatoires
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        // creating users profiles
        for (int i = 0; i < m_nbUsers; i++) {
            std::vector<float> currentUserProfile;
            
            for (int j = 0; j < profileDimension; j++) {
                currentUserProfile.push_back(dis(gen) * maxNote);
            }
            userProfiles.push_back(currentUserProfile);
        }
        
        // creating items profiles
        for (int i = 0; i < m_nbItems; i++) {
            std::vector<float> currentItemProfile;
            
            for (int j = 0; j < profileDimension; j++) {
                currentItemProfile.push_back(dis(gen) * maxNote);
            }
            itemsProfiles.push_back(currentItemProfile);
        }
        
        // generating rates from users and items profiles
        for (int i = 0; i < m_nbUsers; i++) {
            std::vector<float> currentUserNote;
            
            for (int j = 0; j < m_nbItems; j++) {
                float newNote = dis(gen);
                
                if (newNote < _sparsity) {
                    float note = 0.0f;
                    
                    for (int k = 0; k < profileDimension; k++) {
                        note += userProfiles[i][k] * itemsProfiles[j][k];
                    }
                    currentUserNote.push_back(note / (maxNote * profileDimension));
                } else {
                    currentUserNote.push_back(-1.0f);
                }
            }
            m_predicted.push_back(currentUserNote);
            m_users.push_back(currentUserNote);
            std::vector<float> simForUser;
            for (int j = 0; j < m_nbUsers; j++) 
            {
                simForUser.push_back(-1.0f);
            }
            m_similarities.push_back(simForUser);
        }
         
        // for (int i = 0; i < this->nbUsers; i++) {
        //     for (int j = 0; j < this->nbItems; j++) {
        //         std::cout << users[i][j] << " ";
        //     }
        //     std::cout << std::endl;
        // }
        
    }

    float getSimilarityCosinus(int _userA, int _userB) {
        if (m_similarities[_userA][_userB] == -1)
        {
            float dotProduct = 0.0f;
            float normA = 0.0f;
            float normB = 0.0f;

            for (int i = 0; i < m_nbItems; i++) {
                if (m_users[_userA][i] != -1.0f && m_users[_userB][i] != -1.0f) {
                    dotProduct += m_users[_userA][i] * m_users[_userB][i];
                    normA += m_users[_userA][i] * m_users[_userA][i];
                    normB += m_users[_userB][i] * m_users[_userB][i];
                }
            }

            if (normA == 0 || normB == 0) return 0.0f;

            float result = dotProduct / (sqrt(normA) * sqrt(normB));
            m_similarities[_userA][_userB] = result;
            m_similarities[_userB][_userA] = result;
        }
        return m_similarities[_userA][_userB];
    }

    void predictUserRating(int _userId)
    {
        for (int itemId = 0; itemId < m_nbItems; itemId++) {
            if (m_users[_userId][itemId] == -1.0f) {
                float weightedSum = 0.0f;
                float sumOfWeights = 0.0f;

                for (int otherUserId = 0; otherUserId < m_nbUsers; otherUserId++) {
                    if (otherUserId != _userId && m_users[otherUserId][itemId] != -1.0f) {
                        float similarity = getSimilarityCosinus(_userId, otherUserId);
                        weightedSum += similarity * m_users[otherUserId][itemId];
                        sumOfWeights += similarity;
                    }
                }

                if (sumOfWeights > 0) {
                    m_predicted[_userId][itemId] = weightedSum / sumOfWeights;
                } else {
                    m_predicted[_userId][itemId] = 0.0f; // Default prediction if no similar users
                }
            }
        }
    }

    void predictAll()
    {
        for (int i = 0; i < getNbUsers(); i++)
        {
            predictUserRating(i);
        }
    }

    std::vector<int> getRecommendations(int _user, int _k)
    {
        std::vector<std::pair<int, float>> recommendations;
        
        for (int i = 0; i < getNbItems(); i++)
        {
            if (m_users[_user][i] == -1)
            {
                recommendations.push_back({i, m_predicted[_user][i]});
            }
        }
        
        std::sort(recommendations.begin(), recommendations.end(),
                  [](const auto& a, const auto& b) {
                      return a.second > b.second;
                  });
        
        std::vector<int> result;
        for (int i = 0; i < std::min(_k, (int)recommendations.size()); i++)
        {
            result.push_back(recommendations[i].first);
        }
        
        for (int i = 0; i < std::min(_k, (int)recommendations.size()); i++)
        {
            std::cout << "Item " << recommendations[i].first << " - Score: " << recommendations[i].second << std::endl;
        }

        return result;
    }

    void printMaxNotes()
    {
        std::vector<std::pair<int, float>> allRatings;
        for (int i = 0; i < getNbUsers(); i++) {
            for (int j = 0; j < getNbItems(); j++) {
                if (getPredicted()[i][j] > 0) {
                    allRatings.push_back({i * getNbItems() + j, getPredicted()[i][j]});
                }
            }
        }

        std::sort(allRatings.begin(), allRatings.end(),
                [](const auto& a, const auto& b) {
                    return a.second > b.second;
                });

        std::cout << std::endl << "Top 10 highest ratings across all users:" << std::endl;
        for (int i = 0; i < std::min(10, (int)allRatings.size()); i++) {
            int userId = allRatings[i].first / getNbItems();
            int itemId = allRatings[i].first % getNbItems();
            std::cout << "User " << userId << " - Item " << itemId << " - Score: " << allRatings[i].second << std::endl;
        }
    }

    void printDistributionOfRatings()
    {
        std::vector<int> distribution(11, 0); // Bins for ratings 0.0 to 5.0 in steps of 0.5

        for (int i = 0; i < getNbUsers(); i++) {
            for (int j = 0; j < getNbItems(); j++) {
                float rating = getPredicted()[i][j];
                if (rating >= 0) {
                    int bin = static_cast<int>(rating * 2); // Multiply by 2 to get bins of 0.5
                    if (bin >= 0 && bin < distribution.size()) {
                        distribution[bin]++;
                    }
                }
            }
        }

        std::cout << "Distribution of predicted ratings:" << std::endl;
        for (int i = 0; i < distribution.size(); i++) {
            float ratingValue = i * 0.5f;
            std::cout << "Rating " << ratingValue << ": " << distribution[i] << std::endl;
        }
    }
    
    // Getters
    int getNbUsers() const { return m_nbUsers; }
    int getNbItems() const { return m_nbItems; }
    const std::vector<std::vector<float>>& getUsers() const { return m_users; }
    const std::vector<std::vector<float>>& getPredicted() const { return m_predicted; }
    const std::vector<std::vector<float>>& getSimilarities() const { return m_similarities; }
};


int main() {
    RecommenderSystem recSys;
    
    recSys.createFakeData(1000, 1000, 0.2f);
    std::cout << "Similarity between user 0 and user 1: " 
              << recSys.getSimilarityCosinus(0, 1) << std::endl;

    std::cout << "Base note for user 0: ";
    for (int i = 0; i < 10 ; i++)
    {
        std::cout << recSys.getUsers()[0][i] << "; ";
    }
    std::cout << std::endl << "Base note for user 1: ";
    for (int i = 0; i < 10 ; i++)
    {
        std::cout << recSys.getUsers()[1][i] << "; ";
    }

    recSys.predictAll();
    std::cout << std::endl
              << "Predicted note for user 0: ";
    for (int i = 0; i < 10 ; i++)
    {
        std::cout << recSys.getPredicted()[0][i] << "; ";
    }

    recSys.predictUserRating(1);
    std::cout << std::endl << "Predicted note for user 1: ";
    for (int i = 0; i < 10 ; i++)
    {
        std::cout << recSys.getPredicted()[1][i] << "; ";
    }

    std::cout << std::endl << "10 recommendations for user 1: ";
    recSys.getRecommendations(0, 10);

    recSys.printMaxNotes();

    recSys.printDistributionOfRatings();

    std::cout << std::endl;
    
    
    return 0;
}