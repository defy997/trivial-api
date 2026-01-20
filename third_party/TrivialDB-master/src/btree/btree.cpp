/**
 * @file btree.cpp
 * @brief B树数据结构实现
 *
 * 这个文件实现了B树数据结构，支持：
 * - 插入操作（自动处理页面分裂）
 * - 删除操作（自动处理页面合并和借用）
 * - 查找操作（lower_bound查找）
 * - 支持多种键类型（整数、字符串等）
 * - 模板化设计，支持自定义比较器和复制器
 *
 * B树特点：
 * - 自平衡的多路搜索树
 * - 所有叶子节点在同一层
 * - 节点可以有多个子节点
 * - 支持高效的磁盘I/O操作
 */

#include "btree.h"
#include "../algo/search.h"

/**
 * B树模板类构造函数
 * @param pg 页面管理器，用于管理磁盘页面
 * @param root_page_id 根节点页面ID，如果为0则创建新树
 * @param field_size 字段大小，用于页面初始化
 * @param compare 键比较函数
 * @param copier 键复制函数，用于临时存储
 */
template<typename KeyType, typename Comparer, typename Copier>
btree<KeyType, Comparer, Copier>::btree(
		pager *pg, int root_page_id, int field_size,
		Comparer compare, Copier copier)
	: pg(pg), root_page_id(root_page_id),
	  field_size(field_size), compare(compare), copy_to_temp(copier)
{
	// 如果根节点不存在，创建一个新的叶节点页面
	if(root_page_id == 0)
	{
		this->root_page_id = pg->new_page();
		leaf_page { pg->read_for_write(this->root_page_id), pg }.init(field_size);
	}
}

/**
 * 处理根节点分裂的情况
 * 当插入操作导致根节点需要分裂时，创建一个新的根节点
 * @tparam Page 页面类型（叶节点或内节点）
 * @param ret 插入操作的返回结果，包含分裂信息
 */
template<typename KeyType, typename Comparer, typename Copier>
template<typename Page>
inline void btree<KeyType, Comparer, Copier>::insert_split_root(insert_ret ret)
{
	// 如果发生了分裂，需要创建新的根节点
	if(ret.split)
	{
		debug_puts("B-tree split root.");
		// 分配新的页面作为新的根节点
		int new_pid = pg->new_page();
		interior_page page { pg->read_for_write(new_pid), pg };
		page.init(field_size);

		// 获取分裂后的两个半页
		Page lower { ret.lower_half, pg };
		Page upper { ret.upper_half, pg };

		// 在新根节点中插入两个子节点：
		// 第一个子节点指向原来的根，第二个子节点指向分裂出的新页面
		page.insert(0, lower.get_key(lower.size() - 1), root_page_id);
		page.insert(1, upper.get_key(upper.size() - 1), ret.upper_pid);
		root_page_id = new_pid;
	}
}

/**
 * 向B树中插入键值对的主入口函数
 * @param key 要插入的键
 * @param data 要插入的数据
 * @param data_size 数据大小
 */
template<typename KeyType, typename Comparer, typename Copier>
void btree<KeyType, Comparer, Copier>::insert(
		key_t key, const char *data, int data_size)
{
	// 读取根节点页面
	char *addr = pg->read_for_write(root_page_id);
	uint16_t magic = general_page::get_magic_number(addr);

	// 根据根节点类型调用不同的插入函数
	if(magic == PAGE_FIXED)
	{
		// 根节点是内节点，调用内节点插入函数
		insert_ret ret = insert_interior(
			root_page_id, addr, key, data, data_size);
		// 处理可能的根节点分裂
		insert_split_root<interior_page>(ret);
	} else {
		// 根节点是叶节点，调用叶节点插入函数
		assert(magic == PAGE_VARIANT || magic == PAGE_INDEX_LEAF);
		insert_ret ret = insert_leaf(
			root_page_id, addr, key, data, data_size);
		// 处理可能的根节点分裂
		insert_split_root<leaf_page>(ret);
	}
}

/**
 * 插入操作的后处理函数
 * 处理子节点分裂后对父节点的影响
 * @tparam Page 当前页面类型
 * @tparam ChPage 子页面类型
 * @param pid 当前页面ID
 * @param ch_pid 子页面ID
 * @param ch_pos 子节点在父节点中的位置
 * @param ch_ret 子节点插入操作的结果
 * @return 处理后的插入结果
 */
template<typename KeyType, typename Comparer, typename Copier>
template<typename Page, typename ChPage>
inline typename btree<KeyType, Comparer, Copier>::insert_ret
btree<KeyType, Comparer, Copier>::insert_post_process(
	int pid, int ch_pid, int ch_pos, insert_ret ch_ret)
{
	insert_ret ret;
	ret.split = false;
	Page page { pg->read_for_write(pid), pg };

	// 如果子节点发生了分裂
	if(ch_ret.split)
	{
		// 获取分裂后的两个子页面
		ChPage lower_ch { ch_ret.lower_half, pg };
		ChPage upper_ch { ch_ret.upper_half, pg };

		// 更新现有子节点的最大键
		page.set_key(ch_pos, lower_ch.get_key(lower_ch.size() - 1));

		// 复制新子节点的最大键用于插入
		key_t ch_largest = copy_to_temp(upper_ch.get_key(upper_ch.size() - 1));

		// 尝试在当前位置后插入新的子节点指针
		bool succ_ins = page.insert(ch_pos + 1, ch_largest, ch_ret.upper_pid);

		// 如果插入失败（页面已满），需要分裂当前页面
		if(!succ_ins)
		{
			// 分裂当前页面
			auto upper = page.split(pid);
			Page upper_page = upper.second;
			Page lower_page = page;

			// 根据插入位置决定在哪个半页中插入新节点
			if(ch_pos < lower_page.size())
			{
				// 插入位置在下半页
				succ_ins = lower_page.insert(
					ch_pos + 1, ch_largest, ch_ret.upper_pid);
				assert(succ_ins);
			} else {
				// 插入位置在上半页
				succ_ins = upper_page.insert(
					ch_pos - lower_page.size() + 1,
					ch_largest, ch_ret.upper_pid
				);
				assert(succ_ins);
			}

			// 返回分裂结果
			ret.split = true;
			ret.lower_half = lower_page.buf;
			ret.upper_half = upper_page.buf;
			ret.upper_pid  = upper.first;
		}
	} else {
		// 子节点没有分裂，只需要更新最大键
		ChPage ch_page { pg->read(ch_pid), pg };
		page.set_key(ch_pos, ch_page.get_key(ch_page.size() - 1));
	}

	return ret;
}

/**
 * 在内节点中插入键值对
 * 递归向下查找合适的子节点进行插入
 * @param now 当前内节点页面ID
 * @param addr 当前内节点页面地址
 * @param key 要插入的键
 * @param data 要插入的数据
 * @param data_size 数据大小
 * @return 插入操作的结果
 */
template<typename KeyType, typename Comparer, typename Copier>
typename btree<KeyType, Comparer, Copier>::insert_ret
btree<KeyType, Comparer, Copier>::insert_interior(
	int now, char* addr, key_t key, const char *data, int data_size)
{
	interior_page page { addr, pg };

	// 使用二分查找找到合适的子节点位置
	int ch_pos = ::lower_bound(0, page.size(), [&](int id) {
		return compare(page.get_key(id), key) < 0;
	} );

	// 确保位置不越界
	ch_pos = std::min(page.size() - 1, ch_pos);

	// 获取子节点页面ID并读取
	int ch_pid = page.get_child(ch_pos);
	char *ch_addr = pg->read_for_write(ch_pid);
	uint16_t ch_magic = general_page::get_magic_number(ch_addr);

	// 根据子节点类型递归调用相应的插入函数
	if(ch_magic == PAGE_FIXED)
	{
		// 子节点是内节点，递归调用内节点插入
		auto ch_ret = insert_interior(ch_pid, ch_addr, key, data, data_size);
		return insert_post_process<interior_page, interior_page>(
			now, ch_pid, ch_pos, ch_ret
		);
	} else {
		// 子节点是叶节点，调用叶节点插入
		assert(ch_magic == PAGE_VARIANT || ch_magic == PAGE_INDEX_LEAF);
		auto ch_ret = insert_leaf(ch_pid, ch_addr, key, data, data_size);
		return insert_post_process<interior_page, leaf_page>(
			now, ch_pid, ch_pos, ch_ret
		);
	}
}

/**
 * 在叶节点中插入键值对
 * 这是实际存储数据的地方，如果页面已满则进行分裂
 * @param now 当前叶节点页面ID
 * @param addr 当前叶节点页面地址
 * @param key 要插入的键（用于查找位置）
 * @param data 要插入的数据
 * @param data_size 数据大小
 * @return 插入操作的结果
 */
template<typename KeyType, typename Comparer, typename Copier>
typename btree<KeyType, Comparer, Copier>::insert_ret 
btree<KeyType, Comparer, Copier>::insert_leaf(
	int now, char* addr, key_t key, const char *data, int data_size)
{
	leaf_page page { addr, pg };

	// 使用二分查找找到插入位置
	int ch_pos = ::lower_bound(0, page.size(), [&](int id) {
		return compare(page.get_key(id), key) < 0;
	} );

	insert_ret ret;
	ret.split = false;

	/*
	 * 当leaf_page是variant_page时，insert按原始含义工作
	 * 当leaf_page是fixed_page时，data_size被视为子节点指针，
	 * data和key都被视为键
	 */
	bool succ_ins = page.insert(ch_pos, data, data_size);

	// 如果插入失败（页面已满），需要分裂页面
	if(!succ_ins)
	{
		// 分裂当前页面，返回上下两个半页
		auto upper = page.split(now);

		leaf_page upper_page = upper.second;
		leaf_page lower_page = page;

		// 根据插入位置决定在哪个半页中插入数据
		if(ch_pos < lower_page.size())
		{
			// 插入位置在下半页
			succ_ins = lower_page.insert(ch_pos, data, data_size);
			assert(succ_ins);
		} else {
			// 插入位置在上半页
			succ_ins = upper_page.insert(
				ch_pos - lower_page.size(), data, data_size);
			assert(succ_ins);
		}

		// 返回分裂结果
		ret.split = true;
		ret.lower_half = lower_page.buf;
		ret.upper_half = upper_page.buf;
		ret.upper_pid  = upper.first;
	}

	return ret;
}

/**
 * 查找大于等于指定键的最小元素（lower_bound）
 * 从根节点开始递归查找
 * @param key 要查找的键
 * @return 查找结果，包含页面ID和位置
 */
template<typename KeyType, typename Comparer, typename Copier>
typename btree<KeyType, Comparer, Copier>::search_result 
btree<KeyType, Comparer, Copier>::lower_bound(key_t key)
{
	return lower_bound(root_page_id, key);
}

/**
 * 在指定页面中进行lower_bound查找
 * @param now 当前页面ID
 * @param key 要查找的键
 * @return 查找结果
 */
template<typename KeyType, typename Comparer, typename Copier>
typename btree<KeyType, Comparer, Copier>::search_result
btree<KeyType, Comparer, Copier>::lower_bound(int now, key_t key)
{
	char *addr = pg->read_for_write(now);
	uint16_t magic = general_page::get_magic_number(addr);

	if(magic == PAGE_FIXED)
	{
		// 当前是内节点，继续向下查找
		interior_page page { addr, pg };
		int ch_pos = ::lower_bound(0, page.size(), [&](int id) {
			return compare(page.get_key(id), key) < 0;
		} );

		ch_pos = std::min(page.size() - 1, ch_pos);
		// 递归查找子节点
		return lower_bound(page.get_child(ch_pos), key);
	} else {
		// 当前是叶节点，在叶节点中查找
		assert(magic == PAGE_VARIANT || magic == PAGE_INDEX_LEAF);
		leaf_page page { addr, pg };
		int pos = ::lower_bound(0, page.size(), [&](int id) {
			return compare(page.get_key(id), key) < 0;
		} );

		// 如果没有找到合适的元素，返回无效结果
		if(pos == page.size())
			return { 0, 0 };
		else return { now, pos };
	}
}

/**
 * 尝试合并页面以处理下溢（underflow）
 * 当页面元素过少时，尝试从兄弟节点借用元素或与兄弟节点合并
 * @tparam Page 页面类型
 * @param pid 当前页面ID
 * @param addr 当前页面地址
 * @return 合并操作的结果
 */
template<typename KeyType, typename Comparer, typename Copier>
template<typename Page>
typename btree<KeyType, Comparer, Copier>::merge_ret
btree<KeyType, Comparer, Copier>::erase_try_merge(int pid, char *addr)
{
	Page page { addr, pg };

	// 检查是否发生下溢（元素过少）
	if(page.underflow())
	{
		char *next_addr = nullptr, *prev_addr = nullptr;

		// 尝试从右兄弟节点借用元素
		if(page.next_page())
		{
			next_addr = pg->read(page.next_page());
			Page next_page { next_addr, pg };
			// 检查右兄弟能否借出一个元素而不下溢
			if(!next_page.underflow_if_remove(0))
			{
				pg->mark_dirty(page.next_page());
				// 从右兄弟移动一个元素到当前页面
				page.move_from(next_page, 0, page.size());
				return { false, false, 0 };
			}
		}

		// 尝试从左兄弟节点借用元素
		if(page.prev_page())
		{
			prev_addr = pg->read(page.prev_page());
			Page prev_page { prev_addr, pg };
			// 检查左兄弟能否借出一个元素而不下溢
			if(!prev_page.underflow_if_remove(prev_page.size() - 1))
			{
				pg->mark_dirty(page.prev_page());
				// 从左兄弟移动一个元素到当前页面
				page.move_from(prev_page, prev_page.size() - 1, 0);
				return { false, false, 0 };
			}
		}

		// 如果无法借用，只能合并页面
		if(next_addr)
		{
			// 与右兄弟合并
			int next_pid = page.next_page();
			bool succ_merge = page.merge( { next_addr, pg }, pid);
			UNUSED(succ_merge);
			assert(succ_merge);
			pg->free_page(next_pid); // 释放右兄弟页面
			return { false, true, pid };
		} else if(prev_addr) {
			// 与左兄弟合并
			int prev_pid = page.prev_page();
			Page prev_page { prev_addr, pg };
			bool succ_merge = prev_page.merge(page, prev_pid);
			UNUSED(succ_merge);
			assert(succ_merge);
			pg->free_page(pid); // 释放当前页面
			return { true, false, prev_pid };
		}
	} 

	// 没有发生下溢或无法处理，返回无操作
	return { false, false, 0 };
}

/**
 * 在指定页面中删除键值对
 * 递归处理删除操作，包括节点合并和重新平衡
 * @param now 当前页面ID
 * @param key 要删除的键
 * @return 删除操作的结果
 */
template<typename KeyType, typename Comparer, typename Copier>
typename btree<KeyType, Comparer, Copier>::erase_ret
btree<KeyType, Comparer, Copier>::erase(int now, key_t key)
{
	char *addr = pg->read_for_write(now);
	uint16_t magic = general_page::get_magic_number(addr);

	if(magic == PAGE_FIXED)
	{
		// 当前是内节点
		interior_page page { addr, pg };
		// 找到合适的子节点
		int ch_pos = ::lower_bound(0, page.size(), [&](int id) {
			return compare(page.get_key(id), key) < 0;
		} );

		ch_pos = std::min(page.size() - 1, ch_pos);
		// 递归删除子节点中的元素
		erase_ret ret = erase(page.get_child(ch_pos), key);

		if(!ret.found) return ret;

		// 重新读取页面（可能已被修改）
		addr = pg->read_for_write(now);
		page = interior_page { addr, pg };

		// 根据子节点的合并情况更新父节点
		if(ret.merged_right)
		{
			// 右子节点被合并，删除对应的指针并更新键
			page.erase(ch_pos + 1);
			page.set_key(ch_pos, ret.largest);
			page.set_child(ch_pos, ret.merged_pid);
		} else if(ret.merged_left) {
			// 左子节点被合并，删除对应的指针并更新键
			page.erase(ch_pos);
			page.set_key(ch_pos - 1, ret.largest);
			page.set_child(ch_pos - 1, ret.merged_pid);
		} else {
			// 没有合并，只需要更新最大键
			page.set_key(ch_pos, ret.largest);
		}

		// 检查当前页面是否需要合并
		merge_ret mret = erase_try_merge<interior_page>(now, addr);
		return { true, mret.merged_left, mret.merged_right,
			mret.merged_pid, copy_to_temp(page.get_key(page.size() - 1)) };
	} else {
		// 当前是叶节点
		assert(magic == PAGE_VARIANT || magic == PAGE_INDEX_LEAF);
		leaf_page page { addr, pg };

		// 在叶节点中查找要删除的元素
		int pos = ::lower_bound(0, page.size(), [&](int id) {
			return compare(page.get_key(id), key) < 0;
		} );

		// 如果没找到或键不匹配，返回未找到
		if(pos == page.size() || compare(page.get_key(pos), key) != 0)
			return { false, false, false, 0, 0};

		// 删除找到的元素
		page.erase(pos);

		// 检查是否需要合并
		auto ret = erase_try_merge<leaf_page>(now, addr);

		// 对于根节点，不返回最大键（因为根节点没有父节点）
		return { true, ret.merged_left, ret.merged_right, ret.merged_pid,
			now == root_page_id ? 0 : copy_to_temp(page.get_key(page.size() - 1)) };
	}
}

/**
 * 删除操作的公共接口
 * @param key 要删除的键
 * @return 是否成功找到并删除了元素
 */
template<typename KeyType, typename Comparer, typename Copier>
bool btree<KeyType, Comparer, Copier>::erase(key_t key)
{
	// 调用内部删除函数
	erase_ret ret = erase(root_page_id, key);

	// 检查根节点是否需要收缩
	char *addr = pg->read_for_write(root_page_id);
	uint16_t magic = general_page::get_magic_number(addr);
	if(magic == PAGE_FIXED)
	{
		interior_page page { addr, pg };
		// 如果根节点只有一个子节点，收缩树的高度
		if(page.size() == 1 && page.get_child(0))
		{
			debug_puts("B-tree merge root.");
			pg->free_page(root_page_id); // 释放旧根节点
			root_page_id = page.get_child(0); // 子节点成为新根
		}
	}

	return ret.found;
}

/* 显式实例化模板类，确保编译器生成这些特定类型的代码 */
template class btree<int, int(*)(int, int), int(*)(int)>;  // 整数类型B树
template class btree<const char*,                           // 字符串类型B树（用于索引）
		 index_btree::comparer_t,
		 __impl::index_btree_copier_t
	 >;
