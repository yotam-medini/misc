#include <iostream>
#include <algorithm>
#include <array>
#include <vector>
#include <map>
using namespace std;
typedef array<int, 2> edge_t;
typedef vector<edge> vedge_t;
typedef vector<int> vi_t;
class Node {
 public:
  Node(int _parent = -1) : parent(_parent) {}
  // int id;
  int parent;
  vi_t child;
};
typedef vector<Node> vnode_t; // tree
typedef map<int, Node> i2node_t;

void build_tree(int& root, i2node_t& tree, const vedge_t& edges) {
  for (const edge_t& e: edges) {
    if (tree.empty()) {
      root = edge[0];
      Node noderoot;
      noderoot.child.push_back(edge[1]);
      tree.insert(tree.end(), i2node_t::value_type(root, noderoot));
      tree.insert(tree.end(), i2node_t::value_type(edge[1], Node()));
    } else {
      if (edge[1] == root) {
        swap(edge[0], edge[1]);
      }
      i2node::iterator iters[2];
      for (int i = 0; i < 2; ++i) {
        iter[i] = tree.find(e[i]);
        if (iter[i) == tree.end()) {
          iter[i] = tree.insert(tree.end(), Node());
        }
      }
      iter[0]->second.child.push_back(edge[1]);
    }
  }
}

int my_depth(const i2node_t& tree, int inode) {
  int d = 0;
  for (i2node_t::iterator iter = tree.find(inode);
    iter != tree.end(); iter = tree.find(iter->second.parent), ++d) 
  {}
  return d;
}

int #include <iostream>
#include <algorithm>
#include <array>
#include <vector>
#include <map>
using namespace std;
typedef array<int, 2> edge_t;
typedef vector<edge> vedge_t;
typedef vector<int> vi_t;
class Node {
 public:
  Node(int _parent = -1) : parent(_parent) {}
  // int id;
  int parent;
  vi_t child;
};
typedef vector<Node> vnode_t; // tree
typedef map<int, Node> i2node_t;

void build_tree(int& root, i2node_t& tree, const vedge_t& edges) {
  for (const edge_t& e: edges) {
    if (tree.empty()) {
      root = edge[0];
      Node noderoot;
      noderoot.child.push_back(edge[1]);
      tree.insert(tree.end(), i2node_t::value_type(root, noderoot));
      tree.insert(tree.end(), i2node_t::value_type(edge[1], Node()));
    } else {
      if (edge[1] == root) {
        swap(edge[0], edge[1]);
      }
      i2node::iterator iters[2];
      for (int i = 0; i < 2; ++i) {
        iter[i] = tree.find(e[i]);
        if (iter[i) == tree.end()) {
          iter[i] = tree.insert(tree.end(), Node());
        }
      }
      iter[0]->second.child.push_back(edge[1]);
    }
  }
}

int upepth(const i2node_t& tree, int inode) {
  int d = 0;
  for (i2node_t::iterator iter = tree.find(inode);
    iter != tree.end(); iter = tree.find(iter->second.parent), ++d) 
  {}
  return d;
}

int depth(const i2node_t& tree, int inode) {
  int d = 0;
  const Node& node = tree.find(inode)->second;
  for (int c: node.child) {
    int cdepth = depth(tree, c);
    if (d < cdepth + 1) {
      d = cdepth + 1;
    }
  }
  return d;
}

void depth_diam(int &depth, int diam, const i2node_t& tree, int inode) {
  int d = 0;
  vi_t max_depths;
  const Node& node = tree.find(inode)->second;
  for (int c: node.child) {
    int cdepth = 0, cidam = 0;
    depth_diam(cdepth, cdiam, tree, c);
    if (diam < cdiam) { diam = cdiam; }
    max_depths.push_back(cdepth);
    sort(max_depths.begin(), max_depths.end(), greater<int>);
    if (max_depths.size() > 2) {
      max_depths.pop_back()
    }
    if (d < cdepth + 1) {
      d = cdepth + 1;
    }
  }
  int sub_diam = accumulate(max_depths.begin(), max_depths.end(), 0);
  if (diam < sub_diam) { diam = sub_diam; }
}

int main(int argc, char** argv) {
  int ret = 0;
  return ret;
}
