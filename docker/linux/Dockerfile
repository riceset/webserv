# ベースイメージ
FROM ubuntu:22.04

# 必要なパッケージをインストール
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    vim \
    && apt-get clean

# 作業ディレクトリを設定
WORKDIR /usr/src/app

# デフォルトのコマンドをbashに設定
CMD ["/bin/bash"]

