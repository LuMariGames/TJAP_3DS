> [!NOTE]
> 私生活に影響を及ぼしているので暫く休みます。更新は多分もうしません。

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
v1.5.3からはバッファサイズを撤廃し、より高音質で楽しめる様に！  

## その他の内容  
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
Starting with v1.5.3, the buffer size has been eliminated, allowing you to enjoy higher quality music!  

## Other contents
(Except for the information described here, it is almost the same as the original work, so please check the original [Readme](https://github.com/togetg/TJAPlayer_for_3DS/blob/master/README_en.md) for details.)  

# Screenshots  
※開発中の内容、カスタムスキンを使用した内容が含まれています。  
　実際と異なる可能性がある事をお願いします。  
※This content includes content under development and content using custom skins.  
　Please note that the actual content may differ.

![My_Skin_Oni](https://github.com/user-attachments/assets/218a89c9-9758-4aa8-9894-cb298dfb45b0)
![My_Skin_Dani](https://github.com/user-attachments/assets/fe90a3fb-75d4-410f-811d-263ba74398da)  
![Songs_Select](https://github.com/user-attachments/assets/7b766612-867d-477a-9485-844404a21b60)
![SubTitle Search](https://github.com/user-attachments/assets/0c59da77-3ace-4f30-8680-cfc685594e01)

# 対応表 / Complete the table
> `✅` = full supported, `⚠️` = Incorrect behavior, `❌` = Not supported or Ignored,

|命令文 / commands|Supported?|Comment|
|---|---|---|
|`BPMCHANGE`, `SCROLL`, `GOGOTIME(GOGOSTART, GOGOEND)`|✅||
|`SECTION`, `BRANCHSTART`, `BRANCHEND`|⚠️|譜面分岐開始時の挙動がまだ不安定|
|`LEVELHOLD`, `BARLINEON`, `BARLINEOFF`, `DELAY`, `MEASURE`|✅||
|`BMSCROLL`, `HBSCROLL`, `NEXTSONG`|❌||
|`SUDDEN`|⚠️|一部動作が無効|

|metadata|Supported?|Comment|
|---|---|---|
|`COURSE:0~5`, `LEVEL`, `TITLE`, `OFFSET`|✅||
|`WAVE`, `BALLOON`, `SONGVOL`, `EXAM1~4`|✅|個別条件は非対応なので注意|
|`COURSE:6`|⚠️|`NEXTSONG`が非対応の為TNDEやOptk等で作られた段位は出来ない。|
|`DEMOSTART`, `SEVOL`|❌||
