/*
 * =============================================
 *  超簡易HTTPサーバー (C + system call)
 *  機能：
 *     - TCPでポート8080をListen
 *     - GET /calc?query=2+10 の形式を受け取り
 *     - 2+10 を計算して結果を返す
 * =============================================
 */

#include <stdio.h>      // printf, perror など標準入出力
#include <stdlib.h>     // exit, atoi など
#include <string.h>     // strlen, strstr, sscanf など
#include <unistd.h>     // read, write, close などのUNIXシステムコール
#include <arpa/inet.h>  // socket, bind, listen, accept などネットワーク関連

#define PORT 8080       // サーバーが待ち受けるポート番号
#define BUF_SIZE 1024   // 受信バッファのサイズ

int main() {
    int server_fd, client_fd;               // サーバー用・クライアント用のファイルディスクリプタ
    struct sockaddr_in address;             // ソケットアドレス構造体（IPv4用）
    char buffer[BUF_SIZE];                  // クライアントからのリクエストデータ格納用
    int addrlen = sizeof(address);

    // -------------------------------------
    // ① ソケットの作成
    // -------------------------------------
    // PF_INET: IPv4, SOCK_STREAM: TCP通信, 0: デフォルトプロトコル
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");  // ソケット作成失敗時のエラー表示
        exit(EXIT_FAILURE);
    }

    // -------------------------------------
    // ② ソケットアドレスの設定
    // -------------------------------------
    // AF_INET → IPv4
    // INADDR_ANY → 任意のIPアドレスで接続を受け付ける
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);  // ホストバイト順→ネットワークバイト順に変換

    // -------------------------------------
    // ③ ソケットにアドレスを紐付け（bind）
    // -------------------------------------
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // -------------------------------------
    // ④ 接続待機状態へ移行（listen）
    // -------------------------------------
    // backlog=3 → 最大3つの接続要求をキューに保持
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("🚀 HTTPサーバー起動: ポート %d で待機中...\n", PORT);

    // -------------------------------------
    // ⑤ クライアント接続を待ち続ける無限ループ
    // -------------------------------------
    while (1) {
        // accept()でクライアント接続を受け入れる
        client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (client_fd < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        // 受信バッファを初期化
        memset(buffer, 0, BUF_SIZE);

        // -------------------------------------
        // ⑥ HTTPリクエストを受信（read）
        // -------------------------------------
        read(client_fd, buffer, BUF_SIZE);
        printf("📩 受信リクエスト:\n%s\n", buffer);

        // -------------------------------------
        // ⑦ "GET /calc?query=" からクエリ文字列を抽出
        // -------------------------------------
        char *query_start = strstr(buffer, "GET /calc?query=");
        if (query_start) {
            // "GET /calc?query=" の文字分をスキップして先頭を合わせる
            query_start += strlen("GET /calc?query=");

            int a, b;  // 計算用の変数

            // 例: "2+10" のような文字列から整数を2つ読み取る
            if (sscanf(query_start, "%d+%d", &a, &b) == 2) {
                int result = a + b;  // 足し算を実行
                char body[32];
                sprintf(body, "%d", result);  // 結果を文字列に変換

                // -------------------------------------
                // ⑧ HTTPレスポンスの構築
                // -------------------------------------
                // HTTPレスポンスの基本構造：
                // [ステータス行] + [ヘッダ] + 空行 + [ボディ]
                char response[128];
                sprintf(response,
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Length: %ld\r\n"
                        "Content-Type: text/plain\r\n"
                        "\r\n"
                        "%s",
                        strlen(body), body);

                // -------------------------------------
                // ⑨ クライアントへレスポンスを送信
                // -------------------------------------
                write(client_fd, response, strlen(response));
                printf("✅ レスポンス送信完了: %s\n", body);
            } else {
                // クエリ文字列が正しくなかった場合
                char *bad = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
                write(client_fd, bad, strlen(bad));
                printf("⚠️ 不正なクエリです。\n");
            }
        } else {
            // /calc 以外のパスの場合
            char *notfound = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
            write(client_fd, notfound, strlen(notfound));
            printf("❌ 404 Not Found\n");
        }

        // -------------------------------------
        // ⑩ 接続を閉じる（close）
        // -------------------------------------
        close(client_fd);
    }

    // -------------------------------------
    // （理論上ここには到達しない）
    // -------------------------------------
    close(server_fd);
    return 0;
}
