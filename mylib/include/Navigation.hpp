﻿#pragma once

#include "MinDefines.hpp"
#include "Vector2.hpp"
#include <vector>

namespace MinConsoleNative
{
#define NAV_UNLIMITED_DEPTH (-1)

    enum class SearchDirection
    {
        Four = 4,
        Eight = 8,
    };

    enum class SearchMethod
    {
        BFS = 1,
        DFS = 2,
    };

    typedef bool (*SearchFunc)(const Vector2& nextPosition);

    struct Node
    {
    public:
        Vector2 position;   //position of this node
        int depth;          //distance to the origin position(Obstacles have been considered).

        Node(Vector2 position, int depth)
        {
            this->position = position;
            this->depth = depth;
        }
    };

    struct SearchResult
    {
    public:
        bool success = false;
        std::vector<Node> searchedNodes;    //All searched nodes
        std::vector<Node> path;             //The result of pathfinding includes the starting point and the ending point
    };

    class Navigation
    {
    private:
        Navigation()
        {
        }

        struct SearchNode
        {
        public:
            Vector2 position = Vector2::zero;
            int depth = 0;
            SearchNode* preNode = nullptr;

            SearchNode(Vector2 position, int depth, SearchNode* preNode)
            {
                this->position = position;
                this->depth = depth;
                this->preNode = preNode;
            }
        };

        static bool ContainsPosition(const std::vector<SearchNode*>& container, const Vector2& element)
        {
            for (int i = 0; i < container.size(); i++)
            {
                if (container[i]->position == element) return true;
            }
            return false;
        }

    public:
        //BFS搜索模式, 输入:搜索深度(距离原点的距离) 输出:搜索节点的数量
        //calculate when SearchDirection == SearchDirection::Four
        //input:depth(Number of searches) output:Number of nodes searched
        static int BFS4_Depth(int depth)
        {
            if (depth < 1) return 1;
            //Arithmetic sequence
            return (4 + 4 * depth) * depth / 2 + 1;
        }

        //BFS搜索模式, 输入:搜索深度(距离原点的距离) 输出:搜索节点的数量
        //calculate when SearchDirection == SearchDirection::Eight
        //input:depth(Number of searches) output:Number of nodes searched
        static int BFS8_Depth(int depth)
        {
            if (depth < 1) return 1;
            return (2 * depth + 1) * (2 * depth + 1);
        }

        static bool ContainsPosition(const std::vector<Node>& container, const Vector2& element)
        {
            for (int i = 0; i < container.size(); i++)
            {
                if (container[i].position == element) return true;
            }
            return false;
        }

        //如果depthLimit == NAV_UNLIMITED_DEPTH则该函数不会因为搜索深度的增加而停止
        //if depthLimit == NAV_UNLIMITED_DEPTH means the program won't get stop because of depth.
        static SearchResult Navigate(Vector2 startPos, Vector2 endPos, SearchDirection searchDir, int depthLimit, const std::vector<Vector2>& obstacles, SearchMethod searchMethod);

        //如果depthLimit == NAV_UNLIMITED_DEPTH则该函数不会因为搜索深度的增加而停止
        //if depthLimit == NAV_UNLIMITED_DEPTH means the program won't get stop because of depth.
        static SearchResult Navigate(Vector2 startPos, Vector2 endPos, SearchDirection searchDir, int depthLimit, SearchFunc func, SearchMethod searchMethod);
    };
}