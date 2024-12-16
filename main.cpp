/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 13:49:54 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/16 14:55:31 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Listener.hpp"
#include "Connection.hpp"
#include "EpollWrapper.hpp"
#include <iostream>
#include <map>
#include <stdexcept>
#include <cstdlib>   // for exit(), EXIT_FAILURE
#include <cerrno>
#include <cstring>   // for strerror()
#include <unistd.h>  // for close

int main() {
    try {
        // リスナー作成（ポート8080で待ち受け）
        Listener listener(8080);
        
        // epollインスタンス生成（最大100イベントを仮定）
        EpollWrapper epollWrapper(100);
        
        // リスナーソケットをepollに登録
        epollWrapper.addEvent(listener.getFd());

        // 接続管理用のマップ: fd -> Connection*
        std::map<int, Connection*> connections;

        // イベントループ開始
        while (true) {
            int nfds = epollWrapper.epwait();
            std::vector<struct epoll_event> events = epollWrapper.getEventsList();
            
            for (int i = 0; i < nfds; ++i) {
                int eventFd = events[i].data.fd;
                uint32_t ev = events[i].events;

                // リスナーソケットにイベントが来た場合は新規接続
                if (eventFd == listener.getFd()) {
                    // 新規コネクション生成（accept呼び出しはConnectionのコンストラクタ内）
                    try {
                        Connection* newConn = new Connection(listener.getFd());
                        connections[newConn->getFd()] = newConn;
                        // 新たな接続をepollに登録
                        epollWrapper.addEvent(newConn->getFd());
                    } catch (const std::exception &e) {
                        std::cerr << "Accept failed: " << e.what() << std::endl;
                    }
                } else {
                    // 既存接続のイベント
                    std::map<int, Connection*>::iterator it = connections.find(eventFd);
                    if (it == connections.end()) {
                        // 未知のイベントfd、何もしない
                        continue;
                    }

                    Connection* conn = it->second;

                    // EPOLLIN（読み込み可能）イベントの場合、ソケットから読み込み
                    if (ev & EPOLLIN) {
                        try {
                            conn->read();
                            // 読み取ったデータを標準出力に出力
                            std::string data = conn->getRbuff();
                            if (!data.empty()) {
                                std::cout << data;
                                // flushしておくと、すぐ標準出力へ流れる
                                std::cout.flush();
                            }
                        } catch (const std::runtime_error &re) {
                            // クライアント切断やrecv失敗時
                            std::cerr << "Connection closed or read error: " << re.what() << std::endl;
                            // コネクションをクローズして削除
                            close(conn->getFd());
                            delete conn;
                            connections.erase(it);
                        }
                    }

                    // 書き込みが必要であれば（本サンプルではクライアントへの返答はしないので省略）
                    // if (ev & EPOLLOUT) {
                    //    try {
                    //       conn->write();
                    //    } catch (const std::runtime_error &re) {
                    //       std::cerr << "Write error: " << re.what() << std::endl;
                    //       close(conn->getFd());
                    //       delete conn;
                    //       connections.erase(it);
                    //    }
                    // }

                    // その他イベント（エラー等）
                    if (ev & (EPOLLERR | EPOLLHUP)) {
                        // コネクションを閉じる
                        std::cerr << "Error or hangup on connection fd=" << eventFd << std::endl;
                        close(conn->getFd());
                        delete conn;
                        connections.erase(it);
                    }
                }
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Fatal error: " << e.what() << " (" << strerror(errno) << ")" << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}

