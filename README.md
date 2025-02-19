> [!NOTE]
> ❶：今後のアップデートについて
> 恐らく次が最後のメジャーアップデートになります。  
> 不具合等は修正しますが追加要素については  
> 処理能力がこれ以上追い付かないので、今後の予定は無いです。
>
> ❷：新規音ゲーに関するお知らせ  
> 現在「Touch-Keys」という音ゲーの開発をしています。  
> 完成次第リリース日を告知しますので続報をお待ち下さい。

# TJAP_3DS(日本語)  
TJAP_3DSは、[togetg](https://github.com/togetg)氏制作の[TJAPlayer for 3DS](https://github.com/togetg/TJAPlayer_for_3DS)のフォークです。  
とっくん や 固定スクロール 等の新オプションとスキンの変更、  
そして太鼓タワー, 段位道場と大ボリュームに追加された個人改良のソフトです。  

## 新要素一覧  
### オプション  
とっくん, かんぺき がある「とくしゅ」  
見にくい譜面も見やすく出来る「はやさ(固定)」  
オート時の連打を早く遅く出来る「連打速度」  

### コースモード  
CS7のモード、不可を出さずにノルマクリアを目指せ！「太鼓タワー」  
AC15から常連モード、条件を達成しないとノルマ失敗！？「段位道場」  

### 技術的な部分  
右から来る常識を覆す「マイナススクロール」  
見た目を自由に変更！「カスタムスキン」  

## それ以外の内容  
従来のバッファサイズだと動作が不安定になって来たのでv1.4.6(A)で大幅な軽量化を加えました。  
バッファサイズを2500~4000くらいにすると安定するかと思います  
(ここに記載している情報以外は元作品とほぼ同じなので詳しくは元の[Readme](https://github.com/togetg/TJAPlayer_for_3DS/blob/master/README.md)をご確認下さい。)  

# TJAP_3DS (English)
TJAP_3DS is a fork of [TJAPlayer for 3DS](https://github.com/togetg/TJAPlayer_for_3DS) produced by Mr. [togetg](https://github.com/togetg)
New options such as Tokkun and fixed scroll and skin changes,
And it is a personally improved software added to the Taiko Tower, Dani Dojo and the large volume.

## List of new elements
### Option
Tokkun, "Tokushu" with perfect
"Speed (fixed)" that makes it easy to see even sheet music that is difficult to see
"Roll speed" that allows you to speed up and slow down the automatic time

### Course mode
CS7 mode, aim to clear the quota without making no! "Taiko Tower"
Regular mode from AC15, quota failure if you don't meet the conditions!? "Dani Dojo"

### Technical part
"Negative scroll" that overturns the common sense coming from the right
Feel free to change the appearance! "Custom skin"

## Other contents
The operation became unstable with the conventional buffer size, so I added a significant weight reduction with v1.4.6(A).
I think it will be stable if the buffer size is about 2500~4000.
(Except for the information described here, it is almost the same as the original work, so please check the original [Readme](https://github.com/togetg/TJAPlayer_for_3DS/blob/master/README_en.md) for details.)  

# 対応表 / Complete the table
> `✅` = full supported, `⚠️` = Incorrect behavior, `❌` = Not supported or Ignored,

|命令文 / commands|Supported?|Comment|
|---|---|---|
|`BPMCHANGE`, `SCROLL`, `GOGOTIME(GOGOSTART, GOGOEND)`|✅||
|`SECTION`, `BRANCHSTART`, `BRANCHEND`|⚠️|譜面分岐開始時の挙動がまだ不安定|
|`LEVELHOLD`, `BARLINEON`, `BARLINEOFF`, `DELAY`, `MEASURE`|✅||
|`BMSCROLL`, `HBSCROLL`, `NEXTSONG`|❌|技術的に難しい|
|`SUDDEN`|❌|実装したいけど意外に難しいので後回し|

|metadata|Supported?|Comment|
|---|---|---|
|`COURSE:0~5`, `LEVEL`, `TITLE`, `OFFSET`|✅||
|`WAVE`, `BALLOON`, `SONGVOL`, `EXAM1~3`|✅|4条件は非対応なので注意|
|`COURSE:6`|⚠️|`NEXTSONG`が非対応の為TJAP3等で作られた段位は出来ない。|
|`DEMOSTART`, `SEVOL`|❌|選曲画面で曲を流せる様にする予定|
