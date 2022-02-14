from flask import Flask, render_template, request, redirect, url_for, make_response, jsonify
import os
import json
from datetime import timedelta
import ctypes
from gevent import pywsgi

webpath = "ellye.cn:2021"
localpath = "127.0.0.1:2021"
tag_wl = 1
# tag_wl用于控制使用本地路径还是服务器路径进行访问 1代表local 0代表web

app = Flask(__name__)
# 设置静态文件缓存过期时间
app.send_file_max_age_default = timedelta(seconds=1)
app.config['JSON_AS_ASCII'] = False

CUR_PATH = os.path.dirname(__file__)
dllPath = os.path.join(CUR_PATH, "backend.dll")
pDll = ctypes.WinDLL(dllPath)
pDll.mian.restype = ctypes.c_char_p  #规定返回的数据类型为char*


def getRes(ss):
    print(ss)
    global pDll
    pResut = pDll.mian(ss.encode())
    return pResut.decode()


n, m = 0, 0
graphDir = 1
dicS2I = {}
dicI2S = {}
result = {"nodes": [], "edges": []}
s = ""


def mst(soutp):
    print(soutp)
    global dicS2I, dicI2S
    Outp = {}
    if soutp == "":  # 不可构成最小生成树
        return Outp

    soutp = soutp.strip().replace('\r', '').split('\n')
    for i, line in zip(range(len(soutp)), soutp):
        line = line.strip().split(' ')
        u, v, w = dicI2S[int(line[0])], dicI2S[int(line[1])], line[2]
        Outp[i + 1] = [u, v, w]

    return Outp


def addEdge(u, v, w):
    global n, m, s
    m += 1

    if w > 1:
        result['edges'].append([u, v, {"label": w}])
    else:
        result['edges'].append([u, v])
    if u in dicS2I:
        u = dicS2I[u]
    else:
        n += 1
        dicS2I[u] = n
        dicI2S[n] = u
        result['nodes'].append(u)
        u = n
    if v in dicS2I:
        v = dicS2I[v]
    else:
        n += 1
        dicS2I[v] = n
        dicI2S[n] = v
        result['nodes'].append(v)
        v = n
    s += str(u) + ' ' + str(v) + ' ' + str(w) + '\n'


def align(st, leng=10):
    return st + ' ' * max(leng - len(st), 1)  #+ '\t'


@app.route('/', methods=['POST', 'GET'])
def home():
    return redirect(url_for('index'), code=302)  # 重定向


@app.route('/index', methods=['POST', 'GET'])  # 添加路由
def index():
    path = localpath if (tag_wl == 1) else webpath
    global n, m, graphDir, dicI2S, dicS2I, s, result

    if request.method == 'POST' and request.form.get("message") != "":  # 上传了内容
        #解析图
        n, m = 0, 0
        graphDir = 0 if request.form.get("dir") == '0' else 1
        dicS2I = {}
        dicI2S = {}
        s = ""
        result = {"nodes": [], "edges": []}
        sp = request.form.get("startPoint")

        message = request.form.get("message").strip().replace('\r', '').split('\n')

        for i in message:
            w = 1
            i = i.strip().split(' ')
            i = list(filter(lambda x: x != '', i))
            #print(i)
            if len(i) == 0:
                continue
            elif len(i) > 3:
                continue  # 改为返回报错
            elif len(i) == 3:
                try:
                    w = int(i[2])
                except:
                    w = 1
            u, v = i[0], i[1]
            addEdge(u, v, w)
            if graphDir == 0:
                #result['edges'].append([v, u])
                pass  #太丑了，没研究出来怎么取消箭头

        try:
            n = min(max(int(request.form.get("name")), 0), n)
        except:
            pass
        try:
            m = min(max(request.form.get("email"), 0), m)
        except:
            pass
        try:
            sp = min(max(dicS2I[sp], 1), n)
        except:
            sp = 1

        s = str(graphDir) + ' ' + str(n) + ' ' + str(m) + '\n' + s

        # 制作邻接表
        soutp = getRes(s + "1\n")
        ljbOutp = ""
        sline = soutp.strip().replace('\r', '').split('\n')
        for line in sline:
            line = line.strip().split(' ')
            line = list(filter(lambda x: x != '', line))
            ljbOutp += dicI2S[int(line[0])] + ' : '
            for i in line[1:]:
                ljbOutp += dicI2S[int(i)] + ' → '
            ljbOutp = ljbOutp[:-3] + '\n'

        # 制作多源最短路
        soutp = getRes(s + "4\n")
        soutp = soutp.strip().replace('\r', '').split('\n')
        floydOutp = ""
        for i, line in zip(range(len(soutp)), soutp):
            floydOutp += align(dicI2S[i + 1])
            for dot in line.split(' '):
                floydOutp += align(dot, 5)
            floydOutp += '\n'
        fir = "          "
        for i in range(len(soutp)):
            fir += align(dicI2S[i + 1][:4], 5)
        floydOutp = fir + '\n' + floydOutp

        # 制作单源最短路
        soutp = getRes(s + "3 " + str(sp) + "\n")
        soutp = soutp.strip().replace('\r', '').split('\n')
        dijkOutp = "点 " + dicI2S[sp] + " 到以下各点的距离和最短路径为：\n"
        for i, dis in zip(range(len(soutp)), soutp):
            dis = dis.strip().split(' ')
            dijkOutp += align(dicI2S[i + 1]) + ": " + dis[0] + '    '
            for dot in dis[1:]:
                dijkOutp += dicI2S[int(dot)] + '→'
            dijkOutp = dijkOutp[:-1] + '\n'

        # 产品装配
        s = '1' + s[1:]
        soutp = getRes(s + "5 " + str(sp) + "\n")
        soutp = soutp.strip().replace('\r', '').split('\n')

        topoOutp = "替换零部件" + dicI2S[sp] + "的拆解和安装路径为:\n"
        box = ["拆卸：", "组装："]
        for i, line in zip(range(len(soutp)), soutp):
            topoOutp += box[i]
            line = line.strip().split(' ')
            for dot in line:
                topoOutp += dicI2S[int(dot)] + " → "
            topoOutp = topoOutp[:-3] + '\n'
        topoOutp = topoOutp[:-1]

        # 制作最小生成树
        s = '0' + s[1:]
        primOutp = mst(getRes(s + "2 1\n"))  # prim
        print(primOutp)
        krusOutp = mst(getRes(s + "2 2\n"))  # kruskal
        print(krusOutp)

        return render_template('index.html',
                               _path=path,
                               ljbOutp=ljbOutp,
                               floydOutp=floydOutp,
                               graphJSON=json.dumps(result),
                               message=request.form.get("message"),
                               dijkOutp=dijkOutp,
                               topoOutp=topoOutp,
                               primOutp=json.dumps(primOutp),
                               krusOutp=json.dumps(krusOutp))

    return render_template('index.html',
                           _path=path,
                           message='''
Smith Jones 11
Smith Williams 3
Jones    Williams 5 
Smith Brown 7
Williams Taylor 9
Brown Taylor Davis
Miller Davis Wilson
Taylor Wilson Evans
Williams Wilson 10
Williams Evans 8
Davis Evans   6
   Thomas Johnson   4   
Thomas Miller 2
Thomas Garcia 
Johnson Garcia
Smith Garcia 
''')


if __name__ == '__main__':
    server = pywsgi.WSGIServer(('0.0.0.0', 2021), app)
    server.serve_forever()
    #app.debug = True
    #app.run(host='0.0.0.0', port=2021, debug=True)
