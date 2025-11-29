## 🇯🇵 
### 課題内容
- C 言語 + system call で HTTP Server を作ってみよう  
- 次のリクエストに対して正しいレスポンスを返すこと：
* GET /calc?query=2+10 HTTP/1.1に対して、HTTP/1.1 200 OK 
* Content-Length: 2 
* 12

### 余力があれば挑戦すると良いこと
1. IPv4 + IPv6 両対応  
2. CPU 性能を最大限活用できるようにする  
3. Non-blocking 化  
4. マルチスレッド化  
5. 通信タイムアウトの設定  
6. Signal を受け取ったら、全コネクションを正常終了して終了  
7. SSL 対応  

---

## 🇺🇸
### Assignment
- **Create an HTTP Server using C language and system calls.**  
- For the request below, return the exact response shown:

  ```
  Request:
  GET /calc?query=2+10 HTTP/1.1

  Response:
  HTTP/1.1 200 OK
  Content-Length: 2
  12
  ```

### Optional Challenges (OC)
1. Support both **IPv4** and **IPv6**  
2. Maximize **CPU performance utilization**  
3.  Implement **non-blocking I/O**  
4.  Enable **multithreading**  
5.  Configure **communication timeouts**  
6.   **Gracefully terminate** all connections upon receiving a signal  
7.   Add **SSL support**  

---

## C言語コンパイル実行手順
1. コンパイル
```bash
gcc test.c -o add
```

2. 実行
```bash
./add
```
