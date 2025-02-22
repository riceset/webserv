<?php
// HTTPレスポンスヘッダーを設定
header("HTTP/1.1 400 Bad Request");
header("Content-Type: text/plain; charset=UTF-8");
header("Content-Length: 12");
header("Cache-Control: no-cache, no-store, must-revalidate");
header("Pragma: no-cache");
header("Expires: 0");

// ボディ部分を出力
echo "Hello World\n";
?>