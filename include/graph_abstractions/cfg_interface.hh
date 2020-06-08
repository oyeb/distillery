#ifndef DISTILLERY_CFG_INTERFACE_HH
#define DISTILLERY_CFG_INTERFACE_HH

#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

#include <graph_abstractions/block_interface.hh>

namespace distillery {
template<typename GraphType>
class CfgInterface
{

	using Cfg = typename GraphType::Cfg;
	using Block = typename GraphType::Block;
	using Statement = typename GraphType::Statement;

	std::string fun_name;
	std::vector<std::unique_ptr<BlockInterface<GraphType>>> blocks;
	BlockInterface<GraphType> *m_entry{ nullptr };
	BlockInterface<GraphType> *m_exit{ nullptr };

public:
	/**
		 * Specialize the constructor for the CFG interface. The constructor should populate these members:
		 *   - func : reference to the original cfg. This reference is never used. Putting there if required later.
		 *   - blocks : vector of blocks (Wrapped in BlockInterface class). Have to be populated in the reverse post order.
		 *   - m_entry : const pointer to the entry block
		 *   - m_exit : const pointer to the exit block
		 *   - Also the successor and predecessor info of each block has to be updated. Use add_successor and
		 *      add_predecessor functions to populate the info.
		 * @param fun The cfg of function.(Not using const because, Gimple does not provide a const iterator for its cfg.)
		 */
	explicit CfgInterface(Cfg &fun);

	const std::string &get_function_name() const
	{
		return fun_name;
	}

	BlockInterface<GraphType> *entry()
	{
		return m_entry;
	}

	const BlockInterface<GraphType> *entry() const
	{
		return m_entry;
	}

	BlockInterface<GraphType> *exit()
	{
		return m_exit;
	}

	const BlockInterface<GraphType> *exit() const
	{
		return m_exit;
	}

	void set_entry(const BlockInterface<GraphType> *node)
	{
		this->m_entry = node;
	}

	void set_exit(const BlockInterface<GraphType> *node)
	{
		this->m_exit = node;
	}

	std::size_t size() const
	{
		return blocks.size();
	}

	void dump(FILE *out = stdout)
	{
		fprintf(out, "Function : %s\n", fun_name.c_str());
		for (auto &block : blocks) {
			block->dump(out);
		}
	}

	void simplify()
	{
		std::unordered_set<BlockInterface<GraphType> *> reachable;
		std::queue<BlockInterface<GraphType> *> work_list;
		work_list.push(m_entry);
		while (not work_list.empty()) {
			BlockInterface<GraphType> *block = work_list.front();
			work_list.pop();
			reachable.insert(block);
			for (BlockInterface<GraphType> *succ : block->successors()) {
				if (reachable.find(succ) == reachable.end()) {
					work_list.push(succ);
				}
			}
		}

		std::vector<std::unique_ptr<BlockInterface<GraphType>>> duplicate_blocks = std::move(blocks);
		blocks.clear();

		std::unordered_set<BlockInterface<GraphType> *> deleted_nodes;

		for (std::unique_ptr<BlockInterface<GraphType>> &block : duplicate_blocks) {
			if (!block) continue;

			if (deleted_nodes.find(block.get()) != deleted_nodes.end()) continue;
			if (reachable.find(block.get()) == reachable.end()) continue;

			auto successor_count = [](const BlockInterface<GraphType> *block_ptr) -> unsigned {
				return block_ptr->successors().size();
			};

			auto predecessor_count = [&reachable](const BlockInterface<GraphType> *block_ptr) -> unsigned {
				unsigned result{ 0 };
				for (auto *pred : block_ptr->predecessors()) {
					if (reachable.find(pred) != reachable.end()) {
						result++;
					}
				}
				return result;
			};

			while (block) {
				if (successor_count(block.get()) != 1) {
					break;
				}

				if (predecessor_count(block->successors()[0]) != 1) {
					break;
				}

				deleted_nodes.insert(block->successors()[0]);
				block->merge(*block->successors()[0]);
			}

			blocks.push_back(std::move(block));
		}

		for (auto &block : blocks) {
			block->predecessors().clear();
		}

		for (auto &block : blocks) {
			for (auto *succ : block->successors()) {
				succ->add_predecessor(block.get());
			}
		}

		for(auto &block : blocks) {
			if(block->predecessors().size() == 0) {
				m_entry = block.get();
			}

			if(block->successors().size() == 0) {
					m_exit = block.get();
			}
		}
	}

	std::vector< std::vector<int> > getAdjacencyMatrix(){
		std::vector< std::vector<int> > AdjMat;
		std::map<BlockInterface<GraphType>*, int> map_to_index;
		int i=0;
		for(auto &block : blocks) {
			AdjMat.push_back(std::vector<int>());
			map_to_index[block.get()]=i;
			i++;
		}
		i=0;
		for(auto &block : blocks) {
			for(BlockInterface<GraphType> *succ : block->successors()) {
				AdjMat[i].push_back(map_to_index[succ]);
			}
			i++;
		}
		return AdjMat;
	}

	bool check_isomorphic(const CfgInterface<GraphType> &cfg){
		return cfg_interface_helper::check_isomorphic(getAdjacencyMatrix(), cfg.getAdjacencyMatrix());
	}

	/*
		 * The iterator of this class iterates over the blocks. The blocks are
		 * iterated in the order during the construction
		 */
	using iterator = typename std::vector<std::unique_ptr<BlockInterface<GraphType>>>::iterator;
	using const_iterator = typename std::vector<std::unique_ptr<BlockInterface<GraphType>>>::const_iterator;
	using reverse_iterator = typename std::vector<std::unique_ptr<BlockInterface<GraphType>>>::reverse_iterator;
	using const_reverse_iterator = typename std::vector<std::unique_ptr<BlockInterface<GraphType>>>::const_reverse_iterator;

	iterator begin()
	{
		return blocks.begin();
	}

	iterator end()
	{
		return blocks.end();
	}

	const_iterator begin() const
	{
		return blocks.begin();
	}

	const_iterator end() const
	{
		return blocks.end();
	}

	reverse_iterator rbegin()
	{
		return blocks.rbegin();
	}

	reverse_iterator rend()
	{
		return blocks.rend();
	}

	const_reverse_iterator rbegin() const
	{
		return blocks.rbegin();
	}

	const_reverse_iterator rend() const
	{
		return blocks.rend();
	}
};

template<typename GraphType>
struct InterfaceWrapper
{
	using Cfg = CfgInterface<GraphType>;
	using Block = BlockInterface<GraphType>;
	using Statement = StatementInterface<GraphType>;
};
}// namespace distillery

namespace cfg_interface_helper{

	std::tuple<bool,std::vector<int>,std::vector<int> > help_check_isomorphic(std::vector< std::vector<int> > &v1, std::vector< std::vector<int> > &v2, std::vector<int> vis1, std::vector<int> vis2, int st1, int st2){
        if(vis1[st1]*vis2[st2]<0){
            return make_tuple(0,vis1,vis2);
        }
        if(vis1[st1]!=-1){
            return make_tuple(1,vis1,vis2);
        }
        vis1[st1]=st2;
        vis2[st2]=st1;
        if(v1[st1].size()!=v2[st2].size()){
            return make_tuple(0,vis1,vis2);
        }
        for(int i=0;i<v1[st1].size();i++){
            bool ch=0;
            for(int j=0;j<v2[st2].size();j++){
                std::tuple<bool,std::vector<int>,std::vector<int> > temp = help_check_isomorphic(v1,v2,vis1,vis2,v1[st1][i],v2[st2][j]);
                if(get<0>(temp)){
                    ch=1;
                    vis1=get<1>(temp);
                    vis2=get<2>(temp);
                    break;
                }
            }
            if(!ch) return make_tuple(0,vis1,vis2);
        }
        return make_tuple(1,vis1,vis2);
    }


    bool check_isomorphic(std::vector< std::vector<int> > &v1, std::vector< std::vector<int> > &v2){
        std::vector<int> vis1(v1.size(),-1), vis2(v2.size(),-1);
        std::tuple<bool,std::vector<int>,std::vector<int> > t=help_check_isomorphic(v1,v2,vis1,vis2,0,0);
        bool k=get<0>(t);
        vis1 = get<1>(t);
        vis2 = get<2>(t);
        if(!k) {
            return k;
        }
        for(int i=0;i<vis1.size();i++){
            if(vis1[i]==-1) return 0;
        }
        for(int i=0;i<vis2.size();i++){
            if(vis2[i]==-1) return 0;
        }
        return 1;
    }
}

#endif//DISTILLERY_CFG_INTERFACE_HH
