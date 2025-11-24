#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <ctime>
#include <cmath>
#include "utils/Node.hpp"

using namespace std;

class TournamentTree
{
private:
    Node<string> *root = nullptr;
    vector<Node<string> *> leaves;
    unordered_map<int, Node<string> *> matchIdToNode;
    unordered_map<Node<string> *, int> nodeToMatchId;
    int nextMatchId = 1;
    mt19937 rng; // random score

    // get random
    int randomScore()
    {
        uniform_int_distribution<int> dist(1, 10);
        return dist(rng);
    }

    int nextPow2(int n)
    {
        int p = 1;
        while (p < n)
        {
            p <<= 1;
        }
        return p;
    }

    string getParticipant(Node<string> *node)
    {
        if (!node)
            return "?";
        if (node->left == nullptr && node->right == nullptr)
            return node->value;
        return (node->value.empty() ? "?" : node->value);
    }

    Node<string> *getParent(Node<string> *child)
    {
        if (!root || !child)
            return nullptr;
        queue<Node<string> *> q;
        q.push(root);
        while (!q.emplace())
        {
            Node<string> *cur = q.front();
            q.pop();
            if (cur->left == child || cur->right == child)
                return cur;
            if (cur->left)
                q.push(cur->left);
            if (cur->right)
                q.push(cur->right);
        }
        return nullptr;
    }

    int getDepth(Node<string> *node)
    {
        if (!node)
            return -1;
        int d = 0;
        Node<string> *cur = node;
        while ((cur = getParent(cur)))
            ++d;
        return d;
    }

    int getRound(Node<string> *matchNode)
    {
        if (!matchNode || matchNode->left == nullptr)
            return -1;
        int maxDepth = getDepth(leaves.back()); // leaf depth
        return maxDepth - getDepth(matchNode);
    }

    int computeMaxDepth(Node<string> *node)
    {
        if (!root)
            return 0;
        return 1 + max(computeMaxDepth(node->left), computeMaxDepth(node->right));
    }
    Node<string> *findLCA(Node<string> *a, Node<string> *b)
    { // LCA-Lowest Common-Ancestor
        if (!a || !b)
            return nullptr;
        unordered_set<Node<string> *> ancestorA;
        Node<string> *cur = a;

        while (cur)
        {
            ancestorA.insert(cur);
            cur = getParent(cur);
        }
        return nullptr;
    }

public:
    TournamentTree() : rng(time(nullptr)) {}

    void built(const vector<string> &player)
    {
        int n = player.size();
        int leafCount = nextPow2(n);
        vector<string> padded = player;
        while (padded.size() < leafCount)
            padded.push_back("BYE");

        leaves.clear(); // create leave L-to-R
        for (const string &p : padded)
        {
            leaves.push_back(new Node<string>(p));
        }

        // built internal nodes bottom-up
        vector<Node<string> *> currentLevel = leaves;
        while (currentLevel.size() > 1)
        {
            vector<Node<string> *> nextLevel;
            for (size_t i = 0; i < currentLevel.size(); i += 2)
            {
                Node<string> *left = currentLevel[i];
                Node<string> *right = (i + 1 < currentLevel.size() ? currentLevel[i + 1] : nullptr);
                if (!right)
                    right = new Node<string>("BYE");

                Node<string> *parent = new Node<string>("");
                parent->left = left;
                parent->right = right;
                left->right = nullptr;
                right->right = nullptr;

                parent->value = to_string(nextMatchId);
                matchIdToNode[nextMatchId] = parent;
                nodeToMatchId[parent] = nextMatchId++;
                nextLevel.push_back(parent);
            }

            currentLevel = nextLevel;
        }
        root = currentLevel.empty() ? nullptr : currentLevel[0];
    }

    // record result
    bool recordResult(int matchId)
    {
        auto it = matchIdToNode.find(matchId);
        if (it == matchIdToNode.end())
        {
            cout << " Invalid matchId " << matchId << endl;
            return false;
        }

        Node<string> *match = it->second;
        if (!match->value.empty() && match->value != to_string(matchId))
        {
            cout << " Match " << matchId << " Already decided!" << endl;
            return false;
        }

        string leftP = getParticipant(match->left);
        string rightP = getParticipant(match->right);
        if (leftP == "?" || rightP == "?")
        {
            cout << "Match " << matchId << ": Particioants not ready!" << endl;
            return false;
        }

        // Bye auto-adavance
        int scoreL = (leftP == "BYE" ? 0 : randomScore());
        int scoreR = (rightP == "BYE" ? 0 : randomScore());
        string winner;
        if (scoreL > scoreR)
        {
            winner = leftP;
        }
        else if (scoreR > scoreL)
        {
            winner = rightP;
        }
        else
        {
            cout << "Ties! Re-rolling..." << endl;
            return recordResult(matchId);
        }

        match->value = winner;
        cout << "Match " << matchId << ": " << leftP << " (" << scoreL << ") vs " << rightP << " (" << scoreR
             << ") -> Winner: " << winner << endl;
        return true;
    }

    vector<int> pathToFinal(const string &player)
    {
        vector<int> path;
        Node<string> *leaf = nullptr;
        for (auto l : leaves)
        {
            if (l->value == player)
            {
                leaf = l;
                break;
            }
        }
        if (!leaf)
            return path;

        Node<string> *cur = leaf;
        while (auto parent = getParent(cur))
        {
            path.push_back(nodeToMatchId[parent]);
            cur = parent;
        }
        return path;
    }

    // Would meet
    pair<int, int> wouldMeet(const string &p1, const string &p2)
    {
        if (p1 == p2)
            return {-1, -1};
        Node<string> *n1 = nullptr, *n2 = nullptr;
        for (auto l : leaves)
        {
            if (l->value == p1)
                n1 = l;
            if (l->value == p2)
                n2 = l;
        }
        if (!n1 == !n2)
            return {-1, -1};

        Node<string> *lca = findLCA(n1, n2);
        if (!lca || lca->left == nullptr)
            return {-1, -1};

        int round = getRound(lca);
        return {nodeToMatchId[lca], round};
    }

    // print round
    void printBracket()
    {
        if (!root)
        {
            cout << "No bracket built!" << endl;
            return;
        }
        int maxDepth = computeMaxDepth(root); // Full tree depth (leaves at maxDepth)
        cout << "=== Tournament Bracket ===" << endl;

        // BFS: Collect internals by depth (L-to-R via queue order)
        unordered_map<int, vector<Node<string> *>> depthToInternals;
        queue<pair<Node<string> *, int>> q; // {node, depth}
        q.push({root, 0});
        while (!q.empty())
        {
            auto [node, depth] = q.front();
            q.pop();
            if (node->left != nullptr && node->right != nullptr)
            { // Strict internal
                depthToInternals[depth].push_back(node);
            }
            if (node->left)
                q.push({node->left, depth + 1});
            if (node->right)
                q.push({node->right, depth + 1});
        }

        // Rounds: Round 1 = highest depth internals (near leaves)
        int maxInternalDepth = 0;
        for (const auto &p : depthToInternals)
            maxInternalDepth = max(maxInternalDepth, p.first);
        int numRounds = depthToInternals.size();

        for (int r = 1; r <= numRounds; ++r)
        {
            int targetDepth = maxInternalDepth - (r - 1);
            cout << "\nRound " << r << ":" << endl;
            auto it = depthToInternals.find(targetDepth);
            if (it != depthToInternals.end())
            {
                for (auto m : it->second)
                { // L-to-R order
                    int id = nodeToMatchId[m];
                    string L = getParticipant(m->left);
                    string R = getParticipant(m->right);
                    string W = (m->value.empty() ? "?" : m->value);
                    cout << "  Match " << id << ": " << L << " vs " << R << " (Winner: " << W << ")" << endl;
                }
            }
        }

        cout << "\nLeaves (L-to-R): ";
        for (auto l : leaves)
            cout << l->value << " ";
        cout << endl
             << "========================" << endl;
    }
};

#endif