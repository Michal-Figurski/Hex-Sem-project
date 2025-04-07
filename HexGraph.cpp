//
// Created by Michał Figurski on 4/7/2025.
//

#include "HexGraph.h"

namespace HEX {
    enum class Turn {
        Empty,
        Player1,
        Player2
    };
    class HexNode {
    public:
        static int count;
        HexNode(){
            this->visited = false;
            this->index = count++;
            this->neighbors = std::vector<HEX::HexNode*>();
            this->state = Turn::Empty;
        }

        auto equal(const HEX::HexNode* other) const -> bool {
            return this->index == other->index;
        }

        static auto dist(const HEX::HexNode& src, const HEX::HexNode& dest, int depth) -> int{
            if(src.equal(&dest)) return depth;
            int res = -1;
            for(auto& n : src.neighbors){
                if(n->visited == false) res=std::max(0, dist(*n, dest, ++depth));
            }
            return res;
        }

    private:
        int index;
        bool visited;
        std::vector<HEX::HexNode*> neighbors;
        Turn state;
    };

} // HEX