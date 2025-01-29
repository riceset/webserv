# webserv
Ce projet vous demandera d'écrire votre propre serveur HTTP. Vous devrez suivre la RFC d'HTTP et vous serez donc capable de tester avec un vrai browser web. HTTP est un des protocoles les plus utilisés sur internet. Connaitre son fonctionnement sera plus qu'utile même si vous ne faites pas de web à la fin.

このプロジェクトでは、自分でHTTPサーバーをHTTPのRFCに従って実装します。


## GitHub ブランチ運用方法

| **ブランチ名**  | **目的**   　　　　　　　　　　　　　　                                     | **作業内容**                                               | **マージ先**     | **備考**                                                |
|----------------|------------------------------------------------------|--------------------------------------------------------|---------------|---------------------------------------------------------|
| **main**       | 安定したコード                              | コードが安定している状態。直接変更を加えず、プルリクエスト経由でのみ変更が加わります。 | - | プロジェクトのデフォルトブランチ。リリース後のコードはここにマージされます。 |
| **feature/**   | 新機能の開発                                          | **main** ブランチから作業を開始し、新しい機能を実装します。                   | **main**       | ブランチ名は機能に関連する名前を付けてください（例: `feature/login-page`）。 |
| **fix/**       | バグ修正                                                 | **main** ブランチから切り出して、バグの修正を行います。                       | **main**       | 修正後、**main** ブランチにマージします。                 |

---

### プルリクエストの流れ

1. **feature** または **fix** ブランチを作成します。
2. 作業が完了したら、**main** ブランチへのプルリクエストを作成します。
3. プルリクエスト作成前に **main** ブランチを最新に保ち、コンフリクトを解消します。
4. プルリクエストに変更内容を簡潔に記述します。
5. レビュー後、**main** ブランチにマージします。

---

## Formatterの規約

本プロジェクトでは、コードの整形に `clang-format` を使用しています。以下の規約に従ってコードを整形してください。

### 使用するスタイル

`clang-format` は **Googleスタイル** を基準にしています。

### 設定ファイル

プロジェクトルートに配置された `.clang-format` ファイルを使用してコードの整形を行います。この設定ファイルには、インデント、改行、括弧の配置方法などが設定されています。

### コード整形の実行

コード整形は、以下のコマンドで実行できます。

```bash
make format
```

### GitHub Actionsによる整形チェック
main ブランチにマージする際、GitHub Actions で clang-format のフォーマットチェックが自動的に実行されます。エラーを無視してマージすることもできますが、できるだけ main ブランチは整形された状態を保ってください。

