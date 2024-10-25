#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <climits>
#include <unordered_map>
#include <iomanip>

using namespace std;

class BorrowerNetwork {
public:
    unordered_map<int, int> cibilScores;
    vector<vector<int>> transactions;
    int numBorrowers;

    BorrowerNetwork(int n) {
        numBorrowers = n;
        transactions = vector<vector<int>>(n, vector<int>(n, 0));
    }

    void setCIBILScore(int borrower, int score) {
        cibilScores[borrower] = score;
    }

    void addTransaction(int from, int to, int amount) {
        transactions[from][to] = amount;
    }

    void removeTransaction(int from, int to) {
        transactions[from][to] = 0;
    }

    void BFS(int start) {
        vector<bool> visited(numBorrowers, false);
        queue<int> q;
        q.push(start);
        visited[start] = true;

        while (!q.empty()) {
            int borrower = q.front();
            q.pop();
            cout << borrowerToChar(borrower) << " ";

            for (int i = 0; i < numBorrowers; ++i) {
                if (transactions[borrower][i] != 0 && !visited[i]) {
                    q.push(i);
                    visited[i] = true;
                }
            }
        }
        cout << endl;
    }

    void DFS(int start) {
        vector<bool> visited(numBorrowers, false);
        stack<int> s;
        s.push(start);

        while (!s.empty()) {
            int borrower = s.top();
            s.pop();

            if (!visited[borrower]) {
                cout << borrowerToChar(borrower) << " ";
                visited[borrower] = true;
            }

            for (int i = 0; i < numBorrowers; ++i) {
                if (transactions[borrower][i] != 0 && !visited[i]) {
                    s.push(i);
                }
            }
        }
        cout << endl;
    }

    void dijkstra(int start) {
        vector<int> dist(numBorrowers, INT_MAX);
        dist[start] = 0;
        vector<bool> visited(numBorrowers, false);

        for (int count = 0; count < numBorrowers - 1; ++count) {
            int u = minDistance(dist, visited);
            visited[u] = true;

            for (int v = 0; v < numBorrowers; ++v) {
                if (!visited[v] && transactions[u][v] && dist[u] != INT_MAX
                    && dist[u] + transactions[u][v] < dist[v]) {
                    dist[v] = dist[u] + transactions[u][v];
                }
            }
        }

        printSolution(dist);
    }

    void enhancedFraudDetection() {
        for (int i = 0; i < numBorrowers; ++i) {
            for (int j = 0; j < numBorrowers; ++j) {
                if (transactions[i][j] != 0) {
                    int creditLimit = (cibilScores[i] * 5) / 100; // Example: 5% of CIBIL score
                    if (transactions[i][j] > (0.8 * creditLimit)) {
                        cout << "Potential fraud: Borrower " << borrowerToChar(i) << " exceeded 80% of their credit limit with Borrower " << borrowerToChar(j) << ". Transaction Amount: " << transactions[i][j] << endl;
                    }
                }
            }
        }
    }

    void visualizeNetwork() {
        sf::RenderWindow window(sf::VideoMode(800, 600), "Credit Risk Network Visualization");

        vector<sf::CircleShape> nodes(numBorrowers);
        vector<sf::Text> labels(numBorrowers);
        sf::Font font;

        // Update the font loading to use the full path
        if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
            // If loading fails, use a default SFML font
            cout << "Error loading font! Using default font." << endl;
            font = sf::Font(); // Default SFML font
        }

        for (int i = 0; i < numBorrowers; ++i) {
            nodes[i].setRadius(30);
            nodes[i].setFillColor(getNodeColor(cibilScores[i]));
            nodes[i].setPosition(100 + i * 150, 300);
            labels[i].setFont(font);
            labels[i].setString(borrowerToChar(i) + string(": ") + to_string(cibilScores[i]));
            labels[i].setCharacterSize(24);
            labels[i].setFillColor(sf::Color::Black);
            labels[i].setPosition(80 + i * 150, 315);
        }

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear(sf::Color::White);

            for (int i = 0; i < numBorrowers; ++i) {
                for (int j = 0; j < numBorrowers; ++j) {
                    if (transactions[i][j] != 0) {
                        sf::Vertex line[] = {
                            sf::Vertex(nodes[i].getPosition() + sf::Vector2f(30, 30), sf::Color::Black),
                            sf::Vertex(nodes[j].getPosition() + sf::Vector2f(30, 30), sf::Color::Black)
                        };
                        window.draw(line, 2, sf::Lines);
                    }
                }
            }

            for (int i = 0; i < numBorrowers; ++i) {
                window.draw(nodes[i]);
                window.draw(labels[i]);
            }

            window.display();
        }
    }

private:
    char borrowerToChar(int index) {
        return 'A' + index;
    }

    sf::Color getNodeColor(int score) {
        if (score >= 750) return sf::Color::Green; // Good credit
        else if (score >= 600) return sf::Color::Yellow; // Average credit
        else return sf::Color::Red; // Poor credit
    }

    int minDistance(vector<int>& dist, vector<bool>& visited) {
        int min = INT_MAX, min_index;
        for (int v = 0; v < numBorrowers; ++v) {
            if (!visited[v] && dist[v] <= min) {
                min = dist[v];
                min_index = v;
            }
        }
        return min_index;
    }

    void printSolution(vector<int>& dist) {
        cout << fixed << setprecision(2);
        for (int i = 0; i < numBorrowers; ++i) {
            cout << "To borrower " << borrowerToChar(i) << ": " << dist[i] << endl;
        }
    }
};

int main() {
    int n;
    cout << "Enter the number of borrowers: ";
    cin >> n;

    BorrowerNetwork network(n);

    cout << "Enter CIBIL scores of borrowers (0 to 900):" << endl;
    for (int i = 0; i < n; ++i) {
        int score;
        do {
            cout << "Borrower " << char('A' + i) << ": ";
            cin >> score;
            if (score < 0 || score > 900) {
                cout << "Invalid score! Please enter a score between 0 and 900." << endl;
            }
        } while (score < 0 || score > 900);
        network.setCIBILScore(i, score);
    }

    cout << "Enter transaction amounts between borrowers (0 if no transaction):" << endl;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                int amount;
                cout << "Transaction from " << char('A' + i) << " to " << char('A' + j) << ": ";
                cin >> amount;
                network.addTransaction(i, j, amount);
            }
        }
    }

    int choice;
    do {
        cout << "--- Credit Risk Assessment Menu ---" << endl;
        cout << "1. BFS Traversal" << endl;
        cout << "2. DFS Traversal" << endl;
        cout << "3. Dijkstra's Algorithm" << endl;
        cout << "4. Enhanced Fraud Detection" << endl;
        cout << "5. Visualize Network" << endl;
        cout << "6. Remove Transaction" << endl;
        cout << "7. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                int start;
                cout << "Enter start borrower index (0 to " << n - 1 << "): ";
                cin >> start;
                if (start >= 0 && start < n) {
                    network.BFS(start);
                } else {
                    cout << "Invalid index!" << endl;
                }
                break;
            }
            case 2: {
                int start;
                cout << "Enter start borrower index (0 to " << n - 1 << "): ";
                cin >> start;
                if (start >= 0 && start < n) {
                    network.DFS(start);
                } else {
                    cout << "Invalid index!" << endl;
                }
                break;
            }
            case 3: {
                int start;
                cout << "Enter start borrower index (0 to " << n - 1 << "): ";
                cin >> start;
                if (start >= 0 && start < n) {
                    network.dijkstra(start);
                } else {
                    cout << "Invalid index!" << endl;
                }
                break;
            }
            case 4:
                network.enhancedFraudDetection();
                break;
            case 5:
                network.visualizeNetwork();
                break;
            case 6: {
                int from, to;
                cout << "Enter from borrower index: ";
                cin >> from;
                cout << "Enter to borrower index: ";
                cin >> to;
                network.removeTransaction(from, to);
                break;
            }
            case 7:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice! Please try again." << endl;
        }
    } while (choice != 7);

    return 0;
}
