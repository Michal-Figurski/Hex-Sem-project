#include <SFML/Graphics.hpp>
#include <vector>
#ifndef HEXGRAPH_H
#define HEXGRAPH_H
namespace HEX {
    enum class Turn {
        Empty,
        Player1,
        Player2
    };
    class HexNode {
    public:
        HexNode(){
            static int count=0;
            this->visited = false;
            this->index = count++;
            this->neighbors = std::vector<HEX::HexNode*>();
            this->state = Turn::Empty;
        }

        auto equal(const HEX::HexNode* other) const -> bool {
            return this->index == other->index;
        }

        static auto dist(HEX::HexNode& src, const HEX::HexNode& dest, int depth) -> int{
            src.visited = true;
            if(src.equal(&dest)) return depth;
            int res = -1;
            for(const auto& n : src.neighbors){
                if(!n->visited) res=std::max(0, dist(*n, dest, ++depth));
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

#endif //HEXGRAPH_H
