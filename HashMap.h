#include <vector>
#include <utility>
#include <algorithm>

using std::vector;

template<typename K,typename V,typename Hash>
class HashMap {
    Hash hashFunction;
	vector<vector<std::pair<K,V>>> table;	
	int ct;

public:
    typedef K key_type;
    typedef V mapped_type;
    typedef std::pair<K,V> value_type;


    class const_iterator;


    class iterator {

        decltype(table.begin()) mainIter;
        decltype(table.begin()) mainEnd;
        decltype(table[0].begin()) subIter;
    public:
        friend class const_iterator;



        iterator(const decltype(mainIter) mi,const decltype(mainEnd) me):mainIter(mi),mainEnd(me) {
            if(mainIter!=mainEnd) subIter = mainIter->begin();
            while(mainIter!=mainEnd && subIter == mainIter->end()) {
                ++mainIter;
                if(mainIter!=mainEnd) subIter = mainIter->begin();
            }
        }

        iterator(const decltype(mainIter) mi,
                const decltype(mainEnd) me,
                const decltype(subIter) si):
                mainIter(mi),mainEnd(me),subIter(si) {}


        bool operator==(const iterator &i) const { return mainIter==i.mainIter && (mainIter==mainEnd || subIter==i.subIter); }
        bool operator!=(const iterator &i) const { return !(*this==i); }
        std::pair<K,V> &operator*() { return *subIter; }
        iterator &operator++() {
            ++subIter;
            while(mainIter!=mainEnd && subIter==mainIter->end()) {
                ++mainIter;
                if(mainIter!=mainEnd) subIter = mainIter->begin();
            }
            return *this;
        }
        iterator operator++(int) {
            iterator tmp(*this);
            ++(*this);
            return tmp;
        }
    };


    class const_iterator {

        decltype(table.cbegin()) mainIter;
        decltype(table.cbegin()) mainEnd;
        decltype(table[0].cbegin()) subIter;
    public:

        const_iterator(const decltype(table.cbegin()) mi,const decltype(table.cbegin()) me):mainIter(mi),mainEnd(me) {
            if(mainIter!=mainEnd) subIter = mainIter->begin();
            while(mainIter!=mainEnd && subIter == mainIter->end()) {
                ++mainIter;
                if(mainIter!=mainEnd) subIter = mainIter->begin();
            }
        }

        const_iterator(const decltype(table.begin()) mi,
            const decltype(table.begin()) me,
            const decltype(table[0].begin()) si):
                mainIter(mi),mainEnd(me),subIter(si) {}

        const_iterator(const iterator &i):mainIter(i.mainIter),mainEnd(i.mainEnd),subIter(i.subIter) {


        }


        bool operator==(const const_iterator &i) const { return mainIter==i.mainIter && (mainIter==mainEnd || subIter==i.subIter); }
        bool operator!=(const const_iterator &i) const { return !(*this==i); }
        const std::pair<K,V> &operator*() const { return *subIter; }
        const_iterator &operator++() {
            ++subIter;
            while(mainIter!=mainEnd && subIter==mainIter->end()) {
                ++mainIter;
                if(mainIter!=mainEnd) subIter = mainIter->begin();
            }
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp(*this);
            ++(*this);
            return tmp;
        }
    };

    HashMap(const Hash &hf) : hashFunction{hf},table(7),ct{0} {};
    //HashMap(const HashMap<K,V,Hash> &that);
    //HashMap &operator=(const HashMap<K,V,Hash> &that);
    bool empty() const { return ct == 0; }
    unsigned int size() const { return ct; }
    iterator find(const key_type& k); //done
    const_iterator find(const key_type& k) const; //done
    int count(const key_type& k) const {
		int hc = hashFunction(k);
		int bin = hc % table.size();
		auto itr = find_if(table[bin].begin(),table[bin].end(),[&k](const std::pair<K,V> &p) { return p.first == k; } );
		if (itr != table[bin].end()) {
			return 1;
		} else {
			return 0;
		}
	}
    std::pair<iterator,bool> insert(const value_type& val) {
		if (ct >= table.size()) { growTableAndRehash(); }
		K key = val.first;
		int hc = hashFunction(key);
		int bin = hc % table.size();
		auto itr = find_if(table[bin].begin(),table[bin].end(),[&key](const std::pair<K,V> &p) { return p.first == key; } );
		if (itr == table[bin].end()) {	
			++ct;
			table[bin].push_back(val);
			auto itr2 = find_if(table[bin].begin(),table[bin].end(),[&key](const std::pair<K,V> &p) {return p.first == key; } );
			auto itaboy = iterator(table.begin()+bin,table.end(),itr2);
			std::pair<iterator,bool> ret(itaboy,true);
			return ret;
		} else {
			auto itaboy = iterator(table.begin()+bin,table.end(),itr);
			std::pair<iterator,bool> ret(itaboy,false);
			return ret;
		}
	}

    template <class InputIterator>
    void insert(InputIterator first, InputIterator last) {
		if (ct >= table.size()) { growTableAndRehash(); }
		for(auto itr = first; itr != last; ++itr) {
			insert(*itr);
			/*
			K key = (*itr).first;
			V val = (*itr).second;
			int hc = hashFunction(key);
			int bin = hc % table.size();
			auto itr2 = find_if(table[bin].begin(),table[bin].end(),[&key](const std::pair<K,V> &p) {return p.first == key; } );
			if (itr2 == table[bin].end()) {
				table[bin].push_back(make_pair(key,val));
				++ct;
			}*/
		}
    }

    iterator erase(const_iterator position) {
		if(position == cend()) return end();
		K key = (*position).first;
		int hc = hashFunction(key);
		int bin = hc % table.size();
		auto itr = find_if(table[bin].begin(),table[bin].end(),
				[&key](const std::pair<K,V> &p){ return p.first == key;});
		if (itr != table[bin].end()) {
			auto ret = iterator(table.begin()+bin,table.end(),itr);
			++ret;
			table[bin].erase(itr);
			--ct;
			return ret;
		} else {
			return end();
		}
	}

    int erase(const key_type& key) {
		int hc = hashFunction(key);
		int bin = hc % table.size();
		auto itr = find_if(table[bin].begin(),table[bin].end(),
				[&key] (const std::pair<K,V> &p){ return p.first == key;});
		if (itr != table[bin].end()) {
			table[bin].erase(itr);
			--ct;
			return 1;
		} else {
			return 0;
		}
	}
    void clear() {
		for(int bin = 0; bin != table.size(); ++bin) {
			table[bin].clear();
		}
		ct = 0;
	}
    mapped_type &operator[](const K &key) {
		if (ct == table.size()) { growTableAndRehash(); }
		int hc = hashFunction(key);
		int bin = hc % table.size();
		// use find
		auto itr = find_if(table[bin].begin(),table[bin].end(),
				[&key](const std::pair<K,V> &p){ return p.first == key;});
		if (itr != table[bin].end()) {
			return itr->second;
		} else {
			++ct;
			table[bin].push_back(make_pair(key,V()));
			return table[bin].back().second;
		}
	};
    bool operator==(const HashMap<K,V,Hash>& rhs) const {
		if (size() != rhs.size()) return false;
		for( auto &p : *this ) {
			int hc = hashFunction(p.first);
			int bin = hc % rhs.table.size();
			// use find
			auto itr = find_if(rhs.table[bin].begin(),rhs.table[bin].end(),
				[&p](const std::pair<K,V> &p2){ return p.first == p2.first && p.second == p2.second;});
			if (itr == rhs.table[bin].end()) return false;
		}
		return true;
	}
    bool operator!=(const HashMap<K,V,Hash>& rhs) const { return !(*this == rhs); }
    iterator begin() { return iterator(table.begin(),table.end()); };
    const_iterator begin() const { return const_iterator(table.begin(),table.end()); };
    iterator end() {return iterator(table.end(),table.end()); };
    const_iterator end() const { return const_iterator(table.end(), table.end()); };
    const_iterator cbegin() const { return const_iterator(table.begin(), table.end()); };
    const_iterator cend() const { return const_iterator(table.end(), table.end()); };

private:
    void growTableAndRehash() {
		vector<vector<std::pair<K,V>>> newTable(ct*2+1);
		for (auto &p : *this) {
			K key = p.first;
			int hc = hashFunction(key);
			int bin = hc % newTable.size();
			newTable[bin].push_back(make_pair(key,p.second));
		}
		table = std::move(newTable);
	}
};

template<typename K, typename V, typename Hash>
typename HashMap<K,V,Hash>::const_iterator HashMap<K,V,Hash>::find(const key_type& key) const {
	int hc = hashFunction(key);
	int bin = hc % table.size();
	auto itr = find_if(table[bin].begin(),table[bin].end(),
			[&key](const std::pair<K,V> &p){ return p.first == key;});
	if (itr != table[bin].end()) {
		return const_iterator(table.cbegin()+bin,table.cend(),itr);
	} else {
		return cend();
	}

}
template<typename K, typename V, typename Hash>
typename HashMap<K,V,Hash>::iterator HashMap<K,V,Hash>::find(const key_type& key) {
	int hc = hashFunction(key);
	int bin = hc % table.size();
	auto itr = find_if(table[bin].begin(),table[bin].end(),
			[&key](const std::pair<K,V> &p){ return p.first == key;});
	if (itr != table[bin].end()) {
		return iterator(table.begin()+bin,table.end(),itr);
	} else {
		return end();
	}

}
