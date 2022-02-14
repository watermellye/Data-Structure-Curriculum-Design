#define DLLEXPORT extern "C" __declspec(dllexport)
#include"pycall.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <queue>
#include <set>
#include <sstream>
#include <string>

using namespace std;
const int N = 10005;  // 最多支持边数
const int M = 1005;   // 最多支持点数
const int INF = 100000000;
int n, m;
int head[N], dis[N], ma[M][M], cnt, fa[N], deg[N], path[N];
bool vis[N];
int ord;
bool dir;

struct e {
   int l, r, w, ne;
   e() : l(), r(), w(), ne() {}
   e(int l, int r, int w, int ne) : l(l), r(r), w(w), ne(ne) {}
   bool operator<(const e& now) const { return w < now.w; };
} e[N * 2];

struct node {
   int nod, d;
   bool operator<(const node& now) const { return d > now.d; }
};

struct node_prim {
   int l, r, d;
   bool operator<(const node_prim& now) const { return d > now.d; }
};

string fea1() {
   string outp = "";
   for (int l = 1; l <= n; ++l) {
      outp += to_string(l) + " ";

      for (int i = head[l]; i; i = e[i].ne) {
         outp += to_string(e[i].r) + " ";
      }
      outp += '\n';
   }
   return outp;
}

string fea2_prim() {
   string outp = "";
   memset(dis, 0x1f, sizeof(dis));
   memset(vis, 0, sizeof(vis));
   priority_queue<node_prim> q;

   dis[1] = 0;
   node_prim temp = { 1, 1, 0 };
   q.push(temp);
   while (!q.empty()) {
      node_prim tp = q.top();
      int k = tp.r;
      q.pop();
      if (vis[k]) continue;
      if (tp.d) outp += to_string(tp.l) + " " + to_string(tp.r) + " " + to_string(tp.d) + "\n";
      vis[k] = 1;
      for (int i = head[k]; i; i = e[i].ne) {
         int r = e[i].r, w = e[i].w;
         if (!vis[r] && dis[r] > w) {
            dis[r] = w;
            temp = { k, r, w };
            q.push(temp);  //优先队列大根堆变小根堆小骚操作：只需一个‘-’号；
         }
      }
   }
   for (int i = 1; i <= n; i++)
      if (dis[i] == 0x1f1f1f1f) {
         outp = "";  // 不存在最小生成树，返回空
         break;
      }
   return outp;
}

int find(int x) { return fa[x] == x ? x : fa[x] = find(fa[x]); }

string fea2_kruskal() {
   string outp = "";
   for (int i = 1; i <= n; i++) fa[i] = i;
   int tt = 0;
   sort(e + 1, e + m + 1);
   for (int i = 1; i <= 2 * m; i++) {
      int l = find(e[i].l), r = find(e[i].r), w = e[i].w;
      if (l != r) {
         tt++;
         fa[l] = r;
         outp += to_string(e[i].l) + " " + to_string(e[i].r) + " " + to_string(w) + "\n";
      }
   }
   if (tt != n - 1) outp = "";
   return outp;
}

string fea3(int s) {
   memset(path, 0, sizeof(path));
   priority_queue<node> q;  // 清空
   while (!q.empty()) q.pop();
   memset(dis, 0, sizeof(dis));
   string outp = "";
   for (int i = 1; i <= n; i++) dis[i] = 2147483647;
   dis[s] = 0;
   memset(vis, 0, sizeof(vis));
   q.push(node{ s, 0 });
   while (!q.empty()) {
      node now = q.top();
      q.pop();
      int l = now.nod;
      if (vis[l]) continue;
      vis[l] = 1;
      for (int i = head[l]; i; i = e[i].ne)
         if (dis[e[i].r] > dis[l] + e[i].w) {
            dis[e[i].r] = dis[l] + e[i].w;
            q.push(node{ e[i].r, dis[e[i].r] });
            path[e[i].r] = l;
         }
   }
   for (int i = 1; i <= n; ++i) {
      if (dis[i] > 2100000000) {
         outp += "-1\n";
         continue;
      }
      int j = i;
      string pa = "";
      while (j) {
         pa = to_string(j) + " " + pa;
         j = path[j];
      }
      outp += to_string(dis[i]) + " " + pa + "\n";
   }
   return outp;
}


string fea4() {
   string outp = "";
   for (int k = 1; k <= n; k++)
      for (int i = 1; i <= n; i++)
         for (int j = 1; j <= n; j++) ma[i][j] = min(ma[i][j], ma[i][k] + ma[k][j]);
   for (int i = 1; i <= n; ++i) {
      for (int j = 1; j <= n; ++j)
         if (i == j)
            outp += "0 ";
         else if (ma[i][j] == INF)
            outp += "-1 ";
         else
            outp += to_string(ma[i][j]) + " ";
      outp += '\n';
   }
   return outp;
}

string fea5(int s) {
   // 先找到和这个零件相关的所有零件
   memset(vis, 0, sizeof(vis));
   int tt = 0;
   queue<int> q;
   q.push(s);
   while (!q.empty()) {
      int now = q.front();
      q.pop();
      if (vis[now]) continue;
      ++tt;
      if (tt > n) return "";

      vis[now] = 1;
      for (int i = head[now]; i; i = e[i].ne) {
         int r = e[i].r;
         if (!vis[r]) q.push(r);
      }
   }

   // 再拓扑排序
   string outp = "";
   string outp2 = "";
   memset(deg, 0, sizeof(deg));
   for (int i = 1; i <= m; ++i)
      if (vis[e[i].l] && vis[e[i].r]) deg[e[i].r]++;
   while (!q.empty()) q.pop();
   q.push(s);
   while (!q.empty()) {
      int now = q.front();
      outp += to_string(now) + " ";
      outp2 = to_string(now) + " " + outp2;
      q.pop();
      for (int i = head[now]; i; i = e[i].ne) {
         int r = e[i].r;
         if (!--deg[r]) q.push(r);
      }
   }

   return outp2 + "\n" + outp;
}

void addEdge(int l, int r, int w) {
   ma[l][r] = w;
   e[++cnt] = { l, r, w, head[l] };
   head[l] = cnt;
}

void init() {
   memset(head, 0, sizeof(head));
   for (int i = 0; i < M; i++)
      for (int j = 0; j < M; j++) ma[i][j] = INF;
   cnt = 0;
}

DLLEXPORT const char* mian(const char* xx) {
   string sinp = xx;
   stringstream ss;
   ss << sinp;

   init();
   ss >> dir;
   dir = bool(dir);
   ss >> n >> m;
   for (int i = 1; i <= m; i++) {
      int l, r, w;
      ss >> l >> r >> w;
      addEdge(l, r, w);
      if (!dir) addEdge(r, l, w);
   }

   ss >> ord;
   string outp;
   int s;
   switch (ord) {
   case 1:
      outp = fea1();
      break;
   case 2:
      ss >> s;
      if (s == 1)
         outp = fea2_prim();
      else if (s == 2)
         outp = fea2_kruskal();
      break;
   case 3:
      ss >> s;
      outp = fea3(s);
      break;
   case 4:
      outp = fea4();
      break;
   case 5:
      ss >> s;
      outp = fea5(s);
      break;
   }
   const char* p = outp.c_str();
   return p;
}

/*
int main() {
    init();
    cout << "算法实现\n1 显示该图的邻接链表\n2 分别用普里姆算法和克鲁斯卡尔算法构造其最小生成树，随时显示其构造的过程\n3 给出某一确定顶点到所有其他顶点的最短路径\n4 "
            "给出每一对顶点之间的最短路径\n\n综合应用\n5 定义一个产品的装配路径，规划拆解和安装路径\n请输入要实现的功能：\n";
    cin >> ord;
    if (ord == 2)
        dir = 0;
    else if (ord == 5)
        dir = 1;
    else {
        cout << "请输入是否为有向图：（1为有向图，0为无向图）";
        cin >> dir;
    }
    dir = bool(dir);
    cout << "请输入图的点数、边数：";
    cin >> n >> m;
    for (int i = 1; i <= m; i++) {
        int l, r, w;
        scanf("%d%d", &l, &r);
        if (ord != 5) scanf("%d", &w);
        11 addEdge(l, r, w);
        if (!dir) addEdge(r, l, w);
    }

    string outp;
    int s;
    switch (ord) {
        case 1:
            outp = fea1();
            break;
        case 2:
            cout << "使用prim算法请输入1，使用kruskal算法请输入2：";
            cin >> s;
            if (s == 1)
                outp = fea2_prim();
            else if (s == 2)
                outp = fea2_kruskal();
            break;
        case 3:
            cout << "请输入起始点：";
            cin >> s;
            outp = fea3(s);
            break;
        case 4:
            outp = fea4();
            break;
        case 5:
            cout << "请输入需要替换的零部件：";
            cin >> s;
            outp = fea5(s);
            break;
    }
    const char* p = outp.c_str();
    cout << p;
    return 0;
}
*/