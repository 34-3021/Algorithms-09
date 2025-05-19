<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.css" integrity="sha384-nB0miv6/jRmo5UMMR1wu3Gz6NLsoTkbqJghGIsx//Rlm+ZU03BU6SQNC66uf4l5+" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.js" integrity="sha384-7zkQWkzuo3B5mTepMUcHkMB5jZaolc2xDwL6VFqjFALcbeS9Ggm/Yr2r3Dy4lfFg" crossorigin="anonymous"></script>


# 寻找样本DNA序列中的重复片段

23300240009 佟铭洋

## 算法

采用图论算法。显然我们需要用的是最短路算法（而不是最长路）。原因是：最长路算法的时间复杂度太烂了。

我们逆向思维一下，最大化得分就是最小化代价。这样我们可以考虑用最短路算法来实现。

设 $ref$ 为参考序列，$query$ 为查询序列。另外预处理一个 $ref$ 的互补（但不反向）序列 $ref_{rev}$。
$ref$ 和 $query$ 的长度分别为 $n$ 和 $m$。下标从 1 开始。

我们认为有以下的点：

- 第 A 类：$NodeA(i,j)$ 表示正向匹配完 $ref[i]$ 与 $query[j]$ 的状态。 
- 第 B 类：$NodeB(i,j)$ 表示反向匹配完 $ref[i]$ 与 $query[j]$ 的状态。 
- 第 C 类：$NodeC(k)$ 表示某一匹配段以 $query[k]$ 结尾的状态。

对于 A 类点为起点的边：

- $NodeA(i,j) \to NodeA(i+1,j+1)$ ，当 $ref[i+1]$==$query[j+1]$ 边权为 0，否则边权为 1。
- $NodeA(i,j) \to NodeA(i+1,j)$ ，边权为 1。
- $NodeA(i,j) \to NodeA(i,j+1)$ ，边权为 1。
- $NodeA(i,j) \to NodeC(j)$ ，边权为 1。

对于 B 类点为起点的边：

- $NodeB(i,j) \to NodeB(i-1,j+1)$ ，当 $ref_{rev}[i-1]$==$query[j+1]$ 边权为 0，否则边权为 1。
- $NodeB(i,j) \to NodeB(i-1,j)$ ，边权为 1。
- $NodeB(i,j) \to NodeB(i,j+1)$ ，边权为 1。
- $NodeB(i,j) \to NodeC(j)$ ，边权为 1。

对于 C 类点为起点的边：

- $NodeC(j) \to NodeA(i,j)$ ，边权为 0。
- $NodeC(j) \to NodeB(i,j)$ ，边权为 0。

因此，总代价就是 $NodeC(0)$ 到 $NodeA(k,m)$ 的最短路径。

## 复杂度分析

### 初步探索，多了一个 $\log$ 的复杂度

其实这个算法是一个最短路径算法。我们可以用 Dijkstra 算法来实现。考虑到点的个数有 $O(nm)$ 个。其中：
- A 类点有 $\Theta(nm)$ 个，每个 A 类点有至多 $4$ 条边，这是 $O(nm)$。
- B 类点有 $\Theta(nm)$ 个，每个 B 类点有至多 $4$ 条边，这是 $O(nm)$。
- C 类点有 $\Theta(m)$ 个，每个 C 类点有至多 $n$ 条边，这是 $O(nm)$。

故点数为 $O(nm)$，边数为 $O(nm)$。
- 使用 Dijkstra 算法，时间复杂度为 $O((nm) \log(nm))$。

**这个时间复杂度并不满足不差于平方量级的要求，我们需要进一步优化。**

### 去掉 $\log$ 的复杂度

这个 $\log(nm)$ 的复杂度是由于优先队列中最差情况可能会有 $O(nm)$ 个元素。问题来自于优先队列本身。

考虑到 Dijkstra 算法的一个特例：对于 0-1 边权的图，这可以退化为 BFS 算法。我们可以使用双端队列来实现，由于队列的复杂度为 $O(1)$，所以我们可以将复杂度降到 $O(nm)$。
- 使用双端队列，时间复杂度为 $O(nm)$。

## 伪代码：
<!--According to the Node type-->
<pre class="pseudocode">
\begin{algorithm}
\caption{ProcessOfNode}
\begin{algorithmic}
\FUNCTION{Process}{$Node,queue$}
\STATE $i \gets Node.i$
\STATE $j \gets Node.j$
\IF{$Node.type$==$A$}
    \IF{$i + 1 \leq |ref| $ \AND $j + 1 \leq |query| $}
        \STATE $weight \gets 0, if (reference[i + 1] == query[j + 1]), else, 1$
        \IF{$dis[NodeA(i+1,j+1)] > dis[Node] + weight$}
            \STATE $dis[NodeA(i+1,j+1)] \gets dis[Node] + weight$
            \IF{$weight == 0$}
                \STATE $queue.front \gets (NodeA(i+1,j+1))$
            \ELSE
                \STATE $queue.back \gets (NodeA(i+1,j+1))$
            \ENDIF
        \ENDIF
    \ENDIF
    \IF{$i + 1 \leq |ref|$}
        \IF{$dis[NodeA(i+1,j)] > dis[Node] + 1$}
            \STATE $dis[NodeA(i+1,j)] \gets dis[Node] + 1$
            \STATE $queue.back \gets (NodeA(i+1,j))$
        \ENDIF
    \ENDIF
    \IF{$j + 1 \leq |query|$}
        \IF{$dis[NodeA(i,j+1)] > dis[Node] + 1$}
            \STATE $dis[NodeA(i,j+1)] \gets dis[Node] + 1$
            \STATE $queue.back \gets (NodeA(i,j+1))$
        \ENDIF
    \ENDIF
\ELIF{$Node.type$==$B$}
    \IF{$i - 1 \geq 0$ \AND $j + 1 \leq |query|$}
        \STATE $weight \gets 0, if (reference_{rev}[i - 1] == query[j + 1]), else, 1$
        \IF{$dis[NodeB(i-1,j+1)] > dis[Node] + weight$}
            \STATE $dis[NodeB(i-1,j+1)] \gets dis[Node] + weight$
            \IF{$weight == 0$}
                \STATE $queue.front \gets (NodeB(i-1,j+1))$
            \ELSE
                \STATE $queue.back \gets (NodeB(i-1,j+1))$
            \ENDIF
        \ENDIF
    \ENDIF
    \IF{$i - 1 \geq 0$}
        \IF{$dis[NodeB(i-1,j)] > dis[Node] + 1$}
            \STATE $dis[NodeB(i-1,j)] \gets dis[Node] + 1$
            \STATE $queue.back \gets (NodeB(i-1,j))$
        \ENDIF
    \ENDIF
    \IF{$j + 1 \leq |query|$}
        \IF{$dis[NodeB(i,j+1)] > dis[Node] + 1$}
            \STATE $dis[NodeB(i,j+1)] \gets dis[Node] + 1$
            \STATE $queue.back \gets (NodeB(i,j+1))$
        \ENDIF
    \ENDIF
\ELIF{$Node.type$==$C$}
    \FOR {$i \gets 0$ \TO $|ref|$}
        \IF{$dis[NodeA(i,j)] > dis[Node]$}
            \STATE $dis[NodeA(i,j)] \gets dis[Node]$
            \STATE $queue.front \gets (NodeA(i,j))$
        \ENDIF
        \IF{$dis[NodeB(i,j)] > dis[Node]$}
            \STATE $dis[NodeB(i,j)] \gets dis[Node]$
            \STATE $queue.front \gets (NodeB(i,j))$
        \ENDIF
    \ENDFOR
\ENDIF
\ENDFUNCTION
\end{algorithmic}
\end{algorithm}
</pre>

<pre class="pseudocode">
\begin{algorithm}
\caption{ProcessQueue}
\begin{algorithmic}
\FUNCTION{ProcessQueue}{$queue$}
\WHILE{$queue$ is not empty}
    \STATE $Node \gets queue.front$
    \IF{$Node.j$ == $m$}
        \STATE \RETURN
    \ENDIF
    \STATE $queue.pop()$
    \IF{$visited[Node]$}
        \STATE \CONTINUE
    \ENDIF
    \STATE $visited[Node] \gets true$
    \STATE \CALL{Process}{$Node,queue$}
\ENDWHILE
\ENDFUNCTION
\end{algorithmic}
\end{algorithm}
</pre>

## 结果

最终，可以在第一个样例拿到 29.82k（距离基线 0），在第二个样例拿到 2103（高于基线 13）。

## 程序

- `main2.cpp` **需要使用 C++20 编译，因为使用了 std::unordered_map::contains 这个语法糖，若没有对应环境请改成std::unordered_map::find != std::unordered_map::end**

常数比较大，而且需要遍历所有的点，大概需要的内存为 $200\times n\times m\text{Bytes}$，例如对样例 1 这大概需要 190GB。

对内存占用做了一些优化，可以去掉一些点，大约节省了 $40\%$ 的内存，但会导致时间加倍：

- `main2_mem.cpp` **需要使用 C++20 编译，因为使用了 std::unordered_map::contains 这个语法糖，若没有对应环境请改成std::unordered_map::find != std::unordered_map::end**


<script src="https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/contrib/auto-render.min.js" integrity="sha384-43gviWU0YVjaDtb/GhzOouOXtZMP/7XUzwPTstBeZFe/+rCMvRwr4yROQP43s0Xk" crossorigin="anonymous" onload="renderMathInElement(document.body, {delimiters: [{left: '$$', right: '$$', display: true},{left: '$', right: '$', display: false},{left: '\\(', right: '\\)', display: false},{left: '\\[', right: '\\]', display: true}],throwOnError : false});"></script>

<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/pseudocode@latest/build/pseudocode.min.css">
<script src="https://cdn.jsdelivr.net/npm/pseudocode@latest/build/pseudocode.min.js" onload="pseudocode.renderClass('pseudocode')"></script>