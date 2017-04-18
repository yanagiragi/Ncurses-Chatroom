
## 這一篇用來紀錄兩年前System Programming(2015)慘烈期末 Hackton的 Repo

* 作業需求：C/C++ 用 Ncurses介面的 聊天室

* 時限：Six Hours

* 當初進度：在送訊息時無法同步聊天內容

* 後續：利用fork達成 在scanf時不會被block住

* 聲明：為了個資 請容我暫不公開組員資料

* P.S. &nbsp;因為時間不多 + 共同開發，Code很醜 ( ﾟ∀。) 


### 使用方式
編譯：
> <code>make</code>

執行： 

> Server端 :  <code>./goodserver</code>
   
> Cilent端 :  <code>./badclient</code>

說明：

> 一個client斷線大家會一起斷線喔 (?)

> a.k.a. 斷線那邊沒有處理得很好